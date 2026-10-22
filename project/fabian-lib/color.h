// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_COLOR_H_
#define PGT_COLOR_H_

#include <limits>
#include <assert.h>
#include <array>
#include "pgmath.h"

/*! \class Color
\brief A simple templated representation of color.

RGB, RGBA, BGR, BGRA
uchar, float
 ||     ||
sRGB, linear

EXR float, RGB(A), linear
PNG uchar, RGB(A), sRGB

\author Tomáš Fabián
\version 1.1
\date 2020-2021
*/

/*enum ColorSpaces
{
	kLinearRGB = 0,
	kSRGB,
};*/

//#define FAST_INTERP
constexpr auto GAMMA = 2.4f;

template <int N, class T>
class Color
{
public:
	Color()
	{
		data.fill( T( 0 ) );
	}

	Color( const std::array<T, N> & data )
	{
		this->data = data;
	}

	Color<N, T> & operator+=( const Color<N, T> & rhs )
	{
		for ( int i = 0; i < N; ++i )
		{
			data[i] += rhs.data[i];
		}

		return *this;
	}

	friend Color<N, T> operator-( Color<N, T> lhs, const Color<N, T> & rhs )
	{
		// passing lhs by value helps optimize chained a + b + c
		// otherwise, both parameters may be const references
		lhs -= rhs;

		return lhs;
	}

	Color<N, float> & operator-=( const Color<N, float> & rhs )
	{
		for ( int i = 0; i < N; ++i )
		{
			data[i] -= rhs.data[i];
		}

		return *this;
	}

	friend Color<N, T> operator+( Color<N, T> lhs, const Color<N, T> & rhs )
	{
		// passing lhs by value helps optimize chained a + b + c
		// otherwise, both parameters may be const references
		lhs += rhs;

		return lhs;
	}

	Color<N, T> & operator*=( const T rhs )
	{
		for ( int i = 0; i < N; ++i )
		{
			data[i] *= rhs;
		}

		return *this;
	}

	friend Color<N, T> operator*( Color<N, T> lhs, const T rhs )
	{
		lhs *= rhs;

		return lhs;
	}

	Color<N, T> & operator*=( const Color<N, T> rhs )
	{
		for ( int i = 0; i < N; ++i )
		{
			data[i] *= rhs.data[i];
		}

		return *this;
	}

	friend Color<N, T> operator*( Color<N, T> lhs, const Color<N, T> rhs )
	{
		lhs *= rhs;

		return lhs;
	}

	// explicit type casting operator uchar -> float, i.e. sRGB -> linear
	//template<class T2>
	explicit operator Color<N, float>() const
	{
		Color<N, float> lhs;

		for ( int i = 0; i < N; ++i )
		{
			if ( i < 3 )
			{
				lhs.data[i] = c_linear( float( data[i] ) * ( 1.0f / 255.0f ) );
			}
			else
			{
				lhs.data[i] = float( data[i] ) * ( 1.0f / 255.0f );
			}
		}

		return lhs;
	}

	// explicit type casting operator float -> uchar, i.e. linear -> sRGB
	explicit operator Color<N, unsigned char>() const
	{
		Color<N, unsigned char> lhs;

		for ( int i = 0; i < N; ++i )
		{
			if ( i < 3 )
			{
				lhs.data[i] = ( unsigned char )( max( T( 0 ), min( T( 255 ), c_srgb( data[i] ) * T( 255 ) ) ) );
			}
			else
			{
				lhs.data[i] = ( unsigned char )( max( T( 0 ), min( T( 255 ), data[i] * T( 255 ) ) ) );
			}
		}

		return lhs;
	}

	bool is_zero() const
	{
		for ( int i = 0; i < N; ++i )
		{
			if ( data[i] != T( 0 ) ) return false;
		}

		return true;
	}

	bool is_valid( const T from = -T( 100000 ), const int to = T( 100000 ) ) const
	{
		for ( int i = 0; i < N; ++i )
		{
			if ( data[i] < from || data[i] > to || isnan( data[i] ) ) return false;
		}

		return true;
	}

	Color<N, T> reverse()
	{
		if ( N == 4 )
		{
			std::reverse( std::begin( data ), std::end( data ) - 1 );
		}
		else
		{
			std::reverse( std::begin( data ), std::end( data ) );
		}

		return *this;
	}

	T max_value() const
	{
		T value = std::numeric_limits<T>::lowest();

		for ( int i = 0; i < N; ++i )
		{
			if ( ( N != 4 || i != N - 1 ) && data[i] > value )
			{
				value = data[i];
			}
		}

		return value;
	}

	T min_value() const
	{
		T value = ( std::numeric_limits<T>::max )( );

		for ( int i = 0; i < N; ++i )
		{
			if ( ( N != 4 || i != N - 1 ) && data[i] < value )
			{
				value = data[i];
			}
		}

		return value;
	}

	float luminance() const
	{
		switch ( N )
		{
			case 1:
				return float( data[0] );
				break;

			case 3:
			case 4:
				return float( data[0] ) * 0.2126f + float( data[1] ) * 0.7152f + float( data[2] ) * 0.0722f;
				break;
		}

		return -1.0f;
	}

	/* the sRGB decoding function (EOTF) converting from gamma space to linear space defined by the IEC 61966-2-1:1999 standard which can be approximated by c_lin = c_srgb^2.2 */
	/* TODO make it private or https://stackoverflow.com/questions/4390499/how-do-i-call-static-members-of-a-template-class */
	static float c_linear( const float c_srgb, const float gamma = GAMMA )
	{
		if ( c_srgb <= 0.0f ) return 0.0f;
		else if ( c_srgb >= 1.0f ) return 1.0f;

		assert( ( c_srgb >= 0.0f ) && ( c_srgb <= 1.0f ) );

		if ( c_srgb <= 0.04045f )
		{
			return c_srgb / 12.92f;
		}
		else
		{
			const float a = 0.055f;
			return powf( ( c_srgb + a ) / ( 1.0f + a ), gamma );
		}
	}

	static float c_srgb( const float c_linear, const float gamma = GAMMA )
	{
		if ( c_linear <= 0.0f ) return 0.0f;
		else if ( c_linear >= 1.0f ) return 1.0f;

		assert( ( c_linear >= 0.0f ) && ( c_linear <= 1.0f ) );

		if ( c_linear <= 0.0031308f )
		{
			return 12.92f * c_linear;
		}
		else
		{
			const float a = 0.055f;
			return ( 1.0f + a ) * powf( c_linear, 1.0f / gamma ) - a;
		}
	}

	template<int N>
	static Color<N, float> toLinear( Color<N, float> srgb )
	{
		Color<N, float> & linear = srgb;

		for ( int i = 0; i < min( srgb.channels, 3 ); ++i )
		{
			linear.data[i] = Color<N, float>::c_linear( srgb.data[i] );
		}

		return linear;
	}

	template<int N>
	static Color<N, float> toSRGB( Color<N, float> linear )
	{
		Color<N, float> & srgb = linear;

		for ( int i = 0; i < min( srgb.channels, 3 ); ++i )
		{
			srgb.data[i] = Color<N, float>::c_srgb( linear.data[i] );
		}

		return srgb;
	}

	template<int N>
	static Color<N, float> Reinhard( Color<N, float> linear, const float exposition = 1.0f )
	{
		Color<N, float> & tmp = linear;

		for ( int i = 0; i < min( linear.channels, 3 ); ++i )
		{
			const auto c = linear.data[i] * exposition;

			tmp.data[i] = c / ( 1.0f + c );
		}

		return tmp;
	}

	template<int M, class T2>
	explicit operator Color<M, T2>()
	{
		Color<M, T2> color;

		int i = 0;
		for ( ; i < min( M, N ); ++i )
		{
			color.data[i] = saturate_cast< T2, T >( data[i] );
		}
		for ( ; i < M; ++i )
		{
			color.data[i] = static_cast< T2 >( 0 );
		}

		return color;
	}

	std::array<T, N> data;
	static const int channels = N;
	//static const ColorSpaces colorSpace = ColorSpaces::kLinearRGB; // default color space

	static Color<N, T> black;
	static Color<N, T> white;
	static Color<N, T> gray50;
};

using Color1f = Color<1, float>;
using Color3f = Color<3, float>;
using Color4f = Color<4, float>;
using Color1u = Color<1, unsigned char>;
using Color3u = Color<3, unsigned char>;
using Color4u = Color<4, unsigned char>;

Color3f Color3f::black = Color3f( { 0.0f, 0.0f, 0.0f } );
Color3f Color3f::white = Color3f( { 1.0f, 1.0f, 1.0f } );
Color3f Color3f::gray50 = Color3f( Color3u( { 127, 127, 127 } ) );
Color4f Color4f::black = Color4f( { 0.0f, 0.0f, 0.0f, 1.0f } );
Color4f Color4f::white = Color4f( { 1.0f, 1.0f, 1.0f, 1.0f } );
Color3u Color3u::black = Color3u( { 0, 0, 0 } );
Color3u Color3u::white = Color3u( { 255, 255, 255 } );
Color3u Color3u::gray50 = Color3u( { 127, 127, 127 } );

#endif // PGT_COLOR_H_

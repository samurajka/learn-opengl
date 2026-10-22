// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_TEXTURE_H_
#define PGT_TEXTURE_H_

#define USE_BILINEAR_INTERP

//#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include "freeimage.h"
#pragma pack( push ) // resolves warning C4103
#include <glm/vec2.hpp>
#pragma pack( pop )

#include "color.h"

FIBITMAP * BitmapFromFile( const char * file_name, int & width, int & height, int & stride );
// Note that all float images in FreeImage are forced to have a range in <0, 1> after applying build-in conversions!!!
// see https://sourceforge.net/p/freeimage/bugs/259/
FIBITMAP * Custom_FreeImage_ConvertToRGBF( FIBITMAP * dib ); // this fix removes clamp from conversion of float images
FIBITMAP * Custom_FreeImage_ConvertToRGBAF( FIBITMAP * dib );  // this fix removes clamp from conversion of float images

enum class WrappingType
{
	kRepeat = 0,
	KClampToEdge
};

/*! \class Texture
\brief A simple templated representation of texture.

Texture3f normal = Texture3f( "../../../data/denoise/normal_100spp.exr");
Texture3f output = Texture3f( width, height );
output.Save( "denoised.exr" );

\author Tomáš Fabián
\version 1.1
\date 2020-2024
*/

template <class T, FREE_IMAGE_TYPE F>
class Texture
{
public:
	static int get_bpp()
	{
		return sizeof( T ) * 8;
	}

	static int get_stride( const int width, const int padding = 32 )
	{
		return ( ( get_bpp() * width + ( padding - 1 ) ) / padding ) * ( padding / 8 );
	}

	int offset( const int x, const int y ) const
	{
		return y * stride_ + x * ( get_bpp() / 8 );
	}

	Texture( const int width, const int height )
	{
		assert( width > 0 && height > 0 );

		width_ = width;
		height_ = height;
		stride_ = Texture::get_stride( width_ );

		//data_.resize( size_t( width ) * size_t( height ) );
		data_ = std::make_unique<std::byte[]>( stride_ * height_ );
	}

	/*Texture(const int width, const int height, const int pixel_stride, T * data)
	{
		assert( width > 0 && height > 0 && pixel_stride > 0 && data );

		width_ = width;
		height_ = height;

		data_.resize( size_t( width ) * size_t( height ) );

		for ( auto & pixel : data_ )
		{
			pixel = *data;
			data = ( T * )( ( ( BYTE * )data ) + pixel_stride );
		}
	}*/

	Texture( const std::string & file_name, const bool swap_RB = false )
	{
		FIBITMAP * dib = BitmapFromFile( file_name.c_str(), width_, height_, stride_ );

		if ( dib )
		{
			if ( true ) // always make sure that the loaded bitmap will fit the allocated data size
			{
				FIBITMAP * const dib_new = Convert( dib );
				assert( dib_new );
				FreeImage_Unload( dib );
				dib = dib_new;
			}

			//data_.resize( size_t( width_ ) * size_t( height_ ) );

			stride_ = FreeImage_GetPitch( dib ); // (bytes)
			const int bpp = FreeImage_GetBPP( dib ); // (bits)

			assert( bpp == get_bpp() );

			data_ = std::make_unique<std::byte[]>( stride_ * height_ );

			FreeImage_ConvertToRawBits( reinterpret_cast< BYTE * >( data_.get() ), dib, stride_, bpp,
				FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE );

			if ( swap_RB && ( bpp == 24 || bpp == 96 ) )
			{
				for ( int y = 0; y < height_; ++y )
				{
					for ( int x = 0; x < width_; ++x )
					{
						set_pixel( x, y, pixel( x, y ).reverse() );
					}
				}
			}

			FreeImage_Unload( dib );
			dib = nullptr;

			double range[] = { ( std::numeric_limits<double>::max )( ), std::numeric_limits<double>::lowest() };

			for ( int y = 0; y < height_; ++y )
			{
				for ( int x = 0; x < width_; ++x )
				{
					auto value = pixel( x, y );
					range[0] = min( range[0], double( value.min_value() ) );
					range[1] = max( range[1], double( value.max_value() ) );
				}
			}

			printf( "Texture '%s' (%d x %d px, %d bpp, <%0.3f, %0.3f>, %0.1f MB) loaded.\n",
				file_name.c_str(), width_, height_, bpp, range[0], range[1],
				stride_ * height_ / ( 1024.0f * 1024.0f ) );
		}
		else
		{
			printf( "Texture '%s' not loaded.\n", file_name.c_str() );
		}
	}

	Texture<T, F> Roi( const int x0, const int y0, const int x1, const int y1 )
	{
		Texture<T, F> roi( x1 - x0, y1 - y0 );

		for ( int y = y0; y < y1; ++y )
		{
			for ( int x = x0; x < x1; ++x )
			{
				roi.set_pixel( x - x0, y - y0, pixel( x, y ) );
			}
		}

		return roi;
	}

	T pixel( const int x, const int y ) const
	{
		assert( x >= 0 && x < width_ && y >= 0 && y < height_ );

		//return data_[size_t( x ) + size_t( y ) * size_t( width_ )];
		void * ptr = static_cast< void * >( &data_[offset( x, y )] );

		return *static_cast< T * >( ptr );
	}

	void set_pixel( const int x, const int y, const T & color )
	{
		assert( x >= 0 && x < width_ && y >= 0 && y < height_ );

		//data_[size_t( x ) + size_t( y ) * size_t( width_ )] = color;
		void * ptr = static_cast< void * >( &data_[offset( x, y )] );

		*static_cast< T * >( ptr ) = color;
	}

	inline void WrapTextureCoords( float u, float v, int & i0, int & j0, int & i1, int & j1, float & x, float & y, const WrappingType wrapping )
	{
		// flip in v-direction based on 3ds max texturing behaviour
		v = 1.0f - v;

		// texture coordinates wrapping: GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, or GL_MIRROR_CLAMP_TO_EDGE. GL_CLAMP_TO_EDGE
		// default mode: GL_REPEAT		
		switch ( wrapping )
		{
		case WrappingType::KClampToEdge:
		{
			u = clamp( u, 1.0f / ( 2 * width_ ), 1.0f - 1.0f / ( 2 * width_ ) );
			v = clamp( v, 1.0f / ( 2 * height_ ), 1.0f - 1.0f / ( 2 * height_ ) );

			const float u0 = u - 0.5f / width_;
			const float v0 = v - 0.5f / height_;

			int i0 = int( floorf( u0 * width_ ) );
			int j0 = int( floorf( v0 * height_ ) );

			int i1 = clamp( i0 + 1, 0, width_ - 1 );
			int j1 = clamp( j0 + 1, 0, height_ - 1 );

			float du = u * width_ - ( i0 + 0.5f );
			float dv = v * height_ - ( j0 + 0.5f );
			break;
		}

		default:
		{
			float fp;
			u = std::modf( u, &fp );
			if ( u < 0.0f ) u += 1.0f;
			v = std::modf( v, &fp );
			if ( v < 0.0f ) v += 1.0f;

			const float u0 = u - 0.5f / width_;
			const float v0 = v - 0.5f / height_;

			i0 = int( floorf( u0 * width_ ) );
			if ( i0 < 0 ) i0 += width_;
			j0 = int( floorf( v0 * height_ ) );
			if ( j0 < 0 ) j0 += height_;

			i1 = ( i0 + 1 ) % width_;
			j1 = ( j0 + 1 ) % height_;

			x = u * width_ - ( i0 + 0.5f );
			if ( x < 0.0f ) x += width_;
			y = v * height_ - ( j0 + 0.5f );
			if ( y < 0.0f ) y += height_;
		}
		}

		assert( x >= 0.0f && x <= 1.0f );
		assert( y >= 0.0f && y <= 1.0f );
	}

	T texel( const glm::vec2 & uv, const WrappingType wrapping = WrappingType::kRepeat )
	{
		return texel( uv.x, uv.y, wrapping );
	}

	T texel( const float u, const float v, const WrappingType wrapping = WrappingType::kRepeat )
	{
		int i0, j0, i1, j1;
		float x, y;
		WrapTextureCoords( u, v, i0, j0, i1, j1, x, y, wrapping );

		T value;

#ifndef USE_BILINEAR_INTERP
		// nearest neighbour
		value = pixel(
			max( 0, min( int( u * width_ ), width_ - 1 ) ),
			max( 0, min( int( v * height_ ), height_ - 1 ) ) );
#else
		// bilinear interpolation
		const Color3f v00 = Color3f( pixel( i0, j0 ) );
		const Color3f v10 = Color3f( pixel( i1, j0 ) );
		const Color3f v01 = Color3f( pixel( i0, j1 ) );
		const Color3f v11 = Color3f( pixel( i1, j1 ) );

		const Color3f tmp = ( v00 * ( 1.0f - x ) + v10 * x ) * ( 1.0f - y ) + ( v01 * ( 1.0f - x ) + v11 * x ) * y;

		value = T( tmp );
#endif		

		return value;
	}

	int width() const
	{
		return width_;
	}

	int height() const
	{
		return height_;
	}

	void * data()
	{
		return static_cast< void * >( data_.get() );
	}

	size_t size() const
	{
		return row_stride() * height_;
	}

	size_t pixel_stride() const
	{
		return sizeof( T );
	}

	size_t row_stride() const
	{
		return pixel_stride() * width_;
	}

	FIBITMAP * Convert( FIBITMAP * dib )
	{
		throw "Convert method is defined only for particular Texture types";

		return nullptr;
	}

	void set( void * pixel, int pixel_size, int pixel_stride, void * alpha = nullptr, int alpha_size = 0, int alpha_stride = 0 )
	{
		throw "Set method is defined only for particular Texture types";
	}

	/*static Texture Load( const std::string & file_name )
	{
		throw "Load method is defined only for particular Texture types";
	}*/

	void Save( const std::string & file_name ) const
	{
		FIBITMAP * bitmap = FreeImage_AllocateT( F, width_, height_, sizeof( T ) * 8 ); // FIT_BITMAP, FIT_BITMAP, FIT_RGBF, FIT_RGBAF
		BYTE * data = ( BYTE * )( FreeImage_GetBits( bitmap ) );
		const int scan_width = FreeImage_GetPitch( bitmap );
		std::memcpy( data, data_.get(), scan_width * height_ );
		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename( file_name.c_str() );
		if ( fif != FIF_PFM )
		{
			FreeImage_FlipVertical( bitmap );
		}
		if ( FreeImage_Save( fif, bitmap, file_name.c_str() ) )
		{
			printf( "Texture was saved successfully in '%s'.\n", file_name.c_str() );
		}
		else
		{
			printf( "Texture failed to save in '%s'.\n", file_name.c_str() );
		}
		FreeImage_Unload( bitmap );
		bitmap = nullptr;
		data = nullptr;
	}

private:
	//std::vector<T> data_; // TODO does not work for eg. 386x386 image, because row must be padded
	std::unique_ptr<std::byte[]> data_; /* image data */

	int width_{ 0 }; /* image width in pixels */
	int height_{ 0 }; /* image height in pixels */
	int stride_{ 0 }; /* width of the bitmap in bytes (pitch, scan width), rounded to the next 32-bit boundary */
	int bpp_{ 0 }; /* bits per pixel */
};

using Texture3f = Texture<Color3f, FIT_RGBF>;
using Texture4f = Texture<Color4f, FIT_RGBAF>;
using Texture1u = Texture<Color1u, FIT_BITMAP>;
using Texture3u = Texture<Color3u, FIT_BITMAP>;
using Texture4u = Texture<Color4u, FIT_BITMAP>;

template<>
FIBITMAP * Texture1u::Convert( FIBITMAP * dib );

template<>
FIBITMAP * Texture3u::Convert( FIBITMAP * dib );

template<>
FIBITMAP * Texture4u::Convert( FIBITMAP * dib );

template<>
FIBITMAP * Texture3f::Convert( FIBITMAP * dib );

template<>
FIBITMAP * Texture4f::Convert( FIBITMAP * dib );

template<>
void Texture4u::set( void * pixel, int pixel_size, int pixel_stride, void * alpha, int alpha_size, int alpha_stride );

template<>
Color1u Texture<Color1u, FIT_BITMAP>::texel( const float u, const float v, const WrappingType wrapping );

#endif // PGT_TEXTURE_H_

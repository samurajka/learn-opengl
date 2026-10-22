// ------------------------------------------------------------------------------
// PG Tools, (c)2021-2025 Tomas Fabian, VSB-TUO, FEECS, Dept. of Computer Science
// 
// This library is provided exclusively for non-commercial educational use in the
// Computer Graphics I and II courses at VSB-TUO. Redistribution or disclosure to
// third parties in any form is strictly prohibited.
// ------------------------------------------------------------------------------

#ifndef PGT_MATH_H_
#define PGT_MATH_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <assert.h>

#include "../../glm/gtc/type_ptr.hpp"
#include "../../glm/gtx/norm.hpp" // glm::length2

/* return the minimum of two values */
template <class T> inline T min( const T a, const T b )
{
	return ( a <= b ) ? a : b;
}

/* return the maximum of two values */
template <class T> inline T max( const T a, const T b )
{
	return ( a >= b ) ? a : b;
}

/* return the square of the specified value x */
template <class T> inline T sqr( const T x )
{
	return x * x;
}

/* convert degrees to radians */
inline float deg2rad( const float x )
{
	return x * float( M_PI ) / 180.0f;
}

/* update the mean value by new sample providing the total number of samples */
template <class T> inline void update( T & mean, const T new_sample, const int samples_count )
{
	mean = T( ( mean * ( samples_count - T( 1 ) ) + new_sample ) / samples_count );
}

/* restrict value x to the given range <a=0, b=1> */
template <class T> inline T clamp( const T x, const T a = (T)0, const T b = (T)1 )
{
	return min( max( x, a ), b );
}

/* round up to the nearest integer */
template <typename T> inline T round_up( const T x, const T y )
{
	return ( ( x + y - 1 ) / y ) * y;
}

template <class T, class S> inline T saturate_cast( S x )
{
	if ( std::numeric_limits<T>::is_integer && !std::numeric_limits<S>::is_integer )
	{
		x += static_cast< S >( 0.5 ); // do the mathematical rounding only if the target type is integer and the source type is not
	}

	if ( x <= std::numeric_limits<T>::min() )
	{
		return static_cast< T >( std::numeric_limits<T>::min() );
	}
	else if ( x >= std::numeric_limits<T>::max() )
	{
		return static_cast< T >( std::numeric_limits<T>::max() );
	}
	
	return static_cast< T >( x );
}

/* the Heaviside step function */
float Heaviside( const float x );

float gamma_quot( const float a, const float b );

/**
* Returns the full (non-normalised) incomplete beta function
* https://www.wolframalpha.com/input/?i=Beta%280.1%2C+9%2C+9%29
*/
float ibeta( const float z, const float a, const float b, const int n = 16 );

/* simple hash function */
unsigned long long QuickHash( const unsigned char * data, const size_t length, unsigned long long mix = 0 );

/* produces orthonormal vector to the given vector v */
inline glm::vec3 orthonormal( const glm::vec3 & v )
{
	// https://math.stackexchange.com/questions/2373703/generate-an-orthonormal-basis-belonging-to-a-plan-defined-by-it-normal-vector
	//return ( abs( v.x ) > abs( v.z ) ) ? glm::vec3( -v.y, v.x, 0.0f ) : glm::vec3( 0.0f, -v.z, v.y );
	//return ( abs( x ) > abs( z ) ) ? Vector3( y, -x, 0.0f ) : Vector3( 0.0f, z, -y );
	return glm::normalize( ( abs( v.x ) > abs( v.z ) ) ?
		glm::vec3( v.y, -v.x, 0.0f ) : glm::vec3( 0.0f, v.z, -v.y ) );
}

#endif // PG_MATH_H

//////////////////////////////////////////////////////////////////////////////
//
//  --- vec.h ---
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __ANGEL_VEC_H__
#define __ANGEL_VEC_H__

#include <math.h>
#include <stdio.h>

#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

//  Defined constant for when numbers are too small to be used in the
//    denominator of a division operation.  This is only used if the
//    DEBUG macro is defined.
const float  DivideByZeroTolerance = float(1.0e-07);

//  Degrees-to-radians constant 
const float  DegreesToRadians = M_PI / 180.0;

//////////////////////////////////////////////////////////////////////////////
//
//  vec2.h - 2D vector
//

struct vec2 {

    float  x;
    float  y;

    //
    //  --- Constructors and Destructors ---
    //

    vec2( float s = float(0.0) ) :
	x(s), y(s) {}

    vec2( float _x, float _y ) :
	x(_x), y(_y) {}

    vec2( const vec2& v )
	{ x = v.x;  y = v.y;  }

    //
    //  --- Indexing Operator ---
    //

    float& operator [] ( int i ) { return *(&x + i); }
    const float operator [] ( int i ) const { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //

    vec2 operator - () const // unary minus operator
	{ return vec2( -x, -y ); }

    vec2 operator + ( const vec2& v ) const
	{ return vec2( x + v.x, y + v.y ); }

    vec2 operator - ( const vec2& v ) const
	{ return vec2( x - v.x, y - v.y ); }

    vec2 operator * ( const float s ) const
	{ return vec2( s*x, s*y ); }

    vec2 operator * ( const vec2& v ) const
	{ return vec2( x*v.x, y*v.y ); }

    friend vec2 operator * ( const float s, const vec2& v )
	{ return v * s; }

    vec2 operator / ( const float s ) const {
#ifdef DEBUG
	if ( fabs(s) < DivideByZeroTolerance ) {
	    printf("Division by zero\n");
	    return vec2();
	}
#endif // DEBUG

	float r = float(1.0) / s;
	return *this * r;
    }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    vec2& operator += ( const vec2& v )
	{ x += v.x;  y += v.y;   return *this; }

    vec2& operator -= ( const vec2& v )
	{ x -= v.x;  y -= v.y;  return *this; }

    vec2& operator *= ( const float s )
	{ x *= s;  y *= s;   return *this; }

    vec2& operator *= ( const vec2& v )
	{ x *= v.x;  y *= v.y; return *this; }

    vec2& operator /= ( const float s ) {
#ifdef DEBUG
	if ( fabs(s) < DivideByZeroTolerance ) {
	    printf("Division by zero\n");
	}
#endif // DEBUG

	float r = float(1.0) / s;
	*this *= r;

	return *this;
    }
	
    //
    //  --- Conversion Operators ---
    //

    operator const float* () const
	{ return static_cast<const float*>( &x ); }

    operator float* ()
	{ return static_cast<float*>( &x ); }
};

//----------------------------------------------------------------------------
//
//  Non-class vec2 Methods
//

inline
float dot( const vec2& u, const vec2& v ) {
    return u.x * v.x + u.y * v.y;
}

inline
float length( const vec2& v ) {
    return sqrt( dot(v,v) );
}

inline
vec2 normalize( const vec2& v ) {
    return v / length(v);
}

//////////////////////////////////////////////////////////////////////////////
//
//  vec3.h - 3D vector
//
//////////////////////////////////////////////////////////////////////////////

struct vec3 {

    float  x;
    float  y;
    float  z;

    //
    //  --- Constructors and Destructors ---
    //

    vec3( float s = float(0.0) ) :
	x(s), y(s), z(s) {}

    vec3( float x, float y, float z ) :
	x(x), y(y), z(z) {}

    vec3( const vec3& v ) { x = v.x;  y = v.y;  z = v.z; }

    vec3( const vec2& v, const float f ) { x = v.x;  y = v.y;  z = f; }

    //
    //  --- Indexing Operator ---
    //

    float& operator [] ( int i ) { return *(&x + i); }
    const float operator [] ( int i ) const { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //

    vec3 operator - () const  // unary minus operator
	{ return vec3( -x, -y, -z ); }

    vec3 operator + ( const vec3& v ) const
	{ return vec3( x + v.x, y + v.y, z + v.z ); }

    vec3 operator - ( const vec3& v ) const
	{ return vec3( x - v.x, y - v.y, z - v.z ); }

    vec3 operator * ( const float s ) const
	{ return vec3( s*x, s*y, s*z ); }

    vec3 operator * ( const vec3& v ) const
	{ return vec3( x*v.x, y*v.y, z*v.z ); }

    friend vec3 operator * ( const float s, const vec3& v )
	{ return v * s; }

    vec3 operator / ( const float s ) const {
#ifdef DEBUG
	if ( fabs(s) < DivideByZeroTolerance ) {
	    printf("Division by zero\n");
	    return vec3();
	}
#endif // DEBUG

	float r = float(1.0) / s;
	return *this * r;
    }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    vec3& operator += ( const vec3& v )
	{ x += v.x;  y += v.y;  z += v.z;  return *this; }

    vec3& operator -= ( const vec3& v )
	{ x -= v.x;  y -= v.y;  z -= v.z;  return *this; }

    vec3& operator *= ( const float s )
	{ x *= s;  y *= s;  z *= s;  return *this; }

    vec3& operator *= ( const vec3& v )
	{ x *= v.x;  y *= v.y;  z *= v.z;  return *this; }

    vec3& operator /= ( const float s ) {
#ifdef DEBUG
	if ( fabs(s) < DivideByZeroTolerance ) {
	    printf("Division by zero\n");
	}
#endif // DEBUG

	float r = float(1.0) / s;
	*this *= r;

	return *this;
    }
	
    //
    //  --- Conversion Operators ---
    //

    operator const float* () const
	{ return static_cast<const float*>( &x ); }

    operator float* ()
	{ return static_cast<float*>( &x ); }
};

//----------------------------------------------------------------------------
//
//  Non-class vec3 Methods
//

inline
float dot( const vec3& u, const vec3& v ) {
    return u.x*v.x + u.y*v.y + u.z*v.z ;
}

inline
float length( const vec3& v ) {
    return sqrt( dot(v,v) );
}

inline
vec3 normalize( const vec3& v ) {
    return v / length(v);
}

inline
vec3 cross(const vec3& a, const vec3& b )
{
    return vec3( a.y * b.z - a.z * b.y,
		 a.z * b.x - a.x * b.z,
		 a.x * b.y - a.y * b.x );
}


//////////////////////////////////////////////////////////////////////////////
//
//  vec4 - 4D vector
//
//////////////////////////////////////////////////////////////////////////////

struct vec4 {

    float  x;
    float  y;
    float  z;
    float  w;

    //
    //  --- Constructors and Destructors ---
    //

    vec4( float s = float(0.0) ) :
	x(s), y(s), z(s), w(s) {}

    vec4( float x, float y, float z, float w ) :
	x(x), y(y), z(z), w(w) {}

    vec4( const vec4& v ) { x = v.x;  y = v.y;  z = v.z;  w = v.w; }

    vec4( const vec3& v, const float s = 1.0 ) : w(s)
	{ x = v.x;  y = v.y;  z = v.z; }

    vec4( const vec2& v, const float z, const float w ) : z(z), w(w)
	{ x = v.x;  y = v.y; }

    //
    //  --- Indexing Operator ---
    //

    float& operator [] ( int i ) { return *(&x + i); }
    const float operator [] ( int i ) const { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //

    vec4 operator - () const  // unary minus operator
	{ return vec4( -x, -y, -z, -w ); }

    vec4 operator + ( const vec4& v ) const
	{ return vec4( x + v.x, y + v.y, z + v.z, w + v.w ); }

    vec4 operator - ( const vec4& v ) const
	{ return vec4( x - v.x, y - v.y, z - v.z, w - v.w ); }

    vec4 operator * ( const float s ) const
	{ return vec4( s*x, s*y, s*z, s*w ); }

    vec4 operator * ( const vec4& v ) const
	{ return vec4( x*v.x, y*v.y, z*v.z, w*v.z ); }

    friend vec4 operator * ( const float s, const vec4& v )
	{ return v * s; }

    vec4 operator / ( const float s ) const {
#ifdef DEBUG
	if ( fabs(s) < DivideByZeroTolerance ) {
	    printf("Division by zero\n");
	    return vec4();
	}
#endif // DEBUG

	float r = float(1.0) / s;
	return *this * r;
    }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    vec4& operator += ( const vec4& v )
	{ x += v.x;  y += v.y;  z += v.z;  w += v.w;  return *this; }

    vec4& operator -= ( const vec4& v )
	{ x -= v.x;  y -= v.y;  z -= v.z;  w -= v.w;  return *this; }

    vec4& operator *= ( const float s )
	{ x *= s;  y *= s;  z *= s;  w *= s;  return *this; }

    vec4& operator *= ( const vec4& v )
	{ x *= v.x, y *= v.y, z *= v.z, w *= v.w;  return *this; }

    vec4& operator /= ( const float s ) {
#ifdef DEBUG
	if ( fabs(s) < DivideByZeroTolerance ) {
	    printf("Division by zero\n");
	}
#endif // DEBUG

	float r = float(1.0) / s;
	*this *= r;

	return *this;
    }

    //
    //  --- Conversion Operators ---
    //

    operator const float* () const
	{ return static_cast<const float*>( &x ); }

    operator float* ()
	{ return static_cast<float*>( &x ); }
};

//----------------------------------------------------------------------------
//
//  Non-class vec4 Methods
//

inline
float dot( const vec4& u, const vec4& v ) {
    return u.x*v.x + u.y*v.y + u.z*v.z + u.w+v.w;
}

inline
float length( const vec4& v ) {
    return sqrt( dot(v,v) );
}

inline
vec4 normalize( const vec4& v ) {
    return v / length(v);
}

inline
vec3 cross(const vec4& a, const vec4& b )
{
    return vec3( a.y * b.z - a.z * b.y,
		 a.z * b.x - a.x * b.z,
		 a.x * b.y - a.y * b.x );
}

//----------------------------------------------------------------------------

typedef vec2 point2;
typedef vec3 point3;
typedef vec4 point4;

typedef vec3 color3;
typedef vec4 color4;

inline vec3 TriangleNormal(
	const point3 &v0,
	const point3 &v1,
	const point3 &v2)
{
	vec3 n=cross(v1-v0, v2-v0);
	return normalize(n);
}

#endif // __ANGEL_VEC_H__

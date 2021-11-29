#include "math.h"

#ifndef VEC3_H
#define VEC3_H

/*!
 * \class vec3
 * \brief Auxiliary class for vector computations.
 *
 * This class gets handy in detecting collisions and calculating objects
 * movements. No source file is needed since all methods are implemented within
 * this scope.
 */
class vec3
{
public:
    // Data
    float x, y, z;

    // Creators
    vec3( float InX, float InY, float InZ ) : x( InX ), y( InY ), z( InZ )
    {
    }
    vec3( ) : x(0), y(0), z(0)
    {
    }

    // Operator Overloads
    inline bool operator== (const vec3& V2) const
    {
        return (x == V2.x && y == V2.y && z == V2.z);
    }

    inline vec3 operator+ (const vec3& V2) const
    {
        return vec3( x + V2.x,  y + V2.y,  z + V2.z);
    }
    inline vec3 operator- (const vec3& V2) const
    {
        return vec3( x - V2.x,  y - V2.y,  z - V2.z);
    }
    inline vec3 operator- ( ) const
    {
        return vec3(-x, -y, -z);
    }

    inline vec3 operator/ (float S ) const
    {
        float fInv = 1.0f / S;
        return vec3 (x * fInv , y * fInv, z * fInv);
    }
    inline vec3 operator/ (const vec3& V2) const
    {
        return vec3 (x / V2.x,  y / V2.y,  z / V2.z);
    }
    inline vec3 operator* (const vec3& V2) const
    {
        return vec3 (x * V2.x,  y * V2.y,  z * V2.z);
    }
    inline vec3 operator* (float S) const
    {
        return vec3 (x * S,  y * S,  z * S);
    }

    inline void operator+= ( const vec3& V2 )
    {
        x += V2.x;
        y += V2.y;
        z += V2.z;
    }
    inline void operator-= ( const vec3& V2 )
    {
        x -= V2.x;
        y -= V2.y;
        z -= V2.z;
    }

    inline float operator[] ( int i )
    {
        if ( i == 0 ) return x;
        else if ( i == 1 ) return y;
        else return z;
    }

    // Functions
    inline float Dot( const vec3 &V1 ) const
    {
        return V1.x*x + V1.y*y + V1.z*z;
    }

    inline vec3 CrossProduct( const vec3 &V2 ) const
    {
        return vec3(
            y * V2.z  -  z * V2.y,
            z * V2.x  -  x * V2.z,
            x * V2.y  -  y * V2.x 	);
    }

    // These require math.h for the sqrtf function
    float Magnitude( ) const
    {
        return sqrtf( x*x + y*y + z*z );
    }

    float Distance( const vec3 &V1 ) const
    {
        return ( *this - V1 ).Magnitude();
    }

    inline void Normalize()
    {
        float fMag = ( x*x + y*y + z*z );
        if (fMag == 0) {return;}

        float fMult = 1.0f/sqrtf(fMag);
        x *= fMult;
        y *= fMult;
        z *= fMult;
        return;
    }
};


#endif // VEC3_H

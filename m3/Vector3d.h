#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>

class Vector3d
{
public :
        
        float x,y,z;
        
        Vector3d() { this->x = 0.0f; this->y = 0.0f; this->z = 0.0f; }
        Vector3d(float x,float y,float z) { this->x = x; this->y = y; this->z =z;}
};

__forceinline Vector3d  operator- (const Vector3d& vValue) 
{ 
        return Vector3d(-vValue.x,-vValue.y,-vValue.z); 
}
__forceinline Vector3d  operator* (const Vector3d& vValue, float fValue)
{ 
        return Vector3d(vValue.x * fValue,vValue.y * fValue,vValue.z * fValue); 
}
__forceinline Vector3d  operator* (float fValue, const Vector3d& vValue) 
{ 
        return vValue*fValue; 
}
__forceinline Vector3d& operator*=(Vector3d& vValue, float fValue) 
{ 
        vValue.x *= fValue; vValue.y *= fValue; vValue.z *= fValue; 
        return vValue;
}
__forceinline Vector3d  operator/ (const Vector3d& vValue, float fValue)
{ 
        return Vector3d(vValue.x / fValue,vValue.y / fValue,vValue.z / fValue); 
}
__forceinline Vector3d& operator/=(Vector3d& vValue, float fValue) 
{ 
        vValue.x /= fValue; vValue.y /= fValue; vValue.z /= fValue; return vValue;
}

__forceinline Vector3d  operator+ (const Vector3d& vValue_01, const Vector3d& vValue_02)
{
        return Vector3d(vValue_01.x + vValue_02.x,vValue_01.y + vValue_02.y, vValue_01.z + vValue_02.z); 
}
__forceinline Vector3d& operator+=(Vector3d& vValue_01, const Vector3d& vValue_02)
{
        vValue_01.x += vValue_02.x; vValue_01.y += vValue_02.y; vValue_01.z += vValue_02.z;
        return vValue_01;
}
__forceinline Vector3d  operator- (const Vector3d& vValue_01, const Vector3d& vValue_02)
{
        return Vector3d(vValue_01.x - vValue_02.x,vValue_01.y - vValue_02.y, vValue_01.z - vValue_02.z); 
}
__forceinline Vector3d& operator-=(Vector3d& vValue_01, const Vector3d& vValue_02)
{
        vValue_01.x -= vValue_02.x; vValue_01.y -= vValue_02.y; vValue_01.z -= vValue_02.z;
        return vValue_01;
}

__forceinline float Length(const Vector3d& vValue)
{
        return sqrt(pow(vValue.x,2) + pow(vValue.y,2) + pow(vValue.z,2));
}

__forceinline Vector3d CrossProduct(Vector3d& vValue_01, const Vector3d& vValue_02)
{
        return Vector3d( vValue_01.y * vValue_02.z - vValue_01.z * vValue_02.y ,vValue_02.x * vValue_01.z - vValue_02.z * vValue_01.x ,vValue_01.x * vValue_02.y - vValue_01.y * vValue_02.x);
}
__forceinline float DotProduct(Vector3d& vValue_01, const Vector3d& vValue_02)
{
        return vValue_01.x*vValue_02.x + vValue_01.y*vValue_02.y + vValue_01.z*vValue_02.z;
}
        
__forceinline Vector3d Normalize(const Vector3d& vValue)
{
        return vValue / Length(vValue);
}

#endif

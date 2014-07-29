#ifndef GUI_VECTOR_3D_H__
#define GUI_VECTOR_3D_H__

/**
 * A vector in 3D.
 */
struct Vector3d {

	Vector3d() :
		x(0), y(0), z(0) {}

	Vector3d(float x_, float y_, float z_) :
		x(x_), y(y_), z(z_) {};

	float x, y, z;
};

inline Vector3d operator+(const Vector3d& pt3dPoint1, const Vector3d& pt3dPoint2)
{
	Vector3d result;

	result.x = pt3dPoint1.x + pt3dPoint2.x;
	result.y = pt3dPoint1.y + pt3dPoint2.y;
	result.z = pt3dPoint1.z + pt3dPoint2.z;

	return result;
}

inline Vector3d operator-(const Vector3d& pt3dPoint1, const Vector3d& pt3dPoint2)
{
	Vector3d result;

	result.x = pt3dPoint1.x - pt3dPoint2.x;
	result.y = pt3dPoint1.y - pt3dPoint2.y;
	result.z = pt3dPoint1.z - pt3dPoint2.z;

	return result;
}

inline Vector3d operator*(const Vector3d& pt3dPoint, float fScale)
{
	Vector3d result;

	result.x = pt3dPoint.x*fScale;
	result.y = pt3dPoint.y*fScale;
	result.z = pt3dPoint.z*fScale;

	return result;
}

inline Vector3d operator*(float fScale, const Vector3d& pt3dPoint)
{
	Vector3d result;

	result.x = pt3dPoint.x*fScale;
	result.y = pt3dPoint.y*fScale;
	result.z = pt3dPoint.z*fScale;

	return result;
}

inline Vector3d operator/(const Vector3d& pt3dPoint, float fScale)
{
	Vector3d result;

	result.x = pt3dPoint.x/fScale;
	result.y = pt3dPoint.y/fScale;
	result.z = pt3dPoint.z/fScale;
	
	return result;
}

inline Vector3d& operator*=(Vector3d& pt3dPoint, float fScale)
{
	pt3dPoint.x *= fScale;
	pt3dPoint.y *= fScale;
	pt3dPoint.z *= fScale;

	return pt3dPoint;
}

inline Vector3d& operator/=(Vector3d& pt3dPoint, float fScale)
{
	pt3dPoint.x /= fScale;
	pt3dPoint.y /= fScale;
	pt3dPoint.z /= fScale;

	return pt3dPoint;
}

inline Vector3d& operator+=(Vector3d& pt3dPoint1, const Vector3d& pt3dPoint2)
{
	pt3dPoint1.x += pt3dPoint2.x;
	pt3dPoint1.y += pt3dPoint2.y;
	pt3dPoint1.z += pt3dPoint2.z;

	return pt3dPoint1;
}

inline Vector3d& operator-=(Vector3d& pt3dPoint1, const Vector3d& pt3dPoint2)
{
	pt3dPoint1.x -= pt3dPoint2.x;
	pt3dPoint1.y -= pt3dPoint2.y;
	pt3dPoint1.z -= pt3dPoint2.z;
	
	return pt3dPoint1;
}

#endif // GUI_VECTOR_3D_H__


#ifndef GUI_POINT_3D_H__
#define GUI_POINT_3D_H__


/**
 * A point in 3D.
 */
struct Point3d {

	Point3d() :
		x(0), y(0), z(0) {}

	Point3d(float x_, float y_, float z_) :
		x(x_), y(y_), z(z_) {};

	float x, y, z;
};

inline Point3d operator+(const Point3d& pt3dPoint1, const Point3d& pt3dPoint2)
{
	Point3d result;

	result.x = pt3dPoint1.x + pt3dPoint2.x;
	result.y = pt3dPoint1.y + pt3dPoint2.y;
	result.z = pt3dPoint1.z + pt3dPoint2.z;

	return result;
}

inline Point3d operator-(const Point3d& pt3dPoint1, const Point3d& pt3dPoint2)
{
	Point3d result;

	result.x = pt3dPoint1.x - pt3dPoint2.x;
	result.y = pt3dPoint1.y - pt3dPoint2.y;
	result.z = pt3dPoint1.z - pt3dPoint2.z;

	return result;
}

inline Point3d operator*(const Point3d& pt3dPoint, float fScale)
{
	Point3d result;

	result.x = pt3dPoint.x*fScale;
	result.y = pt3dPoint.y*fScale;
	result.z = pt3dPoint.z*fScale;

	return result;
}

inline Point3d operator*(float fScale, const Point3d& pt3dPoint)
{
	Point3d result;

	result.x = pt3dPoint.x*fScale;
	result.y = pt3dPoint.y*fScale;
	result.z = pt3dPoint.z*fScale;

	return result;
}

inline Point3d operator/(const Point3d& pt3dPoint, float fScale)
{
	Point3d result;

	result.x = pt3dPoint.x/fScale;
	result.y = pt3dPoint.y/fScale;
	result.z = pt3dPoint.z/fScale;
	
	return result;
}

inline Point3d& operator*=(Point3d& pt3dPoint, float fScale)
{
	pt3dPoint.x *= fScale;
	pt3dPoint.y *= fScale;
	pt3dPoint.z *= fScale;

	return pt3dPoint;
}

inline Point3d& operator/=(Point3d& pt3dPoint, float fScale)
{
	pt3dPoint.x /= fScale;
	pt3dPoint.y /= fScale;
	pt3dPoint.z /= fScale;

	return pt3dPoint;
}

inline Point3d& operator+=(Point3d& pt3dPoint1, const Point3d& pt3dPoint2)
{
	pt3dPoint1.x += pt3dPoint2.x;
	pt3dPoint1.y += pt3dPoint2.y;
	pt3dPoint1.z += pt3dPoint2.z;

	return pt3dPoint1;
}

inline Point3d& operator-=(Point3d& pt3dPoint1, const Point3d& pt3dPoint2)
{
	pt3dPoint1.x -= pt3dPoint2.x;
	pt3dPoint1.y -= pt3dPoint2.y;
	pt3dPoint1.z -= pt3dPoint2.z;
	
	return pt3dPoint1;
}

#endif // GUI_POINT_3D_H__


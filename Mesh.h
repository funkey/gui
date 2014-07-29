#ifndef GUI_MESH_H__
#define GUI_MESH_H__

#include <pipeline/Data.h>
#include <util/foreach.h>
#include "Point3d.h"
#include "Vector3d.h"
#include "Triangle.h"

/**
 * A 3D mesh as a list of triangles.
 */
class Mesh : public pipeline::Data {

public:

	Mesh() :
		_updateBoundingBox(true) {}

	void setNumVertices(unsigned int numVertices)   { _vertices.resize(numVertices); _normals.resize(numVertices); }
	void setNumTriangles(unsigned int numTriangles) { _triangles.resize(numTriangles); }

	void setVertex(unsigned int index, const Point3d&  vertex) { _vertices[index] = vertex; _updateBoundingBox = true; }
	void setNormal(unsigned int index, const Vector3d& normal) { _normals[index]  = normal; }

	void setTriangle(
			unsigned int index,
			unsigned int v1,
			unsigned int v2,
			unsigned int v3) {

		_triangles[index] = Triangle(v1, v2, v3);
	}

	const Point3d&  getVertex(unsigned int index) const { return _vertices[index]; }
	const Vector3d& getNormal(unsigned int index) const { return _normals[index];  }
	const Triangle& getTriangle(unsigned int index) const { return _triangles[index]; }


	Point3d&  getVertex(unsigned int index) { return _vertices[index]; }
	Vector3d& getNormal(unsigned int index) { return _normals[index];  }
	Triangle& getTriangle(unsigned int index) { return _triangles[index]; }

	const std::vector<Point3d>&  getVertices()  const { return _vertices; }
	const std::vector<Vector3d>& getNormals()   const { return _normals; }
	const std::vector<Triangle>& getTriangles() const { return _triangles; }

	float minX() { if (_updateBoundingBox) updateBoundingBox(); return _minX; }
	float minY() { if (_updateBoundingBox) updateBoundingBox(); return _minY; }
	float minZ() { if (_updateBoundingBox) updateBoundingBox(); return _minZ; }
	float maxX() { if (_updateBoundingBox) updateBoundingBox(); return _maxX; }
	float maxY() { if (_updateBoundingBox) updateBoundingBox(); return _maxY; }
	float maxZ() { if (_updateBoundingBox) updateBoundingBox(); return _maxZ; }

private:

	void updateBoundingBox() {

		_minX = _minY = _minZ = std::numeric_limits<float>::max();
		_maxX = _maxY = _maxZ = std::numeric_limits<float>::min();

		foreach (Point3d& p, _vertices) {

			_minX = std::min(p.x, _minX);
			_minY = std::min(p.y, _minY);
			_minZ = std::min(p.z, _minZ);
			_maxX = std::max(p.x, _maxX);
			_maxY = std::max(p.y, _maxY);
			_maxZ = std::max(p.z, _maxZ);
		}
	}

	// the vertices of the mesh
	std::vector<Point3d>  _vertices;

	// the normals, one for each vertex
	std::vector<Vector3d> _normals;

	// list of triangles that make up the mesh
	std::vector<Triangle> _triangles;

	bool _updateBoundingBox;

	float _minX, _maxX;
	float _minY, _maxY;
	float _minZ, _maxZ;
};

#endif // GUI_MESH_H__


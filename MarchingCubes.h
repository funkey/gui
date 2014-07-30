#ifndef GUI_MARCHING_CUBES_H__
#define GUI_MARCHING_CUBES_H__
// Based on "CIsoSurface" by Raghavendra Chandrashekara:
//
// File Name: CIsoSurface.h
// Last Modified: 5/8/2000
// Author: Raghavendra Chandrashekara (basesd on source code
// provided by Paul Bourke and Cory Gene Bloyd)
// Email: rc99@doc.ic.ac.uk, rchandrashekara@hotmail.com
//
// Description: This is the interface file for the CIsoSurface class.
// CIsoSurface can be used to construct an isosurface from a scalar
// field.

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Point3d.h"
#include "Vector3d.h"
#include "Mesh.h"

/**
 * A point in 3D with an id.
 */
struct Point3dId {

	unsigned int newId;
	float x, y, z;

	// default constructor
	Point3dId() {}

	// construction from Point3d
	Point3dId(const Point3d& p) :
		newId(0), x(p.x), y(p.y), z(p.z) {};

	// conversion to regular Point3d
	operator Point3d () {
		return Point3d(x, y, z);
	}
};

/**
 * A map of ids to points.
 */
typedef std::map<unsigned int, Point3dId> Id2Point3dId;

/**
 * Triples of points that form a triangle.
 */
struct TriangleId {
	unsigned int pointId[3];
};

/**
 * A vector of triangles.
 */
typedef std::vector<TriangleId> TriangleVector;

/**
 * Generic marching cubes implementation for volumes that implement:
 *
 *   // the scalar type of the data
 *   Volume::value_type
 *
 *   // the extends of the volume
 *   float Volume::width()
 *   float Volume::height()
 *   float Volume::depth()
 *
 *   // access to the data
 *   value_type Volume::operator(float x, float y, float z)
 */
template <typename Volume>
class MarchingCubes {

	typedef typename Volume::value_type value_type;

public:

	/**
	 * Functor to find surfaces of components with a gray value above the given 
	 * threshold.
	 */
	struct AcceptAbove {

		AcceptAbove(value_type threshold_) :
			threshold(threshold_) {}

		bool operator()(value_type value) const {

			return value > threshold;
		}

		value_type threshold;
	};

	/**
	 * Functor to find surfaces of components with a gray value that is exactly 
	 * the given value.
	 */
	struct AcceptExactly {

		AcceptExactly(value_type reference_) :
			reference(reference_) {}

		bool operator()(value_type value) const {

			return value == reference;
		}

		value_type reference;
	};

	// Constructor and destructor.
	MarchingCubes();
	~MarchingCubes();
	
	/**
	 * Generate an iso-surface mesh from a volume. For that, the volume is 
	 * divided into cells. At the corners of a cell, the gray value is sampled, 
	 * compared to the provided isoLevel, and replaced by a mesh approximating 
	 * the iso surface.
	 *
	 * @param volume
	 *              The volume.
	 * @param cellSizeX
	 *              The size of a cell in x to sample.
	 * @param cellSizeY
	 *              The size of a cell in y to sample.
	 * @param cellSizeZ
	 *              The size of a cell in z to sample.
	 */
	template <typename InteriorTest>
	boost::shared_ptr<Mesh> generateSurface(
			const Volume& volume,
			const InteriorTest& interiorTest,
			float cellSizeX,
			float cellSizeY,
			float cellSizeZ);

	/**
	 * Returns true if a valid surface has been generated.
	 */
	bool isSurfaceValid();

	/**
	 * Deletes the isosurface.
	 */
	void deleteSurface();

	/**
	 * Returns the length, width, and height of the volume in which the
	 * isosurface is enclosed in.  Returns -1 if the surface is not
	 * valid.
	 */
	int getVolumeLengths(float& sizeX, float& sizeY, float& sizeZ);

private:

	// get the value in the volume corresponding to the given location in cell 
	// coordinates
	inline value_type getValue(const Volume& volume, unsigned int x, unsigned int y, unsigned int z) {

		return volume(x*_cellSizeX, y*_cellSizeY, z*_cellSizeZ);
	}

	// Returns the edge Id.
	unsigned int GetEdgeId(unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo);

	// Returns the vertex Id.
	unsigned int GetVertexId(unsigned int nX, unsigned int nY, unsigned int nZ);

	// Calculates the intersection point of the isosurface with an
	// edge.
	Point3dId CalculateIntersection(const Volume& volume, unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo);

	// Interpolates between two grid points to produce the point at which
	// the isosurface intersects an edge.
	Point3dId Interpolate(float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, value_type tVal1, value_type tVal2);
 
	// Renames vertices and triangles so that they can be accessed more
	// efficiently.
	void RenameVerticesAndTriangles();

	// Calculates the normals.
	void CalculateNormals();

	// The number of vertices which make up the isosurface.
	unsigned int _nVertices;

	// The number of triangles which make up the isosurface.
	unsigned int _nTriangles;

	// The number of normals.
	unsigned int _nNormals;

	// the mesh that represents the surface
	boost::shared_ptr<Mesh> _mesh;

	// List of Point3ds which form the isosurface.
	Id2Point3dId _i2pt3idVertices;

	// List of Triangles which form the triangulation of the isosurface.
	TriangleVector _trivecTriangles;

	// No. of cells in x, y, and z directions.
	unsigned int _nCellsX, _nCellsY, _nCellsZ;

	// Cell length in x, y, and z directions.
	float _cellSizeX, _cellSizeY, _cellSizeZ;

	// The isosurface value.
	value_type _tIsoLevel;

	// Indicates whether a valid surface is present.
	bool _bValidSurface;

	// Lookup tables used in the construction of the isosurface.
	static const unsigned int _edgeTable[256];
	static const unsigned int _triTable[256][16];

	static const unsigned int None = -1;
};


template <typename Volume>
template <typename InteriorTest>
boost::shared_ptr<Mesh>
MarchingCubes<Volume>::generateSurface(
		const Volume& volume,
		const InteriorTest& interiorTest,
		float cellSizeX,
		float cellSizeY,
		float cellSizeZ)
{
	if (_bValidSurface)
		deleteSurface();

	_mesh = boost::make_shared<Mesh>();

	float width  = volume.width();
	float height = volume.height();
	float depth  = volume.depth();

	_nCellsX = ceil(width /cellSizeX) - 1;
	_nCellsY = ceil(height/cellSizeY) - 1;
	_nCellsZ = ceil(depth /cellSizeZ) - 1;
	_cellSizeX = cellSizeX;
	_cellSizeY = cellSizeY;
	_cellSizeZ = cellSizeZ;

	// Generate isosurface.
	for (unsigned int z = 0; z < _nCellsZ; z++)
		for (unsigned int y = 0; y < _nCellsY; y++)
			for (unsigned int x = 0; x < _nCellsX; x++) {
				// Calculate table lookup index from those
				// vertices which are below the isolevel.
				unsigned int tableIndex = 0;
				if (!interiorTest(getValue(volume, x, y, z)))
					tableIndex |= 1;
				if (!interiorTest(getValue(volume, x, y+1, z)))
					tableIndex |= 2;
				if (!interiorTest(getValue(volume, x+1, y+1, z)))
					tableIndex |= 4;
				if (!interiorTest(getValue(volume, x+1, y, z)))
					tableIndex |= 8;
				if (!interiorTest(getValue(volume, x, y, z+1)))
					tableIndex |= 16;
				if (!interiorTest(getValue(volume, x, y+1, z+1)))
					tableIndex |= 32;
				if (!interiorTest(getValue(volume, x+1, y+1, z+1)))
					tableIndex |= 64;
				if (!interiorTest(getValue(volume, x+1, y, z+1)))
					tableIndex |= 128;

				// Now create a triangulation of the isosurface in this
				// cell.
				if (_edgeTable[tableIndex] != 0) {
					if (_edgeTable[tableIndex] & 8) {
						Point3dId pt = CalculateIntersection(volume, x, y, z, 3);
						unsigned int id = GetEdgeId(x, y, z, 3);
						_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
					}
					if (_edgeTable[tableIndex] & 1) {
						Point3dId pt = CalculateIntersection(volume, x, y, z, 0);
						unsigned int id = GetEdgeId(x, y, z, 0);
						_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
					}
					if (_edgeTable[tableIndex] & 256) {
						Point3dId pt = CalculateIntersection(volume, x, y, z, 8);
						unsigned int id = GetEdgeId(x, y, z, 8);
						_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
					}
					
					if (x == _nCellsX - 1) {
						if (_edgeTable[tableIndex] & 4) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 2);
							unsigned int id = GetEdgeId(x, y, z, 2);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
						if (_edgeTable[tableIndex] & 2048) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 11);
							unsigned int id = GetEdgeId(x, y, z, 11);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					}
					if (y == _nCellsY - 1) {
						if (_edgeTable[tableIndex] & 2) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 1);
							unsigned int id = GetEdgeId(x, y, z, 1);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
						if (_edgeTable[tableIndex] & 512) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 9);
							unsigned int id = GetEdgeId(x, y, z, 9);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					}
					if (z == _nCellsZ - 1) {
						if (_edgeTable[tableIndex] & 16) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 4);
							unsigned int id = GetEdgeId(x, y, z, 4);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
						if (_edgeTable[tableIndex] & 128) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 7);
							unsigned int id = GetEdgeId(x, y, z, 7);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					}
					if ((x==_nCellsX - 1) && (y==_nCellsY - 1))
						if (_edgeTable[tableIndex] & 1024) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 10);
							unsigned int id = GetEdgeId(x, y, z, 10);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					if ((x==_nCellsX - 1) && (z==_nCellsZ - 1))
						if (_edgeTable[tableIndex] & 64) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 6);
							unsigned int id = GetEdgeId(x, y, z, 6);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					if ((y==_nCellsY - 1) && (z==_nCellsZ - 1))
						if (_edgeTable[tableIndex] & 32) {
							Point3dId pt = CalculateIntersection(volume, x, y, z, 5);
							unsigned int id = GetEdgeId(x, y, z, 5);
							_i2pt3idVertices.insert(Id2Point3dId::value_type(id, pt));
						}
					
					for (unsigned int i = 0; _triTable[tableIndex][i] != None; i += 3) {
						TriangleId triangle;
						unsigned int pointId0, pointId1, pointId2;
						pointId0 = GetEdgeId(x, y, z, _triTable[tableIndex][i]);
						pointId1 = GetEdgeId(x, y, z, _triTable[tableIndex][i+1]);
						pointId2 = GetEdgeId(x, y, z, _triTable[tableIndex][i+2]);
						triangle.pointId[0] = pointId0;
						triangle.pointId[1] = pointId1;
						triangle.pointId[2] = pointId2;
						_trivecTriangles.push_back(triangle);
					}
				}
			}
	
	RenameVerticesAndTriangles();
	CalculateNormals();
	_bValidSurface = true;

	return _mesh;
}

#endif // GUI_MARCHING_CUBES_H__


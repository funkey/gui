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
	boost::shared_ptr<Mesh> generateSurface(
			const Volume& volume,
			value_type isoLevel,
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
};

#endif // GUI_MARCHING_CUBES_H__


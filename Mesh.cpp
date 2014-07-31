#include "Mesh.h"

Mesh
Mesh::createSubmesh(const std::vector<unsigned int>& triangles) {

	Mesh submesh;

	submesh._vertices = _vertices;
	submesh._normals  = _normals;

	submesh._triangles.reserve(triangles.size());

	foreach (unsigned int triangle, triangles)
		submesh._triangles.push_back(_triangles[triangle]);

	submesh.strip();

	return submesh;
}

void
Mesh::strip() {

	std::vector<unsigned int> vertexTag(getNumVertices(), std::numeric_limits<unsigned int>::max());

	// tag all used vertices with 0

	foreach (const Triangle& triangle, _triangles) {

		vertexTag[triangle.v0] = 0;
		vertexTag[triangle.v1] = 0;
		vertexTag[triangle.v2] = 0;
	}

	// create a vector of the used vertices (and normals)

	std::vector<Point3d>  usedVertices;
	std::vector<Vector3d> usedNormals;

	unsigned int newIndex = 0;
	for (unsigned int i = 0; i < getNumVertices(); i++) {

		if (vertexTag[i] == 0) { // vertex is used

			// keep the vertex

			usedVertices.push_back(_vertices[i]);
			usedNormals.push_back(_normals[i]);

			// tag the vertex with the new index in the reduced vector

			vertexTag[i] = newIndex;
			newIndex++;
		}
	}

	// use the reduced vector of vertices (and normals)

	_vertices = usedVertices;
	_normals  = usedNormals;

	// update the indices in the triangles

	foreach (Triangle& triangle, _triangles) {

		triangle.v0 = vertexTag[triangle.v0];
		triangle.v1 = vertexTag[triangle.v1];
		triangle.v2 = vertexTag[triangle.v2];
	}
}

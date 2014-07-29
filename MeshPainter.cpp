#include <util/Logger.h>
#include "MeshPainter.h"

logger::LogChannel meshpainterlog("meshpainterlog", "[MeshPainter] ");

void
MeshPainter::setMesh(boost::shared_ptr<Mesh> mesh) {

	if (!mesh)
		return;

	_mesh = mesh;

	util::rect<double> size = util::rect<double>(mesh->minX(), mesh->minY(), mesh->maxX(), mesh->maxY());

	LOG_ALL(meshpainterlog) << "setting size to " << size << std::endl;

	setSize(size);

	updateRecording();
}

void
MeshPainter::updateRecording() {

	// make sure OpenGl operations are save
	gui::OpenGl::Guard guard;

	startRecording();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glDisable(GL_CULL_FACE);
	glColor3f(1.0, 0.2, 0.4);
	GLfloat lightpos[] = {0.5, 1.0, 1.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	const std::vector<Triangle>& triangles = _mesh->getTriangles();

	glBegin(GL_TRIANGLES);
	foreach (const Triangle& triangle, triangles) {

		const Point3d&  v0 = _mesh->getVertex(triangle.v0);
		const Point3d&  v1 = _mesh->getVertex(triangle.v1);
		const Point3d&  v2 = _mesh->getVertex(triangle.v2);
		const Vector3d& n0 = _mesh->getNormal(triangle.v0);
		const Vector3d& n1 = _mesh->getNormal(triangle.v1);
		const Vector3d& n2 = _mesh->getNormal(triangle.v2);

		glNormal3f(n0.x, n0.y, n0.z); glVertex3f(v0.x, v0.y, v0.z);
		glNormal3f(n1.x, n1.y, n1.z); glVertex3f(v1.x, v1.y, v1.z);
		glNormal3f(n2.x, n2.y, n2.z); glVertex3f(v2.x, v2.y, v2.z);
	}
	glEnd();

	stopRecording();
}

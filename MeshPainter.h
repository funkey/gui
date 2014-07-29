#ifndef GUI_MESH_PAINTER_H__
#define GUI_MESH_PAINTER_H__

#include "RecordablePainter.h"
#include "Mesh.h"

class MeshPainter : public gui::RecordablePainter {

public:

	void setMesh(boost::shared_ptr<Mesh> mesh);

private:

	void updateRecording();

	boost::shared_ptr<Mesh> _mesh;
};

#endif // GUI_MESH_PAINTER_H__


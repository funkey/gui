#ifndef GUI_MESH_PAINTER_H__
#define GUI_MESH_PAINTER_H__

#include "RecordablePainter.h"
#include "Meshes.h"

class MeshPainter : public gui::RecordablePainter {

public:

	void setMeshes(boost::shared_ptr<Meshes> meshes);

private:

	void updateRecording();

	boost::shared_ptr<Meshes> _meshes;
};

#endif // GUI_MESH_PAINTER_H__


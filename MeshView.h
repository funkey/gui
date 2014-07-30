#ifndef GUI_MESH_VIEW_H__
#define GUI_MESH_VIEW_H__

#include <pipeline/SimpleProcessNode.h>
#include "MeshPainter.h"

class MeshView : public pipeline::SimpleProcessNode<> {

public:

	MeshView();

private:

	void updateOutputs();

	pipeline::Input<Meshes>       _meshes;
	pipeline::Output<MeshPainter> _painter;
};

#endif // GUI_MESH_VIEW_H__


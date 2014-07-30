#include "MeshView.h"

MeshView::MeshView() {

	registerInput(_meshes, "meshes");
	registerOutput(_painter, "painter");
}

void
MeshView::updateOutputs() {

	if (!_painter)
		_painter = new MeshPainter();

	_painter->setMeshes(_meshes);
}

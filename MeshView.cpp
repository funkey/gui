#include "MeshView.h"

MeshView::MeshView() {

	registerInput(_mesh, "mesh");
	registerOutput(_painter, "painter");
}

void
MeshView::updateOutputs() {

	if (!_painter)
		_painter = new MeshPainter();

	_painter->setMesh(_mesh);
}

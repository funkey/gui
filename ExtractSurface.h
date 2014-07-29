#ifndef GUI_EXTRACT_SURFACE_H__
#define GUI_EXTRACT_SURFACE_H__

#include <pipeline/SimpleProcessNode.h>
#include <imageprocessing/ImageStack.h>
#include "Mesh.h"

class ExtractSurface : public pipeline::SimpleProcessNode<> {

public:

	ExtractSurface();

private:

	void updateOutputs();

	pipeline::Input<ImageStack> _stack;
	pipeline::Output<Mesh>      _surface;

};

#endif // GUI_EXTRACT_SURFACE_H__


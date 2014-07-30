#ifndef GUI_EXTRACT_SURFACES_H__
#define GUI_EXTRACT_SURFACES_H__

#include <pipeline/SimpleProcessNode.h>
#include <imageprocessing/ImageStack.h>
#include "Meshes.h"

/**
 * Extracts a set of meshes, one for each gray-level in the image stack.  
 * Intended for image stacks that contain multiple components with different 
 * ids.
 */
class ExtractSurfaces : public pipeline::SimpleProcessNode<> {

public:

	ExtractSurfaces();

private:

	void updateOutputs();

	pipeline::Input<ImageStack> _stack;
	pipeline::Output<Meshes>    _surfaces;

};

#endif // GUI_EXTRACT_SURFACE_H__



#include <imageprocessing/ImageStackVolumeAdaptor.h>
#include "MarchingCubes.h"
#include "ExtractSurface.h"

ExtractSurface::ExtractSurface() {

	registerInput(_stack, "stack");
	registerOutput(_surface, "surface");
}

void
ExtractSurface::updateOutputs() {

	// wrap the input stack into a volume adaptor
	ImageStackVolumeAdaptor volume(*_stack);

	// create a marching cubes instance
	MarchingCubes<ImageStackVolumeAdaptor> marchingCubes;

	_surface = marchingCubes.generateSurface(
			volume,
			MarchingCubes<ImageStackVolumeAdaptor>::AcceptAbove(0.5),
			10.0,
			10.0,
			10.0);
}

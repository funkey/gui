#include <boost/timer/timer.hpp>
#include <imageprocessing/ImageStackVolumeAdaptor.h>
#include "MarchingCubes.h"
#include "ExtractSurfaces.h"

ExtractSurfaces::ExtractSurfaces() {

	registerInput(_stack, "stack");
	registerOutput(_surfaces, "surfaces");
}

void
ExtractSurfaces::updateOutputs() {

	// wrap the input stack into a volume adaptor
	ImageStackVolumeAdaptor volume(*_stack);

	// create a marching cubes instance
	MarchingCubes<ImageStackVolumeAdaptor> marchingCubes;

	// get all ids in the image stack
	std::set<unsigned int> ids;

	foreach (boost::shared_ptr<Image> image, *_stack)
		foreach (Image::value_type value, *image)
			if (value != 0)
				ids.insert(value);

	_surfaces = new Meshes;

	foreach (unsigned int id, ids) {

		_surfaces->add(
				id,
				marchingCubes.generateSurface(
					volume,
					MarchingCubes<ImageStackVolumeAdaptor>::AcceptExactly(id),
					10.0,
					10.0,
					10.0));
	}
}


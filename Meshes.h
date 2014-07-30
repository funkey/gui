#ifndef GUI_MESHES_H__
#define GUI_MESHES_H__

#include <pipeline/Data.h>
#include "Mesh.h"

class Meshes : public pipeline::Data {

public:

	Meshes() :
			_updateBoundingBox(true) {}

	void add(unsigned int id, boost::shared_ptr<Mesh> mesh) {

		_meshes[id] = mesh;
		_ids.push_back(id);

		_updateBoundingBox = true;
	}

	boost::shared_ptr<Mesh> get(unsigned int id) {

		if (_meshes.count(id))
			return _meshes[id];

		return boost::shared_ptr<Mesh>();
	}

	const std::vector<unsigned int>& getMeshIds() const {

		return _ids;
	}

	float minX() { if (_updateBoundingBox) updateBoundingBox(); return _minX; }
	float minY() { if (_updateBoundingBox) updateBoundingBox(); return _minY; }
	float minZ() { if (_updateBoundingBox) updateBoundingBox(); return _minZ; }
	float maxX() { if (_updateBoundingBox) updateBoundingBox(); return _maxX; }
	float maxY() { if (_updateBoundingBox) updateBoundingBox(); return _maxY; }
	float maxZ() { if (_updateBoundingBox) updateBoundingBox(); return _maxZ; }

private:

	void updateBoundingBox() {

		_minX = _minY = _minZ = std::numeric_limits<float>::max();
		_maxX = _maxY = _maxZ = std::numeric_limits<float>::min();

		foreach (unsigned int id, _ids) {

			_minX = std::min(_meshes[id]->minX(), _minX);
			_minY = std::min(_meshes[id]->minY(), _minY);
			_minZ = std::min(_meshes[id]->minZ(), _minZ);
			_maxX = std::max(_meshes[id]->maxX(), _maxX);
			_maxY = std::max(_meshes[id]->maxY(), _maxY);
			_maxZ = std::max(_meshes[id]->maxZ(), _maxZ);
		}

		_updateBoundingBox = false;
	}

	std::map<unsigned int, boost::shared_ptr<Mesh> > _meshes;

	std::vector<unsigned int> _ids;

	bool _updateBoundingBox;

	float _minX, _maxX;
	float _minY, _maxY;
	float _minZ, _maxZ;
};

#endif // GUI_MESHES_H__


#include <util/typename.h>
#include "ContainerPainter.h"

namespace gui {

static logger::LogChannel containerpainterlog("containerpainterlog", "[ContainerPainter] ");

void
ContainerPainter::draw(
			const util::rect<double>&  roi,
			const util::point<double>& resolution) {

	LOG_ALL(containerpainterlog) << "redrawing..." << std::endl;

	// get a read-lock
	boost::shared_lock<boost::shared_mutex> lock(_paintersMutex);

	LOG_ALL(containerpainterlog) << "got a read-lock" << std::endl;

	// draw each painter at its offset position
	for (painter_offsets::iterator i = _painterOffsets.begin();
	     i != _painterOffsets.end(); i++) {

		const boost::shared_ptr<Painter>& painter = i->first;
		const util::point<double>&        offset  = i->second;
		const util::rect<double>&         painterSize = painter->getSize();

		LOG_ALL(containerpainterlog) << "drawing painter " << typeName(*painter) << " at " << offset << std::endl;

		// draw the painter only if it is visible
		if ((painterSize + offset).intersects(roi)) {

			glTranslated(offset.x, offset.y, 0);

			painter->draw(roi - offset, resolution);

			glTranslated(-offset.x, -offset.y, 0);

		} else {

			LOG_ALL(containerpainterlog) << "nope, this one is currently not visible" << std::endl;
		}
	}

	LOG_ALL(containerpainterlog) << "done redrawing" << std::endl;
}

void
ContainerPainter::add(boost::shared_ptr<Painter> painter, const util::point<double>& offset) {

	{
		// get a write lock
		boost::unique_lock<boost::shared_mutex> lock(_paintersMutex);

		LOG_ALL(containerpainterlog) << "new painter: "
		                             << typeName(*painter) << std::endl;

		// store it in list
		_painterOffsets.push_back(painter_offset(painter, offset));
	}

	// update size of this painter
	updateSize();
}

void
ContainerPainter::remove(boost::shared_ptr<Painter> painter) {

	LOG_ALL(containerpainterlog) << "removing painter " << typeName(*painter) << std::endl;

	{
		// get a write lock
		boost::unique_lock<boost::shared_mutex> lock(_paintersMutex);

		// remove painter from list
		for (painter_offsets::iterator i = _painterOffsets.begin(); i != _painterOffsets.end(); i++) {

			if ((*i).first == painter) {

				_painterOffsets.erase(i);

				LOG_ALL(containerpainterlog) << "removed." << std::endl;

				break;
			}
		}
	}

	// update size of this painter
	updateSize();
}

void
ContainerPainter::clear() {

	{
		// get a write lock
		boost::unique_lock<boost::shared_mutex> lock(_paintersMutex);

		// remove from list
		_painterOffsets.clear();
	}

	// update size of this painter
	updateSize();
}

void
ContainerPainter::updateSize() {

	// get a read lock
	boost::shared_lock<boost::shared_mutex> lock(_paintersMutex);

	LOG_ALL(containerpainterlog) << "computing size..." << std::endl;

	util::rect<double> size(0.0, 0.0, 0.0, 0.0);

	if (_painterOffsets.size() == 0) {

		setSize(size);
		return;
	}

	LOG_ALL(containerpainterlog) << "I have at least one painter" << std::endl;

	// initialise size to size of first painter + offset of first painter
	size = _painterOffsets.begin()->first->getSize() + _painterOffsets.begin()->second;

	LOG_ALL(containerpainterlog) << "size of the first painter is " << size << std::endl;

	// get the min and max values over remaining painters
	for (painter_offsets::iterator i = _painterOffsets.begin() + 1; i != _painterOffsets.end(); i++) {

		const util::rect<double>&  painterSize = i->first->getSize();
		const util::point<double>& offset      = i->second;

		// don't consider empty painters
		if (painterSize.area() == 0)
			continue;

		size.maxX  = std::max(size.maxX, (painterSize + offset).maxX);
		size.minX  = std::min(size.minX, (painterSize + offset).minX);
		size.maxY  = std::max(size.maxY, (painterSize + offset).maxY);
		size.minY  = std::min(size.minY, (painterSize + offset).minY);
	}

	LOG_ALL(containerpainterlog) << "my size is " << size << std::endl;

	setSize(size);
}

void
ContainerPainter::setOffsets(const std::vector<util::point<double> >& offsets) {

	{
		// get a write lock
		boost::unique_lock<boost::shared_mutex> lock(_paintersMutex);

		if (offsets.size() != _painterOffsets.size()) {

			LOG_ERROR(containerpainterlog) << "number of offsets given (" << offsets.size()
										   << ") does not match number of painters in the container ("
										   << _painterOffsets.size() << ")" << std::endl;

			return;
		}

		for (unsigned int i = 0; i < _painterOffsets.size(); i++)
			_painterOffsets[i].second = offsets[i];
	}

	updateSize();
}

ContainerPainter&
ContainerPainter::operator=(const ContainerPainter& other) {

	LOG_DEBUG(containerpainterlog) << "assigning new content" << std::endl;

	setSize(other.getSize());
	_painterOffsets = other._painterOffsets;
}

} // namespace gui

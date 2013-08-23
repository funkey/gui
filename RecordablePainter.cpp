#include <util/Logger.h>
#include "RecordablePainter.h"

namespace gui {

static logger::LogChannel recordablepainterlog("recordablepainterlog", "[RecordablePainter] ");


RecordablePainter::RecordablePainter(std::string name) :
	Painter(name),
	_displayList(0) /* glGenLists will never create list with number 0 */ {

}

RecordablePainter::~RecordablePainter() {

	if (glIsList(_displayList)) {

		glCheck(glDeleteLists(_displayList, 1));

		LOG_ALL(recordablepainterlog) << "destructed display list: " << _displayList << std::endl;
	}
}

bool
RecordablePainter::draw(
		const util::rect<double>&  /*roi*/,
		const util::point<double>& /*res*/) {

	LOG_ALL(recordablepainterlog) << getName() << " redrawing display list " << _displayList << std::endl;

	if (!glIsList(_displayList)) {

		LOG_ALL(recordablepainterlog) << getName() << "trying to draw without initialisation of display list" << std::endl;

		return false;
	}

	LOG_ALL(recordablepainterlog) << getName() << "redrawing" << std::endl;

	glCallList(_displayList);

	// display lists are static -- no need to redraw again
	return false;
}

void
RecordablePainter::startRecording() {

	// create a new display list, if needed
	if (!glIsList(_displayList)) {

		_displayList = glGenLists(1);

		LOG_ALL(recordablepainterlog) << getName() << "created a new display list: " << _displayList << std::endl;

		if (!glIsList(_displayList))
			throw OpenGlError() << error_message("Couldn't create display list");
	}

	glCheck(glNewList(_displayList, GL_COMPILE));
}

void
RecordablePainter::stopRecording() {

	glCheck(glEndList());
}

} // namespace gui

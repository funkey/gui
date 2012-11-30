#ifndef WINDOW_MODE_H__
#define WINDOW_MODE_H__

#include <util/point.hpp>

using namespace util;

namespace gui {

struct WindowMode {

	WindowMode(
			int  width  = 320,
			int  height = 240,
			int  x = 0,
			int  y = 0) :
		size(width, height),
		position(x, y),
		mapped(true) {}

	WindowMode(
			const point<int>& size_,
			const point<int>& position_ = point<int>(0, 0)) :
		size(size_),
		position(position_),
		mapped(true) {}

	point<int> size;

	point<int> position;

	bool mapped;
};

} // namespace gui

#endif // WINDOW_MODE_H__


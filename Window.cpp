#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include <util/Logger.h>
#include <util/helpers.hpp>
#include <gui/OpenGl.h>
#include <gui/ContextSettings.h>
#include <gui/Keys.h>
#include "Window.h"

using std::cout;
using std::endl;
using namespace boost::gil;

using namespace logger;

namespace gui {

LogChannel winlog("winlog", "[Window] ");

Window::Window(
		string caption,
		const WindowMode& mode) :
	WindowType(caption, mode),
	_region(0, 0, mode.size.x, mode.size.y),
	_resolution(mode.size.x, mode.size.y),
	_saveFrameRequest(false),
	_frameNumber(0),
	_frameBuffer(0),
	_clear_r(0.5),
	_clear_g(0.5),
	_clear_b(0.5) {

	registerInput(_painter, "painter");

	// register backward signals
	_painter.registerBackwardSlot(_resize);
	_painter.registerBackwardSlot(_keyDown);
	_painter.registerBackwardSlot(_keyUp);
	_painter.registerBackwardSlot(_mouseMove);
	_painter.registerBackwardSlot(_mouseDown);
	_painter.registerBackwardSlot(_mouseUp);

	// register backward callbacks
	_painter.registerBackwardCallback(&Window::onInputAdded, this);
	_painter.registerBackwardCallback(&Window::onModified, this);
	_painter.registerBackwardCallback(&Window::onSizeChanged, this);

	// initiate first redraw
	setDirty();
}

Window::~Window() {

	LOG_DEBUG(winlog) << "[" << getCaption() << "] destructing..." << endl;

	deleteFrameBuffer();

	LOG_DEBUG(winlog) << "[" << getCaption() << "] destructed" << endl;
}

void
Window::createFrameBuffer() {

	deleteFrameBuffer();

	_frameBuffer = new unsigned char[(int)_resolution.x*(int)_resolution.y*3];
}

void
Window::deleteFrameBuffer() {

	if (_frameBuffer) {

		delete[] _frameBuffer;
		_frameBuffer = 0;
	}
}

void
Window::configureViewport() {

	// we want to draw everywhere
	glViewport(0, 0, _resolution.x, _resolution.y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// OpenGl units = pixels
	glOrtho(0, _resolution.x, _resolution.y, 0, -1000, 1000); // x left, x right, y bottom, y top, z min, z max

	glMatrixMode(GL_MODELVIEW);

	LOG_ALL(winlog) << "[" << getCaption() << "] set drawing area to "
	                << "(0, 0, " << _resolution.x << ", " << _resolution.y << ")"
	                << std::endl;

	GL_ASSERT;
}

const point<double>&
Window::getResolution() {

	return _resolution;
}

void
Window::processResizeEvent(int width, int height) {

	_region.maxX = width;
	_region.maxY = height;
	_resolution.x = width;
	_resolution.y = height;

	{
		// ensure that our context is active
		OpenGl::Guard guard(this);

		configureViewport();
		createFrameBuffer();
	}
}

void
Window::processKeyUpEvent(const keys::Key& key, const Modifiers& modifiers) {

	KeyUp signal(key, modifiers);
	_keyUp(signal);
}

void
Window::processKeyDownEvent(const keys::Key& key, const Modifiers& modifiers) {

	KeyDown signal(key, modifiers);
	_keyDown(signal);
}

bool
Window::processButtonUpEvent(
		const buttons::Button&     button,
		const util::point<double>& position,
		const Modifiers&           modifiers) {

	MouseUp signal(button, position, modifiers);
	_mouseUp(signal);
}

bool
Window::processButtonDownEvent(
		const buttons::Button&     button,
		const util::point<double>& position,
		const Modifiers&           modifiers) {

	LOG_ALL(winlog) << "[Window (" << getCaption() << ")] "
	                << "a mouse button was pressed" << std::endl;

	MouseDown signal(button, position, modifiers);
	_mouseDown(signal);
}

bool
Window::processMouseMoveEvent(
		const util::point<double>& position,
		const Modifiers&           modifiers) {

	MouseMove signal(position, modifiers);
	_mouseMove(signal);
}

GlContext*
Window::createGlContext() {

	LOG_ALL(winlog) << "[" << getCaption() << "] creating a new GlContext" << std::endl;

	GlContext* globalContext = OpenGl::getGlobalContext();
	globalContext->activate();

	ContextSettings settings;

	// create the context
	GlContext* glContext = new GlContext(this, settings, globalContext);

	// activate it
	glContext->activate();

	configureViewport();

	// return it
	return glContext;
}

void
Window::redraw() {

	// They say OpenGl is thread safe. They are wrong.
	boost::mutex::scoped_lock lock(OpenGl::getMutex());

	// prepare painters
	_resize(_region);

	// ensure that our context is active
	OpenGl::Guard guard(this);

	clear();

	LOG_ALL(winlog) << "[" << getCaption() << "] redrawing my content" << endl;

	if (_painter) {

		// make sure the painter is up-to-date
		updateInputs();

		// draw the updated painter
		_painter->draw(_region, point<double>(1.0, 1.0));

	} else {

		LOG_ALL(winlog) << "[" << getCaption() << "] no content so far..." << endl;
	}

	GL_ASSERT;

	flush();

	GL_ASSERT;
}

void
Window::clear() {

	glClearColor(_clear_r, _clear_g, _clear_b, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GL_ASSERT;
}

void
Window::flush() {

	OpenGl::flush();
}

void
Window::requestFrameSave() {

	_saveFrameRequest = true;
}

void
Window::saveFrame() {

	if (closed())
		return;

	// ensure that our context is active
	OpenGl::Guard guard(this);

	glReadPixels(
			0, 0,
			_resolution.x, _resolution.y,
			GL_RGB, GL_UNSIGNED_BYTE,
			_frameBuffer);
	GL_ASSERT;

	rgb8c_view_t frameView =
			interleaved_view(
					_resolution.x, _resolution.y,
					(const rgb8_pixel_t*)_frameBuffer,
					_resolution.x*3);

	png_write_view(
			"./shots/" + getCaption() + to_string_with_leading_zeros(_frameNumber, 8) + ".png",
			flipped_up_down_view(frameView));

	_frameNumber++;
}

void
Window::onInputAdded(const pipeline::InputAdded<gui::Painter>& signal) {

	setDirty();
}

void
Window::onModified(const pipeline::Modified& signal) {

	setDirty();
}

void
Window::onSizeChanged(const SizeChanged& signal) {

	// TODO:
	// Here, we could resize the window to fit the view. However, this should be
	// an optional feature.

	setDirty();
}

void
Window::processCloseEvent(){

	LOG_DEBUG(winlog) << "[" << getCaption() << "] invalidating my GlContext" << endl;

	// ensure that our context is destructed
	OpenGl::Guard guard(0);

	LOG_DEBUG(winlog) << "[" << getCaption() << "] closing window now" << endl;

	close();
}

} // namespace gui

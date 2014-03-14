#include "config.h"

#ifdef HAVE_PNG
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
using namespace boost::gil;
#endif

#include <util/Logger.h>
#include <util/helpers.hpp>
#include <gui/OpenGl.h>
#include <gui/ContextSettings.h>
#include <gui/Keys.h>
#include "Window.h"

using std::cout;
using std::endl;

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
	_painter.registerSlot(_resize);
	_painter.registerSlot(_keyDown);
	_painter.registerSlot(_keyUp);
	_painter.registerSlot(_fingerMove);
	_painter.registerSlot(_fingerDown);
	_painter.registerSlot(_fingerUp);
	_painter.registerSlot(_penMove);
	_painter.registerSlot(_penDown);
	_painter.registerSlot(_penUp);
	_painter.registerSlot(_penIn);
	_painter.registerSlot(_penOut);
	_painter.registerSlot(_mouseMove);
	_painter.registerSlot(_mouseDown);
	_painter.registerSlot(_mouseUp);

	// register backward callbacks
	_painter.registerCallback(&Window::onInputAdded, this);
	_painter.registerCallback(&Window::onModified, this);
	_painter.registerCallback(&Window::onSizeChanged, this);
	_painter.registerCallback(&Window::onContentChanged, this);
	_painter.registerCallback(&Window::onFullscreen, this);

	// initiate first redraw
	setDirty();
}

Window::~Window() {

	gui::OpenGl::Guard guard;

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

bool
Window::processResizeEvent(int width, int height) {

	// did the size of the window change?
	if (_region.maxX == width && _region.maxY == height && _resolution.x == width && _resolution.y == height)
		return false;

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

	// prepare painters
	_resize(_region);

	return true;
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

void
Window::processFingerUpEvent(
		unsigned long              timestamp,
		const buttons::Button&     button,
		const util::point<double>& position,
		int                        id,
		const Modifiers&           modifiers) {

	FingerUp signal(timestamp, button, position, id, modifiers);
	_fingerUp(signal);
}

void
Window::processFingerDownEvent(
		unsigned long              timestamp,
		const buttons::Button&     button,
		const util::point<double>& position,
		int                        id,
		const Modifiers&           modifiers) {

	FingerDown signal(timestamp, button, position, id, modifiers);
	_fingerDown(signal);
}

void
Window::processFingerMoveEvent(
		unsigned long              timestamp,
		const util::point<double>& position,
		int                        id,
		const Modifiers&           modifiers) {

	FingerMove signal(timestamp, position, id, modifiers);
	_fingerMove(signal);
}

void
Window::processPenUpEvent(
		unsigned long              timestamp,
		const buttons::Button&     button,
		const util::point<double>& position,
		double                     pressure,
		const Modifiers&           modifiers) {

	PenUp signal(timestamp, button, position, pressure, modifiers);
	_penUp(signal);
}

void
Window::processPenDownEvent(
		unsigned long              timestamp,
		const buttons::Button&     button,
		const util::point<double>& position,
		double                     pressure,
		const Modifiers&           modifiers) {

	PenDown signal(timestamp, button, position, pressure, modifiers);
	_penDown(signal);
}

void
Window::processPenMoveEvent(
		unsigned long              timestamp,
		const util::point<double>& position,
		double                     pressure,
		const Modifiers&           modifiers) {

	LOG_ALL(winlog) << "[Window] sending signal pen move" << std::endl;

	PenMove signal(timestamp, position, pressure, modifiers);
	_penMove(signal);
}

void
Window::processPenInEvent(unsigned long timestamp) {

	PenIn signal(timestamp);
	_penIn(signal);
}

void
Window::processPenOutEvent(unsigned long timestamp) {

	PenOut signal(timestamp);
	_penOut(signal);
}

void
Window::processPenAwayEvent(unsigned long timestamp) {

	PenAway signal(timestamp);
	_penAway(signal);
}

void
Window::processButtonUpEvent(
		unsigned long              timestamp,
		const buttons::Button&     button,
		const util::point<double>& position,
		const Modifiers&           modifiers) {

	MouseUp signal(timestamp, button, position, modifiers);
	_mouseUp(signal);
}

void
Window::processButtonDownEvent(
		unsigned long              timestamp,
		const buttons::Button&     button,
		const util::point<double>& position,
		const Modifiers&           modifiers) {

	MouseDown signal(timestamp, button, position, modifiers);
	_mouseDown(signal);
}

void
Window::processMouseMoveEvent(
		unsigned long              timestamp,
		const util::point<double>& position,
		const Modifiers&           modifiers) {

	MouseMove signal(timestamp, position, modifiers);
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
	//
	// Update: It seems only buffer creation and destruction is not thread-safe 
	// (at least on nvidia hardware). Therefore, we lock at a finer scale now.

	//boost::mutex::scoped_lock lock(OpenGl::getMutex());

	// ensure that our context is active
	OpenGl::Guard guard(this);

	clear();

	LOG_ALL(winlog) << "[" << getCaption() << "] redrawing my content" << endl;

	if (_painter.isSet()) {

		// make sure the painter is up-to-date
		LOG_ALL(winlog) << "[" << getCaption() << "] updating inputs" << endl;
		updateInputs();
		LOG_ALL(winlog) << "[" << getCaption() << "] inputs up-to-date" << endl;

		// draw the updated painter
		LOG_ALL(winlog) << "[" << getCaption() << "] drawing painter content" << endl;
		bool wantsRedraw = _painter->draw(_region, point<double>(1.0, 1.0));

		if (wantsRedraw) {

			LOG_ALL(winlog) << "[" << getCaption() << "] painter indicated redraw request -- set myself dirty again" << endl;
			setDirty();
		}

	} else {

		LOG_ALL(winlog) << "[" << getCaption() << "] no content so far..." << endl;
	}

	GL_ASSERT;

	flush();

	GL_ASSERT;

	LOG_ALL(winlog) << "[" << getCaption() << "] finished redrawing" << endl;
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

#ifdef HAVE_PNG
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
#endif
}

void
Window::onInputAdded(const pipeline::InputAdded<gui::Painter>& /*signal*/) {

	setDirty();
}

void
Window::onModified(const pipeline::Modified& /*signal*/) {

	setDirty();
}

void
Window::onSizeChanged(const SizeChanged& /*signal*/) {

	// TODO:
	// Here, we could resize the window to fit the view. However, this should be
	// an optional feature.

	setDirty();
}

void
Window::onContentChanged(const ContentChanged& /*signal*/) {

	LOG_ALL(winlog) << "[" << getCaption() << "] received a content change signal" << endl;

	setDirty();
}

void
Window::onFullscreen(const WindowFullscreen& signal) {

	LOG_ALL(winlog) << "[" << getCaption() << "] received a fullscreen request" << endl;

	setFullscreen(signal.fullscreen);

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

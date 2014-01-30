#ifndef WINDOW_H__
#define WINDOW_H__

#include <string>

#include <gui/WindowMode.h>
#include <gui/error_handling.h>
#include <gui/OverlayPlacing.h>
#include <gui/GlContext.h>
#include <gui/GlContextCreator.h>
#include <gui/FingerSignals.h>
#include <gui/PenSignals.h>
#include <gui/MouseSignals.h>
#include <gui/KeySignals.h>
#include <gui/WindowSignals.h>
#include <gui/Painter.h>
#include <pipeline/all.h>
#include <signals/Slot.h>
#include <config.h>

// include platform dependent implementations
#if defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

	#include <gui/linux/XWindow.h>
	typedef gui::XWindow WindowType;

#elif defined(SYSTEM_WINDOWS)

	#include <gui/windows/WinWindow.h>
	typedef gui::WinWindow WindowType;

#elif defined(SYSTEM_OSX)

	#include <gui/osx/CocoaWindow.h>
	typedef gui::CocoaWindow WindowType;

#endif

namespace gui {

using namespace util;

/**
 * Platform independent window class. Allows views to be attached to it to draw
 * OpenGL content.
 */
class Window : public pipeline::SimpleProcessNode<>, public WindowType, public GlContextCreator {

public:

	/**
	 * Creates a window with the specified mode.
	 *
	 * @param caption The caption of the window to be created.
	 * @param mode The requested mode of the window (size, position, ...)
	 */
	Window(std::string caption, const WindowMode& mode = WindowMode());

	virtual ~Window();

	/**
	 * Get the resolution of this window.
	 *
	 * @return The resolution of this window.
	 */
	const point<double>& getResolution();

	/**
	 * Set the background color of this window.
	 *
	 * @param r The red portion in [0,1].
	 * @param g The red portion in [0,1].
	 * @param b The red portion in [0,1].
	 */
	void setBackgroundColor(double r, double g, double b) {
	    _clear_r = r;
	    _clear_g = g;
	    _clear_b = b;
	}

	/**
	 * If called, the outcome of the next redraw will be saved in a png file.
	 */
	void requestFrameSave();

	/**
	 * Create a GlContext that allows drawing to this window.
	 */
	GlContext* createGlContext();

private:

	using WindowType::setDirty;

	/**
	 * Overwritten from SimpleProcessNode. We are a sink, there's nothing to do
	 * here.
	 */
	void updateOutputs() {};

	/**
	 * Pipeline callback for new input connections.
	 */
	void onInputAdded(const pipeline::InputAdded<gui::Painter>& signal);

	/**
	 * Pipeline callback for modified signals from the painter.
	 */
	void onModified(const pipeline::Modified& signal);

	/**
	 * Pipeline callback for size change signals from the painter.
	 */
	void onSizeChanged(const SizeChanged& signal);

	/**
	 * Pipeline callback for content change signals from the painter.
	 */
	void onContentChanged(const ContentChanged& signal);

	/**
	 * Pipeline callback for fullscreen requests.
	 */
	void onFullscreen(const WindowFullscreen& signal);

	/**
	 * Callback for top-down resize events.
	 *
	 * @return True, if the size of the window changed and a redraw is in order.
	 */
	bool processResizeEvent(int width, int height);

	/**
	 * Callback for close events.
	 */
	void processCloseEvent();

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processKeyUpEvent(const keys::Key& key, const Modifiers& modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processKeyDownEvent(const keys::Key& key, const Modifiers& modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processFingerUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processFingerDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processFingerMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenInEvent(
			unsigned long              timestamp);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenOutEvent(
			unsigned long              timestamp);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processPenAwayEvent(
			unsigned long              timestamp);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processButtonUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processButtonDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			const Modifiers&           modifiers);

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	void processMouseMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			const Modifiers&           modifiers);

	/**
	 * Redraw the content of the window.
	 */
	void redraw();

	/**
	 * Allocate memory for the frame buffer.
	 */
	void createFrameBuffer();

	/**
	 * Free memory of the frame buffer.
	 */
	void deleteFrameBuffer();

	/**
	 * Clear the window with the background color.
	 */
	void clear();

	/**
	 * Show the updates from the views.
	 */
	void flush();

	/**
	 * Reconfigure the OpenGL viewport on window resize events.
	 */
	void configureViewport();

	/**
	 * Save the current content of the window to a file.
	 */
	void saveFrame();

	// the painter to draw
	pipeline::Input<Painter> _painter;

	// backward signals
	signals::Slot<const Resize>           _resize;
	signals::Slot<KeyDown>                _keyDown;
	signals::Slot<KeyUp>                  _keyUp;
	signals::Slot<FingerMove>             _fingerMove;
	signals::Slot<FingerDown>             _fingerDown;
	signals::Slot<FingerUp>               _fingerUp;
	signals::Slot<PenMove>                _penMove;
	signals::Slot<PenDown>                _penDown;
	signals::Slot<PenUp>                  _penUp;
	signals::Slot<PenIn>                  _penIn;
	signals::Slot<PenOut>                 _penOut;
	signals::Slot<PenAway>                _penAway;
	signals::Slot<MouseMove>              _mouseMove;
	signals::Slot<MouseDown>              _mouseDown;
	signals::Slot<MouseUp>                _mouseUp;

	// the region displayed by this window in GL units
	rect<double>  _region;

	// the resolution of the region in pixels
	point<double> _resolution;

	// set to true if the next frame should be saved to file
	bool          _saveFrameRequest;

	// the next frame number
	unsigned      _frameNumber;

	// a buffer to save the frame to file (exclusively owned)
	unsigned char* _frameBuffer;

	// the background color of this window
	double        _clear_r, _clear_g, _clear_b;
};

} // namespace gui

#endif // WINDOW_H__


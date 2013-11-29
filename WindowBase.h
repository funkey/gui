#ifndef WINDOW_BASE_H__
#define WINDOW_BASE_H__

#include <string>

#include <boost/thread.hpp>

#include <gui/Keys.h>
#include <gui/Buttons.h>
#include <gui/Modifiers.h>
#include <util/point.hpp>

using std::string;

namespace gui {

/**
 * Abstract base class that defines the interface for all window
 * implementations.
 */
class WindowBase {

public:

	WindowBase(string caption) :
		_caption(caption) {}

	/**
	 * Close this window and release all resources.
	 *
	 * Platform dependent.
	 */
	virtual void close() = 0;

	/**
	 * Check whether the window was closed.
	 *
	 * @return <code>true</code> if the window was closed.
	 *
	 * Platform dependent.
	 */
	virtual bool closed() = 0;

	/**
	 * Change to or from fullscreen mode.
	 */
	virtual void setFullscreen(bool fullscreen) = 0;

	/**
	 * Get the resolution of this window.
	 *
	 * @return The resolution of this window.
	 */
	virtual const util::point<double>& getResolution() = 0;

	/**
	 * Process window manager events and redraw. If this call blocks for events, 
	 * make sure to implement interrupt(), which will be called whenever a 
	 * redraw is required.
	 *
	 * Platform dependent.
	 */
	virtual void processEvents() = 0;

	/**
	 * Interrupt the event thread. This method will be called whenever a redraw 
	 * is required. Implementations should guarantee that the event loop gets 
	 * interrupted and initiates the redraw.
	 */
	virtual void interrupt() {};

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processKeyUpEvent(const keys::Key& key, const Modifiers& modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processKeyDownEvent(const keys::Key& key, const Modifiers& modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processFingerUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processFingerDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processFingerMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processPenUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processPenDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processPenMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processPenInEvent(
			unsigned long              timestamp) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processPenOutEvent(
			unsigned long              timestamp) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processPenAwayEvent(
			unsigned long              timestamp) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processButtonUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processButtonDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual void processMouseMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			const Modifiers&           modifiers) = 0;

	/**
	 * Get the caption of this window as assigned by the constructor.
	 */
	string getCaption() {

		return _caption;
	}

protected:

	/**
	 * React on close events.
	 *
	 * Subclass implementation dependent.
	 */
	virtual void processCloseEvent() = 0;

	/**
	 * React on resize events.
	 *
	 * Subclass implementation dependent.
	 */
	virtual void processResizeEvent(int width, int height) = 0;

	/**
	 * Instruct the window to redraw itself.
	 *
	 * Subclass implementation dependent.
	 */
	virtual void redraw() = 0;

	/**
	 * Mark this window as being dirty. redraw() will be called on the next
	 * occasion from the event loop in processEvents(). Subclasses should use 
	 * this method to initiate redrawing instead of calling redraw() themselves.
	 */
	void setDirty(bool dirty = true) {
	
		_dirty = dirty;

		// interrupt the possibly blocking event loop in processEvents()
		interrupt();
	}

	/**
	 * Find out whether this window has been flagged to be dirty. This should be
	 * used in the platform dependent processEvents() to initiate redrawing.
	 */
	bool isDirty() {

		return _dirty;
	}

private:

	bool   _dirty;

	string _caption;
};

} // namespace gui

#endif // WINDOW_BASE_H__


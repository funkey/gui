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
	 * Process all accumulated events since the last call to this function.
	 * This method should be called repeatedly to ensure proper redrawing and
	 * user interaction. Any implementation has to make sure that the
	 * appropriate process...Event() methods are called.
	 *
	 * Platform dependent.
	 */
	virtual void processEvents() = 0;

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
	virtual bool processFingerUpEvent(
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
	virtual bool processFingerDownEvent(
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
	virtual bool processFingerMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			int                        id,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processPenUpEvent(
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
	virtual bool processPenDownEvent(
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
	virtual bool processPenMoveEvent(
			unsigned long              timestamp,
			const util::point<double>& position,
			double                     pressure,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processPenInEvent(
			unsigned long              timestamp) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processPenOutEvent(
			unsigned long              timestamp) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processPenAwayEvent(
			unsigned long              timestamp) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processButtonUpEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processButtonDownEvent(
			unsigned long              timestamp,
			const buttons::Button&     button,
			const util::point<double>& position,
			const Modifiers&           modifiers) = 0;

	/**
	 * Callback for input events.
	 *
	 * @param event The input event.
	 */
	virtual bool processMouseMoveEvent(
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
	 * Flag this window as being dirty. redraw() will be called on the next
	 * occasion. Subclasses should use this method to initiate redrawing and not
	 * call redraw() themselves.
	 */
	void setDirty(bool dirty = true) {
	
		_dirty = dirty;
	}

	/**
	 * Find out whether this window has been flagged to be dirty. This should be
	 * used in the platform dependent processEvents() to initiate redrawing.
	 */
	bool isDirty() {

		return _dirty;
	}

	/**
	 * Get the mutex on the dirty flag to avoid race-conditions.
	 */
	boost::mutex& getDirtyMutex() {

		return _dirtyMutex;
	}

private:

	bool   _dirty;

	boost::mutex _dirtyMutex;

	string _caption;
};

} // namespace gui

#endif // WINDOW_BASE_H__


#ifndef GUI_RECORDABLE_PAINTER_H__
#define GUI_RECORDABLE_PAINTER_H__

#include <gui/OpenGl.h>
#include <gui/Painter.h>

namespace gui {

class RecordablePainter : public Painter {

public:

	RecordablePainter(std::string name = "");

	/**
	 * Destructor to free display list.
	 */
	virtual ~RecordablePainter();

	/**
	 * Draws the recorded content of this painter.
	 */
	virtual void draw(
			const util::rect<double>&  roi,
			const util::point<double>& res);

protected:

	/**
	 * Start the recording of OpenGl commands.
	 */
	void startRecording();

	/**
	 * Stop the recording of OpenGl commands.
	 */
	void stopRecording();

private:

	// the display list holding the OpenGl commands
	GLuint _displayList;
};

} // namespace gui

#endif // GUI_RECORDABLE_PAINTER_H__


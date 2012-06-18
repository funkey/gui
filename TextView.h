#ifndef GUI_TEXT_VIEW_H__
#define GUI_TEXT_VIEW_H__

#include <pipeline/all.h>
#include <gui/TextPainter.h>
#include <gui/Signals.h>

namespace gui {

class TextView : public pipeline::ProcessNode {

public:

	TextView(std::string text = "");

	void setText(std::string text);

	const std::string& getText();

private:

	void onModified(const pipeline::Modified& signal);
	void onUpdate(const pipeline::Update& signal);

	pipeline::Output<TextPainter> _painter;

	signals::Slot<const pipeline::Modified> _modified;
	signals::Slot<const SizeChanged>        _sizeChanged;

	std::string _text;

	bool _dirty;
};

} // namespace gui

#endif // GUI_TEXT_VIEW_H__


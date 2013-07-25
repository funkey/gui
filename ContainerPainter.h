#ifndef CONTAINER_PAINTER_H__
#define CONTAINER_PAINTER_H__

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <util/Logger.h>
#include <util/point.hpp>
#include <gui/Painter.h>
#include <gui/OpenGl.h>

using namespace logger;

namespace gui {

/**
 * A thread-save container of painters.
 *
 * Painters can be added with two-dimensional offsets. The size of this painter
 * will be the bounding box of all containing painters with their respective
 * offset.
 */
class ContainerPainter : public Painter {

	typedef std::pair<boost::shared_ptr<Painter>, util::point<double> > painter_offset;
	typedef std::vector<painter_offset>                                 painter_offsets;

public:

	/**
	 * Overwritten from Painter.
	 *
	 * Draws the painters that are stored in this container at the offsets
	 * associated with them.
	 */
	virtual bool draw(
			const util::rect<double>&  roi,
			const util::point<double>& resolution);

	/**
	 * Add a painter to this container.
	 *
	 * @param painter The painter to add.
	 * @param offset  The offset of the painter.
	 */
	void add(boost::shared_ptr<Painter> painter, const util::point<double>& offset = util::point<double>(0, 0));

	/**
	 * Remove a painter from this container.
	 *
	 * @param painter The painter to remove.
	 */
	void remove(boost::shared_ptr<Painter> painter);

	/**
	 * Remove all painters from this container.
	 */
	void clear();

	/**
	 * Get the number of painters in this container.
	 *
	 * @return The number of painters.
	 */
	unsigned int size() { return _painterOffsets.size(); }

	/**
	 * Recomputes the size of this painter based on the containing painter's
	 * sizes and offsets.
	 */
	void updateSize();

	/**
	 * Update the offsets of the containing painters all at once.
	 */
	void setOffsets(const std::vector<util::point<double> >& offsets);

	/**
	 * Copy assignment.
	 */
	ContainerPainter& operator=(const ContainerPainter& other);

private:

	// the painters this container stores
	painter_offsets _painterOffsets;

	// shared mutex to protect the painters
	boost::shared_mutex _paintersMutex;
};

} // namespace gui

#endif // CONTAINER_PAINTER_H__


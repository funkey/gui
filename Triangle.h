#ifndef GUI_TRIANGLE_H__
#define GUI_TRIANGLE_H__

/**
 * A triangle represented by vertex indices.
 */
struct Triangle {

	Triangle() :
		v0(0),
		v1(0),
		v2(0) {}

	Triangle(
			unsigned int v0_,
			unsigned int v1_,
			unsigned int v2_) :
		v0(v0_),
		v1(v1_),
		v2(v2_) {}

	unsigned int v0, v1, v2;

};

#endif // GUI_TRIANGLE_H__


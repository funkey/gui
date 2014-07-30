#ifndef GUI_COLORS_H__
#define GUI_COLORS_H__


/**
 * Get the RGB values for hsv.
 */
void hsvToRgb(double h, double s, double v, unsigned char& r, unsigned char& g, unsigned char& b);

/**
 * Get a (more or less random) color for an id.
 */
void idToRgb(unsigned int id, unsigned char& r, unsigned char& g, unsigned char& b);

#endif // GUI_COLORS_H__


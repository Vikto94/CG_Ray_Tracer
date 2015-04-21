#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED
#include "utils.h"

class Image {
public:
	Image(unt w, unt h);
	void setPixelColor(unt x, unt y, Color color);
	void saveImage(const char *filename);
private:
	unt m_width, m_height;
	unc* m_img;
	unt m_fSz;

};

#endif
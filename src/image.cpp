#include "image.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void Image::setPixelColor(unt x, unt y, Color color)
{
	m_img[(x + y * m_width) * 3 + 0] = color.blue;
	m_img[(x + y * m_width) * 3 + 1] = color.green;
	m_img[(x + y * m_width) * 3 + 2] = color.red;
}

void Image::saveImage(const char *filename)
{
	FILE *f;
	f = fopen(filename, "wb");

	unc bmpFileHdr[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
	unc bmpInfoHdr[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };
	unc bmpPad[3] = { 0, 0, 0 };

	bmpFileHdr[2] = (unc)(m_fSz);
	bmpFileHdr[3] = (unc)(m_fSz >> 8);
	bmpFileHdr[4] = (unc)(m_fSz >> 16);
	bmpFileHdr[5] = (unc)(m_fSz >> 24);

	bmpInfoHdr[4] = (unc)(m_width);
	bmpInfoHdr[5] = (unc)(m_width >> 8);
	bmpInfoHdr[6] = (unc)(m_width >> 16);
	bmpInfoHdr[7] = (unc)(m_width >> 24);
	bmpInfoHdr[8] = (unc)(m_height);
	bmpInfoHdr[9] = (unc)(m_height >> 8);
	bmpInfoHdr[10] = (unc)(m_height >> 16);
	bmpInfoHdr[11] = (unc)(m_height >> 24);

	fwrite(bmpFileHdr, 1, 14, f);
	fwrite(bmpInfoHdr, 1, 40, f);
	for (unt i = 0; i < m_height; i++) {
		fwrite(m_img + (m_width * (m_height - i - 1) * 3), 3, m_width, f);
		fwrite(bmpPad, 1, (4 - (m_width * 3) % 4) % 4, f);
	}
	fclose(f);
}

Image::Image(unt w, unt h) :m_width(w), m_height(h)
{
	m_img = (unc *)malloc(3 * w * h);
	memset(m_img, 0, sizeof(m_img));
	int m_fSz = 54 + 3 * w * h;
	for (unt i = 0; i < w; i++)
	{
		for (unt j = 0; j < h; j++)
		{
			int x = i;
			int y = (m_height - 1) - j;
			m_img[(x + y * w) * 3] = 0;
			m_img[(x + y * w) * 3 + 1] = 0;
			m_img[(x + y * w) * 3 + 2] = 0;
		}
	}
}
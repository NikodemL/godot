#ifndef __TGA_IMAGE__
#define __TGA_IMAGE__

class TGAImage {
public:
	struct Color {
		unsigned char b, g, r, a;
	};
public:
	TGAImage();
	TGAImage(short width, short height);
	void SetPixel(Color inputcolor, int xposition, int yposition);
	void WriteImage(char* filename);
private:
	Color *m_pixels;
	short m_height;
	short m_width;
	int Convert2Dto1D(int x, int y);
};

#endif

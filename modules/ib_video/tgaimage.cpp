#include "tgaimage.h"

#include <fstream>
#include <iostream>

TGAImage::TGAImage(short width, short height) 
{
	m_width = width;
	m_height = height;
	m_pixels = new Color[m_width*m_height];
}

void TGAImage::SetPixel(Color inputcolor, int x, int y) 
{
	m_pixels[m_width * (m_height - y - 1) + x] = inputcolor;
}

void TGAImage::WriteImage(char* filename)
{
	std::ofstream o(filename, std::ios::out | std::ios::binary);

	// Write the header
	o.put(0);
   	o.put(0);
   	o.put(2); // uncompressed RGB
   	o.put(0); o.put(0);
   	o.put(0); o.put(0);
   	o.put(0);
   	o.put(0); o.put(0); // X origin
   	o.put(0); o.put(0); // y origin
   	o.put((m_width & 0x00FF));
   	o.put((m_width & 0xFF00) / 256);
   	o.put((m_height & 0x00FF));
   	o.put((m_height & 0xFF00) / 256);
   	o.put(32); // 32-bit bitmap
	o.put(0);
   	
	// Write the pixel data
	o.write((char*)m_pixels, m_width*m_height*sizeof(m_pixels[0]));

	o.close();
}

/*
	Copyright (c) 2010 ASTRE Henri (http://www.visual-experiments.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

/** @file SimpleGrayImage.cc
 *  Image Processing Lecture, WS 07/08, Uni Koblenz.
 *  Simple image class to read and write PGM Images.
 *
 *  @author     Detlev Droege
 *  @author     Frank Schmitt
 *  @created    November 2007
 */
#include <cassert>
#include <cctype>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>

#include "JpegGrayImage.h"
#include "JpegUtils.h"

void JpegGrayImage::alloc_mem(int wid, int hig)
{
	if (pixels) delete[] pixels; //delete old pixel data
	if (rows) delete[] rows; //delete old row pointers
	pixels = new byte[wid * hig];	// get memory for pixels
	rows = new byte*[h];		// get memory for row pointers
	byte *pp = pixels;			// let pp point to 1. pixel row
	for (int i = 0; i < h; i++) {	// for every row i
		rows[i] = pp;			// make rows[i] point to it
		pp += w;			// advance pp to next row
	}
}

void JpegGrayImage::init_attributes()
{
	w = h = 0;
	pixels = NULL;
	rows = NULL;
}

JpegGrayImage::JpegGrayImage()
{
	init_attributes();
}

JpegGrayImage::~JpegGrayImage()
{
	delete [] pixels;
	delete [] rows;
	pixels = NULL;
	rows = NULL;
}

JpegGrayImage::JpegGrayImage(int wid, int hig)
{
	init_attributes();
	resize(wid, hig);
}

void JpegGrayImage::resize(int wid, int hig)
{
	assert ((wid > 0) && (hig > 0));
	w = wid;
	h = hig;
	alloc_mem(wid, hig);
}


JpegGrayImage::JpegGrayImage(const std::string& filename)
{
	init_attributes();

	Jpeg::Image img;
	Jpeg::load(filename, img);	
	w = img.width;
	h = img.height;

	
	alloc_mem(w, h);

	unsigned int widthInBytes = w*3;
	for (int i=0; i<img.width; ++i)
	{
		for (int j=0; j<img.height; ++j)
		{			
			unsigned char r = img.buffer[j*widthInBytes + i*3+0];
			unsigned char g = img.buffer[j*widthInBytes + i*3+1];
			unsigned char b = img.buffer[j*widthInBytes + i*3+2];
			pixels[j*img.width+i] = (unsigned char) (0.299*r + 0.587*g + 0.114*b);
		}
	}

	delete[] img.buffer;
}

const byte * JpegGrayImage::operator [] (int i) const
{
	if (! ((i >= 0) && (i < h))){
		std::cerr << "oops: access to row " << i << ", but " << h-1 << " is maximum\n" << std::endl;
	}
	assert ((i >= 0) && (i < h));
	return rows[i];
}

byte * JpegGrayImage::operator [] (int i)
{
	if (! ((i >= 0) && (i < h))){
		std::cerr << "oops: access to row " << i << ", but " << h-1 << " is maximum\n" << std::endl;
	}
	assert ((i >= 0) && (i < h));
	return rows[i];
}

void JpegGrayImage::writeToFile(const std::string& filename) const
{
	Jpeg::Image img;
	img.width  = w;
	img.height = h;
	img.nbComponent = 1;
	img.buffer = pixels;
	Jpeg::writeRaw(filename, img);
}




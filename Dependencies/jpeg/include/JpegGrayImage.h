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

#pragma once

#include <string>
#include <stdexcept>
#include <cassert>

typedef unsigned char byte;

class JpegGrayImage
{
	int	w;
	int	h;
	byte	*pixels;
	byte	**rows;

	void alloc_mem (int wid, int hig);	// allocate and init memory (internal)
	void init_attributes();		// initialize (internal)

public:
	JpegGrayImage();
	JpegGrayImage(int wid, int hig);


	virtual ~JpegGrayImage ();

	int width() const  { return w; }
	int height() const { return h; }

	/// return pointer to array of row pointers in byte**p = (byte**)img;
	operator byte ** () { return rows; }

	/// constructor: Read PGM image (depth 255) from file.
	/// Reads a PGM file into this object.
	/// The value range for the pixels must be 0..255.
	/// @param filename   name of image file
	JpegGrayImage(const std::string& filename);

	/// read only index operator
	/// image[i] will return a pointer to the image row i
	/// thuss offering access to pixel[y][x]
	const byte * operator [] (int i) const;	// access row[i]

	/// index operator
	/// image[i] will return a pointer to the image row i
	/// thuss offering access to pixel[y][x]
	byte * operator [] (int i);		// access row[i]

	/// Write image to file in PGM format.
	/// @param filename   name of image file to be written
	void writeToFile (const std::string& filename) const;

	/// Resize the image to the given size.
	/// The pixels are not initialized.
	/// @param wid  new desired width
	/// @param hid  new desired heigth
	void resize(int wid, int hig);
};
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

#pragma  once

#include <string>

namespace Jpeg
{
	struct Image
	{
		Image();

		unsigned char* buffer;
		int width;
		int height;
		int nbComponent; //3: RGB, 1: luminance

		size_t getBufferSize();
	};
	
	bool load(const std::string& filename, Image& img);                      //load jpeg from file (this function allocate the buffer of img struct)
	bool write(const std::string& filename, Image& img, int quality = 75);   //write jpeg (quality 0: bad, 100: good)
	bool writeRaw(const std::string& filename, Image& img);                  //save image as raw binary
	bool getDimension(const std::string& filename, int& width, int& height); //get dimension from header only
}
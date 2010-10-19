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

#include "JpegUtils.h"

#include "jpeglib.h"

#include <fstream>
#include <iostream>

using namespace Jpeg;

Image::Image()
{
	buffer      = NULL;
	width       = 0;
	height      = 0;
	nbComponent = 0;
}

size_t Image::getBufferSize()
{
	return width * height * nbComponent;
}

bool Jpeg::load(const std::string& filename, Image& img)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp)
	{
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, fp);
		jpeg_read_header(&cinfo, true);
		jpeg_start_decompress(&cinfo);

		int width       = cinfo.output_width;
		int height      = cinfo.output_height;
		int nbComponent = cinfo.output_components;	

		img.width       = width;
		img.height      = height;
		img.nbComponent = nbComponent;
		img.buffer      = new unsigned char[img.getBufferSize()];
		
		size_t widthInBytes = width * nbComponent;
		JSAMPROW row = new JSAMPLE[widthInBytes];

		for (int y=0; y<height; y++) 
		{
			jpeg_read_scanlines(&cinfo, &row, 1);
			memcpy(img.buffer + y*widthInBytes, row, widthInBytes);
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		fclose(fp);
		delete[] row;

		return true;
	}
	else
		return false;
}

bool Jpeg::write(const std::string& filename, Image& img, int quality)
{
	FILE* fp = fopen(filename.c_str(), "wb");
	if (fp)
	{
		struct jpeg_compress_struct cinfo;
		struct jpeg_error_mgr jerr;

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, fp);
		cinfo.image_width      = img.width;
		cinfo.image_height     = img.height;
		cinfo.input_components = img.nbComponent;
		cinfo.in_color_space   = (img.nbComponent == 3 ? JCS_RGB : JCS_GRAYSCALE);

		jpeg_set_defaults(&cinfo);
		jpeg_set_quality(&cinfo, quality, true);
		jpeg_start_compress(&cinfo, true);

		size_t widthInBytes = img.width * img.nbComponent;
		JSAMPROW row = new JSAMPLE[widthInBytes];

		for (int y=0; y<img.height; y++) 
		{
			memcpy(row, img.buffer + y*widthInBytes, widthInBytes);
			jpeg_write_scanlines(&cinfo, &row, 1);
		}

		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);

		fclose(fp);
		delete[] row;

		return true;
	}
	else
		return false;
}

bool Jpeg::writeRaw(const std::string& filename, Image& img)
{
	std::ofstream output(filename.c_str(), std::ios::binary);
	bool opened = output.is_open();
	if (opened)
		output.write((char*) img.buffer, img.getBufferSize());
	output.close();

	return opened;
}

bool Jpeg::getDimension(const std::string& filename, int& width, int& height)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp)
	{
		jpeg_stdio_src(&cinfo, fp);
		jpeg_read_header(&cinfo, true);

		width  = cinfo.image_width;
		height = cinfo.image_height;

		jpeg_destroy_decompress(&cinfo);

		fclose(fp);
		return true;
	}
	else
	{
		width  = 0;
		height = 0;
		return false;
	}
}
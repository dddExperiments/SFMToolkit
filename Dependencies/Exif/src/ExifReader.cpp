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

#include "ExifReader.h"
#include "exif.h"

using namespace Exif;

Info::Info()
{
	isValid     = false;
	width       = 0;
	height      = 0;
	cameraMake  = "";
	cameraModel = "";
	focalLength = 0;
	CCDWidth    = 0;
}

Info Reader::read(const std::string& jpegPath)
{
	Info info;

	FILE* fp = fopen(jpegPath.c_str(), "rb");
	if (fp)
	{
		Cexif exif;
		exif.DecodeExif(fp);
		info.width       = exif.m_exifinfo->Width;
		info.height      = exif.m_exifinfo->Height;
		info.cameraMake  = exif.m_exifinfo->CameraMake;
		info.cameraModel = exif.m_exifinfo->CameraModel;
		info.focalLength = exif.m_exifinfo->FocalLength;
		info.CCDWidth    = exif.m_exifinfo->CCDWidth*2.54*10; //inches -> mm
		info.isValid = true;

		if (info.cameraMake == "")
			info.isValid = false;

		fclose(fp);
	}

	return info;
}

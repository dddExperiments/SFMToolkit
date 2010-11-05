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

#include "BundlerFocalExtractor.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp> 
#include <sstream>
#include <iostream>

#include "ExifReader.h"
#include "JpegUtils.h"

namespace bf = boost::filesystem;

BundlerFocalExtractor::BundlerFocalExtractor(const std::string& inputPath, const std::string& outputListPath, const std::string& outputFocalListPath)
: mDatabase("cameras.xml")
{
	open(inputPath);

	std::ofstream outputList(outputListPath.c_str());
	std::ofstream outputFocalList(outputFocalListPath.c_str());
	if (outputList.is_open() && outputFocalList.is_open())
	{
		for (unsigned int i=0; i<mPictures.size(); ++i)
		{
			const std::string& filename = mPictures[i];
			std::stringstream filepath;
			filepath << inputPath << filename;
			
			int width, height = 0;
			Jpeg::getDimension(filepath.str(), width, height);
			float focal = getFocal(filepath.str(), width, height);
			outputList << filename << std::endl;
			outputFocalList << filename << " 0 " << focal << std::endl;
		}
	}
	outputList.close();
	outputFocalList.close();
}

void BundlerFocalExtractor::open(const std::string& strPath)
{
	if (!bf::is_directory(strPath))
		return;

	bf::path path = bf::path(strPath);

	bf::directory_iterator itEnd;

	for (bf::directory_iterator it(path); it != itEnd; ++it)
	{
		if (!bf::is_directory(it->status()))
		{
			std::string filename = it->filename();
			std::string extension = bf::extension(*it).substr(1); //.JPG -> JPG
			boost::to_lower(extension);                           //jpg

			if (extension == "jpg")
				mPictures.push_back(filename);
		}
	}
}

float BundlerFocalExtractor::getFocal(const std::string& inputJpegPath, int width, int height)
{
	Exif::Info exifInfo = Exif::Reader::read(inputJpegPath);	
	if (exifInfo.isValid)
	{
		float focalInMM = exifInfo.focalLength;	

		//try to find CCD width in database
		float ccdWidthInMM = mDatabase.getCCDWidth(exifInfo.cameraMake, exifInfo.cameraModel);

		//if CCD width not found in database, try to find it in Exif info
		if (ccdWidthInMM == -1)
		{
			std::cout << "Error: Missing CCD width in xml database for (" << exifInfo.cameraMake<< " " << exifInfo.cameraModel << ") in file " << inputJpegPath << std::endl;
			ccdWidthInMM = exifInfo.CCDWidth;
		}

		float largerSizeInPixel = (float) std::max(width, height);

		//compute focal in pixel if the CCD width was found
		if (ccdWidthInMM != 0 && ccdWidthInMM != -1)
			return largerSizeInPixel * (focalInMM/ccdWidthInMM);	
		else
		{
			std::cout << "Error: Missing CCD width in Exif for " << inputJpegPath << std::endl;
			return 0;
		}
	}
	else 
	{
		std::cout << "Error: Invalid Exif in file " << inputJpegPath << std::endl;
		return 0;
	}
}
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

#include <Ogre.h>

struct BundlerCamera
{
	BundlerCamera(Ogre::Real _focalLength, Ogre::Real _radialDistort1, Ogre::Real _radialDistort2, Ogre::Matrix3 _rotation, Ogre::Vector3 _translation);

	Ogre::Real focalLength;
	Ogre::Real radialDistort1;
	Ogre::Real radialDistort2;
	Ogre::Matrix3 rotation;
	Ogre::Vector3 translation;

	std::vector<std::pair<int, Ogre::Vector2>> viewlist;
};

std::ostream& operator <<(std::ostream& output, const BundlerCamera& cam);

struct BundlerFeature
{
	BundlerFeature(Ogre::Vector2 position, float scale, float orientation, float* descriptor);

	Ogre::Vector2 position;
	float scale;
	float orientation;
	float descriptor[128];
};

struct BundlerViewPoint
{
	BundlerViewPoint(unsigned int indexImg, unsigned int indexFeature);

	unsigned int indexImg;
	unsigned int indexFeature;
};

struct Bundler3DPoint
{
	Bundler3DPoint(Ogre::Vector3 _position, Ogre::ColourValue _color, std::vector<BundlerViewPoint> _viewPoints);

	Ogre::Vector3 position;
	Ogre::ColourValue color;
	std::vector<BundlerViewPoint> viewPoints;
};

class BundlerToTracking
{
	public:
		void open(const std::string& inputPath, const std::string& bundlerFilename, const std::string& bundlerListJpeg); //open .out file from Bundler
		void writeOutputFile(const std::string& binFilename, const std::string& txtFilename);

	protected:		
		void writeBinFile(const std::string& filename);
		void writeTxtFile(const std::string& filename);
		void logerror(const std::string& reason);
		void parseFile(const std::string& filename);
		void parseFileList(const std::string& filename);
		std::vector<BundlerFeature> getFeaturesBin(const std::string& baseFilename);

		std::vector<BundlerCamera>  mCameras;
		std::vector<Bundler3DPoint> m3DPoints;
		
		std::vector<std::string> mFilenames;
		std::vector<std::vector<BundlerFeature>> mFeatures;		
};
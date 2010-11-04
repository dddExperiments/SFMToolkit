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

#include "BundlerToTracking.h"

#include <Ogre/OgreFreeImageCodec.h>

using namespace std;

BundlerCamera::BundlerCamera(Ogre::Real _focalLength, Ogre::Real _radialDistort1, Ogre::Real _radialDistort2, Ogre::Matrix3 _rotation, Ogre::Vector3 _translation)
{
	focalLength    = _focalLength;
	radialDistort1 = _radialDistort1;
	radialDistort2 = _radialDistort2;
	rotation       = _rotation;
	translation    = _translation;
}

std::ostream& operator <<(std::ostream& output, const BundlerCamera& cam)
{
	output << cam.focalLength << " " << cam.radialDistort1 << " " << cam.radialDistort2 << std::endl;
	output << cam.translation.x << " " << cam.translation.y << " " << cam.translation.z << std::endl;
	
	Ogre::Matrix3 rot = cam.rotation;
	output << rot[0][0] << " " << rot[0][1] << " " << rot[0][2] << std::endl;
	output << rot[1][0] << " " << rot[1][1] << " " << rot[1][2] << std::endl;
	output << rot[2][0] << " " << rot[2][1] << " " << rot[2][2];

	return output;
}

Bundler3DPoint::Bundler3DPoint(Ogre::Vector3 _position, Ogre::ColourValue _color, std::vector<BundlerViewPoint> _viewPoints)
{
	position   = _position;
	color      = _color;
	viewPoints = _viewPoints;
}

BundlerFeature::BundlerFeature(Ogre::Vector2 position, float scale, float orientation, float* descriptor)
{
	this->position = position;
	this->scale = scale;
	this->orientation = orientation;
	memcpy(&this->descriptor[0], descriptor, sizeof(float)*128);
}

BundlerViewPoint::BundlerViewPoint(unsigned int indexImg, unsigned int indexFeature)
{
	this->indexImg     = indexImg;
	this->indexFeature = indexFeature;
}

void BundlerToTracking::open(const std::string& inputPath, const std::string& bundlerFilename, const std::string& bundlerListJpeg)
{
	parseFile(bundlerFilename);
	parseFileList(bundlerListJpeg);
	for (unsigned int i=0; i<mFilenames.size(); ++i)
	{
		std::stringstream filepath;
		filepath << inputPath << mFilenames[i];
		std::vector<BundlerFeature> features = getFeaturesBin(filepath.str());
		mFeatures.push_back(features);
	}	
}

void BundlerToTracking::writeOutputFile(const std::string& binFilename, const std::string& txtFilename)
{
	writeBinFile(binFilename);
	writeTxtFile(txtFilename);
}

void BundlerToTracking::writeTxtFile(const std::string& filename)
{
	std::ofstream output(filename.c_str());
	if (output.is_open())
	{
		output << "#BundlerTracking version 1" << std::endl;
		output << mCameras.size() << std::endl;
		for (unsigned int i=0; i<mCameras.size(); ++i)
		{
			output << "#" << mFilenames[i] << std::endl;
			BundlerCamera cam = mCameras[i];
			output << cam << std::endl;
			//would be nice to open jpeg and save width and height...
		}
	}
	output.close();
}

void BundlerToTracking::writeBinFile(const std::string& filename)
{
	std::ofstream output(filename.c_str(), std::ios::binary);
	if (output.is_open())
	{
		unsigned int nb3DPoints = m3DPoints.size();
		output.write((char*)&nb3DPoints, sizeof(nb3DPoints));
		for (unsigned int i=0; i<nb3DPoints; ++i)
		{
			Bundler3DPoint p = m3DPoints[i];
			float r = p.color.r;
			float g = p.color.g;
			float b = p.color.b;
			float x = p.position.x;
			float y = p.position.y;
			float z = p.position.z;
			output.write((char*)&r, sizeof(r));
			output.write((char*)&g, sizeof(g));
			output.write((char*)&b, sizeof(b));
			output.write((char*)&x, sizeof(x));
			output.write((char*)&y, sizeof(y));
			output.write((char*)&z, sizeof(z));

			unsigned int viewPointLength = p.viewPoints.size();
			output.write((char*)&viewPointLength, sizeof(viewPointLength));
			for (unsigned int j=0; j<viewPointLength; ++j)
			{
				BundlerViewPoint viewPoint = p.viewPoints[j];
				unsigned int indexImg      = viewPoint.indexImg;
				unsigned int indexFeature  = viewPoint.indexFeature;
				output.write((char*)&indexImg, sizeof(indexImg));

				BundlerFeature feature = mFeatures[indexImg][indexFeature];
				float x           = feature.position.x;
				float y           = feature.position.y;
				float scale       = feature.scale;
				float orientation = feature.orientation;
				float* descriptor = &feature.descriptor[0];
				output.write((char*)&x, sizeof(x));
				output.write((char*)&y, sizeof(y));
				output.write((char*)&scale, sizeof(scale));
				output.write((char*)&orientation, sizeof(orientation));
				output.write((char*)descriptor, sizeof(float)*128);				
			}
		}
	}
	output.close();
}

void BundlerToTracking::logerror(const std::string& reason)
{
	std::cout << "Error while checking " << reason << std::endl;
}

void BundlerToTracking::parseFileList(const std::string& filename)
{
	std::ifstream input(filename.c_str());
	if (input.is_open())
	{
		while(!input.eof())
		{
			std::string line;
			std::getline(input, line);
			if (line != "")
				mFilenames.push_back(line);
		}
	}
	input.close();
}

void BundlerToTracking::parseFile(const std::string& filename)
{
	std::ifstream input(filename.c_str());
	if (input.is_open())
	{
		std::string line;
		std::getline(input, line); //eat first line : # Bundle file v0.3
		unsigned int nbCamera;
		unsigned int nbPoints;
		input >> nbCamera;
		input >> nbPoints;
		
		//
		// C A M E R A     P A R S I N G
		//

		//<f> <k1> <k2>   [the focal length, followed by two radial distortion coeffs]
		//<R>             [a 3x3 matrix representing the camera rotation]
		//<t>             [a 3-vector describing the camera translation]
		for (unsigned int i=0; i<nbCamera; ++i)
		{
			double focalLength, radialDistort1, radialDistort2;
			input >> focalLength;
			input >> radialDistort1;
			input >> radialDistort2;
			
			Ogre::Matrix3 rotation;
			for (unsigned int j=0; j<3; ++j)
				for (unsigned int k=0; k<3; ++k)
				input >> rotation[j][k];
	
			Ogre::Vector3 translation;
			input >> translation.x;
			input >> translation.y;
			input >> translation.z;

			BundlerCamera camera((float)focalLength, (float)radialDistort1, (float)radialDistort2, rotation, translation);
			mCameras.push_back(camera);
		}

		//
		// P O I N T S     P A R S I N G
		//

		//<position>      [a 3-vector describing the 3D position of the point]
		//<color>         [a 3-vector describing the RGB color of the point]
		//<view list>     [a list of views the point is visible in]
		for (unsigned int i=0; i<nbPoints; ++i)
		{
			Ogre::Vector3 position;			
			input >> position.x;
			input >> position.y;
			input >> position.z;
			
			int r, g, b;
			input >> r;
			input >> g;
			input >> b;
			Ogre::ColourValue color(r/255.0f, g/255.0f, b/255.0f);
			
			//
			// V I E W L I S T     P A R S I N G
			//

			//The view list begins with the length of the list (i.e., the number of
			//cameras the point is visible in).  The list is then given as a list of
			//quadruplets <camera> <key> <x> <y>, where <camera> is a camera index,
			//<key> the index of the SIFT keypoint where the point was detected in
			//that camera, and <x> and <y> are the detected positions of that
			//keypoint.
			
			std::vector<BundlerViewPoint> viewPoints;	

			unsigned int viewlistSize;
			input >> viewlistSize;
			for (unsigned int j=0; j<viewlistSize; ++j)
			{
				int cameraIndex;
				int siftIndex;				
				input >> cameraIndex;
				input >> siftIndex;
				
				Ogre::Vector2 position;
				input >> position.x;
				input >> position.y;								
				mCameras[cameraIndex].viewlist.push_back(std::pair<int, Ogre::Vector2>(i, position));
				viewPoints.push_back(BundlerViewPoint(cameraIndex, siftIndex));
			}
			m3DPoints.push_back(Bundler3DPoint(position, color, viewPoints));
		}
	}
	input.close();
}

std::vector<BundlerFeature> BundlerToTracking::getFeaturesBin(const std::string& baseFilename)
{
	std::stringstream filename;
	filename << baseFilename.substr(0, baseFilename.size()-4) << ".key.bin";	
	std::vector<BundlerFeature> features;

	std::ifstream input;
	input.open(filename.str().c_str(), std::ios::binary);
	if (input.is_open())
	{
		int nbFeature;
		input.read((char*)&nbFeature, sizeof(int));
		
		for (int i=0; i<nbFeature; ++i)
		{			
			float x, y, scale, orientation;
			float descriptor[128];
			input.read((char*)&x, sizeof(x));
			input.read((char*)&y, sizeof(y));
			input.read((char*)&scale, sizeof(scale));
			input.read((char*)&orientation, sizeof(orientation));
			input.read((char*)&descriptor[0], sizeof(float)*128);
			features.push_back(BundlerFeature(Ogre::Vector2(x,y), scale, orientation, descriptor));
		}
	}
	input.close();

	return features;
}
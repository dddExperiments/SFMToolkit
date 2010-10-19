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

#include <BundlerParser.h>

using namespace Bundler;

Parser::Parser(const std::string& bundleFilePath)
{
	parseBundlerFile(bundleFilePath);
}

void Parser::parseBundlerFile(const std::string& filepath)
{
	std::ifstream input(filepath.c_str());
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

			Camera camera((float)focalLength, (float)radialDistort1, (float)radialDistort2, rotation, translation);
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

			Vertex point(position, color);
			mVertices.push_back(point);

			//
			// V I E W L I S T     P A R S I N G
			//

			//The view list begins with the length of the list (i.e., the number of
			//cameras the point is visible in).  The list is then given as a list of
			//quadruplets <camera> <key> <x> <y>, where <camera> is a camera index,
			//<key> the index of the SIFT keypoint where the point was detected in
			//that camera, and <x> and <y> are the detected positions of that
			//keypoint.
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
			}
		}
	}
	input.close();
}

unsigned int Parser::getNbCamera() const
{
	return mCameras.size();
}

const Camera& Parser::getCamera(unsigned int index) const
{
	return mCameras[index];
}

const std::vector<Vertex>& Parser::getVertices() const
{
	return mVertices;
}
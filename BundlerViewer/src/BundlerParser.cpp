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

#include "BundlerParser.h"

#include <OgreVector2.h>
#include <OgreString.h>

using namespace Bundler;

Vertex::Vertex(Ogre::Vector3 position, Ogre::ColourValue color, Ogre::Vector3 normal)
{
	this->position = position;
	this->color    = color;
	this->normal   = normal;
}

Triangle::Triangle(unsigned int indexA, unsigned int indexB, unsigned int indexC)
{
	this->indexA = indexA;
	this->indexB = indexB;
	this->indexC = indexC;
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles)
{
	this->vertices = vertices;
	this->triangles = triangles;
}

Camera::Camera(Ogre::Real focalLength, Ogre::Real radialDistort1, Ogre::Real radialDistort2, Ogre::Matrix3 rotation, Ogre::Vector3 translation, const std::string& filename)
{
	this->filename       = filename;
	this->focalLength    = focalLength;
	this->radialDistort1 = radialDistort1;
	this->radialDistort2 = radialDistort2;
	this->rotation       = rotation;
	this->translation    = translation;
}

Parser::Parser(const std::string& bundleFilePath, const std::string& pictureListFilePath)
{
	parseBundlerFile(bundleFilePath);
	parsePictureListFile(pictureListFilePath);
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

void Parser::parsePictureListFile(const std::string& filepath)
{
	unsigned int index = 0;
	std::ifstream input(filepath.c_str());
	if (input.is_open())
	{
		while(!input.eof())
		{
			std::string line;
			std::string tmp;
			std::getline(input, line);

			if (line != "")
			{
				mCameras[index].filename = line;
				index++;
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

const Mesh&	Bundler::importPly(const std::string& filepath)
{
	static Mesh mesh;

	Ogre::Vector3 noNormalValue(Ogre::Vector3::ZERO);
	Ogre::ColourValue noColorValue(0.f, 0.f, 0.f, 0.f);

	std::ifstream input;
	input.open(filepath.c_str(), std::ios::binary);

	if (!input.is_open())
		return mesh;

	std::string line;
	unsigned int nbVertices = 0;
	unsigned int nbTriangles = 0;
	bool isBinaryFile = false;
	bool hasNormals = false;
	bool hasColors  = false;
	bool hasAlpha   = false;

	//parse header
	do 
	{
		std::getline(input, line);

		if (line == "format binary_little_endian 1.0")
			isBinaryFile = true;
		else if (line == "format ascii 1.0")
			isBinaryFile = false;
		else if (line == "property float nx")
			hasNormals = true;
		else if (line == "property uchar red")
			hasColors = true;
		else if (line == "property uchar alpha")
			hasAlpha = true;
		else if (line == "property uchar diffuse_red")
			hasColors = true;
		else if (line == "property uchar diffuse_alpha")
			hasAlpha = true;
		else
		{
			std::string keywordVertex("element vertex ");
			unsigned int lengthVertex = keywordVertex.size();

			std::string keywordFace("element face ");
			unsigned int lengthFace = keywordFace.size();

			if (line.size() > lengthVertex && line.substr(0, lengthVertex) == keywordVertex)
			{
				//std::string tmpVertex = line.substr(lengthVertex);
				nbVertices = atoi(line.substr(lengthVertex).c_str());
			}
			else if (line.size() > lengthFace && line.substr(0, lengthFace) == keywordFace)
			{
				//std::string tmpFace = line.substr(lengthFace);
				nbTriangles = atoi(line.substr(lengthFace).c_str());
			}
		}
	} while (line != "end_header");
	
	//something goes wrong: end_header not found
	if (input.eof()) 
	{
		nbVertices  = 0;
		nbTriangles = 0;
	}

	mesh.vertices.reserve(nbVertices);
	mesh.triangles.reserve(nbTriangles);

	if (!isBinaryFile)
	{
		float* pos = new float[hasNormals?6:3];
		int* color = new int[hasAlpha?4:3];

		for (unsigned int i=0; i<nbVertices; ++i)
		{
			input >> pos[0]; //x
			input >> pos[1]; //y
			input >> pos[2]; //z
			if (hasNormals)
			{
				input >> pos[3]; //nx
				input >> pos[4]; //ny
				input >> pos[5]; //nz
			}
			if (hasColors)
			{
				input >> color[0]; //r
				input >> color[1]; //g
				input >> color[2]; //b
				if (hasAlpha)
					input >> color[3]; //a
			}

			Ogre::Vector3 position((Ogre::Real)pos[0], (Ogre::Real)pos[1], (Ogre::Real)pos[2]);
			Ogre::ColourValue colorValue = hasColors ? Ogre::ColourValue(color[0]/255.0f, color[1]/255.0f, color[2]/255.0f) : noColorValue;
			Ogre::Vector3 normal = hasNormals ? Ogre::Vector3((Ogre::Real)pos[3], (Ogre::Real)pos[4], (Ogre::Real)pos[5]) : noNormalValue;

			mesh.vertices.push_back(Vertex(position, colorValue, normal));
		}

		unsigned char three;
		int indexA;
		int indexB;
		int indexC;
		for (unsigned int i=0; i<nbTriangles; ++i)
		{
			input >> three; //I know a triangle face is composed of 3 vertex ;-)
			input >>indexA;
			input >>indexB;
			input >>indexC;
			mesh.triangles.push_back(Triangle(indexA, indexB, indexC));
		}
		delete[] pos;
		delete[] color;
	}
	else
	{
		float* pos = new float[hasNormals?6:3];
		size_t sizePos = sizeof(float)*(hasNormals?6:3);

		unsigned char* color = new unsigned char[hasAlpha?4:3];
		size_t sizeColor = sizeof(unsigned char)*(hasAlpha?4:3);

		for (unsigned int i=0; i<nbVertices; ++i)
		{
			input.read((char*)pos, sizePos);
			if (hasColors)
				input.read((char*)color, sizeColor);

			Ogre::Vector3 position((Ogre::Real)pos[0], (Ogre::Real)pos[1], (Ogre::Real)pos[2]);
			Ogre::ColourValue colorValue = hasColors ? Ogre::ColourValue(color[0]/255.0f, color[1]/255.0f, color[2]/255.0f) : noColorValue;
			Ogre::Vector3 normal = hasNormals ? Ogre::Vector3((Ogre::Real)pos[3], (Ogre::Real)pos[4], (Ogre::Real)pos[5]) : noNormalValue;
			mesh.vertices.push_back(Vertex(position, colorValue, normal));
		}

		unsigned char three = 3;
		int indexes[3];
		for (unsigned int i=0; i<nbTriangles; ++i)
		{
			input.read((char*)&three, sizeof(three));
			input.read((char*)indexes, sizeof(indexes));
			mesh.triangles.push_back(Triangle(indexes[0], indexes[1], indexes[2]));
		}
		delete[] pos;
		delete[] color;
	}

	input.close();

	return mesh;
}
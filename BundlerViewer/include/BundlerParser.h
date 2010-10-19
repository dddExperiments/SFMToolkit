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

#include <OgreVector3.h>
#include <OgreColourValue.h>
#include <OgreMatrix3.h>

namespace Bundler
{
	struct Vertex
	{
		Vertex(Ogre::Vector3 position, Ogre::ColourValue color, Ogre::Vector3 normal = Ogre::Vector3::UNIT_Z);
		
		Ogre::Vector3     position;
		Ogre::ColourValue color;
		Ogre::Vector3     normal;
	};

	struct Triangle
	{
		Triangle(unsigned int indexA, unsigned int indexB, unsigned int indexC);

		unsigned int indexA;
		unsigned int indexB;
		unsigned int indexC;
	};

	struct Mesh
	{
		Mesh() {}
		Mesh(const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles = std::vector<Triangle>());

		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
	};

	struct Camera
	{
		Camera(Ogre::Real focalLength, Ogre::Real radialDistort1, Ogre::Real radialDistort2, Ogre::Matrix3 rotation, Ogre::Vector3 translation, const std::string& filename = "");

		std::string filename;
		Ogre::Real focalLength;
		Ogre::Real radialDistort1;
		Ogre::Real radialDistort2;
		Ogre::Matrix3 rotation;
		Ogre::Vector3 translation;

		std::vector<std::pair<int, Ogre::Vector2>> viewlist;
	};

	class Parser
	{
		public:
			Parser(const std::string& bundleFilePath, const std::string& pictureListFilePath);

			unsigned int getNbCamera() const;
			const Camera& getCamera(unsigned int index) const;

			const std::vector<Vertex>& getVertices() const;

		protected:
			void parseBundlerFile(const std::string& filename);
			void parsePictureListFile(const std::string& filename);

			std::vector<Vertex> mVertices;
			std::vector<Camera> mCameras;	
	};

	const Mesh&	importPly(const std::string& filepath);
}
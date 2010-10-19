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

#include "BundlerCleaner.h"

VertexIndex::VertexIndex(__int32 index)
{
	this->index = index;
}

VertexIndex::VertexIndex(unsigned int indexA, unsigned int indexB)
{
	this->indexA = indexA;
	this->indexB = indexB;
}

BundlerCleaner::BundlerCleaner(const std::string& bundleSrcFilePath, const std::string& cleanedPlyFilePath)
: mParser(bundleSrcFilePath)
{
	//mark all vertices as deleted
	unsigned int nbVertex = mParser.getVertices().size();
	mVertexVisibility = std::vector<bool>(nbVertex);
	for (unsigned int i=0; i<nbVertex; ++i)
		mVertexVisibility[i] = false;

	//mark all vertices in the ply file as visible
	importPly(cleanedPlyFilePath);
}

void BundlerCleaner::importPly(const std::string& filepath)
{
	Ogre::Vector3 noNormalValue(Ogre::Vector3::ZERO);
	Ogre::ColourValue noColorValue(0.f, 0.f, 0.f, 0.f);

	std::ifstream input;
	input.open(filepath.c_str(), std::ios::binary);

	if (input.is_open())
	{
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
					nbVertices = atoi(line.substr(lengthVertex).c_str());
				}
				else if (line.size() > lengthFace && line.substr(0, lengthFace) == keywordFace)
				{
					nbTriangles = atoi(line.substr(lengthFace).c_str());
				}
			}
		} while (line != "end_header"); //missing eof check...
		
		if (!isBinaryFile || !hasNormals)
		{
			std::cout << "The file was written in ascii format so indexes store in normals are corrupted !" << std::endl;
			std::cout << "You need to clean the mesh again and save it in binary format with normals" << std::endl;
		}
		else
		{		
			float* pos = new float[6];
			size_t sizePos = sizeof(float)*6;

			unsigned char* color = new unsigned char[hasAlpha?4:3];
			size_t sizeColor = sizeof(unsigned char)*(hasAlpha?4:3);

			for (unsigned int i=0; i<nbVertices; ++i)
			{
				input.read((char*)pos, sizePos);
				if (hasColors)
					input.read((char*)color, sizeColor);

				// pos[0] = x
				// pos[1] = y
				// pos[2] = z
				// pos[3] = index.indexA
				// pos[4] = index.indexB
				// pos[5] = 42

				VertexIndex vindex((unsigned int)pos[3], (unsigned int)pos[4]);
				unsigned int index = (unsigned int) vindex.index;
				if ((unsigned int)pos[5] == 42 && index < mVertexVisibility.size())
				{
					mVertexVisibility[index] = true;
				}
			}
			delete[] pos;
			delete[] color;
		}
	}
	input.close();
}

void BundlerCleaner::save(const std::string& bundleDstFilePath)
{
	unsigned int deletedCounter = 0;
	for (unsigned int i=0; i<mVertexVisibility.size(); ++i)
	{
		if (!mVertexVisibility[i])
		{
			deletedCounter++;
		}
	}
	std::cout << deletedCounter << "/"<< mVertexVisibility.size() << " vertex were removed" << std::endl;
}
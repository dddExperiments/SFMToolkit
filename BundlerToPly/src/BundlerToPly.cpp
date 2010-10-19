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

#include "BundlerToPly.h"

VertexIndex::VertexIndex(__int32 index)
{
	this->index = index;
}

VertexIndex::VertexIndex(unsigned int indexA, unsigned int indexB)
{
	this->indexA = indexA;
	this->indexB = indexB;
}

BundlerToPly::BundlerToPly(const std::string& bundlerFilePath)
: mParser(bundlerFilePath)
{}

void BundlerToPly::save(const std::string& plyFilePath)
{
	const std::vector<Bundler::Vertex>& vertices = mParser.getVertices();
	unsigned int nbVertex = vertices.size();
	
	std::ofstream output(plyFilePath.c_str(), std::ios::binary);
	if (output.is_open())
	{
		output << "ply" << std::endl;
		output << "format binary_little_endian 1.0" << std::endl;
		output << "element vertex " << nbVertex << std::endl;
		output << "property float x" << std::endl;
		output << "property float y" << std::endl;
		output << "property float z" << std::endl;
		output << "property float nx" << std::endl;
		output << "property float ny" << std::endl;
		output << "property float nz" << std::endl;
		output << "property uchar red" << std::endl;
		output << "property uchar green" << std::endl;
		output << "property uchar blue" << std::endl;
		output << "property uchar alpha" << std::endl;
		output << "end_header" << std::endl;
		
		float* pos           = new float[6];
		unsigned char* color = new unsigned char[4];

		for (unsigned int i=0; i<nbVertex; ++i)
		{
			pos[0] = vertices[i].position.x;
			pos[1] = vertices[i].position.y;
			pos[2] = vertices[i].position.z;

			VertexIndex index((__int32)i);
			pos[3] = (float)index.indexA;
			pos[4] = (float)index.indexB;
			pos[5] = 42;

			color[0] = (unsigned char)(vertices[i].color.r*255);
			color[1] = (unsigned char)(vertices[i].color.g*255);
			color[2] = (unsigned char)(vertices[i].color.b*255);
			color[3] = (unsigned char)(vertices[i].color.a*255);

			output.write((char*)pos, sizeof(float)*6);
			output.write((char*)color, sizeof(unsigned char)*4);
		}

		delete[] pos;
		delete[] color;
	}
}
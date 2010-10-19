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

#include <OgreSimpleRenderable.h>
#include "BundlerParser.h"

class GPUBillboardSet : public Ogre::SimpleRenderable
{
	public:
		GPUBillboardSet(const Ogre::String& name, const std::vector<Bundler::Vertex>& vertices, bool useGeometryShader, Ogre::Vector2& billboardSize = Ogre::Vector2(0.01f, 0.01f));
		virtual ~GPUBillboardSet();

		void setBillboardSize(Ogre::Vector2 size);
		Ogre::Vector2 getBillboardSize() const;

		void setBillboards(const std::vector<Bundler::Vertex>& vertices, bool useGeometryShader);

	protected:
		virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
		virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
		virtual Ogre::Real getBoundingRadius() const;
		virtual const Ogre::String& getMovableType() const;
		
	private:
		void createVertexDataForVertexShaderOnly(const std::vector<Bundler::Vertex>& vertices);
		void createVertexDataForVertexAndGeometryShaders(const std::vector<Bundler::Vertex>& vertices);
		void updateBoundingBoxAndSphereFromBillboards(const std::vector<Bundler::Vertex>& vertices);
		static Ogre::AxisAlignedBox calculateBillboardsBoundingBox(const std::vector<Bundler::Vertex>& vertices);
		
		static const Ogre::String mMovableType;

		Ogre::Real    mBBRadius;
		Ogre::Vector2 mBillboardSize;
};
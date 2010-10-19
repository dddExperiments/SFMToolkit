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

#include "GPUBillboardSet.h"

#include <OgreHardwareBufferManager.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreRoot.h>

const Ogre::String GPUBillboardSet::mMovableType("GPUBillboardSet");

GPUBillboardSet::GPUBillboardSet(const Ogre::String& name, const std::vector<Bundler::Vertex>& vertices, bool useGeometryShader, Ogre::Vector2& billboardSize)
{
	mBBRadius      = 1.0f;
	mBillboardSize = billboardSize;	
	setBillboards(vertices, useGeometryShader);
}

GPUBillboardSet::~GPUBillboardSet()
{
	OGRE_DELETE mRenderOp.vertexData;
}

void GPUBillboardSet::setBillboards(const std::vector<Bundler::Vertex>& vertices, bool useGeometryShader)
{
	if (mRenderOp.vertexData)
		OGRE_DELETE mRenderOp.vertexData;
	
	if (useGeometryShader)
		createVertexDataForVertexAndGeometryShaders(vertices);
	else
		createVertexDataForVertexShaderOnly(vertices);
	
	// Update bounding box and sphere 
	updateBoundingBoxAndSphereFromBillboards(vertices);

	// Update billboard size
	setBillboardSize(mBillboardSize);
}

void GPUBillboardSet::createVertexDataForVertexAndGeometryShaders(const std::vector<Bundler::Vertex>& vertices)
{
	// Setup render operation
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST; 
	mRenderOp.vertexData = OGRE_NEW Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = vertices.size(); 
	mRenderOp.vertexData->vertexStart = 0; 
	mRenderOp.useIndexes = false; 
	mRenderOp.indexData = 0;

	// Vertex format declaration
	unsigned short sourceBufferIdx = 0;
    Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
    size_t currOffset = 0;
	decl->addElement(sourceBufferIdx, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	decl->addElement(sourceBufferIdx, currOffset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

	// Create vertex buffer
	Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		decl->getVertexSize(sourceBufferIdx),
		mRenderOp.vertexData->vertexCount,
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    // Bind vertex buffer
    Ogre::VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;
	bind->setBinding(sourceBufferIdx, vbuf);

	// Fill vertex buffer (see http://www.ogre3d.org/docs/manual/manual_59.html#SEC287)
	Ogre::RenderSystem* renderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();
	unsigned char* pVert = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	Ogre::Real* pReal;
	Ogre::RGBA* pRGBA;
	Ogre::VertexDeclaration::VertexElementList elems = decl->findElementsBySource(sourceBufferIdx);
	Ogre::VertexDeclaration::VertexElementList::iterator itr;
	for (unsigned int i=0; i<vertices.size(); ++i )
	{
		const Bundler::Vertex& vertex = vertices[i];
		for (itr=elems.begin(); itr!=elems.end(); ++itr)
		{
			Ogre::VertexElement& elem = *itr;
			if (elem.getSemantic() == Ogre::VES_POSITION)
			{
				elem.baseVertexPointerToElement(pVert, &pReal);
				*pReal = vertex.position.x; *pReal++;
				*pReal = vertex.position.y; *pReal++;
				*pReal = vertex.position.z; *pReal++;
			}
			else if (elem.getSemantic() == Ogre::VES_DIFFUSE)
			{
				elem.baseVertexPointerToElement(pVert, &pRGBA);
				renderSystem->convertColourValue(vertex.color, pRGBA);
			}
		}
		// Go to next vertex 
		pVert += vbuf->getVertexSize();
	}
	vbuf->unlock();

    // Set material
    this->setMaterial("GPUBillboardWithGS");
}

void GPUBillboardSet::createVertexDataForVertexShaderOnly(const std::vector<Bundler::Vertex>& vertices)
{
	// Setup render operation
	mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST; 
	mRenderOp.vertexData = OGRE_NEW Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = vertices.size() * 4; 
	mRenderOp.vertexData->vertexStart = 0; 
	mRenderOp.useIndexes = true; 
	mRenderOp.indexData = OGRE_NEW Ogre::IndexData();
	mRenderOp.indexData->indexCount = vertices.size() * 6;
	mRenderOp.indexData->indexStart = 0;

	// Vertex format declaration
	unsigned short sourceBufferIdx = 0;
	Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
    size_t currOffset = 0;
	decl->addElement(sourceBufferIdx, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	decl->addElement(sourceBufferIdx, currOffset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
	decl->addElement(sourceBufferIdx, currOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

	// Create vertex buffer
    Ogre::HardwareVertexBufferSharedPtr vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		decl->getVertexSize(sourceBufferIdx),
        mRenderOp.vertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    // Bind vertex buffer
    Ogre::VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;
	bind->setBinding(sourceBufferIdx, vbuf);

	// Fill vertex buffer (see http://www.ogre3d.org/docs/manual/manual_59.html#SEC287)
	Ogre::RenderSystem* renderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();
	unsigned char* pVert = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
	Ogre::Real* pReal;
	Ogre::RGBA* pRGBA;
	Ogre::VertexDeclaration::VertexElementList elems = decl->findElementsBySource(sourceBufferIdx);
	Ogre::VertexDeclaration::VertexElementList::iterator itr;

	const Ogre::Vector2 uvs[4] = {	Ogre::Vector2( -1.f, 1.f ),
									Ogre::Vector2( -1.f, -1.f ),
									Ogre::Vector2( 1.f, -1.f ),
									Ogre::Vector2( 1.f, 1.f ) };
	for (unsigned int i=0; i<vertices.size(); ++i )
	{
		const Bundler::Vertex& vertex = vertices[i];
		for ( unsigned int j=0; j<4; j++ )
		{
			for (itr=elems.begin(); itr!=elems.end(); ++itr)
			{
				Ogre::VertexElement& elem = *itr;
				if (elem.getSemantic() == Ogre::VES_POSITION)
				{
					elem.baseVertexPointerToElement(pVert, &pReal);
					*pReal = vertex.position.x; *pReal++;
					*pReal = vertex.position.y; *pReal++;
					*pReal = vertex.position.z; *pReal++;
				}
				else if (elem.getSemantic() == Ogre::VES_DIFFUSE)
				{
					elem.baseVertexPointerToElement(pVert, &pRGBA);
					renderSystem->convertColourValue(vertex.color, pRGBA);
				}
				else if (elem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES && elem.getIndex() == 0)
				{
					elem.baseVertexPointerToElement(pVert, &pReal);
					*pReal = uvs[j].x; *pReal++;
					*pReal = uvs[j].y; *pReal++;
				}
			}
			// Go to next vertex 
			pVert += vbuf->getVertexSize();
		}
	}
	vbuf->unlock();

	// Create index buffer
	if (mRenderOp.indexData->indexCount>=65536)
	{
		Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer( 
			Ogre::HardwareIndexBuffer::IT_32BIT, 
			mRenderOp.indexData->indexCount, 
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

		mRenderOp.indexData->indexBuffer = ibuf;
		Ogre::uint32* indices = static_cast<Ogre::uint32*>(ibuf->lock( Ogre::HardwareBuffer::HBL_DISCARD));

		Ogre::uint32 indexFirstVertex = 0;
		const Ogre::uint32 inds[6] = {	0, 1, 2, 3, 0, 2 };
		for (unsigned int i=0; i<vertices.size(); ++i)
		{
			for (unsigned int j=0; j<6; ++j)
			{
				*indices = indexFirstVertex + inds[j];
				indices++;
			}
			indexFirstVertex +=4;
		}
		ibuf->unlock();
	}
	else
	{
		Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_16BIT, 
			mRenderOp.indexData->indexCount, 
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

		mRenderOp.indexData->indexBuffer = ibuf;
		Ogre::uint16* indices = static_cast<Ogre::uint16*>( ibuf->lock( Ogre::HardwareBuffer::HBL_DISCARD ) );

		Ogre::uint32 indexFirstVertex = 0;
		const Ogre::uint16 inds[6] = {	0, 1, 2, 3, 0, 2 };
		for ( unsigned int i=0; i<vertices.size(); ++i )
		{
			for ( unsigned int j=0; j<6; ++j )
			{
				*indices = indexFirstVertex + inds[j];
				indices++;
			}
			indexFirstVertex +=4;
		}
		ibuf->unlock();
	}

    // Set material
    this->setMaterial("GPUBillboard");
}

void GPUBillboardSet::updateBoundingBoxAndSphereFromBillboards(const std::vector<Bundler::Vertex>& vertices)
{
	Ogre::AxisAlignedBox box = calculateBillboardsBoundingBox(vertices);
	setBoundingBox(box);

	Ogre::Vector3 vmax = box.getMaximum();
	Ogre::Vector3 vmin = box.getMinimum();
	Ogre::Real sqLen = std::max(vmax.squaredLength(), vmin.squaredLength());
	mBBRadius = Ogre::Math::Sqrt(sqLen);
}

Ogre::AxisAlignedBox GPUBillboardSet::calculateBillboardsBoundingBox(const std::vector<Bundler::Vertex>& vertices)
{
	Ogre::AxisAlignedBox box;
	for (std::size_t i=0; i<vertices.size(); ++i)
		box.merge(vertices[i].position);	
	return box;
}

void GPUBillboardSet::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Ogre::Matrix4::IDENTITY;
}

Ogre::Real GPUBillboardSet::getSquaredViewDepth(const Ogre::Camera* cam) const
{
	Ogre::Vector3 min, max, mid, dist;
	min = mBox.getMinimum();
	max = mBox.getMaximum();
	mid = ((max - min) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;

	return dist.squaredLength();
}

const Ogre::String& GPUBillboardSet::getMovableType(void) const
{
	return mMovableType; 
}

void GPUBillboardSet::setBillboardSize(Ogre::Vector2 size)
{
	// The billboard size is stored as custom parameter #0 of this renderable.
	// In "Vertex shader only" mode, the parameter is used by the vertex program,
	// In "Vertex+Geometry shader" mode, it is used by the geometry program.
	setCustomParameter(0, Ogre::Vector4(size.x, size.y, 0.f, 0.f));
	mBillboardSize = size;
}
Ogre::Vector2 GPUBillboardSet::getBillboardSize() const 
{ 
	return mBillboardSize; 
}

Ogre::Real GPUBillboardSet::getBoundingRadius() const 
{ 
	return mBBRadius;
}
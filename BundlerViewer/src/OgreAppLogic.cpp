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

#include "OgreAppLogic.h"
#include "OgreApp.h"
#include <Ogre.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp> 

#include "StatsFrameListener.h"

using namespace Ogre;

namespace bf = boost::filesystem;

OgreAppLogic::OgreAppLogic() : mApplication(0)
{
	// ogre
	mSceneMgr		= 0;
	mViewport		= 0;
	mCamera         = 0;
	mCameraNode     = 0;
	mObjectNode     = 0;
	mCameraIndex    = 0;
	mStatsFrameListener = 0;
	mAnimState = 0;
	mTimeUntilNextToggle = 0;
	mDistanceFromCamera = 40000.0f;
	mOISListener.mParent = this;
	mDensePointCloud = NULL;
	mDensePointCloudFilePath = "";
	mBillboardSize = 0.01f;
	mBillboardInterval = 0.0005f;
	mIsSparsePointCloudVisible = true;
	mIsCameraPlaneVisible = false;
}

OgreAppLogic::~OgreAppLogic()
{}

// preAppInit
bool OgreAppLogic::preInit(const Ogre::StringVector &commandArgs)
{
	mProjectPath = commandArgs[0];
	std::stringstream bundlerFilePath;
	std::stringstream pictureFilePath;
	std::stringstream densePointCloudFilePath;

	bundlerFilePath         << mProjectPath << "\\bundler_output\\bundle.out";
	pictureFilePath         << mProjectPath << "\\bundler_tmp\\list.txt";
	densePointCloudFilePath << mProjectPath << "\\pmvs\\models\\mesh.ply";

	if (bf::exists(densePointCloudFilePath.str()))
		mDensePointCloudFilePath = densePointCloudFilePath.str();

	mBundlerParser = new Bundler::Parser(bundlerFilePath.str(), pictureFilePath.str());	

	return true;
}

// postAppInit
bool OgreAppLogic::init(void)
{
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mProjectPath, "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	createSceneManager();
	createViewport();
	createCamera();
	createScene();
			
	createCameraPlane();

	mStatsFrameListener = new StatsFrameListener(mApplication->getRenderWindow());
	mApplication->getOgreRoot()->addFrameListener(mStatsFrameListener);
	mStatsFrameListener->showDebugOverlay(true);

	mApplication->getKeyboard()->setEventCallback(&mOISListener);
	mApplication->getMouse()->setEventCallback(&mOISListener);

	return true;
}

bool OgreAppLogic::preUpdate(Ogre::Real deltaTime)
{
	return true;
}

bool OgreAppLogic::update(Ogre::Real deltaTime)
{
	if (mAnimState)
		mAnimState->addTime(deltaTime);

	bool result = processInputs(deltaTime);
	return result;
}

void OgreAppLogic::shutdown(void)
{
	mApplication->getOgreRoot()->removeFrameListener(mStatsFrameListener);
	delete mStatsFrameListener;
	mStatsFrameListener = 0;
	
	if(mSceneMgr)
		mApplication->getOgreRoot()->destroySceneManager(mSceneMgr);
	mSceneMgr = 0;
}

void OgreAppLogic::postShutdown(void)
{

}

//--------------------------------- Init --------------------------------

void OgreAppLogic::createSceneManager(void)
{
	mSceneMgr = mApplication->getOgreRoot()->createSceneManager(ST_GENERIC, "SceneManager");
}

void OgreAppLogic::createViewport(void)
{
	mViewport = mApplication->getRenderWindow()->addViewport(0);
}

void OgreAppLogic::createCamera(void)
{
	mCamera = mSceneMgr->createCamera("camera");
	mCamera->setNearClipDistance(0.5);
	mCamera->setFarClipDistance(50000);
	mCamera->setFOVy(Degree(40)); //FOVy camera Ogre = 40°
	mCamera->setAspectRatio((float) mViewport->getActualWidth() / (float) mViewport->getActualHeight());	
	mViewport->setCamera(mCamera);
	mViewport->setBackgroundColour(Ogre::ColourValue::White);
}

void OgreAppLogic::createScene(void)
{
	SceneNode* root = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	SceneNode* pointCloud = root->createChildSceneNode();

	const RenderSystemCapabilities* caps = Ogre::Root::getSingletonPtr()->getRenderSystem()->getCapabilities();
	bool useGeometryShader = caps->hasCapability(RSC_GEOMETRY_PROGRAM);
	mSparsePointCloud = new GPUBillboardSet("sparsePointCloud", mBundlerParser->getVertices(), useGeometryShader);
	pointCloud->attachObject(mSparsePointCloud);
	if (mDensePointCloudFilePath != "")
	{		
		mDensePointCloud = new GPUBillboardSet("densePointCloud", Bundler::importPly(mDensePointCloudFilePath).vertices, useGeometryShader);
		pointCloud->attachObject(mDensePointCloud);
		mIsSparsePointCloudVisible = false;
		mSparsePointCloud->setVisible(false);
	}
	
	SceneNode* objets = root->createChildSceneNode();
	mCameraNode = objets->createChildSceneNode();
	mCameraNode->attachObject(mCamera);
}

void OgreAppLogic::createCameraPlane()
{
	createCameraMaterial();

	Plane p(Vector3::UNIT_Z, 0.0);
	MeshManager::getSingleton().createPlane("VerticalPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, p , 1, 1, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);
	Entity* planeEntity = mSceneMgr->createEntity("CameraPlane", "VerticalPlane"); 
	planeEntity->setMaterialName("CameraPlaneMaterial");
	planeEntity->setRenderQueueGroup(RENDER_QUEUE_2);

	// Create a node for the plane, inserts it in the scene
	mCameraPlaneNode = mCameraNode->createChildSceneNode("planeNode");
	mCameraPlaneNode->attachObject(planeEntity);

	// Update position    
	mCameraPlaneNode->setPosition(0, 0, -mDistanceFromCamera);
}

//--------------------------------- update --------------------------------

bool OgreAppLogic::processInputs(Ogre::Real deltaTime)
{
	const Degree ROT_SCALE = Degree(40.0f);
	const Real MOVE_SCALE = 2.0;
	Vector3 translateVector(Vector3::ZERO);
	Degree rotX(0);
	Degree rotY(0);
	OIS::Keyboard *keyboard = mApplication->getKeyboard();
	OIS::Mouse *mouse = mApplication->getMouse();

	if(keyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		return false;
	}

	//////// moves  //////

	// keyboard moves
	if(keyboard->isKeyDown(OIS::KC_A))
		translateVector.x = -MOVE_SCALE;	// Move camera left

	if(keyboard->isKeyDown(OIS::KC_D))
		translateVector.x = +MOVE_SCALE;	// Move camera RIGHT

	if(keyboard->isKeyDown(OIS::KC_UP) || keyboard->isKeyDown(OIS::KC_W) )
		translateVector.z = -MOVE_SCALE;	// Move camera forward

	if(keyboard->isKeyDown(OIS::KC_DOWN) || keyboard->isKeyDown(OIS::KC_S) )
		translateVector.z = +MOVE_SCALE;	// Move camera backward

	if(keyboard->isKeyDown(OIS::KC_PGUP))
		translateVector.y = +MOVE_SCALE;	// Move camera up

	if(keyboard->isKeyDown(OIS::KC_PGDOWN))
		translateVector.y = -MOVE_SCALE;	// Move camera down

	if(keyboard->isKeyDown(OIS::KC_RIGHT))
		rotX -= ROT_SCALE;					// Turn camera right

	if(keyboard->isKeyDown(OIS::KC_LEFT))
		rotX += ROT_SCALE;					// Turn camea left


	rotX *= deltaTime;
	rotY *= deltaTime;
	translateVector *= deltaTime;

	// mouse moves
	const OIS::MouseState &ms = mouse->getMouseState();
	if (ms.buttonDown(OIS::MB_Right))
	{
		translateVector.x += ms.X.rel * 0.005f * MOVE_SCALE;	// Move camera horizontaly
		translateVector.y -= ms.Y.rel * 0.005f * MOVE_SCALE;	// Move camera verticaly
	}
	else
	{
		rotX += Degree(-ms.X.rel * 0.005f * ROT_SCALE);		// Rotate camera horizontaly
		rotY += Degree(-ms.Y.rel * 0.005f * ROT_SCALE);		// Rotate camera verticaly
	}

	mCameraNode->translate(mCameraNode->getOrientation()*translateVector);
	mCameraNode->yaw(rotX);
	mCameraNode->pitch(rotY);

	if (keyboard->isKeyDown(OIS::KC_ADD) && mTimeUntilNextToggle <= 0)
	{
		nextCamera();
		mTimeUntilNextToggle = 0.2f;
	}
	else if (keyboard->isKeyDown(OIS::KC_SUBTRACT) && mTimeUntilNextToggle <= 0)
	{
		previousCamera();
		mTimeUntilNextToggle = 0.2f;
	}
	else if (keyboard->isKeyDown(OIS::KC_O) && mTimeUntilNextToggle <= 0)
	{
		increaseBillboardSize();
		mTimeUntilNextToggle = 0.1f;
	}
	else if (keyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
	{
		decreaseBillboardSize();
		mTimeUntilNextToggle = 0.1f;
	}
	else if (keyboard->isKeyDown(OIS::KC_SPACE) && mTimeUntilNextToggle <= 0)
	{
		toggleSparseDensePointCloud();
		mTimeUntilNextToggle = 0.2f;
	}
	else if (keyboard->isKeyDown(OIS::KC_C) && mTimeUntilNextToggle <= 0)
	{
		toggleCameraPlaneVisibility();
		mTimeUntilNextToggle = 0.2f;
	}

	if (mTimeUntilNextToggle >= 0)
		mTimeUntilNextToggle -= deltaTime;

	return true;
}

bool OgreAppLogic::OISListener::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool OgreAppLogic::OISListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool OgreAppLogic::OISListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool OgreAppLogic::OISListener::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool OgreAppLogic::OISListener::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

void OgreAppLogic::nextCamera()
{
	mCameraIndex++;
	if (mCameraIndex >= (int)mBundlerParser->getNbCamera())
		mCameraIndex = 0;

	setCamera(mCameraIndex);
}

void OgreAppLogic::previousCamera()
{
	mCameraIndex--;
	if (mCameraIndex < 0)
		mCameraIndex = mBundlerParser->getNbCamera()-1;

	setCamera(mCameraIndex);
}

void OgreAppLogic::setCamera(unsigned int index)
{
	std::cout << "setCamera("<<index<<")" << std::endl;
	const Bundler::Camera& cam = mBundlerParser->getCamera(index);
	Ogre::Matrix3 rot = cam.rotation.Transpose();
	Ogre::Vector3 pos = -rot*cam.translation;
	mCameraNode->setPosition(pos);
	mCameraNode->setOrientation(rot);

	adaptPlaneToCamera(cam);
}

void OgreAppLogic::increaseBillboardSize()
{
	mBillboardSize += mBillboardInterval;

	setBillboardSize(mBillboardSize);
}

void OgreAppLogic::decreaseBillboardSize()
{
	mBillboardSize -= mBillboardInterval;
	if (mBillboardSize < 0)
		mBillboardSize = mBillboardInterval;

	setBillboardSize(mBillboardSize);
}

void OgreAppLogic::setBillboardSize(Ogre::Real size)
{
	mSparsePointCloud->setBillboardSize(size*Ogre::Vector2::UNIT_SCALE);
	if (mDensePointCloud)
		mDensePointCloud->setBillboardSize(size*Ogre::Vector2::UNIT_SCALE);
}

void OgreAppLogic::toggleSparseDensePointCloud()
{
	if (mDensePointCloud)
	{
		mIsSparsePointCloudVisible = !mIsSparsePointCloudVisible;
		if (mIsSparsePointCloudVisible)
		{
			mSparsePointCloud->setVisible(true);
			mDensePointCloud->setVisible(false);
		}
		else
		{
			mSparsePointCloud->setVisible(false);
			mDensePointCloud->setVisible(true);
		}
	}
}

void OgreAppLogic::toggleCameraPlaneVisibility()
{
	mIsCameraPlaneVisible = !mIsCameraPlaneVisible;
	mCameraPlaneNode->setVisible(mIsCameraPlaneVisible);
	
	if (mIsCameraPlaneVisible)
		setCamera(mCameraIndex);
}

void OgreAppLogic::adaptPlaneToCamera(const Bundler::Camera& cam)
{
	//load image
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->load(cam.filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	float width  = (float) tex->getWidth();
	float height = (float) tex->getHeight();

	//compute fov from image size and camera focal
	Ogre::Radian fovy = 2.0f * Ogre::Math::ATan(height / (2.0f*cam.focalLength));

	//update plane dimension with texture dimension
	float videoAspectRatio = width / height;
	float planeHeight = 2 * mDistanceFromCamera * Ogre::Math::Tan(fovy*0.5);
	float planeWidth  = planeHeight * videoAspectRatio;
	mCameraPlaneNode->setScale(planeWidth, planeHeight, 1.0f);

	//update plane with current texture
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingletonPtr()->getByName("CameraPlaneMaterial");
	Ogre::TextureUnitState* unit = material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	unit->setTextureName(tex->getName());
	material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("distort1", (float) cam.radialDistort1);
	material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("distort2", (float) cam.radialDistort2);
	material->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("focal",    (float) cam.focalLength);
}

void OgreAppLogic::createCameraMaterial()
{
	//Load Dummy Texture
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->load("dummy.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	
	//Create Material
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("CameraPlaneMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->createTextureUnitState(tex->getName());
	material->getTechnique(0)->getPass(0)->setVertexProgram("RadialUndistort_vp");
	material->getTechnique(0)->getPass(0)->setFragmentProgram("RadialUndistort_fp");
}
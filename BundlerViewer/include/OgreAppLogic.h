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

#ifndef OGREAPPLOGIC_H
#define OGREAPPLOGIC_H

#include <OgrePrerequisites.h>
#include <OgreStringVector.h>
#include <OIS/OIS.h>

#include "BundlerParser.h"
#include "GPUBillboardSet.h"

class OgreApp;
class StatsFrameListener;

class OgreAppLogic
{
public:
	OgreAppLogic();
	~OgreAppLogic();

	void setParentApp(OgreApp *app) { mApplication = app; }

	/// Called before Ogre and everything in the framework is initialized
	/// Configure the framework here
	bool preInit(const Ogre::StringVector &commandArgs);
	/// Called when Ogre and the framework is initialized
	/// Init the logic here
	bool init(void);

	/// Called before everything in the framework is updated
	bool preUpdate(Ogre::Real deltaTime);
	/// Called when the framework is updated
	/// update the logic here
	bool update(Ogre::Real deltaTime);

	/// Called before Ogre and the framework are shut down
	/// shutdown the logic here
	void shutdown(void);
	/// Called when Ogre and the framework are shut down
	void postShutdown(void);

protected:
	void createSceneManager(void);
	void createViewport(void);
	void createCamera(void);
	void createScene(void);
	
	
	bool processInputs(Ogre::Real deltaTime);

	// OGRE
	OgreApp *mApplication;
	Ogre::SceneManager *mSceneMgr;
	Ogre::Viewport *mViewport;
	Ogre::Camera *mCamera;
	Ogre::SceneNode* mCameraNode;
	Ogre::SceneNode* mObjectNode;
	Ogre::AnimationState* mAnimState;
	Ogre::SceneNode* mAxesNode;
	Ogre::SceneNode* mCameraPlaneNode;
	Ogre::Real mDistanceFromCamera;
	Bundler::Parser* mBundlerParser;
	GPUBillboardSet* mSparsePointCloud;
	GPUBillboardSet* mDensePointCloud;
	std::string mProjectPath;
	std::string mDensePointCloudFilePath;

	void adaptPlaneToCamera(const Bundler::Camera& cam);
	void createCameraPlane();
	void createCameraMaterial();	

	void nextCamera();
	void previousCamera();
	void setCamera(unsigned int index);
	int mCameraIndex;

	void increaseBillboardSize();
	void decreaseBillboardSize();
	void setBillboardSize(Ogre::Real size);
	Ogre::Real mBillboardSize;
	Ogre::Real mBillboardInterval;

	void toggleSparseDensePointCloud();
	bool mIsSparsePointCloudVisible;

	void toggleCameraPlaneVisibility();
	bool mIsCameraPlaneVisible;

	Ogre::Real mTimeUntilNextToggle;
	StatsFrameListener *mStatsFrameListener;

	// OIS
	class OISListener : public OIS::MouseListener, public OIS::KeyListener
	{
	public:
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		OgreAppLogic *mParent;
	};
	friend class OISListener;
	OISListener mOISListener;
};

#endif // OGREAPPLOGIC_H
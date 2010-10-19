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

#ifndef OGREAPP_H
#define OGREAPP_H

#include <OgrePrerequisites.h>
#include <OgreStringVector.h>
#include <OgreSingleton.h>
#include <OIS/OIS.h>

class OgreAppLogic;

class OgreApp : public Ogre::Singleton<OgreApp>
{
public:

	OgreApp();
	~OgreApp();
	
	void setAppLogic(OgreAppLogic *appLogic);

	void run(void);
	bool update(Ogre::Real deltaTime);
	void notifyWindowMetrics(Ogre::RenderWindow* rw, int left, int top, int width, int height);
	void notifyWindowClosed(Ogre::RenderWindow* rw);

	void setCommandLine(const Ogre::String &commandLine);
	void setCommandLine(int argc, char *argv[]);

	inline Ogre::RenderWindow *getRenderWindow(void) const { return mWindow; }
	inline Ogre::Root *getOgreRoot(void) const { return mRoot; }

	inline OIS::Mouse *getMouse(void) const { return mMouse; }
	inline OIS::Keyboard *getKeyboard(void) const { return mKeyboard; }
protected:

	bool init(void);
	void shutdown(void);

	void createInputDevices(bool exclusive);
	void updateInputDevices(void);
	void destroyInputDevices(void);

	void setupResources(void);

	// Config
	bool mInitialisationBegun;
	bool mInitialisationComplete;

	Ogre::String mCommandLine;
	Ogre::StringVector mCommandLineArgs;

	// OGRE
	Ogre::Root *mRoot;
	Ogre::FrameListener *mFrameListener;
	Ogre::RenderWindow *mWindow;

	// OIS
	OIS::InputManager *mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;  

	// Classes supplied by the user
	OgreAppLogic *mAppLogic;
};

#endif // OGREAPP_H
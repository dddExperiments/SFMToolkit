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

#ifndef OGREOGREAPPFRAMELISTENER_H
#define OGREOGREAPPFRAMELISTENER_H

#include <Ogre.h>

class OgreApp;

class OgreAppFrameListener : public Ogre::FrameListener, public Ogre::WindowEventListener
{
public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	OgreAppFrameListener(OgreApp* app);
	virtual ~OgreAppFrameListener();

	//Adjust mouse clipping area
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowMoved(Ogre::RenderWindow* rw);

	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(Ogre::RenderWindow* rw);

	// Override frameStarted event to process that (don't care about frameEnded)
	//bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
	OgreApp *mApplication;
	bool mWindowClosed;
};

#endif // OGREOGREAPPFRAMELISTENER_H
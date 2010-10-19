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

#include "OgreAppFrameListener.h"
#include "OgreApp.h"
#include <Ogre.h>

using namespace Ogre;

// Constructor takes a RenderWindow because it uses that to determine input context
OgreAppFrameListener::OgreAppFrameListener(OgreApp* app) : mApplication(app)
{
	Ogre::RenderWindow *window = mApplication->getRenderWindow();

	if(window)
	{
		windowResized(window);
		WindowEventUtilities::addWindowEventListener(window, this);
	}

	mWindowClosed = false;
}

OgreAppFrameListener::~OgreAppFrameListener()
{
	Ogre::RenderWindow *window = mApplication->getRenderWindow();

	WindowEventUtilities::removeWindowEventListener(window, this);
	if(!window->isClosed())
		windowClosed(window);
}


//Adjust mouse clipping area
void OgreAppFrameListener::windowResized(RenderWindow* rw)
{
	windowMoved(rw);
}

void OgreAppFrameListener::windowMoved(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);
	mApplication->notifyWindowMetrics(rw, left, top, width, height);
}

//Unattach OIS before window shutdown (very important under Linux)
void OgreAppFrameListener::windowClosed(RenderWindow* rw)
{
	mApplication->notifyWindowClosed(rw);
	mWindowClosed = true;
}

// Override frameStarted event to process that (don't care about frameEnded)
//bool OgreAppFrameListener::frameStarted(const FrameEvent& evt)
bool OgreAppFrameListener::frameRenderingQueued(const FrameEvent& evt)
{
	if(mWindowClosed)
		return false;

	if(!mApplication->update(evt.timeSinceLastFrame))
		return false;

	return true;
}

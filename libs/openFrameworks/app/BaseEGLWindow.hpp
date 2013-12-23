/*==============================================================================

 Copyright (c) 2013 Andreas Bergmeier

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

 ==============================================================================*/

#pragma once

#include "ofBaseApp.h"

#include "ofAppBaseWindow.h"

#include <map>

namespace of {
	using std::map;

	typedef map<EGLint,EGLint> EGLAttributeList;
	typedef map<EGLint,EGLint>::iterator EGLAttributeListIterator;

	extern string eglErrorString( EGLint err );

	class BaseEGLWindow : public ofAppBaseWindow {
	private:
		typedef ofAppBaseWindow __base;
	public:
		virtual ~BaseEGLWindow() noexcept = default;

		virtual void setGLESVersion( int glesVersion );
		virtual void setupOpenGL(int w, int h, int screenMode) = 0;

		struct Settings {
			EGLAttributeList frameBufferAttributes;
			// surface creation
			EGLAttributeList windowSurfaceAttributes;

			ofColor initialClearColor;

			int screenNum;
			int layer;
		};

		virtual int  getWindowMode(); // TODO use enum

		virtual void hideCursor();
		virtual void showCursor();

		virtual void setFullscreen( bool fullscreen );
		virtual void toggleFullscreen();

		virtual void setVerticalSync( bool enabled );

	//------------------------------------------------------------
	// EGL
	//------------------------------------------------------------

		EGLDisplay getEglDisplay() const;
		EGLSurface getEglSurface() const;
		EGLContext getEglContext() const;
		EGLConfig  getEglConfig() const;

		EGLint getEglVersionMajor () const;
		EGLint getEglVersionMinor() const;
	protected:
		BaseEGLWindow( int glesVersion ) noexcept;
		BaseEGLWindow( int glesVersion, Settings settings ) noexcept;

		void idle();

		bool     terminate;

		bool     bNewScreenMode;

		virtual bool createSurface();
		virtual bool destroySurface();
		int glesVersion;

		bool isSurfaceInited;

		bool bShowCursor;

		int      nFramesSinceResized;
	private:
		// bool resizeSurface();

		EGLDisplay eglDisplay;  // EGL display connection
		EGLSurface eglSurface;
		EGLContext eglContext;

		EGLConfig eglConfig;

		EGLint eglVersionMajor;
		EGLint eglVersionMinor;

	//------------------------------------------------------------
	// WINDOWING
	//------------------------------------------------------------
	protected:
		int         windowMode;
		// EGL window

		virtual EGLNativeWindowType getNativeWindow() = 0;
		virtual EGLNativeDisplayType getNativeDisplay() = 0;
	private:
		Settings 			settings;
	protected:
		ofBaseApp* ofAppPtr;
	};
} // namespace of


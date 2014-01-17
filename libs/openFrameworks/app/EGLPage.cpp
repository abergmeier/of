/*==============================================================================

 Copyright (c) 2013, 2014 Andreas Bergmeier <abergmeier@gmx.net>

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

#include "EGLPage.hpp"

#include <stdexcept>
#include <emscripten/emscripten.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <SDL/SDL.h>
#include <ofGLProgrammableRenderer.h>

using std::runtime_error;

// FIXME: I belong in a $%&§ header!
void ofGLReadyCallback();

using namespace of::emscripten;

extern "C" {
	void _emscripten_show_mouse();
	void emscripten_set_canvas_opacity( float opacity );
}

//-------------------------------------------------------------------------------------
EGLPage::Settings::Settings() {
	eglCanvasOpacity = 255;
}

EGLPage::EGLPage() noexcept :
	BaseEGLWindow( 2 ), _canvasOpacity{ numeric_limits<float>::infinity() }
{
}

EGLPage::EGLPage( Settings settings ) :
	BaseEGLWindow( 2, settings ),
	_canvasOpacity{ settings.eglCanvasOpacity / 255.0f }
{
}

EGLPage::~EGLPage() noexcept {
	SDL_Quit();
}

void
EGLPage::setGLESVersion( int glesVersion ) {
	if( glesVersion < 2 || glesVersion > 3 )
		throw runtime_error( "Invalid GLES version" );
	base_type::setGLESVersion( glesVersion );
}

namespace {
	template< ofMouseEventArgs::Type Type >
	void
	signal( const SDL_MouseButtonEvent& sdl_event ) {
		auto of_event = ofMouseEventArgs{};
		of_event.button = sdl_event.button;
		of_event.type   = Type;
		of_event.x      = sdl_event.x;
		of_event.y      = sdl_event.y;
		ofNotifyMouseEvent( of_event );
	};

	template< ofMouseEventArgs::Type Type >
	void
	signal( const SDL_MouseMotionEvent& sdl_event ) {
		auto of_event = ofMouseEventArgs{};
		of_event.type   = Type;
		of_event.x      = sdl_event.x;
		of_event.y      = sdl_event.y;
		ofNotifyMouseEvent( of_event );
	};

	template< ofKeyEventArgs::Type Type >
	void
	signal( const SDL_KeyboardEvent& sdl_event ) {
		auto of_event = ofKeyEventArgs{};
		of_event.key  = sdl_event.keysym.sym;
		of_event.type = Type;
		ofNotifyKeyEvent( of_event );
	}

	template< ofTouchEventArgs::Type Type >
	ofTouchEventArgs
	convert( const SDL_TouchFingerEvent& sdl_event ) {
		auto of_event = ofTouchEventArgs{};
		of_event.x          = sdl_event.x;
		of_event.y          = sdl_event.y;
		of_event.pressure   = sdl_event.pressure;
		of_event.id         = sdl_event.touchId;
		of_event.numTouches = 1;
		return of_event;
	}
}

//------------------------------------------------------------
void
EGLPage::checkEvents() {

	auto sdl_event = SDL_Event{};
	while( SDL_PollEvent( &sdl_event ) > 0 ) {
		switch( sdl_event.type ) {
		case SDL_KEYUP:
			signal<ofKeyEventArgs::Type::Released  >( sdl_event.key    );
			break;
		case SDL_KEYDOWN:
			signal<ofKeyEventArgs::Type::Pressed   >( sdl_event.key    );
			break;
		case SDL_MOUSEMOTION:
			signal<ofMouseEventArgs::Type::Moved   >( sdl_event.motion );
			break;
		case SDL_MOUSEBUTTONDOWN:
			signal<ofMouseEventArgs::Type::Pressed >( sdl_event.button );
			break;
		case SDL_MOUSEBUTTONUP:
			signal<ofMouseEventArgs::Type::Released>( sdl_event.button );
			break;
		case SDL_MOUSEWHEEL:
			break;
		case SDL_FINGERMOTION:
			convert<ofTouchEventArgs::Type::move>( sdl_event.tfinger );
			break;
		case SDL_FINGERDOWN:
			convert<ofTouchEventArgs::Type::down>( sdl_event.tfinger );
			break;
		case SDL_FINGERUP:
			convert<ofTouchEventArgs::Type::up>( sdl_event.tfinger );
			break;
		}
	}
}

namespace {
	function<void()> main_callback;

	void
	invoke_main_callback() {
		main_callback();
	}
}

//------------------------------------------------------------
EGLNativeWindowType
EGLPage::getNativeWindow() {

	// Emscripten ignores the window
	return 1;
}

//------------------------------------------------------------
EGLNativeDisplayType
EGLPage::getNativeDisplay() {

	return EGL_DEFAULT_DISPLAY;
}



//------------------------------------------------------------
void
EGLPage::setupOpenGL( int w, int h, int screenMode ) {

	// we set this here, and if we need to make a fullscreen
	// app, we do it during the first loop.
	windowMode = screenMode;

	nonFullscreenSize[0] = w;
	nonFullscreenSize[1] = h;

	switch( windowMode ) {
	case OF_GAME_MODE:
		ofLogWarning( "of::emscripten::EGLPage" ) << "setupOpenGL(): OF_GAME_MODE not supported, using OF_WINDOW";
		break;
	case OF_FULLSCREEN:
		ofLogWarning( "of::emscripten::EGLPage" ) << "setupOpenGL(): Setting fullscreen (OF_FULLSCREEN) not supported without user interaction, using OF_WINDOW";
		break;
	}

	isSurfaceInited = createSurface();

	if( _canvasOpacity < 1.0f )
		emscripten_set_canvas_opacity( _canvasOpacity );

	if(!isSurfaceInited) {
		ofLogError("of::emscripten::EGLPage")  << "setupOpenGL(): screen creation failed, canvas not inited";
	}

	setupPeripherals();

	ofLogNotice( "of::emscripten::EGLPage" ) << "setupOpenGL(): peripheral setup complete";
	ofGLReadyCallback();
}

//------------------------------------------------------------
void
EGLPage::setupPeripherals() {

	{
		int flags = 0;
#ifdef SDL_INIT_EVENTS
		flags |= SDL_INIT_EVENTS;
#endif
		// Necessary so SDL events are reported
		if( SDL_Init(flags) != 0 )
			ofLogFatalError( "of::emscripten::EGLPage" ) << "setupPeripherals: SDL_Init failed - " << SDL_GetError();
	}

#if 0
	// roll our own cursor!
	mouseCursor.allocate(mouse_cursor_data.width,mouse_cursor_data.height,OF_IMAGE_COLOR_ALPHA);
	MOUSE_CURSOR_RUN_LENGTH_DECODE(mouseCursor.getPixels(),mouse_cursor_data.rle_pixel_data,mouse_cursor_data.width*mouse_cursor_data.height,mouse_cursor_data.bpp);
	mouseCursor.update();
#endif
	ofLogNotice( "of::emscripten::EGLPage" ) << "setupPeripherals(): peripheral setup complete";
#if 0
	setupNativeEvents();
#endif
	ofLogNotice( "of::emscripten::EGLPage" ) << "setupPeripherals(): native event setup complete";
}

//------------------------------------------------------------
void
EGLPage::runAppViaInfiniteLoop( ofBaseApp *appPtr ) {
	ofLogNotice("of::emscripten::EGLPage") << "runAppViaInfiniteLoop(): entering infinite loop";

	ofAppPtr = appPtr; // make a local copy

	ofNotifySetup();
	ofLogNotice("of::emscripten::EGLPage") << "runAppViaInfiniteLoop(): setting up notifications complete";

	main_callback = bind( &EGLPage::main, this );

	// Emulate loop via callbacks
	emscripten_set_main_loop( invoke_main_callback, -1, 1 );
}

void
EGLPage::main() {
	if( terminate ) {
		ofLogNotice("of::emscripten::EGLPage") << "runAppViaInfiniteLoop(): exiting infinite loop";
		emscripten_cancel_main_loop();
		return;
	}

	checkEvents();
	idle();
	display();
}

#if 0
//------------------------------------------------------------
void ofAppEGLWindow::setOrientation(ofOrientation orientationIn){
  orientation = orientationIn;
}
#endif

//------------------------------------------------------------
ofOrientation
EGLPage::getOrientation() {

	//TODO: Implement actual values
	auto value = 0;
	auto alpha = value / 360.0f;
	const auto alpha_cos = cos(alpha);
	const auto alpha_range = acos(2*PI/8);

	if( alpha_cos >= alpha_range )
		return OF_ORIENTATION_DEFAULT;

	if( alpha_cos <= -alpha_range )
		return OF_ORIENTATION_180;

	if( alpha > 180 )
		return OF_ORIENTATION_90_LEFT;

	return OF_ORIENTATION_90_RIGHT;
}

//------------------------------------------------------------
bool
EGLPage::doesHWOrientation() {
	return base_type::doesHWOrientation();
}

//------------------------------------------------------------
void
EGLPage::display() {

	// take care of any requests for a new screen mode
	if( windowMode != OF_GAME_MODE && bNewScreenMode ) {
		switch( windowMode ) {
		case OF_FULLSCREEN:
			// not supported!
			break;
		case OF_WINDOW:
			// Necessary to get mouse events via SDL
			SDL_SetVideoMode( nonFullscreenSize[0],
			                  nonFullscreenSize[1],
			                  0,
			                  SDL_HWSURFACE | SDL_OPENGL );
			break;
		};

		bNewScreenMode = false;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// set viewport, clear the screen

	auto renderer = ofGetGLProgrammableRenderer();
	if( renderer )
		renderer->startRender();

	{
		auto width = int{},
		     height = int{};

		auto isFullscreen = int{};
		emscripten_get_canvas_size( &width, &height, &isFullscreen );

		ofViewport( 0, 0, width, height, false );    // used to be glViewport( 0, 0, width, height );
	}

	auto bgPtr = ofBgColorPtr();
	auto bClearAuto = ofbClearBg();

	if( bClearAuto == true || ofGetFrameNum() < 3 )
		ofClear(bgPtr[0]*255,bgPtr[1]*255,bgPtr[2]*255, bgPtr[3]*255);

#if 0
	if( bEnableSetupScreen ) ofSetupScreen(); // this calls into the current renderer (ofSetupScreenPerspective)
#endif

	ofNotifyDraw();

	if( bShowCursor ) {

		auto bIsDepthTestEnabled = GLboolean{ GL_FALSE };
		glGetBooleanv( GL_DEPTH_TEST, &bIsDepthTestEnabled );

		if( bIsDepthTestEnabled == GL_TRUE )
			glDisable( GL_DEPTH_TEST );

		auto isUsingNormalizedTexCoords = ofGetUsingNormalizedTexCoords();
		if( isUsingNormalizedTexCoords )
			ofDisableNormalizedTexCoords();

		ofPushStyle();
		ofEnableAlphaBlending();
		ofDisableTextureEdgeHack();
		ofSetColor(255);
#if 0
		mouseCursor.draw(ofGetMouseX(),ofGetMouseY());
#endif
		ofEnableTextureEdgeHack();
		//TODO: we need a way of querying the previous state of texture hack
		ofPopStyle();

		if( bIsDepthTestEnabled == GL_TRUE )
			glEnable( GL_DEPTH_TEST );

		if( isUsingNormalizedTexCoords )
			ofEnableNormalizedTexCoords();

	}

	if( renderer )
		renderer->finishRender();

	auto success = eglSwapBuffers( getEglDisplay(), getEglSurface() );
	if( !success ) {
		auto error = eglGetError();
		ofLogNotice("of::emscripten::EGLPage") << "display(): eglSwapBuffers failed: " << eglErrorString(error);
	}

	nFramesSinceResized++;
}

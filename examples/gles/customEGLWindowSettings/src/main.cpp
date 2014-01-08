#include "ofMain.h"
#include "ofApp.h"
#ifdef TARGET_EMSCRIPTEN
#include "EGLPage.hpp"
using EGLWindow = of::emscripten::EGLPage;
namespace {
	constexpr const auto WINDOW_MODE = ofWindowMode::OF_WINDOW;

	EGLWindow::Settings
	create_settings( unsigned short opacity ) {
		auto settings = EGLWindow::Settings{};
		settings.eglCanvasOpacity = opacity;
		return settings;
	}
}
#else
#include "ofAppEGLWindow.h"
using EGLWindow = ofAppEGLWindow;
namespace {
	constexpr const auto WINDOW_MODE = ofWindowMode::OF_FULLSCREEN;

	EGLWindow::Settings
	create_settings( unsigned short opacity ) {
		auto settings = EGLWindow::Settings{};
		settings.eglWindowOpacity = opacity;
		return settings;
	}
}
#endif

//========================================================================
int main( ){

	auto settings = create_settings( 127 );

        settings.frameBufferAttributes[EGL_DEPTH_SIZE]   = 0; // 0 bits for depth
        settings.frameBufferAttributes[EGL_STENCIL_SIZE] = 0; // 0 bits for stencil
	
	auto window = make_unique<EGLWindow>( settings );

	ofSetupOpenGL( std::move(window), 1024, 768, WINDOW_MODE );// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( make_unique<ofApp>());

}

#include "BaseEGLWindow.hpp"

#include <ofGraphics.h>

using namespace of;

// from http://cantuna.googlecode.com/svn-history/r16/trunk/src/screen.cpp
#define CASE_STR(x,y) case x: str = y; break

static std::string eglErrorString(EGLint err) {
    std::string str;
    switch (err) {
        CASE_STR(EGL_SUCCESS, "no error");
        CASE_STR(EGL_NOT_INITIALIZED, "EGL not, or could not be, initialized");
        CASE_STR(EGL_BAD_ACCESS, "access violation");
        CASE_STR(EGL_BAD_ALLOC, "could not allocate resources");
        CASE_STR(EGL_BAD_ATTRIBUTE, "invalid attribute");
        CASE_STR(EGL_BAD_CONTEXT, "invalid context specified");
        CASE_STR(EGL_BAD_CONFIG, "invald frame buffer configuration specified");
        CASE_STR(EGL_BAD_CURRENT_SURFACE, "current window, pbuffer or pixmap surface is no longer valid");
        CASE_STR(EGL_BAD_DISPLAY, "invalid display specified");
        CASE_STR(EGL_BAD_SURFACE, "invalid surface specified");
        CASE_STR(EGL_BAD_MATCH, "bad argument match");
        CASE_STR(EGL_BAD_PARAMETER, "invalid paramater");
        CASE_STR(EGL_BAD_NATIVE_PIXMAP, "invalid NativePixmap");
        CASE_STR(EGL_BAD_NATIVE_WINDOW, "invalid NativeWindow");
        CASE_STR(EGL_CONTEXT_LOST, "APM event caused context loss");
        default: (str = "unknown error ") += err; break;
    }
    return str;
}

BaseEGLWindow::BaseEGLWindow( int glesVersion ) noexcept:
	bNewScreenMode( true ), glesVersion( glesVersion ), settings(),
	ofAppPtr( nullptr )
{
}

BaseEGLWindow::BaseEGLWindow( int glesVersion, Settings settings ) noexcept:
	bNewScreenMode( true ), glesVersion( glesVersion ), settings( std::move(settings) ),
	ofAppPtr( nullptr )
{
}

void
BaseEGLWindow::setGLESVersion( int _glesVersion ) {
	glesVersion = _glesVersion;
}

//------------------------------------------------------------
bool
BaseEGLWindow::createSurface() {

  EGLNativeWindowType nativeWindow = getNativeWindow();
  EGLNativeDisplayType display = getNativeDisplay();

  ofLogNotice("of::BaseEGLWindow") << "createSurface(): setting up EGL Display";
    // get an EGL eglDisplay connection

    isSurfaceInited = false;

    EGLint result;
#if 0 // following code is superfluous
    if(display==0){
      eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }else{
#endif
      eglDisplay = eglGetDisplay(display);
#if 0
    }
#endif

    if(eglDisplay == EGL_NO_DISPLAY) {
      ofLogNotice("of::BaseEGLWindow") << "createSurface(): eglGetDisplay returned: " << eglDisplay;
     return false;
    }else{
      ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL Display correctly set";
    }

    // initialize the EGL eglDisplay connection
    result = eglInitialize(eglDisplay,
                           &eglVersionMajor,
                           &eglVersionMinor);

    if(result == EGL_BAD_DISPLAY) {
//  eglDisplay is not an EGL connection
        ofLogError("of::BaseEGLWindow") << "createSurface(): eglInitialize returned EGL_BAD_DISPLAY";
        return false;
    } else if(result == EGL_NOT_INITIALIZED) {
        // eglDisplay cannot be intitialized
        ofLogError("of::BaseEGLWindow") << "createSurface(): eglInitialize returned EGL_NOT_INITIALIZED";
        return false;
    } else if(result == EGL_FALSE) {
        // eglinitialize was not initialiezd
        ofLogError("of::BaseEGLWindow") << "createSurface(): eglInitialize returned EGL_FALSE";
        return false;
    } else {
        // result == EGL_TRUE
        // success!
    }

    EGLint glesVersion;
    int glesVersionForContext;

    if(ofGetCurrentRenderer()) {
      ofLogNotice("of::BaseEGLWindow") << "createSurface(): current renderer type: " << ofGetCurrentRenderer()->getType();
    } else {
      ofLogNotice("of::BaseEGLWindow") << "createSurface(): no current renderer selected";
    }

    if(this->glesVersion==2){
      glesVersion = EGL_OPENGL_ES2_BIT;
      glesVersionForContext = 2;
        ofLogNotice("of::BaseEGLWindow") << "createSurface(): GLES2 renderer detected";
    }else{
      glesVersion = EGL_OPENGL_ES_BIT;
      glesVersionForContext = 1;
      ofLogNotice("of::BaseEGLWindow") << "createSurface(): default renderer detected";
    }

    EGLAttributeListIterator iter, iterEnd;
    int i;

    // each attribute has 2 values, and we need one extra for the EGL_NONE terminator
    EGLint attribute_list_framebuffer_config[settings.frameBufferAttributes.size() * 2 + 3];

    iter = settings.frameBufferAttributes.begin();
    iterEnd = settings.frameBufferAttributes.end();
    i = 0;
    for(; iter != iterEnd; iter++) {
        attribute_list_framebuffer_config[i++] = iter->first;
        attribute_list_framebuffer_config[i++] = iter->second;
    }
	attribute_list_framebuffer_config[i++] = EGL_RENDERABLE_TYPE;
	attribute_list_framebuffer_config[i++] = glesVersion; //openGL ES version
    attribute_list_framebuffer_config[i] = EGL_NONE; // add the terminator

    EGLint num_configs;

    // get an appropriate EGL frame buffer configuration
    // http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/eglChooseConfig.html
    result = eglChooseConfig(eglDisplay,
                             attribute_list_framebuffer_config,
                             &eglConfig,
                             1, // we only want the first one.  if we want more,
                                // we need to pass in an array.
                                // we are optimistic and don't give it more chances
                                // to find a good configuration
                             &num_configs);

    if(result == EGL_FALSE) {
        EGLint error = eglGetError();
        ofLogError("of::BaseEGLWindow") << "createSurface(): error finding valid configuration based on settings: " << eglErrorString(error);
        return false;
    }

    if(num_configs <= 0 || eglConfig == NULL) {
        ofLogError("of::BaseEGLWindow") << "createSurface(): no matching configs were found, num_configs: " << num_configs;
        return false;
    }


    // each attribute has 2 values, and we need one extra for the EGL_NONE terminator
    EGLint attribute_list_window_surface[settings.windowSurfaceAttributes.size() * 2 + 1];

    iter = settings.windowSurfaceAttributes.begin();
    iterEnd = settings.windowSurfaceAttributes.end();

    i = 0;
    for(; iter != iterEnd; iter++) {
        attribute_list_window_surface[i++] = iter->first;
        attribute_list_window_surface[i++] = iter->second;
    }
    attribute_list_window_surface[i] = EGL_NONE; // add the terminator

    // create a surface
    eglSurface = eglCreateWindowSurface( eglDisplay, // our display handle
                                         eglConfig,    // our first config
                                         nativeWindow, // our native window
                                         attribute_list_window_surface); // surface attribute list

    if(eglSurface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        switch(error) {
            case EGL_BAD_MATCH:
                ofLogError("of::BaseEGLWindow") << "createSurface(): error creating surface: EGL_BAD_MATCH " << eglErrorString(error);
                ofLogError("of::BaseEGLWindow") << "createSurface(): check window and EGLConfig attributes to determine compatibility, ";
                ofLogError("of::BaseEGLWindow") << "createSurface(): or verify that the EGLConfig supports rendering to a window";
                 break;
            case EGL_BAD_CONFIG:
                ofLogError("of::BaseEGLWindow") << "createSurface(): error creating surface: EGL_BAD_CONFIG " << eglErrorString(error);
                ofLogError("of::BaseEGLWindow") << "createSurface(): verify that provided EGLConfig is valid";
                 break;
            case EGL_BAD_NATIVE_WINDOW:
                ofLogError("of::BaseEGLWindow") << "createSurface(): error creating surface: EGL_BAD_NATIVE_WINDOW " << eglErrorString(error);
                ofLogError("of::BaseEGLWindow") << "createSurface(): verify that provided EGLNativeWindow is valid";
                 break;
            case EGL_BAD_ALLOC:
                ofLogError("of::BaseEGLWindow") << "createSurface(): error creating surface: EGL_BAD_ALLOC " << eglErrorString(error);
                ofLogError("of::BaseEGLWindow") << "createSurface(): not enough resources available";
                 break;
             default:
              ofLogError("of::BaseEGLWindow") << "createSurface(): error creating surface: << " << error << eglErrorString(error);
           }

        return false;
    }else{
        ofLogNotice("of::BaseEGLWindow") << "createSurface(): surface created correctly";
    }

  // get an appropriate EGL frame buffer configuration
  result = eglBindAPI(EGL_OPENGL_ES_API);

  if(result == EGL_FALSE) {
      ofLogError("of::BaseEGLWindow") << "createSurface(): error binding API: " << eglErrorString(eglGetError());
      return false;
  }else{
      ofLogNotice("of::BaseEGLWindow") << "createSurface(): API bound correctly";
  }

  // create an EGL rendering eglContext
  EGLint attribute_list_surface_context[] = {
    EGL_CONTEXT_CLIENT_VERSION, glesVersionForContext,
    EGL_NONE
  };

    eglContext = eglCreateContext(eglDisplay,
                                  eglConfig,
                                  EGL_NO_CONTEXT,
                                  attribute_list_surface_context);

    if(eglContext == EGL_NO_CONTEXT) {
       EGLint error = eglGetError();
       if(error == EGL_BAD_CONFIG) {
            ofLogError("of::BaseEGLWindow") << "createSurface(): error creating context: EGL_BAD_CONFIG " << eglErrorString(error);
            return false;
       } else {
            ofLogError("of::BaseEGLWindow") << "createSurface(): error creating context: " << error << " " << eglErrorString(error);
            return false;
       }
    }

    // connect the eglContext to the eglSurface
    result = eglMakeCurrent(eglDisplay,
                            eglSurface, // draw surface
                            eglSurface, // read surface
                            eglContext);

    if(eglContext == EGL_FALSE) {
        EGLint error = eglGetError();
        ofLogError("of::BaseEGLWindow") << "createSurface(): couldn't making current surface: " << eglErrorString(error);
        return false;
    }

    // Set background color and clear buffers
    glClearColor(settings.initialClearColor.r / 255.0f,
                 settings.initialClearColor.g / 255.0f,
                 settings.initialClearColor.b / 255.0f,
                 settings.initialClearColor.a / 255.0f);
    glClear( GL_COLOR_BUFFER_BIT );
    glClear( GL_DEPTH_BUFFER_BIT );

    ofLogNotice("of::BaseEGLWindow") << "createSurface(): -----EGL-----";
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL_VERSION_MAJOR = " << eglVersionMajor;
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL_VERSION_MINOR = " << eglVersionMinor;
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL_CLIENT_APIS = " << eglQueryString(eglDisplay, EGL_CLIENT_APIS);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL_VENDOR = "  << eglQueryString(eglDisplay, EGL_VENDOR);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL_VERSION = " << eglQueryString(eglDisplay, EGL_VERSION);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): EGL_EXTENSIONS = " << eglQueryString(eglDisplay, EGL_EXTENSIONS);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): GL_RENDERER = " << glGetString(GL_RENDERER);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): GL_VERSION  = " << glGetString(GL_VERSION);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): GL_VENDOR   = " << glGetString(GL_VENDOR);
    ofLogNotice("of::BaseEGLWindow") << "createSurface(): -------------";

    isSurfaceInited = true;

    return true;
}

//------------------------------------------------------------
bool
BaseEGLWindow::destroySurface() {
    if(isSurfaceInited) {
        ofLogNotice("of::BaseEGLWindow") << "destroySurface(): destroying EGL surface";
        eglSwapBuffers(eglDisplay, eglSurface);
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(eglDisplay, eglSurface);
        eglDestroyContext(eglDisplay, eglContext);
        eglTerminate(eglDisplay);
        isSurfaceInited = false;

        eglDisplay = NULL;
        eglSurface = NULL;
        eglContext = NULL;
        eglConfig  = NULL;
        eglVersionMinor = -1;
        eglVersionMinor = -1;

        return true;
    } else {
        ofLogError("of::BaseEGLWindow") << "destroySurface(): attempted to destroy uninitialized window";
        return false;
    }
}

//------------------------------------------------------------
EGLDisplay
BaseEGLWindow::getEglDisplay() const {
	return eglDisplay;
}

//------------------------------------------------------------
EGLSurface
BaseEGLWindow::getEglSurface() const {
	return eglSurface;
}

//------------------------------------------------------------
void
BaseEGLWindow::hideCursor(){
	bShowCursor = false;
}

//------------------------------------------------------------
void
BaseEGLWindow::showCursor(){
	bShowCursor = true;
}

//------------------------------------------------------------
int
BaseEGLWindow::getWindowMode() {
	return windowMode;
}

//------------------------------------------------------------
void
BaseEGLWindow::toggleFullscreen(){
  if( windowMode == OF_GAME_MODE) return;

  if( windowMode == OF_WINDOW ){
    setFullscreen(true);
  }else{
    setFullscreen(false);
  }

}

//------------------------------------------------------------
void
BaseEGLWindow::setFullscreen(bool fullscreen){
    if( windowMode == OF_GAME_MODE) return;

    if(fullscreen && windowMode != OF_FULLSCREEN){
        bNewScreenMode  = true;
        windowMode      = OF_FULLSCREEN;
    }else if(!fullscreen && windowMode != OF_WINDOW) {
        bNewScreenMode  = true;
        windowMode      = OF_WINDOW;
    }
}

//------------------------------------------------------------
void
BaseEGLWindow::idle() {
  ofNotifyUpdate();
}

//------------------------------------------------------------
void
BaseEGLWindow::setVerticalSync(bool enabled){
  eglSwapInterval(eglDisplay, enabled ? 1 : 0);
}

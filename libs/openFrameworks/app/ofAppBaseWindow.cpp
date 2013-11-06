
#include "ofAppBaseWindow.h"

#include <ofEvents.h>
#include <ofLog.h>

void
ofAppBaseWindow::processFrame( std::function<int()> kbhit, std::function<int()> getch ) noexcept {
	/// listen for escape
#ifdef TARGET_WIN32
	if (GetAsyncKeyState(VK_ESCAPE))
		ofNotifyKeyPressed(OF_KEY_ESC);
#endif

#if defined TARGET_OSX || defined TARGET_LINUX
	while ( kbhit() )
	{
		int key = getch();
		if ( key == 27 )
		{
			ofNotifyKeyPressed(OF_KEY_ESC);
		}
		else if ( key == /* ctrl-c */ 3 )
		{
			ofLogNotice( "ofAppBaseWindow" ) << "Ctrl-C pressed" << endl;
			OF_EXIT_APP(0);
		}
		else
		{
			ofNotifyKeyPressed(key);
		}
	}
#endif

	ofNotifyUpdate();
	ofNotifyDraw();
}

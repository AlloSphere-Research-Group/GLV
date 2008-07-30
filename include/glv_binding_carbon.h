#ifndef INC_GLV_BINDING_CARBON_H
#define INC_GLV_BINDING_CARBON_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#define GLV_CARBON_PATH_DEFAULT_FONT	"/Library/Frameworks/GLV.framework/Versions/A/Resources/ProFontWindows.ttf"

typedef ::Rect CarbonRect;

#include "glv_core.h"
#include "glv_abstract_binding.h"

//#include <Carbon/Carbon.h>
#include <AGL/agl.h>
#include <map>
#include <string>

namespace glv {	

class Application : public AbstractApplication
{
public:

	static void			run();
	static void			quit();

protected:

	static EventHandlerRef *	carbonApplicationHandlerRef;
	static OSStatus		carbonApplicationHandler(EventHandlerCallRef nextHandler, EventRef e, void * udata);

};

#define CARBON_TITLEBARHEIGHT (24)

class Window : public AbstractWindow
{
public:

	Window(unsigned int width=800, unsigned int height=600, char * title="GLV Window", GLV * glv=0, double framerate=40);
	virtual ~Window();
	
protected:
	virtual void platformResize(int width, int height);		// called at tail of AbstractWindow::resize()
	virtual void platformShowHide();						// called at tail of AbstractWindow::show()

	// Carbon window pointer:
	WindowRef			windowRef;
	
	// Carbon event system callbacks:
	EventLoopTimerUPP	timerUPP;	
	static OSStatus		carbonWindowHandler(EventHandlerCallRef nextHandler, EventRef e, void * udata);	
	static pascal void	carbonWindowDrawHandler(EventLoopTimerRef inTimer, void * userData);
	
	// OpenGL driver & state:
	AGLContext			aglcontext;
	CarbonRect			rectport;
	GDHandle			graphicsdevice;
	GLint				attrib[64];
	AGLPixelFormat		pixelformat;
	
	std::map<UInt32, std::string>	eventClassDescription;
	std::map<UInt32, std::string>	eventKindDescription;
};

} // namespace

#endif

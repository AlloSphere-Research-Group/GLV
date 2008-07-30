#ifndef INC_GLV_BINDING_FLTK_H
#define INC_GLV_BINDING_FLTK_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_abstract_binding.h"

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

//#define GLV_KEY_BACKSPACE	FL_BackSpace
//#define GLV_KEY_DELETE		FL_Delete
//#define GLV_KEY_ENTER		FL_Enter
//#define GLV_KEY_ESC			FL_Escape
//#define GLV_KEY_TAB			FL_Tab
//#define GLV_KEY_PAUSE		FL_Pause
//#define GLV_KEY_SCROLL_LOCK	FL_Scroll_Lock
//#define GLV_KEY_HOME		FL_Home
//#define GLV_KEY_LEFT		FL_Left
//#define GLV_KEY_UP			FL_Up
//#define GLV_KEY_RIGHT		FL_Right
//#define GLV_KEY_DOWN		FL_Down
//#define GLV_KEY_PAGE_UP		FL_Page_Up
//#define GLV_KEY_PAGE_DOWN	FL_Page_Down
//#define GLV_KEY_END			FL_End
//#define GLV_KEY_PRINT		FL_Print
//#define GLV_KEY_INSERT		FL_Insert
//#define GLV_KEY_NUM_LOCK	FL_Num_Lock
//#define GLV_KEY_F			FL_F		/* use GLV_KEY_F + n for function key n */

namespace glv {	

class Application : public AbstractApplication{
public:
	static void	run();
	static void	quit();
protected:

};

class Window : public AbstractWindow, public Fl_Gl_Window{
public:
	Window(unsigned int width = 800, unsigned int height = 600, char * title = "GLV Window", double framerate = 40.0);
	virtual ~Window();

protected:
	bool mFullscreenDeferred;
	void draw();								// FLTK draw callback
	int handle(int event);						// FLTK event callback
	void resize(int l, int t, int w, int h);	// FLTK resize callback
	void fltkFullscreen();						// FLTK fullscreen called at start of draw loop
	static void scheduleDraw(void * userdata);
	
	virtual void platformFullscreen();						// defers fullscreen to draw loop for thread safety
	virtual void platformResize(int width, int height);		// called at tail of AbstractWindow::resize()
	virtual void platformShowHide();						// called at tail of AbstractWindow::show()
};

} // namespace

#endif


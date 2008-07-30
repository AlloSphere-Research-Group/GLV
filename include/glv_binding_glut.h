#ifndef INC_GLV_BINDING_GLUT_H
#define INC_GLV_BINDING_GLUT_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_abstract_binding.h"

#if defined (__APPLE__) || defined (OSX)
	#include <GLUT/glut.h>
#endif
#if defined(__linux__)
	#include <GL/glut.h>
#endif
#ifdef WIN32
	#include <GL/glut.h>
#endif

namespace glv {

class Application : public AbstractApplication{
public:
	static void	run();
	static void	quit();
	
protected:
};

class Window : public AbstractWindow{
public:
	Window(
		unsigned int width=800, unsigned int height=600, 
		char * title="GLV Window", GLV * glv=0, double framerate=40, int mode=DefaultBuf
	);
	virtual ~Window();

	void draw();	// GLUT draw function

protected:
	
	virtual void platformFullscreen();
	virtual void platformHideCursor(bool hide);
	virtual void platformResize(int width, int height);		// called at tail of AbstractWindow::resize()
	virtual void platformShowHide();						// called at tail of AbstractWindow::show()
};

} // namespace

#endif


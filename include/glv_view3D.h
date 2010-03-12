#ifndef INC_GLV_VIEW3D_H
#define INC_GLV_VIEW3D_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {

class View3D : public View {
public:
	View3D(const Rect& r=Rect()): View(r){}
	virtual ~View3D(){}
	
	virtual void onDraw3D(){}

protected:

	virtual void onDraw(){
	
		using namespace glv::draw;
	
		// Viewport needs to be in absolute screen coordinates, 
		// so we need to convert from relative...
		space_t ax,ay;
		const View * top = posAbs(ax,ay);

		// y starts at top in GLV, but at bottom in screen coords, so need to flip
		ay = top->h - (ay + h);
	
		//create viewport just at widget location
		glViewport((int)ax, (int)ay, (int)w, (int)h);
		
		//save openGL state
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		draw::enable(ScissorTest);
		scissor(ax, ay, w, h);
		clearColor(colors().back);
		clear(ColorBufferBit | DepthBufferBit);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)w/(GLfloat)h, 1.0f, 100.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Do all 3D drawing
		onDraw3D();

		//return openGL state
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glPopAttrib();
		
		//return viewport to being entire top-level rect
		glViewport(0, 0, (int)top->w, (int)top->h);
	}
};


} // glv::
#endif

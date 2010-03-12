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
	
	virtual void onDraw3D() = 0;

protected:

	virtual void onDraw();
};


} // glv::
#endif

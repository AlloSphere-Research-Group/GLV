#ifndef INC_GLV_VIEW3D_H
#define INC_GLV_VIEW3D_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {

class View3D : public View {
public:
	View3D(const Rect& r=Rect());
	virtual ~View3D(){}
	
	virtual void onDraw3D() = 0;

	void far(double v){ mFar=v; }
	void near(double v){ mNear=v; }
	void viewAngle(double v){ mFOVY=v; }

	double far() const { return mFar; }
	double near() const { return mNear; }
	double viewAngle() const { return mFOVY; }

protected:

	virtual void onDraw();
	
	double mNear, mFar, mFOVY;
};


} // glv::
#endif

#ifndef INC_GLV_VIEW3D_H
#define INC_GLV_VIEW3D_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {

class View3D : public View {
public:
	View3D(const Rect& r=Rect(0));
	virtual ~View3D(){}
	
	/// 3D drawing callback
	virtual void onDraw3D() = 0;

	void far(double v){ mFar=v; }				///< Set far clip distance
	void near(double v){ mNear=v; }				///< Set near clip distance
	void fovy(double v){ mFOVY=v; }				///< Set field of view angle, in degrees, in the y direction

	double far() const { return mFar; }			///< Get far clip distance
	double near() const { return mNear; }		///< Get near clip distance
	double fovy() const { return mFOVY; }		///< Get field of view angle, in degrees, in the y direction

protected:
	virtual void onDraw();
	double mNear, mFar, mFOVY;
};


} // glv::
#endif

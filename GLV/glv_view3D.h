#ifndef INC_GLV_VIEW3D_H
#define INC_GLV_VIEW3D_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {

/// View for three-dimensional rendering
class View3D : public View {
public:

	/// @param[in] r	geometry
	View3D(const Rect& r=Rect(0));
	virtual ~View3D(){}

	virtual const char * className() const { return "View3D"; }

	/// 2D drawing callback called after 3D
	virtual void onDraw2D(GLV& g){}
	
	/// 3D drawing callback
	virtual void onDraw3D(GLV& g){}

	float far() const { return mFar; }			///< Get far clip distance
	float near() const { return mNear; }		///< Get near clip distance
	float fovy() const { return mFOVY; }		///< Get field of view angle, in degrees, in the y direction

	void far(float v){ mFar=v; }				///< Set far clip distance
	void near(float v){ mNear=v; }				///< Set near clip distance
	void fovy(float v){ mFOVY=v; }				///< Set field of view angle, in degrees, in the y direction

	/// Set 4x4 model transform matrix (column-major)
	template <class T>
	void modelView(T * mat4x4){
		for(int i=0; i<16; ++i) mModelView[i]=mat4x4[i];
	}
	
	/// Get 4x4 model transform matrix
	float * modelView(){ return mModelView; }

	void resetModelView();

protected:
	virtual void onDraw(GLV& g);
	float mNear, mFar, mFOVY;
	float mModelView[16];
	
//	struct ViewModel : public Model{
//		ViewModel(View3D& self): v(self){}
//
//		virtual std::string modelToToken(){
//			std::string s;
//			addKeyValue(s, "near", v.mNear);
//			addKeyValue(s, "far", v.mFar);
//			addKeyValue(s, "fovy", v.mFOVY);
//			return s;
//		}
//
//		virtual int modelFromString(const std::string& s){
//			getValue(s, "near", v.mNear);
//			getValue(s, "far", v.mFar);
//			getValue(s, "fovy", v.mFOVY);
//		}
//		View3D& v;
//	};
};


} // glv::
#endif

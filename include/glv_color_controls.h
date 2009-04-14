#ifndef INC_GLV_COLOR_CONTROLS_H
#define INC_GLV_COLOR_CONTROLS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_sliders.h"

namespace glv {

class ColorSliders : public View{
public:

	ColorSliders(const Rect& r, bool isHSV=true, bool controlsAlpha=false)
	: View(r), mStyle(false), mIsHSV(isHSV), mControlsAlpha(controlsAlpha)
	{
		int num = mControlsAlpha ? 4 : 3;
		mComp1.anchor(0,0./num).stretch(1,1./num).set(0,0./num,0,0);
		mComp2.anchor(0,1./num).stretch(1,1./num).set(0,1./num,0,0);
		mComp3.anchor(0,2./num).stretch(1,1./num).set(0,2./num,0,0);
		mCompA.anchor(0,3./num).stretch(1,1./num).set(0,3./num,0,0);
		mComp1.value(1).style(&mStyle).attach(updateComp, Update::Value, this);
		mComp2.value(1).style(&mStyle).attach(updateComp, Update::Value, this);
		mComp3.value(1).style(&mStyle).attach(updateComp, Update::Value, this);
		mCompA.value(1).style(&mStyle).attach(updateComp, Update::Value, this);
		setColor();
		(*this) << mComp1 << mComp2 << mComp3 << mCompA;
		mCompA.property(Visible, mControlsAlpha);
	}
	
	bool isHSV() const { return mIsHSV; }
	const Color& value() const { return mColor; }
	ColorSliders& value(const Color& c){ mColor=c; return *this; }
	
protected:
	Color mColor;
	Style mStyle;
	Slider mComp1, mComp2, mComp3, mCompA;
	bool mIsHSV, mControlsAlpha;

	void setColor(){
		float c1=mComp1.value(), c2=mComp2.value(), c3=mComp3.value();
		isHSV() ? value(HSV(c1,c2,c3)) : value(Color(c1,c2,c3));
		if(mControlsAlpha) mColor.a = mCompA.value();
		mStyle.color.fore = mColor;
		//value().print();
	}

	static void updateComp(const Notification& n){
		ColorSliders& c = *(ColorSliders *)n.receiver();
		c.setColor();
	}
};

} // glv::

#endif

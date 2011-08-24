#ifndef INC_GLV_COLOR_CONTROLS_H
#define INC_GLV_COLOR_CONTROLS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_sliders.h"

namespace glv {

class ColorSliders : public Sliders{
public:

	using Sliders::setValue;

	ColorSliders(const Rect& r=Rect(100, 10*3), bool isHSV=true, bool controlsAlpha=false)
	:	Sliders(r, 1, controlsAlpha ? 4:3),
		mSliderStyle(false), mIsHSV(isHSV), mControlsAlpha(controlsAlpha)
	{
		style(&mSliderStyle);
		attach(ntUpdateStyle, Update::Value, this);
		setStyle();
	}
	
	bool isHSV() const { return mIsHSV; }

	Color getValue() const {
		Color col;
		float c1 = data().at<float>(0);
		float c2 = data().at<float>(1);
		float c3 = data().at<float>(2);
		if(isHSV()) col = HSV(c1,c2,c3);
		else		col.set(c1,c2,c3);
		col.a = mControlsAlpha ? data().at<float>(3) : 1;
		return col;
	}

	ColorSliders& setValue(const Color& v){
		float c1,c2,c3;
		if(isHSV()){ HSV h=v; c1=h.h; c2=h.s; c3=h.v; }
		else{ c1=v.r; c2=v.g; c3=v.b; }

		setValue(c1,0);
		setValue(c2,1);
		setValue(c3,2);
		if(mControlsAlpha) setValue(v.a,3);

		setStyle();
		return *this;
	}

	ColorSliders& setValue(const HSV& v){
		float c1,c2,c3;
		if(isHSV()){ c1=v.h; c2=v.s; c3=v.v; }
		else{ Color c=v; c1=c.r; c2=c.g; c3=c.b; }

		setValue(c1,0);
		setValue(c2,1);
		setValue(c3,2);

		setStyle();
		return *this;
	}

	virtual const char * className() const { return "ColorSliders"; }
	
protected:
	Color mColor;
	Style mSliderStyle;
	bool mIsHSV, mControlsAlpha;

	void setStyle(){
		Color col = getValue();
		mSliderStyle.color.fore = col;
		//mSliderStyle.color.back = Color(1-col.luminance());
		//mSliderStyle.color.back = Color(col.luminance() > 0.5f ? 0:1);
		mSliderStyle.color.back = Color(0);
	}

	static void ntUpdateStyle(const Notification& n){
		ColorSliders& c = *n.receiver<ColorSliders>();
		c.setStyle();
	}
};




//class ColorSliders : public View{
//public:
//
//	ColorSliders(const Rect& r=Rect(100, 10*3), bool isHSV=true, bool controlsAlpha=false)
//	: Widget(r), mSliderStyle(false), mIsHSV(isHSV), mControlsAlpha(controlsAlpha)
//	{
//		int num = mControlsAlpha ? 4 : 3;
//		mComp1.anchor(0,0./num).stretch(1,1./num).set(0,0./num,0,0);
//		mComp2.anchor(0,1./num).stretch(1,1./num).set(0,1./num,0,0);
//		mComp3.anchor(0,2./num).stretch(1,1./num).set(0,2./num,0,0);
//		mCompA.anchor(0,3./num).stretch(1,1./num).set(0,3./num,0,0);
//		mComp1.setValue(1).style(&mSliderStyle).attach(updateComp, Update::Value, this);
//		mComp2.setValue(1).style(&mSliderStyle).attach(updateComp, Update::Value, this);
//		mComp3.setValue(1).style(&mSliderStyle).attach(updateComp, Update::Value, this);
//		mCompA.setValue(1).style(&mSliderStyle).attach(updateComp, Update::Value, this);
//		setColor();
//		(*this) << mComp1 << mComp2 << mComp3 << mCompA;
//		mCompA.property(Visible, mControlsAlpha);
//
//		data().resize(Data::FLOAT, 4);
//		//data().set(mColor.components, 4);
//	}
//	
//	bool isHSV() const { return mIsHSV; }
//	const Color& getValue() const { return mColor; }
//	ColorSliders& setValue(const Color& v){ setColor(v); return *this; }
//	ColorSliders& setValue(const HSV& v){ setColor(v); return *this; }
//
//	virtual const glv::Data& getData(Data& d) const {
//		glv::Data& md = const_cast<glv::Data&>(data());
//		if(isHSV()){
//			HSV h = mColor;
//			const float v[4] = {h.h, h.s, h.v, mColor.a};
//			md.assignFromArray(v, 4);
//		}
//		else{
//			md.assignFromArray(mColor.components, 4);
//		}		
//		
//		return data();
//	}
//
//	virtual void setData(const glv::Data& v){
//		Color c;
//		if(isHSV()){
//			c = HSV(v.at<float>(0), v.at<float>(1), v.at<float>(2));
//			c.a = v.at<float>(3);
//		}
//		else{
//			c = Color(v.at<float>(0), v.at<float>(1), v.at<float>(2), v.at<float>(3));
//		}
//		setColor(c);
//	}
//
//	virtual const char * className() const { return "ColorSliders"; }
//	
//protected:
//	Color mColor;
//	Style mSliderStyle;
//	Slider mComp1, mComp2, mComp3, mCompA;
//	bool mIsHSV, mControlsAlpha;
//
//	void setColor(const Color& v){
//		float c1,c2,c3;
//		if(isHSV()){ HSV h=v; c1=h.h; c2=h.s; c3=h.v; }
//		else{ c1=v.r; c2=v.g; c3=v.b; }
//		// assuming value() does not send notification...
//		mComp1.setValue(c1); mComp2.setValue(c2); mComp3.setValue(c3);
//		mCompA.setValue(v.a);
//		setColor();
//	}
//
//	void setColor(const HSV& v){
//		float c1,c2,c3;
//		if(isHSV()){ c1=v.h; c2=v.s; c3=v.v; }
//		else{ Color c=v; c1=c.r; c2=c.g; c3=c.b; }
//		// assuming value() does not send notification...
//		mComp1.setValue(c1); mComp2.setValue(c2); mComp3.setValue(c3);
//		//mCompA.value(v.a);
//		setColor();
//	}
//
//	void setColor(){
//		float c1=mComp1.getValue(), c2=mComp2.getValue(), c3=mComp3.getValue();
//		if(isHSV()){	mColor = HSV(c1,c2,c3);}
//		else{			mColor.set(c1,c2,c3);}
//		if(mControlsAlpha) mColor.a = mCompA.getValue();
//		mSliderStyle.color.fore = mColor;
//		mSliderStyle.color.back = Color(1-mColor.luminance());
//		//mSliderStyle.color.back = Color(mColor.luminance() > 0.5f ? 0:1);
//		//value().print();
//	}
//
//	static void updateComp(const Notification& n){
//		ColorSliders& c = *n.receiver<ColorSliders>();
//		c.setColor();
//	}
//};

} // glv::

#endif

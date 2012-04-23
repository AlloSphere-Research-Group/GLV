#ifndef INC_GLV_COLOR_CONTROLS_H
#define INC_GLV_COLOR_CONTROLS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_sliders.h"

namespace glv {

/// Color picker in HSV space
class ColorPicker : public View{
public:

	/// @param[in] r			geometry
	/// @param[in] hueHeight	height of hue slider
	ColorPicker(const Rect& r=Rect(60), float hueHeight=12);

	/// Get value
	HSV getValue() const;
	
	/// Set value
	ColorPicker& setValue(const HSV& v);

	virtual const Data& getData(Data& temp) const;
	virtual void setData(const Data& d);

	virtual const char * className() const { return "ColorPicker"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	Slider mCtrlH;
	Slider2D mCtrlSV;
};


/// Color sliders in HSV or RGB space
class ColorSliders : public Sliders{
public:

	using Sliders::setValue;

	/// @param[in] r			geometry
	/// @param[in] isHSV		whether to control HSV (versus RGB)
	/// @param[in] useAlpha		whether to include an extra slider for alpha
	ColorSliders(const Rect& r=Rect(100, 10*3), bool isHSV=true, bool useAlpha=false);
	
	bool isHSV() const { return mIsHSV; }

	/// Get value
	Color getValue() const;

	/// Set value
	ColorSliders& setValue(const Color& v);

	/// Set value
	ColorSliders& setValue(const HSV& v);

	virtual const char * className() const { return "ColorSliders"; }
	
protected:
	Color mColor;
	Style mSliderStyle;
	bool mIsHSV, mUseAlpha;

	void setStyle(){
		Color col = getValue();
		mSliderStyle.color.fore = col;
		//mSliderStyle.color.back = Color(1-col.luminance());
		//mSliderStyle.color.back = Color(col.luminance() > 0.5f ? 0:1);
		mSliderStyle.color.back = Color(0,0,0, Style::standard().color.back.a);
	}

	static void ntUpdateStyle(const Notification& n){
		ColorSliders& c = *n.receiver<ColorSliders>();
		c.setStyle();
	}
};



} // glv::

#endif

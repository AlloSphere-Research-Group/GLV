/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_color_controls.h"

namespace glv{

ColorPicker::ColorPicker(const Rect& r, float hueHeight)
:	View(r),
	mCtrlH(Rect(0,0,0,hueHeight)),
	mCtrlSV(Rect(0,hueHeight,0,-hueHeight))
{
//	mCtrlH .stretch(1,0.25).anchor(0,0.00).disable(DrawBorder | DrawBack);
//	mCtrlSV.stretch(1,0.75).anchor(0,0.25).disable(DrawBorder | DrawBack);

	mCtrlH .stretch(1,0).disable(DrawBorder | DrawBack);
	mCtrlSV.stretch(1,1).disable(DrawBorder | DrawBack);

	struct F{
		static void line(float l, float t, float r, float b){
			//float x0 = r-1;
			//float x1 = r+1;
			//draw::shape(draw::Lines, x0,t, x0,b, x1,t, x1,b);
			draw::shape(draw::Lines, r,t, r,b);
			//draw::shape(draw::Lines, r-2,(b-t)/2, r+2,(b-t)/2);
			
			//float d = (b-t)/2;
			//draw::shape(draw::Lines, r-d,t,r+d,b, r-d,b,r+d,t);
		}
	};
	
	mCtrlH.knobSymbol(F::line);
	mCtrlH.orientation(Sliders::HORIZONTAL);
	
	mCtrlSV.knobSymbol(draw::circle<8>);
	mCtrlSV.knobSize(8);
	mCtrlSV.constrainKnob(0);
	(*this) << mCtrlH << mCtrlSV;
}


HSV ColorPicker::getValue() const {
	return HSV(mCtrlH.getValue(), mCtrlSV.getValue(0), mCtrlSV.getValue(1));
}

ColorPicker& ColorPicker::setValue(const HSV& v){
	mCtrlH .setValue(v.h);
	mCtrlSV.setValue(v.s,0);
	mCtrlSV.setValue(v.v,1);
	return *this;
}

const Data& ColorPicker::getData(Data& temp) const {
	temp.resize(Data::FLOAT,3);
	HSV val = getValue();
	return temp.assignFromArray(&val.h, 3);
}

void ColorPicker::setData(const Data& d){
	if(d.size() >= 3){
		setValue(HSV(d.at<float>(0), d.at<float>(1), d.at<float>(2)));
	}
}

void ColorPicker::onDraw(GLV& g){
	using namespace glv::draw;
	
	GraphicsData& gd = g.graphicsData();

	static const Color hues[] = { 
		Color(1,0,0), Color(1,1,0), Color(0,1,0), Color(0,1,1), Color(0,0,1), Color(1,0,1), Color(1,0,0)
	};

	Color col = HSV(mCtrlH.getValue());
	//col.r = mCtrlH.getValue();

	gd.reset();

	for(int i=0; i<7; ++i){
		float x = float(i)/6 * w;
		gd.addVertex(x,0);
		gd.addVertex(x,mCtrlH.h);
		gd.addColor(hues[i]);
		gd.addColor(hues[i]);
		gd.addIndex(2*i  );
		gd.addIndex(2*i+1);
	}
	
	gd.addIndex(gd.indices()[gd.indices().size()-1]);

	int idxOffset = gd.vertices2().size();

	const int Nx = 5;
	const int Ny = 4;

	for(int j=0; j<Ny; ++j){ float y = float(j)/(Ny-1);
	for(int i=0; i<Nx; ++i){ float x = float(i)/(Nx-1);
		gd.addVertex(x*w, y*mCtrlSV.h + mCtrlSV.t);
		gd.addColor(HSVFromH(col,x,y));
	}}

	for(int j=0; j<Ny-1; ++j){
		gd.addIndex(j*Nx + idxOffset);
	for(int i=0; i<Nx; ++i){
		int idx = j*Nx + i;
		gd.addIndex(idx + idxOffset);
		gd.addIndex(idx+Nx + idxOffset);
	}
		gd.addIndex(gd.indices()[gd.indices().size()-1]);
	}

	paint(draw::TriangleStrip, gd);
}

bool ColorPicker::onEvent(Event::t e, GLV& g){
	//Base::onEvent(e,g);
	
	//const Mouse& m = g.mouse();
	
	switch(e){
		case Event::MouseDown:
		case Event::MouseDrag:
			break;
		case Event::MouseUp:
			break;
		default:;
	}
	return true;
}




ColorSliders::ColorSliders(const Rect& r, bool isHSV, bool useAlpha)
:	Sliders(r, 1, useAlpha ? 4:3),
	mSliderStyle(false), mIsHSV(isHSV), mUseAlpha(useAlpha)
{
	style(&mSliderStyle);
	attach(ntUpdateStyle, Update::Value, this);
	setStyle();
}

Color ColorSliders::getValue() const {
	Color col;
	float c1 = data().at<float>(0);
	float c2 = data().at<float>(1);
	float c3 = data().at<float>(2);
	if(isHSV()) col = HSV(c1,c2,c3);
	else		col.set(c1,c2,c3);
	col.a = mUseAlpha ? data().at<float>(3) : 1;
	return col;
}

ColorSliders& ColorSliders::setValue(const Color& v){
	float c1,c2,c3;
	if(isHSV()){ HSV h=v; c1=h.h; c2=h.s; c3=h.v; }
	else{ c1=v.r; c2=v.g; c3=v.b; }

	setValue(c1,0);
	setValue(c2,1);
	setValue(c3,2);
	if(mUseAlpha) setValue(v.a,3);

	setStyle();
	return *this;
}

ColorSliders& ColorSliders::setValue(const HSV& v){
	float c1,c2,c3;
	if(isHSV()){ c1=v.h; c2=v.s; c3=v.v; }
	else{ Color c=v; c1=c.r; c2=c.g; c3=c.b; }

	setValue(c1,0);
	setValue(c2,1);
	setValue(c3,2);

	setStyle();
	return *this;
}





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

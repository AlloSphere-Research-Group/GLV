#ifndef INC_GLV_SLIDERS_H
#define INC_GLV_SLIDERS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_widget.h"
#include <vector>

namespace glv {

/// Type for slider value changes
typedef ChangedValue<float> SliderChange;



/// Abstract multidimensional slider

/// The foreground color determines the color of the slider.
///
template <int Dim>
class SliderBase : public ValueWidget<Values<float, Dim>, float>{
public:
	typedef Values<float, Dim> value_type;
	GLV_INHERIT_VALUEWIDGET(value_type, float);

	/// Constructor
	SliderBase(const Rect& r);
	
	/// Get value at 1D index
	float value(int idx=0) const;
	
	/// Set value at 1D index and notify observers
	SliderBase& value(float val, int idx);
	
	/// Set all values to maximum and notify observers
	SliderBase& valueMax();
	
	/// Set all values to middle value and notify observers
	SliderBase& valueMid();

	virtual void valueToString(std::string& s){ toString(s, &Base::values()[0], size()); }
	virtual bool valueFromString(const std::string& s){
		float v[size()];
		if(fromString(v,size(), s)){
			for(int i=0; i<size(); ++i) value(v[i], i);
			return true;
		}
		return false;
	}

protected:
	float mAcc[Dim];

	virtual void onSetValueNotify(const float& v, int idx);

	// Add amount to value at 1D index and notify observers
	SliderBase& valueAdd(float val, int idx);
	SliderBase& valueAdd(float val, int idx, float min, float max);

	void clipAccs(){ for(int i=0; i<Dim; ++i) mAcc[i]=glv::clip(mAcc[i],mMax,mMin); }
};



/// A 2-D slider
class Slider2D : public SliderBase<2>{
public:

	/// @param[in] r			geometry
	/// @param[in] valX			initial value along x
	/// @param[in] valY			initial value along y
	/// @param[in] knobSize		size of slider knob in pixels
	Slider2D(const Rect& r=glv::Rect(100), float valX=0, float valY=0, space_t knobSize=12);

	space_t knobSize;	///< Size of slider knob
	
	virtual const char * className() const { return "Slider2D"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);
	
	static void drawKnob(const Slider2D& s);	
};



/// A slider with an adjustable interval
class SliderRange : public SliderBase<2>{
public:

	/// @param[in] r			geometry
	/// @param[in] val1			initial value on left or top
	/// @param[in] val2			initial value on right or bottom
	SliderRange(const Rect& r=glv::Rect(100,20), float val1=0.25, float val2=0.5);
	
	SliderRange& center(float v);							///< Set center of interval
	SliderRange& centerRange(float center, float range);	///< Set center and range of interval
	SliderRange& extrema(float min, float max);				///< Set extrema of interval
	
	/// Sets how much the slider should move when an empty region is clicked.
	
	/// The slider is constrained not jump past the click point.
	///
	SliderRange& jump(float v);
	SliderRange& range(float v);	///< Set range
	
	float center() const;			///< Get center of interval
	float jump() const;				///< Get click jump amount
	float range() const;			///< Get distance of interval
	
	virtual const char * className() const { return "SliderRange"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);
	
private:
	int mDragMode;	// 0,1,2,3: off, lower, upper, center
	float mJump;
};



/// A multi-parameter slider grid

/// The cells of the grid represent all the pair permutations of parameters.
/// The parameter values run from 0 to N, going left-to-right along the x-axis
/// and bottom-to-top along th y-axis. Cells along the diagonal control the
/// parameters individually.
template <int Dim>
class SliderGrid : public SliderBase<Dim>{
public:

	/// @param[in] r			geometry
	/// @param[in] knobSize		size of slider knob in pixels
	SliderGrid(const Rect& r=glv::Rect(100), space_t knobSize=4);

	space_t knobSize; ///< Knob size

	virtual const char * className() const { return "SliderGrid"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);

	using SliderBase<Dim>::colors;
	using SliderBase<Dim>::w;
	using SliderBase<Dim>::h;
	using SliderBase<Dim>::value;
	using SliderBase<Dim>::clipAccs;
	using SliderBase<Dim>::sens;
	using SliderBase<Dim>::diam;
	using SliderBase<Dim>::toInterval;

protected:
	int cx, cy;
};



/// Base class for 1-D slider types
template <class V>
class Slider1DBase: public ValueWidget<V, float>{
public:
	GLV_INHERIT_VALUEWIDGET(V, float);

	/// @param[in] r			geometry
	/// @param[in] nx			number along x (ignored by fixed size value types)
	/// @param[in] ny			number along y (ignored by fixed size value types)
	/// @param[in] dragSelect	whether new sliders are selected while dragging
	/// @param[in] isSigned		whether slider values are signed
	Slider1DBase(const Rect& r, int nx, int ny, bool dragSelect=false, bool isSigned=false);

	virtual ~Slider1DBase(){}

	bool isSigned() const { return mSigned; }
	Slider1DBase& isSigned(bool v){ mSigned=v; return *this; }

	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);
	virtual const char * className() const { return "Slider1DBase"; }
	
protected:
	float mAcc;
	bool mSigned;

	void selectSlider(GLV& g, bool click);
	bool isVertical() const { return Base::dy() > Base::dx(); }
	
	virtual void onSetValueNotify(const float& vnew, int idx){
		float v=vnew;
		v = glv::clip(v, mMax, mMin);
		if(v == Base::values()[idx]) return;
		Base::values()[idx] = v;
		notify(Update::Value, SliderChange(v, idx));
	}
};



/// Single slider
class Slider : public Slider1DBase<Values<float> >{
public:
	typedef Slider1DBase<Values<float> > Base;

	/// @param[in] r			geometry
	/// @param[in] v			initial value between 0 and 1
	/// @param[in] isSigned		whether slider value is signed
	Slider(const Rect& r=Rect(100, 20), float v=0, bool isSigned=false)
	:	Base(r, 1, 1, false, isSigned)
	{	value(v); }
	
	/// Get value
	float value() const { return Base::values()[0]; }
	
	/// Set value and notify observers
	Slider& value(float v){
		select(0); setValueNotify(v);
		return *this;
	}
	
	virtual const char * className() const { return "Slider"; }
	virtual void valueToString(std::string& v){ toString(v, value()); }
	virtual bool valueFromString(const std::string& v){
		float r; if(fromString(r,v)){ value(r);	return true; } return false;
	}
};



/// Multiple sliders
class Sliders : public Slider1DBase<Array<float> >{
public:
	typedef Slider1DBase<Array<float> > Base;

	/// @param[in] r			geometry
	/// @param[in] nx			number along x (ignored by fixed size value types)
	/// @param[in] ny			number along y (ignored by fixed size value types)
	/// @param[in] dragSelect	whether new sliders are selected while dragging
	/// @param[in] isSigned		whether slider values are signed
	Sliders(const Rect& r=Rect(100, 20), int nx=1, int ny=1, bool dragSelect=false, bool isSigned=false)
	:	Base(r, nx, ny, dragSelect, isSigned)
	{}
	
	/// Get value at 1D index
	float value(int i) const { return Base::values()[i]; }
	
	/// Set value at 1D index and notify observers
	Sliders& value(float v, int i){
		select(i); setValueNotify(v);
		return *this;
	}

	/// Set value at 2D index and notify observers
	Sliders& value(float v, int ix, int iy){
		select(ix, iy); setValueNotify(v);
		return *this;
	}
	
	virtual const char * className() const { return "Sliders"; }
	virtual void valueToString(std::string& s){ toString(s, &Base::values()[0], size()); }
	virtual bool valueFromString(const std::string& s){
		float v[size()];
		if(fromString(v,size(), s)){
			for(int i=0; i<size(); ++i) value(v[i], i);
			return true;
		}
		return false;
	}
};




//there must be at least 3 points
class FunctionGraph : public View{
public:
	class Curve : public Array<float>{
	public:
		Curve(int res)
		: Array<float>(res)
		{}
		
		~Curve() {}
	};
	
	struct Knot {
	public:
		Knot() : x(0), y(0)
		{}
		
		~Knot() {}
		
		Knot(const Knot& k) : x(k.x), y(k.y)
		{}
		
		float x;
		float y;
	};

public:
	FunctionGraph(const Rect& r, int nKnots, int res);
	virtual ~FunctionGraph();
	
	virtual const char * className() const { return "FunctionGraph"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);

	void eval(int n, float *vals);
	void tension(float v) {mTension = v; calcCurves();}
	float tension() {return mTension;}

protected:
	void calcCurves();
	
	/*
	Tension: 1 is high, 0 normal, -1 is low
	Bias: 0 is even,
	positive is towards first segment,
	negative towards the other
	*/
	inline float HermiteInterpolate( float y0, float y1,
						float y2, float y3,
						float mu,
						float tension,
						float bias)
	{
		float m0,m1,mu2,mu3;
		float a0,a1,a2,a3;

		mu2 = mu * mu;
		mu3 = mu2 * mu;
		m0  = (y1-y0)*(1+bias)*(1-tension)/2;
		m0 += (y2-y1)*(1-bias)*(1-tension)/2;
		m1  = (y2-y1)*(1+bias)*(1-tension)/2;
		m1 += (y3-y2)*(1-bias)*(1-tension)/2;
		a0 =  2*mu3 - 3*mu2 + 1;
		a1 =    mu3 - 2*mu2 + mu;
		a2 =    mu3 -   mu2;
		a3 = -2*mu3 + 3*mu2;
		
		return (a0*y1+a1*m0+a2*m1+a3*y2);
	}
	
	//returns -1 on failure (> mKnobSize pixels away)
	int knotHitTest(space_t x, space_t y);

	float mTension;
	int mKnobSize;
	int mCurrentKnot;
	int mNKnots;
	Knot *mKnots;
	std::vector<Curve *> mCurves;
};




// Implementation ______________________________________________________________

// SliderBase

#define TEM template <int Dim>

TEM SliderBase<Dim>::SliderBase(const Rect& r)
:	Base(r, Dim, 1, 0, false, false, false)
{
	memset(mAcc, 0, sizeof(float) * Dim);
//	memset(mVal, 0, sizeof(float) * Dim);
}

TEM inline float SliderBase<Dim>::value(int dim) const{
	return Base::validIndex(dim) ? Base::values()[dim] : 0;
}

TEM inline void SliderBase<Dim>::onSetValueNotify(const float& v, int idx){

	if(v == Base::values()[idx]) return;

	//mAcc[idx] = value()[idx] = v;
	Base::values()[idx] = v;
	notify(Update::Value, SliderChange(v, idx));
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::value(float v, int dim){
	if(Base::validIndex(dim)){
		mAcc[dim] = v;
		Base::setValueNotify(glv::clip(v, mMax,mMin), dim);
	}
	return *this;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueAdd(float add, int dim){
	return valueAdd(add,dim,mMin,mMax);
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueAdd(float add, int dim, float min, float max){	
	if(Base::validIndex(dim)){
		mAcc[dim] += add;
		Base::setValueNotify(glv::clip(mAcc[dim], max, min), dim);
	}
	return *this;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueMax(){
	for(int i=0; i<Dim; ++i){ value(mMax, i); } return *this;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueMid(){
	for(int i=0; i<Dim; ++i){ value(Base::mid(), i); } return *this;
}



#define TEMV template <class V>

TEMV Slider1DBase<V>::Slider1DBase(const Rect& r, int nx, int ny, bool dragSelect, bool sgn)
:	Base(r, nx, ny, 1, false, false, true),
	mAcc(0), mSigned(sgn)
{
	property(SelectOnDrag, dragSelect);
}

TEMV void Slider1DBase<V>::onDraw(){
	using namespace glv::draw;
	float x=padding()*0.5, xd=this->dx(), yd=this->dy();

//	TODO: dial drawing code...
//		for(int i=0; i<sizeX(); ++i){
//			float y=padding()*0.5;
//		
//			for(int j=0; j<sizeY(); ++j){
//				int ind = index(i,j);
//				if(isSelected(i,j)) color(colors().fore);
//				else color(colors().fore, colors().fore.a*0.5);
//				
//				color(colors().fore, colors().fore.a*0.5);
//				disc<32>(x,y,xd,yd);
//				lineWidth(1);
//				float v = Base::values()[ind];
//				color(colors().fore);
//				static const float pi = 3.141592653589793;
//				shape(Lines, xd/2, yd/2, xd/2 + xd/2*cos(v*2*pi+pi/2), yd/2 + yd/2*sin(v*2*pi-pi/2));
//
//				y += yd;
//			}
//			x += xd;	
//		}


	if(isVertical()){
		for(int i=0; i<sizeX(); ++i){
		
			float y=padding()*0.5;
		
			for(int j=0; j<sizeY(); ++j){
				int ind = index(i,j);
				if(isSelected(i,j)) color(colors().fore);
				else color(colors().fore, colors().fore.a*0.5);

				float v01 = to01(Base::values()[ind]);
				float y0 = to01(0)*yd;
				//rect(x + x0, y, f*xd+x, y+yd-padding());
				
				rect(x, y+yd-v01*yd, x+xd-padding(), y+yd-y0);

				// if zero line showing
				if(mMax>0 && mMin<0){
					color(colors().border);
					float linePos = draw::pix(y+yd-y0);
					shape(draw::Lines, x, linePos, x+xd, linePos);
				}
				y += yd;
			}
			x += xd;	
		}
	}
	else{
		for(int i=0; i<sizeX(); ++i){
		
			float y=padding()*0.5;
		
			for(int j=0; j<sizeY(); ++j){
				int ind = index(i,j);
				if(isSelected(i,j)) color(colors().fore);
				else color(colors().fore, colors().fore.a*0.5);

				float v01 = to01(Base::values()[ind]);
				float x0 = to01(0)*xd;
				rect(x + x0, y, v01*xd+x, y+yd-padding());

				// if zero line showing
				if(mMax>0 && mMin<0){
					color(colors().border);
					float linePos = draw::pix(x+x0);
					shape(draw::Lines, linePos, y, linePos, y+yd);
				}
				y += yd;
			}
			x += xd;
		}
	}
}

TEMV bool Slider1DBase<V>::onEvent(Event::t e, GLV& g){

	switch(e){
		case Event::MouseDrag:
			// if drag settable
			if(enabled(SelectOnDrag)){
				selectSlider(g, false);
			}
			if(g.mouse.right() || g.mouse.left()) {
				// accumulate differences
				mAcc += diam()*(isVertical() ? -g.mouse.dy()/h*sizeY() : g.mouse.dx()/w*sizeX()) * this->sens(g.mouse);
				Base::setValueNotify(mAcc);
			}
			
			return false;
			
		case Event::MouseDown:
			selectSlider(g, true);
			return false;
			
		case Event::KeyDown:{
			Base::onSelectKey(g);
			int i = selected();

			switch(g.keyboard.key()){
			case 'x':
			case 'a': setValueNotify(Base::values()[i] + diam()/32.); return false;
			case 'z': setValueNotify(Base::values()[i] - diam()/32.); return false;
			default:;
			}
		}

		default:;
			
	}
	return true;
}

TEMV void Slider1DBase<V>::selectSlider(GLV& g, bool click){

	Mouse& m = g.mouse;
	
	int oldIdx = selected();
	Base::onSelectClick(g);
	
	float val = isVertical() ? (1-(m.yRel()/h*sizeY() - selectedY())) : (m.xRel()/w*sizeX() - selectedX());
	val = toInterval(val);

	int idx = selected();
	
	// if left-button, set value
	if(m.left() && !m.right()){
		Base::setValueNotify(val);
	}
	
	// if click or new slider, reset accumulator
	if(click || (oldIdx != idx)){
		if(m.left() && !m.right()) mAcc = val;
		else mAcc = Base::values()[idx];
	}
}

#undef TEMV





TEM SliderGrid<Dim>::SliderGrid(const Rect& r, space_t knobSize)
:	SliderBase<Dim>(r), knobSize(knobSize), cx(0), cy(0)
{
	//this->cropSelf = false;
	this->disable(CropSelf);
}

TEM void SliderGrid<Dim>::onDraw(){
	using namespace glv::draw;

	float rDim = 1./Dim;
	color(colors().fore);
	lineWidth(1);
	frame(cx * w * rDim, (Dim-cy-1) * h * rDim, (cx+1) * w * rDim, (Dim-cy) * h * rDim);
	enable(LineStipple);
	lineStipple(1, 0xAAAA);
	grid(0,0,w,h,Dim,Dim, false);
	shape(Lines,0,h,w,0);
	disable(LineStipple);

	/*
	float dx = rDim*w;
	float dy = rDim*h;

	glShadeModel(GL_SMOOTH);
	glBegin(GL_QUADS);
	for(int i=0; i<Dim; ++i){
		float x = i*dx;
		HSV xc(i/(float)Dim, 0.8, 0.2);
		
		for(int j=0; j<Dim; ++j){
			float y = (Dim-j-1)*dy;
			
			HSV yc(j/(float)Dim, 0.8, 0.2);
			HSV mid( (xc.h+yc.h)*0.5, 0.8, 0.2);
			
			color(Color(yc));
			glVertex3f(x, y, 0);
			
			glColor4f(1, 1, 1, 1.);
			glVertex3f(x, y+dy, 0);
			
			color(Color(xc));
			glVertex3f(x+dx, y+dy, 0);
			
			color(Color(mid));
			glVertex3f(x+dx, y, 0);
		}
	}
	glEnd();

	glColor4f(1, 1, 1, 1.);*/
	pointSize(knobSize);

	Point2 pts[Dim*Dim];
	for(int i=0; i<Dim; ++i){
		float f = (i+to01(value(i))) * rDim;
		float x = f*w;
		
		for(int j=0; j<Dim; ++j){
			pts[i*Dim+j](x, (1.-(j+to01(value(j))) * rDim) * h);
		}
	}
	paint(Points, pts, GLV_ARRAY_SIZE(pts));

}

TEM bool SliderGrid<Dim>::onEvent(Event::t e, GLV& g){

	switch(e){
	case Event::MouseDrag:
					valueAdd( g.mouse.dx()/w * diam() * Dim * sens(g.mouse), cx);
		if(cx!=cy)	valueAdd(-g.mouse.dy()/h * diam() * Dim * sens(g.mouse), cy);
		break;
		
	case Event::MouseDown:
		cx = int(g.mouse.xRel()/w * Dim);
		cy = int(g.mouse.yRel()/h * Dim);
		cy = (Dim-1) - cy;
		cx = glv::clip(cx, Dim-1);
		cy = glv::clip(cy, Dim-1);

		if(g.mouse.left() && !g.mouse.right()){
			float cw = w/Dim;
			float ch = h/Dim;
						value(toInterval(    (g.mouse.xRel()/cw - cx)), cx);
			if(cx!=cy)	value(toInterval(1.f-(g.mouse.yRel()/ch - ((Dim-1)-cy))), cy);
		}
		break;
		
	case Event::MouseUp: clipAccs(); break;
	default: break;
	}
	return false;
}

#undef TEM




} // glv::

#endif


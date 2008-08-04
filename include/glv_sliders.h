#ifndef INC_GLV_SLIDERS_H
#define INC_GLV_SLIDERS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_widget.h"
#include <vector>

namespace glv {


// LJP: The constructor for widgets takes a Rect as an argument rather than
// individual l,t,w,h components since it's more flexible for layout and 
// only requires writing one constructor.

/// Abstract multidimensional slider

/// The foreground color determines the color of the slider.
///
template <int Dim>
class SliderBase : public View{
public:

	/// Constructor
	SliderBase(const Rect& r);
	
	float value(int dim=0) const;			///< Returns a slider value
	
	SliderBase& value(float val, int dim);		///< Sets a slider value
	SliderBase& valueAdd(float val, int dim);	///< Add to a slider value
	SliderBase& valueMax();						///< Max all slider values
	SliderBase& valueMid();						///< Center all slider values
	
	static int dimensions(){ return Dim; }	///< Returns number of dimensions of slider

protected:
	float mAcc[Dim], mVal[Dim];
	//float mSens;
	
	void clip(float & v){ v<0.f ? v=0.f : v>1.f ? v=1.f : 0; }
	void clipAccs(){ for(int i=0; i<Dim; ++i) clip(mAcc[i]); }
	bool validDim(int dim) const { return (dim < Dim) && (dim >= 0); }
	float sens(const GLV& g){ return (g.mouse.left() && g.mouse.right()) ? 0.25 : 1; }
};



/// A 1-D slider
//class Slider : public SliderBase<1>{
//public:
//
//	/// Constructor
//	Slider(const Rect& r=Rect(200,20), float val=0);
//
//	float value() const { return SliderBase<1>::value(0); }		///< Return slider value
//	Slider& value(float v){ SliderBase<1>::value(v,0); return *this; }		///< Set slider value
//
//	virtual void onDraw();
//	virtual bool onEvent(Event::t e, GLV & glv);
//};



/// A 2-D slider
class Slider2D : public SliderBase<2>{
public:

	/// Constructor
	Slider2D(const Rect& r, float valX=0, float valY=0, space_t knobSize=12);

	space_t knobSize;	///< Size of slider knob
	
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV & glv);
	
	static void drawKnob(const Slider2D& s);
	
};



/// A multi-parameter slider grid

/// The cells of the grid represent all the pair permutations of parameters.
/// The parameter values run from 0 to N, going left-to-right along the x-axis
/// and bottom-to-top along th y-axis. Cells along the diagonal control the
/// parameters individually.
template <int Dim>
class SliderGrid : public SliderBase<Dim>{
public:
	using SliderBase<Dim>::colors;
	using SliderBase<Dim>::w;
	using SliderBase<Dim>::h;
	using SliderBase<Dim>::value;
	using SliderBase<Dim>::clipAccs;
	using SliderBase<Dim>::sens;

	/// Constructor
	SliderGrid(const Rect& r, space_t knobSize=4);

	space_t knobSize; ///< Knob size

	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);

protected:
	int cx, cy;
};



template <class V>
class Slider1DBase: public ValueWidget<V>{
public:
	GLV_INHERIT_VALUEWIDGET

	Slider1DBase(const Rect& r, int nx, int ny, bool dragSelect=false);

	virtual ~Slider1DBase(){}
	
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);
	
protected:
	float mAcc;

	void selectSlider(GLV& g, bool click);
	bool isVertical() const { return this->dy() > this->dx(); }
};


/// Single slider
class Slider : public Slider1DBase<Values<float> >{
public:
	typedef Slider1DBase<Values<float> > super;

	Slider(const Rect& r=Rect(20)): super(r, 1, 1, false){}
	
	float value() const { return super::value()[0]; }
	Slider& value(float v){ super::value()[0] = v; return *this; }
};


/// Multiple sliders
typedef Slider1DBase<Array<float> > Sliders;




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
:	View(r)
{
	memset(mAcc, 0, sizeof(float) * Dim);
	memset(mVal, 0, sizeof(float) * Dim);
}

TEM inline float SliderBase<Dim>::value(int dim) const{
	return validDim(dim) ? mVal[dim] : 0;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::value(float value, int dim){
	if(validDim(dim)) mVal[dim] = mAcc[dim] = value; return *this;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueAdd(float add, int dim){
	if(!validDim(dim)) return *this;
	float acc = mAcc[dim] + add;
	mAcc[dim] = mVal[dim] = acc;
	clip(mVal[dim]);	// clip in [0, 1]
	return *this;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueMax(){
	for(int i=0; i<Dim; ++i) mVal[i] = mAcc[i] = 1; return *this;
}

TEM inline SliderBase<Dim>& SliderBase<Dim>::valueMid(){
	for(int i=0; i<Dim; ++i) mVal[i] = mAcc[i] = 0.5; return *this;
}



#define TEMV template <class V>

TEMV Slider1DBase<V>::Slider1DBase(const Rect& r, int nx, int ny, bool dragSelect)
:	ValueWidget<V>(r, nx, ny, 1, false, false, true),
	mAcc(0)
{
	property(SelectOnDrag, dragSelect);
}

TEMV void Slider1DBase<V>::onDraw(){

	float x=padding()*0.5, xd=this->dx(), yd=this->dy();

	if(isVertical()){
		for(int i=0; i<sizeX(); ++i){
		
			float y = padding()*0.5;
		
			for(int j=0; j<sizeY(); ++j){
				int ind = index(i,j);
				if(isSelected(i,j)) draw::color(colors().fore);
				else draw::color(colors().fore, colors().fore.a*0.5);
				draw::rect(x, y+yd-value()[ind]*yd, x+xd-padding(), y+yd);
				y += yd;
			}
			x += xd;	
		}
	}
	else{
		for(int i=0; i<sizeX(); ++i){
		
			float y = padding()*0.5;
		
			for(int j=0; j<sizeY(); ++j){
				int ind = index(i,j);
				if(isSelected(i,j)) draw::color(colors().fore);
				else draw::color(colors().fore, colors().fore.a*0.5);
				draw::rect(x, y, value()[ind]*xd+x, y+yd-padding());
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
				mAcc += (isVertical() ? -g.mouse.dy()*sizeY()/h : g.mouse.dx()*sizeX()/w) * this->sens(g.mouse);
				value()[selected()] = this->clip1(mAcc);
			}

			notify();
			return false;
			
		case Event::MouseDown:
			selectSlider(g, true);
			notify();
			return false;
			
		case Event::KeyDown:{
			ValueWidget<V>::onSelectKey(g);
			int i = selected();

			switch(g.keyboard.key()){
			case 'x':
			case 'a': value()[i] = this->clip1(value()[i] + 1/32.); return false;
			case 'z': value()[i] = this->clip1(value()[i] - 1/32.); return false;
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
	ValueWidget<V>::onSelectClick(g);
	
	float val = isVertical() ? 1-(m.yRel()*sizeY()/h - selectedY()) : m.xRel()*sizeX()/w - selectedX();
	
	int idx = selected();
	
	// if left-button, set value
	if(m.left() && !m.right()) value()[idx] = val;
	
	// if click or new slider, reset accumulator
	if(click || (oldIdx != idx)){
		if(m.left() && !m.right()) mAcc = val;
		else mAcc = value()[idx];
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
	begin(Points);
	for(int i=0; i<Dim; ++i){
		float f = (i+value(i)) * rDim;
		float x = f*w;
		//float y = f*h;
		
		for(int j=0; j<Dim; ++j){
			vertex(x, (1.-(j+value(j)) * rDim) * h);
		}
	}
	end();
}

TEM bool SliderGrid<Dim>::onEvent(Event::t e, GLV& g){

	switch(e){
	case Event::MouseDrag:
					valueAdd( g.mouse.dx()/w * sens(g) * Dim, cx);
		if(cx!=cy)	valueAdd(-g.mouse.dy()/h * sens(g) * Dim, cy);
		break;
		
	case Event::MouseDown:
		cx = (int)((g.mouse.xRel() / w) * Dim);
		cy = (int)((g.mouse.yRel() / h) * Dim);
		cy = (Dim-1) - cy;
		cx < 0 ? cx=0 : cx>=Dim ? cx=Dim-1 : 0;
		cy < 0 ? cy=0 : cy>=Dim ? cy=Dim-1 : 0;

		if(g.mouse.left() && !g.mouse.right()){
			float cw = w/Dim;
			float ch = h/Dim;
						value(      (g.mouse.xRel()/cw - cx), cx);
			if(cx!=cy)	value(1.f - (g.mouse.yRel()/ch - ((Dim-1)-cy)), cy);
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


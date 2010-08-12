#ifndef INC_GLV_WIDGET_H
#define INC_GLV_WIDGET_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {


// inheritance definitions for templated subclasses
#define GLV_INHERIT_VALUEWIDGET(Tv,Tm)\
	using ValueWidget<Tv,Tm>::w;\
	using ValueWidget<Tv,Tm>::h;\
	using ValueWidget<Tv,Tm>::notify;\
	using ValueWidget<Tv,Tm>::colors;\
	using ValueWidget<Tv,Tm>::enabled;\
	using ValueWidget<Tv,Tm>::property;\
	using ValueWidget<Tv,Tm>::padding;\
	using ValueWidget<Tv,Tm>::size;\
	using ValueWidget<Tv,Tm>::sizeX;\
	using ValueWidget<Tv,Tm>::sizeY;\
	using ValueWidget<Tv,Tm>::index;\
	using ValueWidget<Tv,Tm>::isSelected;\
	using ValueWidget<Tv,Tm>::selected;\
	using ValueWidget<Tv,Tm>::selectedX;\
	using ValueWidget<Tv,Tm>::selectedY;\
	using ValueWidget<Tv,Tm>::variables;\
	using ValueWidget<Tv,Tm>::diam;\
	using ValueWidget<Tv,Tm>::to01;\
	using ValueWidget<Tv,Tm>::toInterval;\
	using ValueWidget<Tv,Tm>::mMin;\
	using ValueWidget<Tv,Tm>::mMax;\
	typedef ValueWidget<Tv,Tm> Base

/*
Button:
	bool value() const { return Base::values()[0]; }

	Button& value(bool v){ select(0); setValueNotify(v); return *this; }

Buttons:
	bool value(int i) const { return Base::values()[i]; }

	Buttons& value(bool v, int i){ select(i); setValueNotify(v); return *this; }

	Buttons& value(bool v, int ix, int iy){ select(ix, iy); setValueNotify(v); return *this; }

SliderBase (Slider2D, SliderRange, SliderGrid):
	float value(int idx=0) const {
		return Base::validIndex(dim) ? Base::values()[dim] : 0;
	}
	SliderBase& value(float val, int idx){
		if(Base::validIndex(dim)){
			mAcc[dim] = v;
			Base::setValueNotify(glv::clip(v, mMax,mMin), dim);
		}
		return *this;
	}
	
Slider:
	float value() const { return Base::values()[0]; }

	Slider& value(float v){	select(0); setValueNotify(v); return *this; }

Sliders:
	float value(int i) const { return Base::values()[i]; }
	
	Sliders& value(float v, int i){ select(i); setValueNotify(v); return *this; }

	Sliders& value(float v, int ix, int iy){ select(ix, iy); setValueNotify(v); return *this; }
*/

/// Base class for a widget with a grid of values
template <class Tv, class Tm>
class ValueWidget : public View{
protected:
	virtual void onSetValueNotify(const Tm& v, int idx) = 0;

public:
	typedef Tv values_types;
	typedef Tm model_type;

	/// @param[in] r		geometry
	/// @param[in] nx		number along x (ignored by fixed size value types)
	/// @param[in] ny		number along y (ignored by fixed size value types)
	/// @param[in] pad		padding from border
	/// @param[in] toggles	whether the value elements toggle
	/// @param[in] mutExc	whether value elements operate mutually exclusively
	/// @param[in] drawGrid	whether to draw grid separater for multiple elements
	ValueWidget(const Rect& r, int nx, int ny, space_t pad, bool toggles, bool mutExc, bool drawGrid=true);

	/// Returns whether this element coordinate is selected
	bool isSelected(int x, int y) const { return x == selectedX() && y == selectedY(); }

	const Tm& max() const { return mMax; }			///< Get maximum of value interval
	const Tm& min() const { return mMin; }			///< Get minimum of value interval
	Tm mid() const { return (max()+min())/2; }		///< Get middle point of value interval

	space_t padding() const { return mPadding; }	///< Get element padding amount

	int selected() const { return index(sx, sy); }	///< Get selected element index
	int selectedX() const { return sx; }			///< Get selected element x coordinate
	int selectedY() const { return sy; }			///< Get selected element y coordinate
	int size () const { return values().size (); }	///< Get total number of elements
	int sizeX() const { return values().sizeX(); }	///< Get number of elements along x
	int sizeY() const { return values().sizeY(); }	///< Get number of elements along y
	const Tv& values() const { return mVal; }		///< Returns 1D value array

	/// Attach a model variable at a specified index
	void attachVariable(model_type& v, int i=0){
		variables()[i] = &v;
	}

	ValueWidget& interval(const Tm& max, const Tm& min=Tm(0)){
		sort(mMin=min, mMax=max); return *this;
	}

	/// Set element padding amount
	ValueWidget& padding(space_t v){ mPadding=v; return *this; }

	/// Resize grid
	void resize(int nx, int ny){ values().resize(nx,ny); }

	/// Select element at 1D index
	ValueWidget& select(int i){ return select(i%sizeX(), i/sizeX()); }

	/// Select element at 2D index
	ValueWidget& select(int ix, int iy){ sx=ix; sy=iy; return *this; }

	virtual void onModelSync();

protected:
	Tv mVal;						// value(s)
	space_t mPadding;				// num pixels to inset icon
	int sx, sy;						// last clicked position
	std::map<int, Tm *> mVariables;	// model variables to sync to
	Tm mMin, mMax;

	Tv& values(){ return mVal; }

	std::map<int, Tm *>& variables(){ return mVariables; }
	const std::map<int, Tm *>& variables() const { return mVariables; }

	void setValueNotify(const Tm& v){
		setValueNotify(v, selected());
	}

	void setValueNotify(const Tm& v, int idx){
		if(validIndex(idx)){
			
			// call derived class specialized value updater/notifier
			//if(v != values()[idx]){	// call only when new value is different
				onSetValueNotify(v, idx);
			//}

			// Update model variable at this index, if any...
			if(variables().count(idx)){
				if(idx < size()) *(variables()[idx]) = values()[idx];
			}
		}
	}

	float dx() const { return w/sizeX(); } // width, in pixels, per element
	float dy() const { return h/sizeY(); } // height, in pixels, per element
	int index(int ix, int iy) const { return ix + iy*sizeX(); }
	float sens(const Mouse& m) const { return (m.left() && m.right()) ? 0.25 : 1; }

	static void clip(int& i, int max){ i<0 ? i=0 : i>=max ? i=max-1 : 0; }
	void clipIndices(){ clip(sx, sizeX()); clip(sy, sizeY()); }
	bool validIndex(int i) const { return (i < size()) && (i >= 0); }

	Tm diam() const { return max()-min(); }
	Tm to01(const Tm& v) const { return (v-min())/diam(); }
	Tm toInterval(const Tm& v) const { return v*diam() + min(); }

	// draw the grid lines
	void drawGrid(){
		
		if(enabled(DrawGrid)){
			using namespace glv::draw;

			float xd = dx();
			float yd = dy();

			color(colors().border); lineWidth(1);

			Point2 pts[(sizeX()+sizeY()-2)*2];
			int i=0;

			for(int x=1; x<sizeX(); ++x){
				pts[i++](x*xd, 0);
				pts[i++](x*xd, h);
			}
			for(int y=1; y<sizeY(); ++y){
				pts[i++](0, y*yd);
				pts[i++](w, y*yd);
			}
			paint(Lines, pts, GLV_ARRAY_SIZE(pts));
		}
	}


	void onSelectClick(GLV& g){
		sx = (int)((g.mouse.xRel() / w) * sizeX());
		sy = (int)((g.mouse.yRel() / h) * sizeY());
		clipIndices();
	}

	
	void onSelectKey(GLV& g){
		//printf("shift %d\n", g.keyboard.shift());
		//printf("shift %d\n", g.keyboard.key());
		switch(g.keyboard.key()){
		case Key::Down:	sy++; break;
		case Key::Up:	sy--; break;
		case Key::Right:sx++; break;
		case Key::Left:	sx--; break;
		case Key::Tab:
			
			if(!g.keyboard.shift()){
				if(sx == sizeX()-1){
					sx=0;
					if(sy == sizeY()-1)	sy=0;
					else				sy++;
				}
				else sx++;
			}
			else{	// doesn't work with GLUT; key == 25 (end-of-medium) with shift down
				if(sx == 0){
					sx=sizeX()-1;
					if(sy == 0) sy=sizeY()-1;
					else		sy--;
				}
				else sx--;
			}
			break;
		}
		clipIndices();
	}

};




template<class Tv, class Tm>
ValueWidget<Tv,Tm>::ValueWidget(
	const Rect& r, int nx, int ny, space_t pad, bool toggles, bool mutExc, bool drawGrid
)
:	View(r),
	mPadding(pad), sx(0), sy(0), mMin(0), mMax(1)
{
	resize(nx, ny);	// req'd for dynamically sized values
	values().zero();
	property(DrawGrid, drawGrid);
	property(MutualExc, mutExc);
	property(Toggleable, toggles);
}

template<class Tv, class Tm>
void ValueWidget<Tv,Tm>::onModelSync(){
	typename std::map<int, Tm *>::iterator it = variables().begin();
	for(; it!=variables().end(); ++it){
		int idx = it->first;
		if(validIndex(idx)){
			if(values()[idx] != *it->second){
				onSetValueNotify(*it->second, idx);
			}
		}
	}
}


// Scrapyard....

// One-to-one value maps
//namespace map{
//
//	template <class T>
//	T clip(T v, T max, T min){ return v<min ? min : v>max ? max : v; }
//	
//	template <class T>
//	T wrap(T v, T max, T min){ return v<min ? v + (max-min) : v>max ? v - (max-min) : v; }
//}

//class ValueMapper{
//
//	float map(float v1){
//		float r = mBoundFunc(v1);
//		if(mSigned) r = r*2-1;
//		r = mWarpFunc(r);
//		r = r * (mMax - mMin) + mMin;
//		if(mStep != 0){
//			r /= mStep;
//			r += r > 0 ? 0.5 : -0.5;	// round
//			r = int(r) * mStep;
//		}
//		return r;
//	}
//
//	float mMin, mMax, mStep;
//	WarpFunc mWarpFunc;
//	BoundFunc mBoundFunc;	// clip, wrap, none
//	bool mSigned;
//};



} // glv::
#endif

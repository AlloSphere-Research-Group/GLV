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
	typedef ValueWidget<Tv,Tm> Base


/// Base class for a widget with a grid of values
template <class Tv, class Tm>
class ValueWidget : public View{
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

	/// Resize grid
	void resize(int nx, int ny){ values().resize(nx,ny); }

	int size () const { return values().size (); }	///< Get total number of elements
	int sizeX() const { return values().sizeX(); }	///< Get number of elements along x
	int sizeY() const { return values().sizeY(); }	///< Get number of elements along y

	/// Returns whether this element coordinate is selected
	bool isSelected(int x, int y) const { return x == selectedX() && y == selectedY(); }
	
	space_t padding() const { return mPadding; }	///< Get element padding amount
	int selected() const { return index(sx, sy); }	///< Get selected element index
	int selectedX() const { return sx; }			///< Get selected element x coordinate
	int selectedY() const { return sy; }			///< Get selected element y coordinate

	/// Attach a model variable at a specified index
	void attachVariable(model_type& v, int i=0){
		variables()[i] = &v;
	}

	/// Select element at 1D index
	ValueWidget& select(int i){ return select(i%sizeX(), i/sizeX()); }

	/// Select element at 2D index
	ValueWidget& select(int ix, int iy){ sx=ix; sy=iy; return *this; }

	/// Set element padding amount
	ValueWidget& padding(space_t v){ mPadding=v; return *this; }

	virtual void onModelSync();

protected:
	Tv mVal;						// value(s)
	space_t mPadding;				// num pixels to inset icon
	int sx, sy;						// last clicked position
	std::map<int, Tm *> mVariables;	// model variables to sync to

	Tv& values(){ return mVal; }
	const Tv& values() const { return mVal; }

	virtual void onSetValueNotify(const Tm& v, int idx){}

	std::map<int, Tm *>& variables(){ return mVariables; }
	const std::map<int, Tm *>& variables() const { return mVariables; }

	void setValueNotify(const Tm& v){
		setValueNotify(v, selected());
	}

	void setValueNotify(const Tm& v, int idx){
		if(validIndex(idx)){
			// Update model variable at this index, if any...
			if(variables().count(idx)){
				if(idx < size()) *(variables()[idx]) = v;
			}
			
			// call derived class specialized value updater/notifier
			//if(v != values()[idx]){	// call only when new value is different
				onSetValueNotify(v, idx);
			//}
		}
	}

	float dx() const { return w/sizeX(); } // width, in pixels, per element
	float dy() const { return h/sizeY(); } // height, in pixels, per element
	int index(int ix, int iy) const { return ix + iy*sizeX(); }
	float sens(const Mouse& m) const { return (m.left() && m.right()) ? 0.25 : 1; }

	static void clip(int& i, int max){ i<0 ? i=0 : i>=max ? i=max-1 : 0; }
	//static float clip1(float v){ return v < 0. ? 0. : v > 1. ? 1 : v; }
	//static float clip(float v, float mx=1, float mn=0){ return v < mn ? mn : v > mx ? mx : v; }
	void clipIndices(){ clip(sx, sizeX()); clip(sy, sizeY()); }
	bool validIndex(int i) const { return (i < size()) && (i >= 0); }

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
ValueWidget<Tv,Tm>::ValueWidget(const Rect& r, int nx, int ny, space_t pad, bool toggles, bool mutExc, bool drawGrid)
:	View(r),
	mPadding(pad), sx(0), sy(0)
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

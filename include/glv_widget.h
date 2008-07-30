#ifndef INC_GLV_WIDGET_H
#define INC_GLV_WIDGET_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {


// One-to-one value maps
//namespace map{
//
//	template <class T>
//	T clip(T v, T max, T min){ return v<min ? min : v>max ? max : v; }
//	
//	template <class T>
//	T wrap(T v, T max, T min){ return v<min ? v + (max-min) : v>max ? v - (max-min) : v; }
//}


// inheritance definitions for templated subclasses
#define GLV_INHERIT_VALUEWIDGET\
	using ValueWidget<V>::w;\
	using ValueWidget<V>::h;\
	using ValueWidget<V>::notify;\
	using ValueWidget<V>::colors;\
	using ValueWidget<V>::enabled;\
	using ValueWidget<V>::property;\
	using ValueWidget<V>::padding;\
	using ValueWidget<V>::sizeX;\
	using ValueWidget<V>::sizeY;\
	using ValueWidget<V>::value;\
	using ValueWidget<V>::index;\
	using ValueWidget<V>::isSelected;\
	using ValueWidget<V>::selected;\
	using ValueWidget<V>::selectedX;\
	using ValueWidget<V>::selectedY;\


/// Base class of generic grid-based, valued widget
template <class V>
class ValueWidget : public View{
public:

	ValueWidget(const Rect& r, int nx, int ny, space_t pad, bool toggles, bool mutExc, bool drawGrid=true)
	:	View(r),
		mPadding(pad), sx(0), sy(0)
	{
		value().resize(nx, ny);	// req'd for dynamically sized values
		//valueMin();
		value().zero();
		
		property(DrawGrid, drawGrid);
		property(MutualExc, mutExc);
		property(Toggleable, toggles);
	}

	int size () const { return value().size (); }
	int sizeX() const { return value().sizeX(); }
	int sizeY() const { return value().sizeY(); }
	
	V& value(){ return mVal; }
	const V& value() const { return mVal; }

	bool isSelected(int x, int y) const { return x == selectedX() && y == selectedY(); }

	int lastClicked() const { return index(sx, sy); }
	
	space_t padding() const { return mPadding; }
	ValueWidget& padding(space_t v){ mPadding=v; return *this; }

	int selected() const { return index(sx, sy); }
	int selectedX() const { return sx; }
	int selectedY() const { return sy; }

	//ValueWidget& valueMin(){ value().zero(); return *this; }
	//ValueWidget& valueMid(){ for(int i=0; i<size(); ++i) value()[i]=0.5; return *this; }
	//ValueWidget& valueMax(){ for(int i=0; i<size(); ++i) value()[i]=1.0; return *this; }


protected:
	V mVal;
	space_t mPadding;			// num pixels to inset icon
	int sx, sy;					// last clicked position


	float dx() const { return w/sizeX(); }
	float dy() const { return h/sizeY(); }
	int index(int x, int y) const { return x + y*value().sizeX(); }
	float sens(const Mouse& m){ return (m.left() && m.right()) ? 0.25 : 1; }


	// draw the grid lines
	void drawGrid(){
		
		if(enabled(DrawGrid)){
			using namespace glv::draw;

			float xd = dx();
			float yd = dy();

			color(colors().border); lineWidth(1);
			begin(Lines);
			for(int x=1; x<sizeX(); ++x){ vertex(x*xd, 0); vertex(x*xd, h); }
			for(int y=1; y<sizeY(); ++y){ vertex(0, y*yd); vertex(w, y*yd); }
			end();
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

	
	static void clip(int& i, int max){ i<0 ? i=0 : i>=max ? i=max-1 : 0; }
	static float clip1(float v){ return v < 0. ? 0. : v > 1. ? 1 : v; }
	void clipIndices(){ clip(sx, sizeX()); clip(sy, sizeY()); }
};




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

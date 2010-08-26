#ifndef INC_GLV_WIDGET_H
#define INC_GLV_WIDGET_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_model.h"

namespace glv {


class Widget : public View {
public:

	Widget(
		const Rect& r, const Data& data=Data(),
		space_t pad=3, bool toggles=false, bool mutExc=false, bool drawGrid=true)
	:	View(r), mPadding(pad), sx(0), sy(0), mMin(0), mMax(1), mUseInterval(false)
	{
		setModel(data);
		model().clone();
		if(model().isNumerical()){ model().assignAll(0); }

		property(DrawGrid, drawGrid);
		property(MutualExc, mutExc);
		property(Toggleable, toggles);
	}
	
	//Widget(const Rect& r, int nx, int ny, space_t pad, bool toggles, bool mutExc, bool drawGrid=true);

	//virtual void onModelSync();

	/// Returns whether this element coordinate is selected
	bool isSelected(int x, int y) const { return x == selectedX() && y == selectedY(); }

	const double& max() const { return mMax; }			///< Get maximum of value interval
	const double& min() const { return mMin; }			///< Get minimum of value interval
	double mid() const { return (max()+min())/2; }		///< Get middle point of value interval

	space_t padding() const { return mPadding; }	///< Get element padding amount

	int selected() const { return model().indexFlat(sx, sy); }	///< Get selected element index
	int selectedX() const { return sx; }			///< Get selected element x coordinate
	int selectedY() const { return sy; }			///< Get selected element y coordinate
	int size () const { return model().size(); }	///< Get total number of elements
	int sizeX() const { return model().size1(); }	///< Get number of elements along x
	int sizeY() const { return model().size2(); }	///< Get number of elements along y

	bool useInterval() const { return mUseInterval; }

	Widget& interval(const double& max, const double& min=0){
		glv::sort(mMin=min, mMax=max); return *this;
	}

	/// Set element padding amount
	Widget& padding(space_t v){ mPadding=v; return *this; }

	/// Select element at 1D index
	Widget& select(int i){ int i1,i2; model().indexTo2(i,i1,i2); return select(i1, i2); }

	/// Select element at 2D index
	Widget& select(int ix, int iy){ sx=ix; sy=iy; return *this; }

	Widget& useInterval(bool v){ mUseInterval=v; return *this; }

//	Widget& value(bool v, int i){ select(i); setValueNotify(v); return *this; }
//
//	Widget& value(bool v, int ix, int iy){ select(ix, iy); setValueNotify(v); return *this; }

	template <class T>
	Widget& setValue(const T& v, int ix=0, int iy=0){
		select(ix,iy);
		return setValueSelected(v);
	}

protected:
	space_t mPadding;				// num pixels to inset icon
	int sx, sy;						// last clicked position
	double mMin, mMax;				// value interval for all elements
	bool mUseInterval;

	float dx() const { return w/sizeX(); } // width, in pixels, per element
	float dy() const { return h/sizeY(); } // height, in pixels, per element
	int index(int ix, int iy) const { return ix + iy*sizeX(); }

	static void clip(int& i, int max){ i<0 ? i=0 : i>=max ? i=max-1 : 0; }
	void clipIndices(){ clip(sx, sizeX()); clip(sy, sizeY()); }
	bool validIndex(int i) const { return (i < size()) && (i >= 0); }

	double diam() const { return max()-min(); }
	double to01(const double& v) const { return (v-min())/diam(); }
	double toInterval(const double& v) const { return v*diam() + min(); }

	template <class T>
	Widget& setValueSelected(const T& v){
		T t=v; Data d(v); assignModel(d, selectedX(), selectedY());
		return *this;
	}

	virtual void onAssignModel(Data& d, int ind1, int ind2){
	
		if(enabled(MutualExc)){
			double v = 0;
			if(useInterval()) v = glv::clip(v, mMax, mMin);
			model().assignAll(v);
		}
	
		if(useInterval()){
			double v = d.at<double>(0);
			v = glv::clip(v, max(), min());
			d.put(v);
		}

		// TODO: notify observers...
	}

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


//	void setValueNotify(const T& v){
//		setValueNotify(v, selected());
//	}
//
//	void setValueNotify(const T& v, int idx){
//		if(validIndex(idx)){
//			
//			// call derived class specialized value updater/notifier
//			//if(v != values()[idx]){	// call only when new value is different
//				onSetValueNotify(v, idx);
//			//}
//
//			// Update model variable at this index, if any...
//			if(variables().count(idx)){
//				Data& d = variables()[idx];
//				if(idx < size()) d.assign(&values()[idx], d.size());
//			}
//		}
//	}

};


// inheritance definitions for templated subclasses
#define GLV_INHERIT_VALUEWIDGET(Tvalues,T)\
	using ValueWidget<Tvalues,T>::w;\
	using ValueWidget<Tvalues,T>::h;\
	using ValueWidget<Tvalues,T>::notify;\
	using ValueWidget<Tvalues,T>::colors;\
	using ValueWidget<Tvalues,T>::enabled;\
	using ValueWidget<Tvalues,T>::property;\
	using ValueWidget<Tvalues,T>::padding;\
	using ValueWidget<Tvalues,T>::size;\
	using ValueWidget<Tvalues,T>::sizeX;\
	using ValueWidget<Tvalues,T>::sizeY;\
	using ValueWidget<Tvalues,T>::index;\
	using ValueWidget<Tvalues,T>::isSelected;\
	using ValueWidget<Tvalues,T>::selected;\
	using ValueWidget<Tvalues,T>::selectedX;\
	using ValueWidget<Tvalues,T>::selectedY;\
	using ValueWidget<Tvalues,T>::variables;\
	using ValueWidget<Tvalues,T>::diam;\
	using ValueWidget<Tvalues,T>::to01;\
	using ValueWidget<Tvalues,T>::toInterval;\
	using ValueWidget<Tvalues,T>::mMin;\
	using ValueWidget<Tvalues,T>::mMax;\
	typedef ValueWidget<Tvalues,T> Base


/// Base class for a widget with a grid of values
template <class Tvalues, class T>
class ValueWidget : public View{
protected:
	virtual void onSetValueNotify(const T& v, int idx) = 0;

public:
	typedef Tvalues values_t;
	typedef T model_t;

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

	const T& max() const { return mMax; }			///< Get maximum of value interval
	const T& min() const { return mMin; }			///< Get minimum of value interval
	T mid() const { return (max()+min())/2; }		///< Get middle point of value interval

	space_t padding() const { return mPadding; }	///< Get element padding amount

	int selected() const { return index(sx, sy); }	///< Get selected element index
	int selectedX() const { return sx; }			///< Get selected element x coordinate
	int selectedY() const { return sy; }			///< Get selected element y coordinate
	int size () const { return values().size (); }	///< Get total number of elements
	int sizeX() const { return values().sizeX(); }	///< Get number of elements along x
	int sizeY() const { return values().sizeY(); }	///< Get number of elements along y
	const Tvalues& values() const { return mValues; }	///< Get non-mutable reference to values

	/// Attach a single variable at a specified index
	template <class V>
	void attachVariable(V& val, int i=0){
		variables()[i] = Data(val);
	}

//	/// Attach an array of variables at a specified index
//	template <class V>
//	void attachVariable(V * val, int num, int i=0){
//		variables()[i] = Data(val, num);
//	}

	ValueWidget& interval(const T& max, const T& min=T(0)){
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

	/// Get mutable reference to values
	Tvalues& values(){ return mValues; }

	virtual void onModelSync();

protected:
	typedef std::map<int, glv::Data> IndexDataMap;

	Tvalues mValues;				// value(s)
	space_t mPadding;				// num pixels to inset icon
	int sx, sy;						// last clicked position
	IndexDataMap mVariables;		// external variables to sync to, index-Data
	T mMin, mMax;					// value interval for all elements

	IndexDataMap& variables(){ return mVariables; }
	const IndexDataMap& variables() const { return mVariables; }

	void setValueNotify(const T& v){
		setValueNotify(v, selected());
	}

	void setValueNotify(const T& v, int idx){
		if(validIndex(idx)){
			
			// call derived class specialized value updater/notifier
			//if(v != values()[idx]){	// call only when new value is different
				onSetValueNotify(v, idx);
			//}

			// Update model variable at this index, if any...
			if(variables().count(idx)){
				Data& d = variables()[idx];
				if(idx < size()) d.assign(&values()[idx], d.size());
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

	T diam() const { return max()-min(); }
	T to01(const T& v) const { return (v-min())/diam(); }
	T toInterval(const T& v) const { return v*diam() + min(); }

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




template<class Tvalues, class T>
ValueWidget<Tvalues,T>::ValueWidget(
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

template<class Tvalues, class T>
void ValueWidget<Tvalues,T>::onModelSync(){
	typename IndexDataMap::iterator it = variables().begin();

	for(; it!=variables().end(); ++it){
		int idx = it->first;

		if(validIndex(idx)){
			const Data& data = it->second;
			for(int i=0; i<data.size(); ++i){
				if(values()[i+idx] != data.at<T>(i)){
					onSetValueNotify(data.at<T>(i), i+idx);
				}
			}
		}
	}
}


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

#ifndef INC_GLV_WIDGET_H
#define INC_GLV_WIDGET_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_model.h"

namespace glv {

typedef ChangedValue<Data> ModelChange;

/// Base class for views having a regular grid of elements
class Widget : public View {
public:

	/// @param[in] r		geometry
	/// @param[in] pad		padding from border
	/// @param[in] toggles	whether the value elements toggle
	/// @param[in] mutExc	whether value elements operate mutually exclusively
	/// @param[in] drawGrid	whether to draw grid separater for multiple elements
	Widget(
		const Rect& r, space_t pad=3,
		bool momentary=false, bool mutExc=false, bool drawGrid=true
	);

	/// Get selected element value type-casted to template parameter
	template <class T>
	T getValue() const { return getValue<T>(selectedX(), selectedY()); }
	
	/// Get element value at 1D index type-casted to template parameter
	template <class T>
	T getValue(int i) const { return data().Data::at<T>(i); }
	
	/// Get element value at 2D index type-casted to template parameter
	template <class T>
	T getValue(int i1, int i2) const { return data().Data::at<T>(i1, i2); }

	/// Returns whether this element coordinate is selected
	bool isSelected(int x, int y) const { return x == selectedX() && y == selectedY(); }

	const Interval<double>& interval() const { return mInterval; } ///< Get value interval
	const double& max() const { return mInterval.max(); }	///< Get maximum of value interval
	const double& min() const { return mInterval.min(); }	///< Get minimum of value interval
	double mid() const { return mInterval.center(); }		///< Get middle point of value interval

	space_t paddingX() const { return mPadding[0]; }		///< Get element padding amount along x
	space_t paddingY() const { return mPadding[1]; }		///< Get element padding amount along y

	int selected() const { return data().indexFlat(sx, sy); }	///< Get selected element index
	int selectedX() const { return sx; }					///< Get selected element x coordinate
	int selectedY() const { return sy; }					///< Get selected element y coordinate
	int size () const { return data().size(); }				///< Get total number of elements
	int sizeX() const { return data().size(0); }			///< Get number of elements along x
	int sizeY() const { return data().size(1); }			///< Get number of elements along y
	bool useInterval() const { return mUseInterval; }


	/// Attach a single variable at a specified index
	template <class V>
	void attachVariable(V& val, int i=0){
		variables()[i] = Data(val);
	}

	/// Set interval for numerical values
	Widget& interval(const double& max, const double& min=0){
		mInterval.endpoints(min,max); return *this;
	}

	/// Set element padding amount
	Widget& padding(space_t v){ for(int i=0; i<DIMS; ++i){ mPadding[i]=v; } return *this; }

	/// Set element padding amount
	Widget& padding(space_t v, int dim){ mPadding[dim]=v; return *this; }

	/// Select element at 1D index
	Widget& select(int i){ int i1,i2; data().indexDim(i1,i2,i); return select(i1,i2); }

	/// Select element at 2D index
	Widget& select(int ix, int iy);

	virtual void onCellChange(int indexOld, int indexNew){}

	template <class T>
	Widget& setValue(const T& v);

	/// Set value at specified 1D index
	template <class T>
	Widget& setValue(const T& v, int i){ select(i); return setValue(v); }

	/// Set value at specified 2D index
	template <class T>
	Widget& setValue(const T& v, int ix, int iy){ select(ix,iy); return setValue(v); }

	Widget& setValue(const char * v){ return setValue(std::string(v)); }

	/// Set all values to maximum
	Widget& setValueMax();
	
	/// Set all values to middle value
	Widget& setValueMid();

	Widget& useInterval(bool v){ mUseInterval=v; return *this; }
	
	virtual void onDataModelSync();
	virtual const char * className() const { return "Widget"; }

protected:
	enum{ DIMS=2 };
	typedef std::map<int, glv::Data> IndexDataMap;

	Lazy<IndexDataMap> mVariables;	// external variables to sync to, index-Data
	space_t mPadding[DIMS];			// num pixels to inset icon
	int sx, sy;						// last clicked position
	Interval<double> mInterval;		
	double mPrevVal;				// used for momentary value
	bool mUseInterval;

	bool hasVariables() const { return mVariables.created(); }
	IndexDataMap& variables(){ return mVariables(); }
	const IndexDataMap& variables() const { return mVariables(); }

	float dx(int dim=0) const { return w/data().size(dim); } // width, in pixels, per element
	float dy(int dim=1) const { return h/data().size(dim); } // height, in pixels, per element
	int index(int ix, int iy) const { return ix + iy*sizeX(); }

	static void clip(int& i, int max){ i<0 ? i=0 : i>=max ? i=max-1 : 0; }
	void clipIndices(int& x, int& y){ clip(x, sizeX()); clip(y, sizeY()); }
	void clipIndices(){ clipIndices(sx,sy); }
	bool validIndex(int i) const { return (i < size()) && (i >= 0); }

	double diam() const { return mInterval.diameter(); }
	double to01(const double& v) const { return mInterval.toUnit(v); }
	double toInterval(const double& v) const { return mInterval.fromUnit(v); }

	template <class T>
	Widget& setValue(const T& v, int i, double mn, double mx){
		double omn=min(), omx=max();
		interval(mx, mn);
		setValue(v,i);
		return interval(omx, omn);
	}

	virtual bool onAssignData(Data& d, int ind1, int ind2);
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

	void drawGrid(GraphicsData& g);
	void drawSelectionBox();	
	void selectFromMousePos(GLV& g);
	
	static bool widgetKeyDown(View * v, GLV& g);
};


template <class T>
Widget& Widget::setValue(const T& v){
	T t = v;
	Data d(t);
	assignData(d, selectedX(), selectedY());
	return *this;
}

} // glv::
#endif

#ifndef INC_GLV_WIDGET_H
#define INC_GLV_WIDGET_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_model.h"

namespace glv {

typedef ChangedValue<Data> ModelChange;

/// Base class for views having a regular grid of elements
class Widget : public View, public DataObject {
public:

	/// @param[in] r			geometry
	/// @param[in] pad			padding from border
	/// @param[in] momentary	whether the value elements are momentary
	/// @param[in] mutExc		whether value elements operate mutually exclusively
	/// @param[in] drawGrid		whether to draw grid separater for multiple elements
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

	
	float dx(int dim=0) const { return w/data().size(dim); } ///< Get width, in pixels, per element
	float dy(int dim=1) const { return h/data().size(dim); } ///< Get height, in pixels, per element



	/// Attach a single variable at a specified index
	template <class V>
	void attachVariable(V& val, int i=0){
		attachVariable(&val, 1, i);
	}

	/// Attach an array of variables at a specified index
	template <class T>
	void attachVariable(T * src, int size, int i=0){
		variables()[i] = Data(src, size);
	}

	void clipIndices(){ clipIndices(sx,sy); }

	/// Set interval for numerical values
	Widget& interval(const double& max, const double& min=0){
		mInterval.endpoints(min,max); return *this;
	}

	/// Set padding around elements
	Widget& padding(space_t v){ for(int i=0; i<DIMS; ++i){ mPadding[i]=v; } onResize(0,0); return *this; }

	/// Set padding around elements
	Widget& padding(space_t v, int dim){ mPadding[dim]=v; onResize(0,0); return *this; }

	/// Set padding around elements in x direction
	Widget& paddingX(space_t v){ return padding(v,0); }

	/// Set padding around elements in x direction
	Widget& paddingY(space_t v){ return padding(v,1); }

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

	virtual const Data& getData(Data& temp) const { return data(); }
	virtual void setData(const Data& d){ assignData(d); }

	void assignData(const Data& d, const int& ind=0){
		int i1=0,i2=0; data().indexDim(i1,i2, ind);
		assignData(d, i1,i2);
	}

	// Assigns argument to elements at specified index.
	
	// assignData() is called with the input data if the indices are valid.
	// assignData() can be used to constrain the input data before it is
	// assigned.
	void assignData(const Data& d, int ind1, int ind2){
		if(data().inBounds(ind1, ind2)){
			Data t=d; t.clone();
			if(onAssignData(t, ind1, ind2)){
				//model().assign(t, ind1, ind2);
			}
		}
	}

//	virtual bool onAssignData(Data& d, int ind1, int ind2){
//		data().assign(d, ind1, ind2);
//		return true;
//	}

protected:
	enum{ DIMS=2 };
	typedef std::map<int, glv::Data> IndexDataMap;

	Lazy<IndexDataMap> mVariables;	// external variables to sync to, index-Data
	space_t mPadding[DIMS];			// num pixels to inset icon
	short sx, sy;					// selected element position
	Interval<double> mInterval;		
	double mPrevVal;				// used for momentary value
	bool mUseInterval;

	bool hasVariables() const { return mVariables.created(); }
	IndexDataMap& variables(){ return mVariables(); }
	const IndexDataMap& variables() const { return mVariables(); }

	int index(int ix, int iy) const { return ix + iy*sizeX(); }
	bool tallElems() const { return dy() > dx(); }

	template<class Int>
	static void clip(Int& i, int max){ i<0 ? i=0 : i>=max ? i=max-1 : 0; }
	template<class Int>
	void clipIndices(Int& x, Int& y){ clip(x, sizeX()); clip(y, sizeY()); }
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

	// This is the central go to for all data assignment methods of Widget.
	// It is responsible for updating the widget's data and attached variables,
	// and for sending out Update::Value notifications.
	// The order of events is 1) update widget's data, 2) update attached
	// variables, if any, and 3) send out notifications.
	// @param[in] d		the source data
	// @param[in] ind1	destination index of first dimension
	// @param[in] ind2	destitation index of second dimension
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

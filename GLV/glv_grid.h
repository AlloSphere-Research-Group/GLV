#ifndef INC_GLV_GRID_H
#define INC_GLV_GRID_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_font.h"

#ifdef minor
#undef minor
#endif
#ifdef major
#undef major
#endif

namespace glv{

/// Interactive grid for graphing
class Grid : public View{
public:
	
	/// @param[in] r			geometry
	/// @param[in] intervalMin	minimum displayed value for all dimensions
	/// @param[in] intervalMax	maximum displayed value for all dimensions
	/// @param[in] majorDist	distance between major grid lines
	/// @param[in] minorDiv		number of minor divisions between major lines
	Grid(
		const Rect& r=Rect(100,100),
		double intervalMin=-1, double intervalMax=1,
		double majorDist=1, int minorDiv=4
	);

	/// Get interval of a dimension
	const Interval<double>& interval(int dim) const { return mInterval[dim]; }
	
	/// Set interval of a dimension
	Interval<double>& interval(int dim){ return mInterval[dim]; }

	/// Get major division diameters
	const double * major() const { return mMajor; }

	/// Get minor divisions
	const int * minor() const { return mMinor; }

	/// Get lockScroll booleans
	const bool * lockScroll() const { return mLockScroll; }

	/// Get lockZoom booleans
	const bool * lockZoom() const { return mLockZoom; }

	/// Get showAxis booleans
	const bool * showAxis() const { return mShowAxis; }

	/// Get showGrid booleans
	const bool * showGrid() const { return mShowGrid; }

	/// Get showNumbering booleans
	const bool * showNumbering() const { return mShowNumbering; }

	/// Returns whether point is contained within grid region
	bool contains(double x, double y) const {
		return interval(0).contains(x) && interval(1).contains(y);
	}


	/// Set minor division

	/// @param[in] v	number of minor divisions per major division
	/// @param[in] dim	dimension or -1 for all dimensions
	Grid& minor(int v, int dim=-1);

	/// Set major division width

	/// @param[in] v	width of major division
	/// @param[in] dim	dimension or -1 for all dimensions
	Grid& major(double v, int dim=-1);

	/// Center grid on origin
	Grid& origin();

	/// Set whether to equalize distances along axes
	Grid& equalizeAxes(bool v){ mEqualize=v; return *this; }

	/// Set dimension interval to [min, max]
	
	/// @param[in] min	minimum value
	/// @param[in] max	maximum value
	/// @param[in] dim	dimension or -1 for all dimensions
	Grid& range(double min, double max, int dim=-1);

	/// Set all dimension intervals to [-v, v]
	Grid& range(double v){ return range(-v,v); }

	/// Set whether to show axis
	
	/// @param[in] v	whether to show axis
	/// @param[in] dim	dimension or -1 for all dimensions
	Grid& showAxis(bool v, int dim=-1);
	
	/// Set whether to show grid

	/// @param[in] v	whether to show grid
	/// @param[in] dim	dimension or -1 for all dimensions
	Grid& showGrid(bool v, int dim=-1);
	
	/// Set whether to show numbering

	/// @param[in] v	whether to show numbering
	/// @param[in] dim	dimension or -1 for all dimensions
	Grid& showNumbering(bool v, int dim=-1);

	/// Set whether to lock interactive scrolling
	Grid& lockScroll(bool v, int dim=-1);

	/// Set whether to lock zooming
	Grid& lockZoom(bool v, int dim=-1);

	/// Zoom grid on a point
	Grid& zoom(double amt, double x, double y);

	virtual bool onEvent(Event::t e, GLV& g);
	virtual void onAnimate(double dt);
	virtual void onDraw(GLV& g);
	virtual void onResize(space_t dx, space_t dy);
	
protected:
	typedef Interval<double> interval_t;
	enum{DIM=2};

	interval_t mInterval[DIM];
	double mMajor[DIM];
	int mMinor[DIM];
	float mVel[DIM], mVelW;
	bool mShowAxis[DIM], mShowGrid[DIM], mShowNumbering[DIM], mEqualize;
	bool mLockZoom[DIM];
	bool mLockScroll[DIM];

	int addGridLines(int i, double dist, GraphicsData& gb);

	// map grid coordinate to GLV pixel coordinate
	double gridToPix(int i, double v){
		double r = interval(i).toUnit(v);
		return (i ? 1-r : r) * extentVector[i];
	}

	// map GLV pixel coordinate to grid coordinate
	double pixToGrid(int i, double v){
		if(i) v = extentVector[i]-v;
		return v/extentVector[i] * interval(i).diameter() + interval(i).min();
	}
	double pixToGridMul(int i, double v){ return (interval(i).diameter()/extentVector[i])*v; }

	void pushGrid();	// push into grid space for drawing
	void popGrid();		// pop out of grid space

	void zoomOnMousePos(double amt, const Mouse& m);
};

} // glv::
#endif

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

class Grid : public View{
public:
	Grid(const Rect& r, double rangeMin=-1, double rangeMax=1, double majorDist=1, int minorDiv=4);

	const Interval<double>& interval(int i) const { return mInterval[i]; }
	Interval<double>& interval(int i){ return mInterval[i]; }
//	double major(int dim) const { return mMajor[i]; }
//	int minor(int dim) const { return mMinor[i]; }

	/// Returns whether point is contained within grid region
	bool contains(double x, double y){
		return interval(0).contains(x) && interval(1).contains(y);
	}

	bool showAxes() const { return mShowAxes; }
	bool showGrid() const { return mShowGrid; }

	#define LOOP for(int i=0;i<DIM;++i)
	Grid& minor(int v){ LOOP{ minor(v,i); } return *this; }
	Grid& minor(int v, int i){ mMinor[i]=v; return *this; }
	Grid& major(double v){ LOOP{ major(v,i); } return *this; }
	Grid& major(double v, int i){ mMajor[i]=v; return *this; }

	Grid& origin(){ LOOP{ interval(i).center(0); } return *this; }

	Grid& preserveAspect(bool v){ mPreserveAspect=v; return *this; }

	/// Set all dimension intervals to [-v, v]
	Grid& range(double v){ return range(-v,v); }

	/// Set all dimension intervals to [min, max]
	Grid& range(double min, double max){ LOOP{ range(i, min,max); } return *this; }
	
	/// Set a dimension's interval to [min, max]
	Grid& range(int i, double min, double max){ interval(i).endpoints(min,max); return *this; }

	Grid& showAxes(bool v){ mShowAxes=v; return *this; }
	Grid& showGrid(bool v){ mShowGrid=v; return *this; }

	Grid& zoom(double amt, double x, double y);

	virtual bool onEvent(Event::t e, GLV& g);
	virtual void onAnimate(double dt, GLV& g);
	virtual void onDraw(GLV& g);
//	virtual void onResize(space_t dx, space_t dy);
	
protected:
	typedef Interval<double> interval_t;
	enum{DIM=2};

	interval_t mInterval[DIM];
	double mMajor[DIM];
	int mMinor[DIM];
	float mVel[DIM], mVelW;
	bool mShowAxes, mShowGrid, mPreserveAspect;

	int addGridLines(int i, double dist, draw::GraphicsData& gb);

	// map grid coordinate to pixel coordinate
	double gridToPix(int i, double v){
		double r = interval(i).toUnit(v);
		return (i ? 1-r : r) * extentVector[i];
	}

	// map pixel coordinate to grid coordinate
	double pixToGrid(int i, double v){
		if(i) v = extentVector[i]-v;
		return v/extentVector[i] * interval(i).diameter() + interval(i).min();
	}
	double pixToGridMul(int i, double v){ return (interval(i).diameter()/extentVector[i])*v; }

	// push into grid space for drawing
	void pushGrid(){
		float tx = gridToPix(0, 0);
		float ty = gridToPix(1, 0);
		float sx = gridToPix(0, interval(0).min()+1);
		float sy = gridToPix(1, interval(1).min()+1);
		draw::push();
		draw::translate(tx, ty);
		draw::scale(sx,sy-h);
	}

	// pop out of grid space
	void popGrid(){ draw::pop(); }

	void zoomOnMousePos(double amt, const Mouse& m);
	#undef LOOP
};

} // glv::
#endif

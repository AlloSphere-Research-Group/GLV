#ifndef INC_GLV_LAYOUT_H
#define INC_GLV_LAYOUT_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"


namespace glv{

/// View placer

/// A Placer is used for aligning a series of views. It can position views
/// according to an absolute increment and/or increment relative to the views'
/// geometric extent.
class Placer{
public:

	Placer(space_t absX=0, space_t absY=0);

	Placer(View& parent, Direction dir, Place::t align, space_t x, space_t y, space_t pad=4);

	/// Set a view's position
	Placer& operator<< (View& v);

	/// Set the absolute increment amounts
	Placer& abs(space_t vx, space_t vy);
	
	/// Set view alignment location
	Placer& align(space_t vx, space_t vy);
	
	/// Set view alignment place
	Placer& align(Place::t p);
	
	/// Set anchor place of Views
	Placer& anchor(Place::t p);
	
	/// Reassign the placement flow direction
	Placer& flow(Direction d);
	
	/// Set the placement flow along with absolute and relative incrementing values
	Placer& flow(Direction d, space_t abs, space_t rel=1);
	
	/// Set the current position
	Placer& pos(space_t vx, space_t vy);
	
	/// Set the current x-position
	Placer& posX(space_t v);
	
	/// Set the current y-position
	Placer& posY(space_t v);
	
	/// Set the relative increment factors
	Placer& rel(space_t vx, space_t vy);

protected:
	space_t x, y;	// trajectory point
	space_t rx, ry;	// relative increment factors
	space_t ax, ay;	// absolute increment amounts
	space_t fw, fh; // view alignment fraction [0, 1]
	Place::t mAnchor;
	View * parent;
};




// Base interface class for View auto-layout functors
struct LayoutFunc{

	/// @param[in] w		Width of rect element
	/// @param[in] h		Height of rect element
	/// @param[in] bounds	Layout bounding rect
	LayoutFunc(space_t w, space_t h, const Rect& bounds);
	virtual ~LayoutFunc();
	
	/// Returns next rect element of layout function
	virtual Rect& operator()() = 0;

	void layoutChildren(View& v);
	
	Rect elem;		///< The current rect element
	Rect bounds;		///< The layout bounding rect
	View * parent;
};



// Grid auto-layout functor

// Generates evenly distributed rects within a bounding region.
//
struct LayoutGrid : public LayoutFunc{

	/// @param[in] w		Width of rect element
	/// @param[in] h		Height of rect element
	/// @param[in] bounds	Layout bounding rect
	/// @param[in] numV		Number of vertical divisions
	/// @param[in] numH		Number of horizontal divisions
	LayoutGrid(space_t w, space_t h, const Rect& bounds, int numV, int numH);
	virtual ~LayoutGrid();
	
	
	/// Constructor for auto-sized elements with uniform padding 
	
	/// @param[in] pad		Padding between elements
	/// @param[in] bounds	Layout bounding rect
	/// @param[in] numV		Number of vertical divisions
	/// @param[in] numH		Number of horizontal divisions	
	LayoutGrid(space_t pad, const Rect& bounds, int numV, int numH);
	
	
	LayoutGrid(View& parent, int numH, int numV, space_t pad);
	
	/// Generate next layout rect going left-to-right, then top-to-bottom.
	virtual Rect& operator()();


	LayoutGrid& operator<< (View& v){
		if(parent) (*parent) << v;
		v.set((*this)());
		return *this;
	}
	
protected:
	int numV, numH, cntV, cntH;
};



} // glv::

#endif

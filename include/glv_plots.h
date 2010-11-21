#ifndef INC_GLV_PLOTS_H
#define INC_GLV_PLOTS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <algorithm>
#include <vector>
#include <set>
#include "glv_core.h"
#include "glv_draw.h"
#include "glv_grid.h"
#include "glv_texture.h"
#include "glv_widget.h"

namespace glv{

class Plot;
class Plottable;


/*
We need to be able to map arbitrary vectors of data to the plot colors
and vertices. We can do this by having an ordered list of functors in 
Plottable.
*/

/// Map from model data to graphics data
class GraphicsMap{
public:
	virtual ~GraphicsMap(){}
	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind) = 0;
};



/// Defines a routine for generating plot graphics from model data
class Plottable{
public:

	/// @param[in] prim		drawing primitive
	/// @param[in] stroke	width of lines or points
	/// @param[in] col		color
	Plottable(int prim=draw::Points, float stroke=1, const Color& col=Color(1,0,0))
	:	mPrim(prim), mStroke(stroke), mColor(col), mDrawUnder(false)
	{}

	virtual ~Plottable(){}

	/// Plotting callback
	
	/// The passed in graphics buffers should be filled with the plot data. It
	/// is not necessary to explicitly call any drawing commands as this will be
	/// handled by the Plot.
	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind) = 0;

	/// Called when a new graphics context is created
	virtual void onContextCreate(){}
	
	/// Called when the current graphics context is destroyed
	virtual void onContextDestroy(){}

	/// Get color
	const Color& color() const { return mColor; }
	Plottable& color(const Color& v){ mColor=v; return *this; }

	/// Get data
	const Data& data() const { return mData; }
	Data& data(){ return mData; }
	
	bool drawUnderGrid() const { return mDrawUnder; }
	Plottable& drawUnderGrid(bool v){ mDrawUnder=v; return *this; }
	
	/// Get geometric primitive
	int prim() const { return mPrim; }	
	Plottable& prim(int v){ mPrim=v; return *this; }

	/// Get stroke width (for lines and points)
	int stroke() const { return mStroke; }	
	Plottable& stroke(int v){ mStroke=v; return *this; }

	Plottable& add(GraphicsMap& v){ mGraphicsMaps.push_back(&v); return *this; }

	Plottable& remove(GraphicsMap& v){
		std::remove(mGraphicsMaps.begin(), mGraphicsMaps.end(), &v);
		return *this;
	}

protected:
	friend class Plot;

	typedef std::vector<GraphicsMap *> GraphicsMaps;

	int mPrim;
	float mStroke;
	Color mColor;
	Data mData;
	GraphicsMaps mGraphicsMaps;
	bool mDrawUnder;
	
	void doPlot(GraphicsData& gd, const Data& d){
		if(!d.hasData()) return;
		//draw::color(mColor);
		draw::stroke(stroke());
		
		Indexer ind(d.shape()+1); // dimension 0 is non-spatial
		//onMap(gd, d, ind);

		{	GraphicsMaps::iterator it = mGraphicsMaps.begin();
			while(it != mGraphicsMaps.end()){
				(*it)->onMap(gd, d, ind);
				ind.reset();
				++it;
			}
		}
		
		onDraw(gd, d);
	}
	
	// defines how graphics data should be drawn
	virtual void onDraw(GraphicsData& gd, const Data& d){
		draw::paint(prim(), gd);
	}
};




/// Density plotter
class PlotDensity : public Plottable{
public:

	struct DefaultColorMap : public GraphicsMap{
		virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);
	};
	
	static GraphicsMap& defaultColorMap(){
		static GraphicsMap * m = new DefaultColorMap;
		return *m;
	}
	

	PlotDensity(const Color& c=Color(1,0,0), int interpolate=0);

	/// Set interpolation mode (0=none, 1=linear)
	PlotDensity& interpolate(int v){ mIpol=v; return *this; }

	void onMap(GraphicsData& b, const Data& d, const Indexer& i);

protected:
	virtual void onContextCreate();
	virtual void onContextDestroy();
	virtual void onDraw(GraphicsData& gd, const Data& d);
	Texture2 mTex;
	int mIpol;
};


/// One-dimensional function plotter
struct PlotFunction1D : public Plottable{
	PlotFunction1D(const Color& c=Color(0)): Plottable(draw::LineStrip, 1,c){}
	void onMap(GraphicsData& g, const Data& d, const Indexer& i);
};


/// Two-dimensional function plotter
struct PlotFunction2D : public Plottable{
	PlotFunction2D(const Color& c=Color(0)): Plottable(draw::LineStrip, 1,c){}
	void onMap(GraphicsData& g, const Data& d, const Indexer& i);
};



/// Plots data according to one or more attached Plottables
class Plot : public Grid {
public:
	typedef std::vector<Plottable *> Plottables;

	Plot(const Rect& r=Rect(0));

	Plot(const Rect& r, Plottable& p);

	Plottables& plottables(){ return mPlottables; }
	const Plottables& plottables() const { return mPlottables; }

	int valueIndex(int i) const { return mValInd[i]; }

	/// Add new plotting routine
	Plot& add(Plottable& v);

	Plot& remove(Plottable& v);

	Plot& valueIndex(int from, int to){
		mValInd[from] = to; return *this;
	}

	virtual const char * className() const { return "Plot"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	Plottables mPlottables;
	int mValInd[4];
	
	void resetValInd(){
		for(int i=0; i<4; ++i) mValInd[i]=i;
	}
};


} // glv::
#endif

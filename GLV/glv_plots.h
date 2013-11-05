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



/// Map from model data to graphics data
class GraphicsMap{
public:
	virtual ~GraphicsMap(){}
	
	/// Defines how data should be mapped to graphics primitives
	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind) = 0;
};



/// Defines a routine for generating plot graphics from model data
class Plottable : public GraphicsMap{
public:

	enum BlendMode {
		TRANSLUCENT,
		ADDITIVE,
		SUBTRACTIVE
	};

	Plottable(const Plottable& src);

	/// @param[in] prim		drawing primitive
	/// @param[in] stroke	width of lines or points
	Plottable(int prim=draw::Points, float stroke=1);

	/// @param[in] prim		drawing primitive
	/// @param[in] stroke	width of lines or points
	/// @param[in] col		color
	Plottable(int prim, float stroke, const Color& col);

	virtual ~Plottable(){}


	/// Called when a new graphics context is created
	virtual void onContextCreate(){}
	
	/// Called when the current graphics context is destroyed
	virtual void onContextDestroy(){}

	/// Defines how graphics data should be drawn
	virtual void onDraw(GraphicsData& gd, const Data& d){
		draw::paint(prim(), gd);
	}

	/// Defines how data should be mapped to graphics primitives
	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind){}


	/// Set whether active
	Plottable& active(bool v){ mActive=v; return *this; }
	
	/// Get whether active
	bool active() const { return mActive; }

	/// Set blend mode
	Plottable& blendMode(BlendMode v){ mBlendMode=v; return *this; }

	/// Get blend mode
	BlendMode blendMode() const { return mBlendMode; }

	/// Get color
	const Color& color() const { return mUseStyleColor ? Style::standard().color.fore : mColor; }
	Plottable& color(const Color& v){ mUseStyleColor=false; mColor=v; return *this; }
	
	Plottable& useStyleColor(bool v){ mUseStyleColor=v; return *this; }

	/// Get data
	const Data& data() const { return mData; }
	Data& data(){ return mData; }
	
	bool drawUnderGrid() const { return mDrawUnder; }
	Plottable& drawUnderGrid(bool v){ mDrawUnder=v; return *this; }
	
	/// Get line stippling pattern
	short lineStipple() const { return mLineStipple; }
	
	/// Set line stippling pattern
	Plottable& lineStipple(short v){ mLineStipple=v; return *this; }
	
	/// Get geometric primitive
	int prim() const { return mPrim; }	

	/// Set geometric primitive
	Plottable& prim(int v){ mPrim=v; return *this; }

	/// Get line stroke width (for lines and points)
	float stroke() const { return mStroke; }	

	/// Set line stroke width (for lines and points)
	Plottable& stroke(float v){ mStroke=v; return *this; }

	/// Add a graphics map
	Plottable& add(GraphicsMap& v);

	/// Remove a graphics map
	Plottable& remove(GraphicsMap& v);


	Plottable& operator= (const Plottable& src);

	// Plot (map and draw) data
	virtual void doPlot(GraphicsData& gd, const Data& d);

protected:
	friend class Plot;
	typedef std::vector<GraphicsMap *> GraphicsMaps;

	Data mData;
	GraphicsMaps mGraphicsMaps;

	struct POD1{} mPOD1;
	int mPrim;
	float mStroke;
	BlendMode mBlendMode;
	Color mColor;
	short mLineStipple;
	bool mDrawUnder;
	bool mUseStyleColor;
	bool mActive;
	struct POD2{} mPOD2;

};




/// Density plotter
class PlotDensity : public Plottable{
public:

	/// @param[in] color		plot color
	/// @param[in] hueSpread	amount spread hue for positive and negative values
	/// @param[in] interpolate	interpolation (0=none, 1=linear)
	PlotDensity(const Color& color=Color(1,0,0), float hueSpread=0, int interpolate=0);

	PlotDensity& hueSpread(float v){ mHueSpread=v; return *this; }

	/// Set interpolation mode (0=none, 1=linear)
	PlotDensity& interpolate(int v){ mIpol=v; return *this; }

	/// Set rectangular plotting region
	PlotDensity& plotRegion(const Interval<double>& vx, const Interval<double>& vy){
		mRegion[0] = vx; mRegion[1] = vy; return *this;
	}

//	static GraphicsMap& defaultColorMap();
//
//	struct DefaultColorMap : public GraphicsMap{
//		virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);
//	};

	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);

protected:
	virtual void onContextCreate();
	virtual void onContextDestroy();
	virtual void onDraw(GraphicsData& gd, const Data& d);
	Texture2 mTex;
	Interval<double> mRegion[2];
	float mHueSpread;
	int mIpol;
};


/// One-dimensional function plotter

/// The shape of the model data should be (1, N or 1, 1 or N) where N is the
/// number of elements to plot. If the shape is (1, N, 1) then the domain
/// is along the x axis. Likewise, if the shape is (1, 1, N) then the domain
/// is along the y axis. Sizes greater than 1 for dimension 0 are reserved for
/// future use.
class PlotFunction1D : public Plottable{
public:

	/// Path styles
	enum PathStyle{
		DIRECT,			/**< Function values are vertices */
		ZIGZAG,			/**< Zig-zag between domain axis and function values */
//		SPIKE,
//		STEP
	};


	/// @param[in] color	plot color
	/// @param[in] stroke	stroke width
	/// @param[in] prim		drawing primitive
	/// @param[in] path		path style
	PlotFunction1D(
		const Color& color = Color(0),
		float stroke = 1,
		int prim = draw::LineStrip,
		PathStyle path = DIRECT
	);

	/// Set offset from zero of plot domain
	PlotFunction1D& domainOffset(int v){ mDomainOffset=v; return *this; }

	/// Set drawing path style
	PlotFunction1D& pathStyle(PathStyle v){ mPathStyle=v; return *this; }

	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);

//	static GraphicsMap& defaultVertexMap();
//
//	struct DefaultVertexMap : public GraphicsMap{
//		virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);
//	};

protected:
	PathStyle mPathStyle;
	int mDomainOffset;
};


/// Two-dimensional function plotter
class PlotFunction2D : public Plottable{
public:

	/// @param[in] color	plot color
	PlotFunction2D(const Color& color=Color(0));

	virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);

//	static GraphicsMap& defaultVertexMap();
//
//	struct DefaultVertexMap : public GraphicsMap{
//		virtual void onMap(GraphicsData& b, const Data& d, const Indexer& ind);
//	};
};



/// Plots data according to one or more attached Plottables
class Plot : public Grid, public DataObject {
public:
	typedef std::vector<Plottable *> Plottables;

	/// @param[in] r	geometry
	Plot(const Rect& r=Rect(0));

	/// @param[in] r	geometry
	/// @param[in] p	plottable
	Plot(const Rect& r, Plottable& p);
	
	/// @param[in] r	geometry
	/// @param[in] p1	first plottable
	/// @param[in] p2	second plottable
	Plot(const Rect& r, Plottable& p1, Plottable& p2);

	Plottables& plottables(){ return mPlottables; }
	const Plottables& plottables() const { return mPlottables; }

	/// Add new plotting routine
	Plot& add(Plottable& v);

	Plot& remove(Plottable& v);

	virtual const char * className() const { return "Plot"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	Plottables mPlottables;
	std::vector<char> mActive;
//	int mValInd[4];
//	
//	void resetValInd(){
//		for(int i=0; i<4; ++i) mValInd[i]=i;
//	}

//	int valueIndex(int i) const { return mValInd[i]; }

//	Plot& valueIndex(int from, int to){
//		mValInd[from] = to; return *this;
//	}
};


} // glv::
#endif

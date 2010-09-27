#ifndef INC_GLV_PLOTS_H
#define INC_GLV_PLOTS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>
#include <set>
#include "glv_core.h"
#include "glv_draw.h"
#include "glv_widget.h"

namespace glv {

/* TODO:
- multiple representations of the same data (use 1D pointers and copy-on-draw)
- multiple plots on same graph
*/


/*
Plot plotX, plotY, plotXY;

vector<Point2> data1, data2;

plotX.addData(&data1[0].x, data1.size(), 2, PlotDim::X);
plotY.addData(&data1[0].y, data1.size(), 2, PlotDim::Y);
plotXY.addData(&data1[0], data1.size(), 1, PlotDim::XY);
*/

class Plot;

/// Define the dependent dimension(s) of a plot
namespace PlotDim{ 
	enum t{
//		None= 0,
		X	= 1<<0,
		Y	= 1<<1,
		XY	= X|Y
	};
	inline t operator| (const t& a, const t& b){ return t(int(a) | int(b)); }
}


class PlotData{
public:

	PlotData();
	explicit PlotData(int size, PlotDim::t dim);

	~PlotData();

	PlotData& dotEnds(bool v);				///< Set whether to draw dots at end points of 2-D plot
	PlotData& dotPoints(bool v);			///< Set whether to draw dots at points of 2-D plot
	PlotData& interpolate(bool v);			///< Set whether to apply cubic interpolation to 2-D plot
	PlotData& color(const Color& c);		///< Set plotting color
	PlotData& drawType(int primitive);		///< Set drawing primitive type
	PlotData& interval(int min, int max);	///< Set range of buffer indices to plot to [min, max)
	PlotData& stroke(float width);			///< Set stroke width of pen

	PlotData& reference(int dim, float * src, int size, int stride=1){
		mPointsRef[dim] = src;
		mStrides[dim] = stride;
		mSize=size;
		interval(0, mSize);
		return *this;
	}

	PlotData& resize(int size, PlotDim::t dim);		///< Change size of internal buffers
	PlotData& zero();								///< Zero point buffer

	Color * colors();
	float * points(int dim) const;

	bool dotPoints() const { return mDotPoints; }	///< Get whether points are dotted
	bool dotEnds() const { return mDotEnds; }		///< Get whether endpoints are dotted
	int size() const { return mSize; }				///< Get size of internal data buffer(s)
	int stride(int dim) const { return mPointsRef[dim] ? mStrides[dim] : 1; }
	float stroke() const { return mStroke;}			///< Get stroke width

protected:
	friend class Plot;
	enum{ DIMS=2 };

	int mSize;
	int mIMin, mIMax;
	int mStrides[DIMS];
	float * mPoints[DIMS];
	float * mPointsRef[DIMS];
	Color * mColor;
	Color * mColors;

	int mDrawPrim;
	float mStroke;
	bool mDotEnds, mDotPoints, mInterpolate;

	bool isX() const { return mPoints[0] || mPointsRef[0]; }
	bool isY() const { return mPoints[1] || mPointsRef[1]; }
	bool isXY() const { return isX() && isY(); }

	void allocColors(){ freeColors(); mColors = new Color[size()]; }
	void free(){
		for(int i=0; i<DIMS; ++i){
			if(mPoints[i]) delete[] mPoints[i];
		}
		freeColors();
	}
	void freeColors(){ if(mColors) delete[] mColors; mColors=0; }
};




//class PlotData{
//public:
//
//	PlotData();
//	~PlotData();
//
//	explicit PlotData(int size);//, const Color& color=Color(0.5));
//
//	Point2 * points();
//	Color * colors();
//
//	PlotData& dotEnds(bool v);				///< Set whether to draw dots at end points of 2-D plot
//	PlotData& dotPoints(bool v);			///< Set whether to draw dots at points of 2-D plot
//	PlotData& interpolate(bool v);			///< Set whether to apply cubic interpolation to 2-D plot
//	PlotData& color(const Color& c);		///< Set plotting color
//	PlotData& drawType(int primitive);		///< Set drawing primitive type
//	PlotData& interval(int min, int max);	///< Set range of buffer indices to plot to [min, max)
//	PlotData& PlotData::plotDim(PlotDim::t v);
//	PlotData& stroke(float width);			///< Set stroke width of pen
//
//	PlotData& resize(int n);				///< Change size of internal buffers
//	PlotData& zero();						///< Zero point buffer
//
//	bool dotPoints() const { return mDotPoints; }	///< Get whether points are dotted
//	bool dotEnds() const { return mDotEnds; }		///< Get whether endpoints are dotted
//	int size() const { return mSize; }				///< Get size of internal data buffer(s)
//	float stroke() const { return mStroke;}			///< Get stroke width
//
//	void draw(float mulX, float addX, float mulY, float addY, const Color& defaultCol) const;
//
//protected:
//	PlotDim::t mPlotDim;
//	int mSize;
//	int mIMin, mIMax;
//	Point2 * mPoints;
//	Color * mColors;
//	Color * mColor;
//	int mDrawPrim;
//	float mStroke;
//	bool mDotEnds, mDotPoints, mInterpolate;
//
//	void allocPoints(){ freePoints(); mPoints = new Point2[size()]; zero(); }
//	void allocColors(){ freeColors(); mColors = new Color[size()]; }
//	void freePoints(){ if(mPoints) delete[] mPoints; mPoints=0; }
//	void freeColors(){ if(mColors) delete[] mColors; mColors=0; }
//	bool hasX() const { return mPlotDim==PlotDim::X || mPlotDim==PlotDim::XY; }
//	bool hasY() const { return mPlotDim==PlotDim::Y || mPlotDim==PlotDim::XY; }
//};


/// A 1D or 2D function plot

/// This can be used for either line or scatter plots.
///
class Plot : public View{
public:

	Plot(const Rect& r=Rect(100));

//	/// @param[in] r			geometry
//	/// @param[in] data			plot data
//	Plot(const Rect& r, const PlotData& data);

	Plot(const Rect& r, const Color& plotColor);

	/// @param[in] r			geometry
	/// @param[in] size			size of internal data buffer(s)
	Plot(const Rect& r, int size, PlotDim::t dim);
	
	/// @param[in] r			geometry
	/// @param[in] size			size of internal data buffer(s)
	/// @param[in] plotColor	color of plot
	Plot(const Rect& r, int size, PlotDim::t dim, const Color& plotColor);

	virtual ~Plot();

	Plot& center();							///< Center axes
	Plot& equalizeAxes();					///< Make axis dimensions the same
	Plot& preserveAspect(bool v);			///< Set whether aspect ratio is preserved to 1:1
	Plot& range(float ext);					///< Set range of x & y axes to [-ext,ext]
	Plot& rangeFit(float padPercentage=10);
	Plot& rangeX(float ext);				///< Set range of x axis to [-ext,ext]
	Plot& rangeY(float ext);				///< Set range of y axis to [-ext,ext]
	Plot& rangeX(float min, float max);		///< Set range of x axis to [min,max]
	Plot& rangeY(float min, float max);		///< Set range of y axis to [min,max]
	Plot& showAxes(bool v);					///< Set whether to show axes
	Plot& tickMajor(float d);				///< Set major tick distance for x and y (<= 0 turns off)
	Plot& tickMajorX(float d);				///< Set major tick distance for x (<= 0 turns off)
	Plot& tickMajorY(float d);				///< Set major tick distance for x (<= 0 turns off)
//	Plot& zero();							///< Zeroes all buffer elements

	PlotData * createData(int size, PlotDim::t dim);
	Plot& addData(PlotData& v);

	PlotData& data(int i=0);

	virtual const char * className() const { return "Plot"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);

protected:
	enum{ DIM=2 };
	std::vector<Point2> mPoints;
	std::vector<PlotData *> mData;
	std::set<PlotData *> mOwnData;

	float mMin[DIM], mMax[DIM];
	float mTickMajorX, mTickMajorY;
	bool mShowAxes, mPreserveAspect;

	void plotExtent(float& pw, float& ph);
	void zoom(float px, float py, float zm);
	Plot& validate();

	void draw(const PlotData& d, float mulX, float addX, float mulY, float addY, const Color& defaultCol);
};


/// Density plot

/// Model data should have a shape of (nc, nx, ny) where 'nc' si the number of
/// components of the value (e.g., a scalar is 1) and 'nx' and 'ny' are the size
/// of the x and y dimensions, respectively.
class DensityPlot : public Widget {
public:
	DensityPlot(const Rect& r=Rect());

	DensityPlot& color(const Color& v){ mColor1=v; return *this; }

	virtual const char * className() const { return "DensityPlot"; }
	virtual void onDraw();

protected:
	Color mColor1;
	float val(int i, int j, int k){ return to01(model().at<float>(i,j,k)); }
};


/*
Requirements:
Multiple graphs viewable within same plotting region.
Support for different plotting styles- line, bar, scatter, polar
Support for 1D and 2D plots

Shape
(1, Nx,  1, 1)	1D independent on y-axis
(1,  1, Ny, 1)	1D independent on x-axis
(1, Nx, Ny, 1)	?
(2, Nx,  1, 1)	2D x-y plot
(2,  1, Ny, 1)	2D x-y plot
()

All plotters present multidimensional arrays in a consistent way. Dimensions
are treated as follows:

0: color/position
1: along x
2: along y
3: planes

Type		Shape			Memory
1D x		(1, n, 1)		(x1, x2, x3)
1D y		(1, 1, n)		((y1, y2, y3))
2D Scatter	(2, n)			((x1, y1), (x2, y2), (x3, y3))				
3D Scatter	(3, n)			((x1, y1, z1), (x2, y2, z2), (x3, y3, z3))
xy			(n, 2)			((x1, x2, x3), (y1, y2, y3))
xyz			(n, 3)			((x1, x2, x3), (y1, y2, y3), (z1, z2, z3))

Density		(C, n, m)		((v11, v21, v31), (v12, v22, v32))
Surface z	(1, n, m, 1)	((v11, v21, v31), (v12, v22, v32))
Surface y	(1, n, 1, m)	((v11, v21, v31), (v12, v22, v32))
Surface x	(1, 1, n, m)	((v11, v21, v31), (v12, v22, v32))

*/
class DataPlot : public Widget {
public:

protected:
};



} // glv::
#endif

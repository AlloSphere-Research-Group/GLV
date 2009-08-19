#ifndef INC_GLV_PLOTS_H
#define INC_GLV_PLOTS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv {


/// A 1D or 2D function plot

///
///
class FunctionPlot : public View{
public:

	/// @param[in] r			geometry
	/// @param[in] size			size of internal data buffer(s)
	FunctionPlot(const Rect& r=Rect(0), int size=128);
	
	/// @param[in] r			geometry
	/// @param[in] size			size of internal data buffer(s)
	/// @param[in] plotColor	color of plot
	FunctionPlot(const Rect& r, int size, const Color& plotColor);

	~FunctionPlot();

	int size() const { return mSize; }				///< Get size of internal data buffer(s)
	
	FunctionPlot& drawType(int primitive);			///< Set drawing primitive type
	FunctionPlot& center();							///< Center axes
	FunctionPlot& dotEnds(bool v);					///< Set whether to draw dots at end points of 2-D plot
	FunctionPlot& plotColor(const Color& c);		///< Set plotting color
	FunctionPlot& range(float ext);					///< Set range of x & y axes to [-ext,ext]
	FunctionPlot& rangeX(float ext);				///< Set range of x axis to [-ext,ext]
	FunctionPlot& rangeY(float ext);				///< Set range of y axis to [-ext,ext]
	FunctionPlot& rangeX(float min, float max);		///< Set range of x axis to [min,max]
	FunctionPlot& rangeY(float min, float max);		///< Set range of y axis to [min,max]
	FunctionPlot& resize(int n);					///< Change size of internal buffers
	FunctionPlot& showAxes(bool v);					///< Set whether to show axes
	FunctionPlot& stroke(float width);				///< Set stroke width of pen
	FunctionPlot& tickMajor(float d);				///< Set major tick distance (<= 0 turns off)
	
	FunctionPlot& zero();							///< Zeroes all buffer elements
	
	float * bufferX();								///< Returns internal x value buffer
	float * bufferY();								///< Returns internal y value buffer
	Color * bufferColor();							///< Returns internal color (z) buffer

	virtual const char * className() const { return "FunctionPlot"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& glv);

protected:
	int mSize;
	float * mBufX, * mBufY;
	Color * mBufCol;
	Color * mPlotColor;
	float mMinX, mMaxX, mMinY, mMaxY;
	int mDrawPrim;
	float mStroke;
	float mTickMajor;
	bool mShowAxes, mDotEnds;
	void freeX(){ if(mBufX) delete[] mBufX; mBufX=0; }
	void freeY(){ if(mBufY) delete[] mBufY; mBufY=0; }
	void freeCol(){ if(mBufCol) delete[] mBufCol; mBufCol=0; }
	void allocX(){ freeX(); mBufX = new float[size()]; zero(mBufX); }
	void allocY(){ freeY(); mBufY = new float[size()]; zero(mBufY); }
	void allocCol(){ freeCol(); mBufCol = new Color[size()]; }
	void sort(float& a, float& b){ if(a>b){ float t=a; a=b; b=t; }  }
	void zero(float * b){ for(int i=0; i<size(); ++i) b[i]=0.f; }
};


} // glv::
#endif
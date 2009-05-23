/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_plots.h"

namespace glv{

#define CTOR_INIT_LIST \
	View(r), mSize(size), mBufX(0), mBufY(0), mBufCol(0), mPlotColor(0),\
	mMinX(-1), mMaxX(1), mMinY(-1), mMaxY(1), \
	mDrawPrim(draw::LineStrip), mStroke(1)

FunctionPlot::FunctionPlot(const Rect& r, int size)
:	CTOR_INIT_LIST
{}

FunctionPlot::FunctionPlot(const Rect& r, int size, const Color& c)
:	CTOR_INIT_LIST
{	plotColor(c); }

#undef CTOR_INIT_LIST


FunctionPlot::~FunctionPlot(){
	freeX(); freeY(); freeCol();
	if(mPlotColor) delete mPlotColor;
}

FunctionPlot& FunctionPlot::drawType(int primitive){ mDrawPrim=primitive; return *this; }
FunctionPlot& FunctionPlot::range(float ext){ rangeX(ext); return rangeY(ext); }
FunctionPlot& FunctionPlot::rangeX(float ext){ return rangeX(-ext, ext); }
FunctionPlot& FunctionPlot::rangeY(float ext){ return rangeY(-ext, ext); }
FunctionPlot& FunctionPlot::rangeX(float min, float max){ mMaxX=max; mMinX=min; return *this; }
FunctionPlot& FunctionPlot::rangeY(float min, float max){ mMaxY=max; mMinY=min; return *this; }
FunctionPlot& FunctionPlot::stroke(float width){ mStroke=width; return *this; }

float * FunctionPlot::bufferX(){ if(0 == mBufX) allocX(); return mBufX; }
float * FunctionPlot::bufferY(){ if(0 == mBufY) allocY(); return mBufY; }
Color * FunctionPlot::bufferColor(){ if(0 == mBufCol) allocCol(); return mBufCol; }

FunctionPlot& FunctionPlot::plotColor(const Color& c){
	if(!mPlotColor) mPlotColor = new Color(c);
	else mPlotColor->set(c);
	return *this;
}

void FunctionPlot::onDraw(){
	using namespace glv::draw;

	enable(PointSmooth);
	draw::stroke(mStroke);

	float mulX = w / (mMaxX - mMinX);
	float addX = -mulX*mMinX;
	float mulY = -h/ (mMaxY - mMinY);
	float addY = mulY*mMinY;

	if(0 == mBufCol){
		color(mPlotColor ? *mPlotColor : colors().fore);

		begin(mDrawPrim);
		
			if(mBufX && mBufY){			// xy-plot
				for(int i=0; i<size(); ++i) vertex(mBufX[i]*mulX+addX, mBufY[i]*mulY+addY);
			}
			
			else if(mBufY && !mBufX){	// y-plot
				double dx = w/(size()-1), x=0;
				for(int i=0; i<size(); ++i){
					vertex(x, mBufY[i]*mulY+addY); x+=dx;
				}
			}
			
			else if(mBufX && !mBufY){	// x-plot
				double dy = h/(size()-1), y=0;
				for(int i=0; i<size(); ++i){
					vertex(mBufX[i]*mulX+addX, y); y+=dy;
				}
			}
		end();
	}
}

} // glv::

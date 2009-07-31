/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_plots.h"

namespace glv{

#define CTOR_INIT_LIST \
	View(r), mSize(size), mBufX(0), mBufY(0), mBufCol(0), mPlotColor(0),\
	mMinX(-1), mMaxX(1), mMinY(-1), mMaxY(1), \
	mDrawPrim(draw::LineStrip), mStroke(1), mTickMajor(1), \
	mShowAxes(true)

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

FunctionPlot& FunctionPlot::center(){
	rangeX((mMaxX-mMinX)*0.5);
	rangeY((mMaxY-mMinY)*0.5);
}

FunctionPlot& FunctionPlot::range(float v){ rangeX(v); return rangeY(v); }
FunctionPlot& FunctionPlot::rangeX(float v){ return rangeX(-v, v); }
FunctionPlot& FunctionPlot::rangeY(float v){ return rangeY(-v, v); }
FunctionPlot& FunctionPlot::rangeX(float min, float max){ sort(min, max); mMaxX=max; mMinX=min; return *this; }
FunctionPlot& FunctionPlot::rangeY(float min, float max){ sort(min, max); mMaxY=max; mMinY=min; return *this; }
FunctionPlot& FunctionPlot::showAxes(bool v){ mShowAxes=v; return *this; }	
FunctionPlot& FunctionPlot::stroke(float v){ mStroke=v; return *this; }
FunctionPlot& FunctionPlot::tickMajor(float v){ mTickMajor=v; return *this; }

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

	float mulX = w / (mMaxX - mMinX);
	float addX =-mulX*mMinX;
	float mulY =-h / (mMaxY - mMinY);
	float addY = mulY*mMinY;

	// draw axes
	if(mShowAxes){
		color(colors().text);
		lineWidth(1);
		begin(Lines);
			if(mBufY) vertex(0,addY); vertex(w,addY);
			if(mBufX) vertex(addX,0); vertex(addX,h);
		end();
		
		if(mTickMajor > 0){
		
			struct F{
				F(float tick, float min, float max, float mul1, float add1, float add2, bool isX){
					int n = (max - min)/tick + 1;

					if(n < 128){
						// find closest multiple of mTickMajor to mMinX
						float p1 = (int(min/tick) * tick)*mul1+add1;
						float d1 = tick*mul1;
						float d2 = 4, d2m = add2-d2, d2p = add2+d2;
						
						begin(Lines);
						if(isX) for(int i=0; i<n; ++i){ vertex(p1, d2m); vertex(p1, d2p); p1 += d1; }
						else    for(int i=0; i<n; ++i){ vertex(d2m, p1); vertex(d2p, p1); p1 += d1; }
						end();
					}				
				}
			};
			
			if(mBufY && mBufX){
				F(mTickMajor, mMinX, mMaxX, mulX, addX, addY, true);
				F(mTickMajor, mMinY, mMaxY,-mulY, addY, addX, false);
			}

		}
	}

	if(0 == mBufCol){

		// draw curve
		color(mPlotColor ? *mPlotColor : colors().fore);
		draw::stroke(mStroke);
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


bool FunctionPlot::onEvent(Event::t e, GLV& g){
	Keyboard& k = g.keyboard;
	Mouse& m    = g.mouse;

	switch(e){
	case Event::MouseDown:
		return false;
	
	case Event::MouseDrag:{

		float dx = m.dx();
		float dy = m.dy();
		
		if(m.left()){
			float ax = ((mMaxX - mMinX)/w) * dx;
			float ay = ((mMaxY - mMinY)/h) * dy;
			if(mBufX) rangeX(mMinX - ax, mMaxX - ax);
			if(mBufY) rangeY(mMinY - ay, mMaxY - ay);
		}
		
		if(m.right()){
			float px = m.xRel(Mouse::Right);
			float py = m.yRel(Mouse::Right);
			float mul = pow(2, dy*0.1);
			float cx = mMinX + (px/w)*(mMaxX-mMinX);
			float cy = mMinY + (py/h)*(mMaxY-mMinY);
			rangeX((mMinX-cx)*mul+cx, (mMaxX-cx)*mul+cx);
			rangeY((mMinY-cy)*mul+cy, (mMaxY-cy)*mul+cy);
		}
		
		return false;
	}
		
	
	case Event::KeyDown:
		switch(k.key()){
			case 'c': center(); return false;
		}
		
	}
	
	return true;
}


void FunctionPlot::zero(){
	if(mBufX) for(int i=0; i<size(); ++i) mBufX[i]=0.f;
	if(mBufY) for(int i=0; i<size(); ++i) mBufY[i]=0.f;
}

} // glv::

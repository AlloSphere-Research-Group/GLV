/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_sliders.h"
#include "glv_draw.h"

namespace glv{

// Slider2D

Slider2D::Slider2D(const Rect& r, float valX, float valY, space_t knobSize)
:	SliderBase<2>(r), knobSize(knobSize)
{
	value(valX, 0);
	value(valY, 1);
}

bool Slider2D::onEvent(Event::t e, GLV& g){

	switch(e){
		case Event::MouseDrag:
			valueAdd( g.mouse.dx()/w * sens(g), 0);
			valueAdd(-g.mouse.dy()/h * sens(g), 1); 		
			notify();
			break;
			
		case Event::MouseDown:
			if(g.mouse.left() && !g.mouse.right()){
				value(      g.mouse.xRel() / w, 0);
				value(1.f - g.mouse.yRel() / h, 1);
			}
			notify();
			break;
			
		case Event::MouseUp: clipAccs(); break;
			
		case Event::KeyDown:
			switch(g.keyboard.key()){
				case Key::Left:		valueAdd(-1. / w, 0); notify(); break;
				case Key::Right:	valueAdd( 1. / w, 0); notify(); break;
				case Key::Up:		valueAdd( 1. / h, 1); notify(); break;
				case Key::Down:		valueAdd(-1. / h, 1); notify(); break;
				default: return true;
			}
			break;
		default: break;
	}
	return false;
}


void Slider2D::drawKnob(const Slider2D& s){
	using namespace glv::draw;
	float sz = s.knobSize;	// size of indicator block
	float sz2 = sz * 0.5f;
	float posX = sz2 + (s.w - sz) * s.value(0);
	float posY = sz2 + (s.h - sz) * (1.f - s.value(1));
	
	color(s.colors().fore);
	rect(pix(posX - sz2), pix(posY - sz2), pix(posX + sz2), pix(posY + sz2));
}

/*
static void drawQuad(const Slider2D& s){
	using namespace glv::draw;
	float posX = s.w * s.value(0);
	float posY = s.h * (1.f - s.value(1));
	
	color(s.colors().fore);
	
	begin(Quads);
		vertex(0.f, posY);
		vertex(posX, s.h);
		vertex(s.w, posY);
		vertex(posX, 0.f);
	end();
	
	// clock hand
	//shape(Lines, posX, posY, s.w*0.5, s.h*0.5);
}
*/

void Slider2D::onDraw(){

	drawKnob(*this);
	//drawQuad(*this);
	
//	float hh = h * 0.5f;
//	float hw = w * 0.5f;

	// Web
//	glBegin(GL_LINES);
//		glVertex2f(posX, posY); glVertex2f(hw, h);
//		glVertex2f(posX, posY); glVertex2f(w, hh);
//		glVertex2f(posX, posY); glVertex2f(hw, 0.f);
//		glVertex2f(posX, posY); glVertex2f(0.f, hh);
//
//		glVertex2f(posX, posY); glVertex2f(0.f, h);
//		glVertex2f(posX, posY); glVertex2f(w, h);
//		glVertex2f(posX, posY); glVertex2f(w, 0.f);
//		glVertex2f(posX, posY); glVertex2f(0.f, 0.f);
//
////		glVertex2f(0.f, posY); glVertex2f(w, posY);
////		glVertex2f(posX, 0.f); glVertex2f(posX, h);
//	glEnd();
	
	//glColor4f(glvColor4(bgColor));
	//glRectf(posX, h, posX+1, 0.f);
	//glRectf(0.f, posY, w, posY+1);

//	if((posX > 6.f) && (posY < (h - 6.f))) return;

	// fade color in
	//s->sliderColor->glColor3(1.f - ((posX < h - posY) ? posX : h - posY) * 0.16667f);

	// double diagonal indicators
//	glBegin(GL_LINES);
//		glVertex2f(posX, posY); glVertex2f(w, hh);
//		glVertex2f(posX, posY); glVertex2f(hw, 0.f);
//	glEnd();
	
	// x
//	glBegin(GL_LINES);
//		float d = 6.f;
//		glVertex2f(posX - d, posY - d);
//		glVertex2f(posX + d, posY + d);
//		glVertex2f(posX, posY);
//		glVertex2f(posX + d, posY - d);
//	glEnd();
	
	// overline
//	glBegin(GL_LINES);
//		float d = 6.f;
//		glVertex2f(0.f, h - d);
//		glVertex2f(posX, h - d);
//		glVertex2f(d, h);
//		glVertex2f(d, posY);
//	glEnd();
}



FunctionGraph::FunctionGraph(const Rect& r, int nKnots, int res)
: View(r), mTension(0.), mKnobSize(3), mCurrentKnot(-1), mNKnots(nKnots), mKnots(0)
{
	//minimum 3 knots
	if(mNKnots < 3) mNKnots = 3;
	
	mKnots = new Knot[mNKnots];
	for(int i=0; i < mNKnots; i++) {
		mKnots[i].x = ((float)i)/(mNKnots-1);
		mKnots[i].y = mKnots[i].x*mKnots[i].x;
	}
	
	for(int i=0; i < mNKnots-1; i++) {
		mCurves.push_back(new Curve(res));
	}
	calcCurves();
}

FunctionGraph::~FunctionGraph()
{
	if(mKnots) delete[] mKnots;
	std::vector<Curve *>::iterator it = mCurves.begin();
	std::vector<Curve *>::iterator it_e = mCurves.end();
	for(; it != it_e; ++it){
		delete *it;
	}
}

void FunctionGraph::calcCurves()
{
	std::vector<Curve *>::iterator it = mCurves.begin();
	std::vector<Curve *>::iterator it_e = mCurves.end();
	int i=0;
	for(; it != it_e; ++it){
		//for now we duplicate boundry points
		int idx0, idx1, idx2, idx3;
		
		if(i==0) {
			idx0 = idx1 = 0;
			idx2 = 1;
			idx3 = 2;
		}
		else if(i == (mNKnots-2)) {
			idx0 = i-1;
			idx1 = i;
			idx2 = idx3 = mNKnots-1;
		}
		else {
			idx0 = i-1;
			idx1 = i;
			idx2 = i+1;
			idx3 = i+2;
		}
	
		Curve &c = *(*it);
	
		float mu = 0.;
		float dmu = 1./(float)(c.size()-1);
		for(int t = 0; t < c.size(); t++) {
			c[t] = HermiteInterpolate(mKnots[idx0].y, mKnots[idx1].y, mKnots[idx2].y, mKnots[idx3].y, mu, mTension, 0);
			mu += dmu;
		}
		
		i++;
	}
}

void FunctionGraph::eval(int n, float *vals)
{
	float dx = 1./(n-1);
	float x = 0;
	int idx = 0;
	int k = 0;
	vals[idx] = mKnots[k].y;
	
	int idx0 = 0;
	int idx1 = 0;
	int idx2 = 1;
	int idx3 = 2;

	for(; idx < n-1; idx++) {
		if(x > mKnots[k+1].x) {
			while(x > mKnots[k+1].x) {
				k++;
			}
			
			if(k == (mNKnots-2)) {
				idx0 = k-1;
				idx1 = k;
				idx2 = idx3 = mNKnots-1;
			}
			else {
				idx0 = k-1;
				idx1 = k;
				idx2 = k+1;
				idx3 = k+2;
			}
		}
		
		float mu = (x - mKnots[k].x) / (mKnots[k+1].x - mKnots[k].x);
		vals[idx] = HermiteInterpolate(mKnots[idx0].y, mKnots[idx1].y, mKnots[idx2].y, mKnots[idx3].y, mu, mTension, 0);
		
		x += dx;
	}
	
	vals[idx] = mKnots[k].y;
}

void FunctionGraph::onDraw()
{
	using namespace glv::draw;

	
	std::vector<Curve *>::iterator it = mCurves.begin();
	std::vector<Curve *>::iterator it_e = mCurves.end();
	
	color(mStyle->color.fore);
	begin(LineStrip);
	int i=0;
	for(; it != it_e; ++it){
		Curve &c = *(*it);
		
		float dx = (mKnots[i+1].x - mKnots[i].x)/(c.size()-1);
		float x = mKnots[i].x;
		for(int t = 0; t < c.size(); t++) {
			vertex(x*w, (1.-c[t])*h);
			x += dx;
		}
		i++;
	}
	end();
	
	color(mStyle->color.fore, mStyle->color.fore.a*0.5);
	begin(QuadStrip);
	i=0;
	it = mCurves.begin();
	for(; it != it_e; ++it){
		Curve &c = *(*it);
		
		float dx = (mKnots[i+1].x - mKnots[i].x)/(c.size()-1);
		float x = mKnots[i].x;
		for(int t = 0; t < c.size(); t++) {
			vertex(x*w, (1.-c[t])*h);
			vertex(x*w, h);
			x += dx;
		}
		i++;
	}
	end();
	
	for(int k = 0; k < mNKnots; k++) {
		int cx = mKnots[k].x*w;
		int cy = (1.-mKnots[k].y)*h;
		frame(cx-mKnobSize, cy-mKnobSize, cx+mKnobSize, cy+mKnobSize);
	}
}

bool FunctionGraph::onEvent(Event::t e, GLV& glv)
{
	switch(e){
	case Event::MouseDrag: {
		if(glv.mouse.left() && mCurrentKnot >= 0) {
			if(mCurrentKnot == 0 || mCurrentKnot == (mNKnots-1)) {
				mKnots[mCurrentKnot].y = 1.-(glv.mouse.yRel()/h);
				mKnots[mCurrentKnot].y = (mKnots[mCurrentKnot].y < 0.) ? 0. : 
												((mKnots[mCurrentKnot].y > 1.) ? 1. : mKnots[mCurrentKnot].y);
			}
			else {
				mKnots[mCurrentKnot].x = (glv.mouse.xRel()/w);
				mKnots[mCurrentKnot].y = 1.-(glv.mouse.yRel()/h);
				
				mKnots[mCurrentKnot].x = (mKnots[mCurrentKnot].x < 0) ? 0 : 
											((mKnots[mCurrentKnot].x > 1.) ? 1 : mKnots[mCurrentKnot].x);
				
				mKnots[mCurrentKnot].y = (mKnots[mCurrentKnot].y < 0.) ? 0. : 
												((mKnots[mCurrentKnot].y > 1.) ? 1. : mKnots[mCurrentKnot].y);
				
				//check if we went beyond neighboring knots
				if(mKnots[mCurrentKnot].x < mKnots[mCurrentKnot-1].x && mCurrentKnot != mCurrentKnot+1) {
					Knot kt;
					kt.x = mKnots[mCurrentKnot].x;
					kt.y = mKnots[mCurrentKnot].y;
					
					mKnots[mCurrentKnot].x = mKnots[mCurrentKnot-1].x;
					mKnots[mCurrentKnot].y = mKnots[mCurrentKnot-1].y;
					
					mKnots[mCurrentKnot-1].x = kt.x;
					mKnots[mCurrentKnot-1].y = kt.y;
					
					mCurrentKnot--;
				}
				else if(mKnots[mCurrentKnot].x > mKnots[mCurrentKnot+1].x && mCurrentKnot != mNKnots-2) {
					Knot kt;
					kt.x = mKnots[mCurrentKnot].x;
					kt.y = mKnots[mCurrentKnot].y;
					
					mKnots[mCurrentKnot].x = mKnots[mCurrentKnot+1].x;
					mKnots[mCurrentKnot].y = mKnots[mCurrentKnot+1].y;
					
					mKnots[mCurrentKnot+1].x = kt.x;
					mKnots[mCurrentKnot+1].y = kt.y;
					
					mCurrentKnot++;
				}
			}
			
			calcCurves();
		}
	}
	break;
	
	case Event::MouseDown: {
		if(glv.mouse.left()) {
			mCurrentKnot = knotHitTest(glv.mouse.xRel(), glv.mouse.yRel());
		}
	}
	break;
	
	default:
	break;
	}
	
	return false;
}

int FunctionGraph::knotHitTest(space_t x, space_t y)
{
	int idx = -1;
	float min_dsq = mKnobSize*mKnobSize*2.5;
	
	for(int k = 0; k < mNKnots; k++) {
		float dx = mKnots[k].x*w - x;
		float dy =(1.-mKnots[k].y)*h - y;
		float dsq = dx*dx+dy*dy;
		if(dsq < min_dsq) {
			min_dsq = dsq;
			idx = k;
		}
	}
	
	return idx;
}

} // end namespace glv

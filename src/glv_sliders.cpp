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
			updateValue( g.mouse.dx()/w * sens(g), 0, &Slider2D::valueAdd);
			updateValue(-g.mouse.dy()/h * sens(g), 1, &Slider2D::valueAdd);
			break;
			
		case Event::MouseDown:
			if(g.mouse.left() && !g.mouse.right()){
				updateValue(      g.mouse.xRel() / w, 0, &Slider2D::value);
				updateValue(1.f - g.mouse.yRel() / h, 1, &Slider2D::value);
			}
			break;
			
		case Event::MouseUp: clipAccs(); break;
			
		case Event::KeyDown:
			switch(g.keyboard.key()){
				case 'x': updateValue(-1./w, 0, &Slider2D::valueAdd); break;
				case 'c': updateValue( 1./w, 0, &Slider2D::valueAdd); break;
				case 'a': updateValue( 1./h, 1, &Slider2D::valueAdd); break;
				case 'z': updateValue(-1./h, 1, &Slider2D::valueAdd); break;
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



SliderRange::SliderRange(const Rect& r, float val1, float val2)
:	SliderBase<2>(r), mJump(0.1), mDragMode(0)
{
	extrema(val1, val2);
}

float SliderRange::center() const { return (value(0) + value(1))*0.5; }
float SliderRange::jump() const { return mJump; }
float SliderRange::range() const { return value(1)-value(0); }

SliderRange& SliderRange::center(float v){ return centerRange(v, range()); }

SliderRange& SliderRange::centerRange(float c, float r){
	float mn = c-(r/2.);
	float mx = mn+r;
	if(mn<0){ mx -= mn  ; mn=0; }
	if(mx>1){ mn -= mx-1; mx=1; }
	return extrema(mn,mx);
}

SliderRange& SliderRange::extrema(float min, float max){
	if(min>max){ float t=min; min=max; max=t; }
	value(min,0);
	value(max,1);
	return *this;
}

SliderRange& SliderRange::jump(float v){ mJump=v; return *this; }
SliderRange& SliderRange::range(float v){ return centerRange(center(), v); }

void SliderRange::onDraw(){
	using namespace glv::draw;

	float v1 = value(0);
	float v2 = value(1);
	if(v2<v1){ float t=v1; v1=v2; v2=t; }
	
	// prevent degenerecy
	float p1 = 1./((w>h)?w:h); // 1 pixel
	if(v2-v1 <= p1){ v1-=p1*0.5; v2+=p1*0.5; }

	color(colors().fore);
	if(w>h){	// horizontal
		rect(v1*w,0, v2*w,h);
	}
	else{
		rect(0,v1*h, w,v2*h);
	}
}


bool SliderRange::onEvent(Event::t e, GLV& g){

	float dv = (w>h) ? g.mouse.dx()/w : g.mouse.dy()/h;
	float mp = (w>h) ? g.mouse.xRel()/w : g.mouse.yRel()/h;
	float d1 = mp-value(0); if(d1<0) d1=-d1;
	float d2 = mp-value(1); if(d2<0) d2=-d2;
	int ind = d1<d2 ? 0:1;
	float rg = range();
	float endRegion = 4 / (w>h ? w:h);

	switch(e){
	case Event::MouseDown:
		//if(g.mouse.right()) value(pt,ind);
		
		// NOTE: There is more than one way we might want to move the slider 
		// on a click. We can set its center or increment its center in the
		// direction of the click. Also, we may not want to move the slider
		// if the click lands within the range...
		if(g.mouse.left()){
			float v1 = value(0);
			float v2 = value(1);
			if(mp<(v1-endRegion) || mp>(v2+endRegion)){
				float dc = mp - center();
				float dcAbs = dc<0 ? -dc : dc;
				if(jump() > dcAbs){
					center(mp);
				}
				else{
					center(center() + (dc<0 ? -jump() : jump()));
				}
				updateValue(value(0), 0, &SliderRange::value);
				updateValue(value(1), 1, &SliderRange::value);
				mDragMode=0;
			}
			else if(mp > (v1-endRegion) && mp < (v1+endRegion)){
				mDragMode=1;
			}
			else if(mp > (v2-endRegion) && mp < (v2+endRegion)){
				mDragMode=2;
			}
			else{
				mDragMode=3;
			}
		}
		
		return false;
	
	case Event::MouseDrag:
		dv *= sens(g);
		if(mDragMode == 3){
			updateValue(dv, 0, 0,  1-rg, &SliderRange::valueAdd);
			updateValue(dv, 1, rg, 1,    &SliderRange::valueAdd);
		}
		else if(mDragMode == 1) updateValue(dv, 0, 0, 1, &SliderRange::valueAdd);
		else if(mDragMode == 2) updateValue(dv, 1, 0, 1, &SliderRange::valueAdd);

		return false;

	case Event::MouseUp:
		if(mDragMode == 3){
			clip(mAcc[0], 0, 1-rg);
			clip(mAcc[1], rg, 1);
		}
		else if(mDragMode == 1) clip(mAcc[0], 0, 1);
		else if(mDragMode == 2) clip(mAcc[1], 0, 1);
		return false;

	default:;
	}

	return true;
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

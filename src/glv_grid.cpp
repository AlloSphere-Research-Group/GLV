/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_grid.h"

namespace glv{

Grid::Grid(const Rect& r, double rangeMin, double rangeMax, double majorDist, int minorDiv)
:	View(r), mShowAxes(true), mShowGrid(true), mShowNumbering(false), mEqualize(false)
{
	range(rangeMin, rangeMax);
	major(majorDist); minor(minorDiv);
	numbering(true);
	for(int i=0; i<DIM; ++i){
		mVel[i]=0;
		lockScroll(false, i);
		lockZoom(false, i);
	}
	mVelW=0;
}

// add grid lines to vertex buffer, returns number of lines added
int Grid::addGridLines(int i, double dist, GraphicsData& gb){

	float  p = gridToPix(i, floor(interval(i).min(), dist));				
	float dp = gridToPix(i, dist) - gridToPix(i, 0);
	if(fabs(dp) < 2) return 0;

	double nr = interval(i).diameter()/dist;
	int n = nr;
	int j = (i+1)%DIM;
	n += 2; // safety measure for edge cases

	for(int idx=0; idx<n; ++idx){
		float v[2][2] = {
			{p+idx*dp, 0},
			{p+idx*dp, extentVector[j]}
		};
		gb.addVertex2(v[0][i], v[0][j], v[1][i], v[1][j]);
	}
	return n;
}

void Grid::onAnimate(double dt, GLV& g){
//	for(int i=0; i<DIM; ++i){
//		if(mVel[i] != 0) interval(i).translate(mVel[i]);
//	}
//	if(mVelW != 0) zoomOnMousePos(mVelW, g.mouse);
}

void Grid::onDraw(GLV& g){

	for(int i=0; i<DIM; ++i){
		if(!lockScroll(i) && mVel[i] != 0) interval(i).translate(mVel[i]);
	}
	if(mVelW != 0) zoomOnMousePos(mVelW, g.mouse());

	using namespace glv::draw;
	GraphicsData& gb = g.graphicsData();

	lineWidth(1);

	// draw minor lines
	if(showGrid()){
		gb.reset();
		color(colors().border.mix(colors().back, 14./16));
		
		for(int i=0; i<DIM; ++i){
			if(mMinor[i] < 2) continue;
			addGridLines(i, mMajor[i]/mMinor[i], gb);
		}
		paint(Lines, &gb.vertices2()[0], gb.vertices2().size());
	}

	// draw major lines / numbering
	if(showGrid() || showNumbering()){
		gb.reset();
		
		for(int i=0; i<DIM; ++i){
			int b = gb.vertices2().size();
			int n = addGridLines(i, mMajor[i], gb);

			if(showNumbering() && mNumbering[i]){
			
				color(colors().border);
			
				for(int j=b; j<b+n*2; j+=2){
					double p = gb.vertices2()[j].elems[i];
					double v[] = { 
						i ?   4 : p+4,
						i ? p+4 : h-(4+font().cap())
					};
					double gp = pixToGrid(i, p);
					if(fabs(gp) < 1e-5) gp=0;
					char buf[16];
					snprintf(buf, sizeof(buf), "%.3g", gp);
					font().render(buf, v[0], v[1]);
				}
			}
		}
		if(showGrid()){
			color(colors().border.mix(colors().back, 10./16));
			paint(Lines, &gb.vertices2()[0], gb.vertices2().size());
		}
	}

	// draw axes
	if(showAxes()){
		color(colors().border.mix(colors().back, 0./4));
		if(interval(0).contains(0)){
			float p = gridToPix(0, 0);
			shape(Lines, p, 0, p, h);
		}
		if(interval(1).contains(0)){
			float p = gridToPix(1, 0);
			shape(Lines, 0, p, w, p);
		}
	}

//	if(mEqualize){ // NOTE: this always works when called from draw loop
//		w>=h	? interval(0).diameter(interval(1).diameter()*w/h)
//				: interval(1).diameter(interval(0).diameter()*h/w);
//	}
}

bool Grid::onEvent(Event::t e, GLV& g){
//	printf("[% 6.3f, % 6.3f], [% 6.3f, % 6.3f]\n",
//		interval(0).min(), interval(0).max(), interval(1).min(), interval(1).max());

	const Mouse& m = g.mouse();
	const Keyboard& k = g.keyboard();
	switch(e){			
		case Event::MouseDown:
			return false;

		case Event::MouseDrag:
			if(m.left()){
				if(!lockScroll(0)) interval(0).translate(-pixToGridMul(0, m.dx()));
				if(!lockScroll(1)) interval(1).translate( pixToGridMul(1, m.dy()));
			}
			if(m.right()){
				zoomOnMousePos(m.dy()*0.01, m);			
			}
			return false;

		case Event::KeyDown:
			//printf("%c %d\n", k.key(), k.key());
			//if(k.shift()){
				switch(k.key()){
					case 'a': mVel[0] =-pixToGridMul(0,8); break;
					case 'd': mVel[0] = pixToGridMul(0,8); break;
					case 'x': mVel[1] =-pixToGridMul(0,8); break;
					case 'w': mVel[1] = pixToGridMul(0,8); break;
					case 'e': mVelW =-0.04; break;
					case 'c': mVelW = 0.04; break;
					case 's': origin(); break;
					case 'g': mShowGrid ^= 1; break;
					case 'b': mShowAxes ^= 1; break;
					case 'n': mShowNumbering ^= 1; break;
//					case 'p': mPolarGrid ^= 1; break;
					default: return true;
				}
				return false;
//			}
//			else{
//				return true;
//			}
		
		case Event::MouseUp:	return false;
		case Event::KeyUp:
			switch(k.key()){
				case 'a':
				case 'd': mVel[0] = 0; break;
				case 'x':
				case 'w': mVel[1] = 0; break;
				case 'e':
				case 'c': mVelW = 0; break;
				default: return true;
			}
			return false;

		default: return true;
	}
}

void Grid::onResize(space_t dx, space_t dy){
	if(mEqualize){
		// always adjust x interval, unless there is a better way...
		interval(0).diameter(interval(1).diameter()*(h>0 ? w/h : 1));
//		w>=h	? interval(0).diameter(interval(1).diameter()*w/h)
//				: interval(1).diameter(interval(0).diameter()*h/w);
//		float pw = w-dx;
//		float ph = h-dy;
	}
}

Grid& Grid::zoom(double amt, double x, double y){
	// change diameter around arbitrary point in interval
	// 1. translate center of interval to inflection point
	// 2. scale diameter
	// 3. translate center of interval back to scaled original					
	float scale = pow(2, amt);
	float gs[] = { x, y };
	
	for(int i=0; i<DIM; ++i){
		if(lockZoom(i)) continue;

		interval_t iv = interval(i);
		float t = gs[i] - iv.center();
		iv.translate(t);
		iv.diameter(iv.diameter()*scale);
		iv.translate(-t*scale);
		interval(i) = iv;				
	}
	return *this;
}

void Grid::zoomOnMousePos(double amt, const Mouse& m){
	float px = m.xRel(m.button());	// pixel x coord
	float py = m.yRel(m.button());	// pixel y coord					
	zoom(amt, pixToGrid(0, px), pixToGrid(1, py));
}

} // glv::

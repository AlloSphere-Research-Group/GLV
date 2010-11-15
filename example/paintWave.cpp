/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 

Wave Equation

d^2f/dt^2 = v^2 del^2(f)

(f(r, t+1) - f(r,t)) - (f(r,t) - f(r,t-1))
	= v^2 [(f(r+1, t) - f(r,t)) - (f(r,t) - f(r-1,t))]

f(r, t+1)	= 2f(r,t) - f(r,t-1) + v^2 [f(r+1, t) - 2f(r,t) + f(r-1,t)]
*/

#include "example.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r){
		P.resize(Data::FLOAT, 64*1,64*1,3);
		it0=0;
	}

	void onAnimate(double dt, GLV& g){

		// set plot data to current z plane
		data() = P.slice(it0*P.size(0,1)).shape(1, P.size(0), P.size(1));
		
		Indexer i(P.size(0), P.size(1));

		while(i()){

			int x1 = i[0];			
			int x0 = x1-1; if(x0<0) x0 = i.size(0)-1;
			int x2 = x1+1; if(x2==i.size(0)) x2 = 0;

			int y1 = i[1];
			int y0 = y1-1; if(y0<0) y0 = i.size(1)-1;
			int y2 = y1+1; if(y2==i.size(1)) y2 = 0;
			
			// get neighboring element values
			float u11 = P.elem<float>(x1,y1,it(-1));
			float v11 = P.elem<float>(x1,y1,it0);	// center
			float v01 = P.elem<float>(x0,y1,it0);	// left
			float v21 = P.elem<float>(x2,y1,it0);	// right
			float v10 = P.elem<float>(x1,y0,it0);	// down
			float v12 = P.elem<float>(x1,y2,it0);	// up

			float damp = 0.99;
			float vel = sqrt(2);

			// compute wave equation
			float val = (v11*2 - u11 + ((v01 + v21 + v10 + v12)/4 - v11)*vel*vel)*damp;

			P.elem<float>(x1,y1,it(1)) = val;
		}

		// "swap" buffers
		it0 = it(1);
	}

	bool onEvent(Event::t e, GLV& g){

		float x = g.mouse.xRel();
		float y = height()-g.mouse.yRel();
		int cx = x/width() * P.size(0);
		int cy = y/height()* P.size(1);

		switch(e){
		case Event::MouseDown:
		case Event::MouseDrag:
			if(P.inBounds(cx, cy)){
				P.elem<float>(cx,cy,it0) = g.mouse.button() ? -1:1;
			}
			break;
		case Event::KeyDown:
			switch(g.keyboard.key()){
			case 'c': P.assignAll(0); break;
			}
			break;
		default:;
		}
		
		return true;
	}

	Data P;
	int it0;
	
	int it(int offset){
		int r=it0+offset;
		return r<0 ? r+3 : (r>2) ? r-3 : r;
	}
};


int main(){
	GLV top;
	Canvas v1(Rect(0));
	v1.add(*new PlotDensity(Color(1)));
	v1.stretch(1,1);
	top << v1;

	Window win(600,600, "Wave Paint", &top);
	Application::run();
	return 0;
}


/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "example.h"

namespace glv{

struct DRScatter : public DataRenderer{

	void onDrawElements(draw::GraphicBuffers& b, const Data& d, const Indexer& i){
		prim(draw::Points);
		draw::pointSize(2);
		while(i()){
			double x = d.at<double>(0,i[0],i[1]);
			double y = d.at<double>(1,i[0],i[1]);
//			double x = d.at<double>(0,i[0],i[1],i[2]);
//			double y = d.at<double>(1,i[0],i[1],i[2]);

			b.addVertex2(x, y);
		}
	}
};

struct DRVector : public DataRenderer{

	void onDrawElements(draw::GraphicBuffers& b, const Data& d, const Indexer& i){
		//prim(draw::Lines);
		prim(draw::Triangles);
		draw::lineWidth(1);
		double dx = 2./d.size(1);
		double dy = 2./d.size(2);
		while(i()){
			double x = i.frac(0)*2 - 1 + dx/2;
			double y = i.frac(1)*2 - 1 + dy/2;
			double vx=d.at<double>(0,i[0],i[1],i[2])*dx*2;
			double vy=d.at<double>(1,i[0],i[1],i[2])*dy*2;

			double r = 0.2/hypot(vx, vy);
			double Nx = vy*r*dx;
			double Ny =-vx*r*dy;

//			b.addVertex2(
//				x, y,
//				x + d.at<double>(0,i[0],i[1],i[2])*dx,
//				y + d.at<double>(1,i[0],i[1],i[2])*dy
//			);
			b.addVertex2(
				x+Nx, y+Ny,
				x-Nx, y-Ny,
				x + vx,
				y + vy
			);
			b.addColor(
				Color(0,0),
				Color(0,0),
				Color(0)
			);
		}
	}
};

struct DRDensity : public DataRenderer{

	void onDrawElements(draw::GraphicBuffers& b, const Data& d, const Indexer& i){
		prim(draw::Triangles);
		double dx = 2./d.size(1);
		double dy = 2./d.size(2);
		while(i()){
			double x = i.frac(0)*2 - 1;
			double y = i.frac(1)*2 - 1;
			double w0 = d.at<double>(0,i[0],i[1],i[2]);
			double w1 = d.at<double>(1,i[0],i[1],i[2]);

			//Color c = HSV(0.1, fabs(w1), fabs(w0));
			//Color c = HSV(0.1, atan2(w1,w0)/(2*M_PI)+0.5, hypot(w0,w1));
			
			double m = hypot(w0,w1);
			double a = atan2(w1,w0)/(-2*M_PI); if(a<0) a=1+a;
			Color c = HSV(a, 1, m);
			int idx = b.vertices2().size();

			b.addVertex2(x,y, x+dx,y, x+dx,y+dy, x,y+dy);
			b.addColor(c,c,c,c);
			b.addIndex(idx+0, idx+1, idx+3);
			b.addIndex(idx+1, idx+2, idx+3);
		}
	}
};

struct MyGLV : GLV {

	MyGLV(): phase(0){
		model().resize(Data::DOUBLE, 2,16,16);
	}

	void onAnimate(double dt, GLV& g){
		phase += 0.013;
		Data& d = model();
		
		Indexer i(d.size(1), d.size(2));

		while(i()){
			float x = i.frac(0);
			float y = i.frac(1);
			x = (cos(x*7 + phase) + cos(x*-2 + phase*1.01))/2;
			y = (sin(y*5 + phase) + sin(y*-3 + phase*1.01))/2;
			d.assign(x, 0,i[0],i[1]);
			d.assign(y, 1,i[0],i[1]);
		}
	}

	double phase;
};

}

int main(){

	// Create the Views
	MyGLV top;
	DataPlot<DRDensity> v1(Rect(000,0, 400,400));
	DataPlot<DRVector>  v2(Rect(400,0, 400,400));
	DataPlot<DRScatter> v3(Rect(800,0, 400,400));

//	v1.disable(CropSelf | DrawBack);
//	v2.disable(CropSelf | DrawBack);
//	v3.disable(CropSelf | DrawBack);
	v1.model() = top.model();
	v2.model() = top.model();
	v3.model() = top.model();
	
	top << v1 << v2 << v3;

	Window win(0,0, "DataPlots", &top);
	win.fit();
	Application::run();
}


/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: Chladni Pattern
*/

#include "example.h"
using namespace glv;

struct Canvas : public Plot{
	Canvas(const Rect& r): Plot(r), plotDensity(HSV(0.3,0.5,1), 1./16){
		int N = 256;
		data().resize(Data::FLOAT, 1, N,N);
		//plotDensity.interpolate(true).drawUnderGrid(true);
		add(plotDensity);
		equalizeAxes(true);
		showAxis(false);
		showGrid(false);
		disable(DrawBorder);
	}

	void onAnimate(double dt){
		
		plotDensity.plotRegion(interval(0), interval(1));
		
		Indexer i(data().size(1), data().size(2));

		while(i()){
			int ix = i[0];
			int iy = i[1];
			
			double px = interval(0).fromUnit(i.fracClosed(0)) * M_PI;
			double py = interval(1).fromUnit(i.fracClosed(1)) * M_PI;
			
			// plate vibration frequencies
			double f1 = 7;
			double f2 = 17;
			
			double val = (cos(f1*px) * cos(f2*py) - cos(f2*px) * cos(f1*py))/2;
			
			data().elem<float>(0, ix, iy) = val;
		}

	}

	PlotDensity plotDensity;
};


int main(){
	GLV top;
	Canvas v1(Rect(0));
	Window win(600,600, "Chladni Pattern", &top);

	top.colors().set(StyleColor::WhiteOnBlack);
	v1.stretch(1,1);
	top << v1;

	top.addHandler(Event::KeyDown, *new FullScreenToggler(win));

	Application::run();
	return 0;
}


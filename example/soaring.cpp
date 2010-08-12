/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "example.h"

const int N = 128;

struct Scene : View3D{

	Scene()
	:	pos(0,0,0)
	{
		stretch(1,1); disable(DrawBorder);
		
		// Set x,z positions of terrain
		for(int j=0; j<N; ++j){			
			for(int i=0; i<N; ++i){
				float z = (float(j)/N)*-10;
				float x = (float(i)/N*2-1)*4;
				int ind = j*N+i;
				terV[ind].x = x;
				terV[ind].z = z;

				if(j<(N-1)){
					terI.push_back(ind);
					terI.push_back(ind+N);
					if(i==(N-1) && j!=(N-2)){
						terI.push_back(ind+N);
						terI.push_back(ind+1);
					}
				}
			}
		}
	}
	
	float getValue(float x, float z){
		float r = hypot(x, z)*10;
		r = (cos(r) + cos(r*0.51) + cos(r*5.03)/5)/(1+1+1./5);
		return r;
	}

	virtual void onDraw3D(){
		using namespace glv::draw;

		for(int j=0; j<N; ++j){
			for(int i=0; i<N; ++i){
				int ind = j*N+i;
				float x = (float(i)/N)*2 + pos.x;
				float z = (float(j)/N)*2 + pos.z;

				float v1 = getValue(x-1000, z-1000);
				float v2 = getValue(x+1000, z-1000);
				float v3 = getValue(x, z+800);
				float v4 = getValue(x, z);
				float v = v1 + v2 + v3 + v4;
				v *= v*v*0.1;
				
				terV[ind].y = v - 0.5;
				terC[ind] = v<0 ? HSV(0.6,1,-v*0.5) : HSV(0.1, 0.7*(1-v*v), v*0.5);				
			}
		}

		pos.x += incx;
		pos.z += incz;

		draw::enable(Fog);
		fog(10, 4, colors().back);
		color(1);
		stroke(2);
		paint(TriangleStrip, terV, terC, &terI[0], terI.size());
	}

	float incx, incz;
	Point3 pos;
	Point3 terV[N*N];
	Color terC[N*N];
	std::vector<unsigned> terI;
};

Scene scene;
Slider2D sliderXZ(Rect(100));

int main (int argc, char ** argv){
	GLV top;
	top.colors().set(Color(HSV(0.6,0.5,0.5), 0.9), 0.4);
	
	sliderXZ.interval(0.02, -0.02);
	sliderXZ.attachVariable(scene.incx, 0);
	sliderXZ.attachVariable(scene.incz, 1);
	sliderXZ.value(0.0005, 0);
	sliderXZ.value(0.0057, 1);
	
	top << scene << sliderXZ;

	Window win(800, 600, "Soaring", &top);
	Application::run();
}


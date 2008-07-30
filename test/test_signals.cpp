#include <math.h>
#include "test_glv.h"

using namespace glv;

float rx = 0, ry = 0, rz = 0;
float tz = 0, sx = 1, sy = 1;
Color c(0, 0.3, 0, 1);

bool s1r(View * v, GLV & glv){
	Slider * s = (Slider *)v;
	rz = s->value() * 360.f;
	return false;
}

bool s1t(View * v, GLV & glv){
	Slider * s = (Slider *)v;
	tz = -pow(2.f, s->value() * 4.f) + 1.f;
	return false;
}

bool s2color(View * v, GLV & glv){
	Slider2D * s = (Slider2D *)v;
	c.setHSV(s->value(0), 1, s->value(1));
	return false;
}

bool s2r(View * v, GLV & glv){
	Slider2D * s = (Slider2D *)v;
	ry = s->value(0) * 360.f;
	rx = s->value(1) * 360.f;
	return false;
}

bool bdo(View * v, GLV & glv){
//	glv::Button * b = (glv::Button *)v;
	if(glv.keyboard.shift())	sx -= 0.1;
	else						sx += 0.1;
	glv.keyboard.print();	
	return false;
}

bool keydown(View * v, GLV & glv){
	glv.keyboard.print();
	switch(glv.keyboard.key()){
		default:;
	}
	return false;
}

void drawCB(View * v){
	using namespace draw;
	push3D(v->w, v->h);

	color(c);
	translate(0, 0, tz);
	rotate(rx, ry, rz);
	scale(sx, sy, 1);
	
	rect(-1, -1, 2, 2);

	pop3D();

	//push2D(v->w, v->h);
}

int main (int argc, char ** argv){

	GLV glv(drawCB);
	glv.colors().back.set(0);

	Window win(320, 240);
	win.setGLV(glv);

	Slider s11(Rect(10, 10, 20, 100));
	Slider s12(Rect(35, 10, 20, 100));
	Slider2D s21(Rect(60, 10, 100, 100));
	Button b1(Rect(10, 115, 20, 20), false);
	Button b2(Rect(35, 115, 20, 20), true);
	
	// append callbacks
	s11(Event::MouseDrag, s1r)(Event::MouseDown, s1r);
	s12(Event::MouseDrag, s1t)(Event::MouseDown, s1t);
	s21(Event::MouseDrag, s2color)(Event::MouseDown, s2color);
	b1(Event::MouseDown, bdo);
	glv(Event::KeyDown, keydown);
	
	// add widgets to top view
	glv << s11 << s12 << s21 << b1 << b2;
	
	Application::run();		// Application is also an AL binding, but unlike Window, it is a singleton
}


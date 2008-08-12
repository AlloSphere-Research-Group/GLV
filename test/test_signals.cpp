#include "test_glv.h"

using namespace glv;

void updateSlider12(const Notification& n){
	Slider2D& s2 = *(Slider2D *)n.receiver();
	Slider& s = *(Slider *)n.sender();
	s2.value(s.value(), 0);
}

void updateSlider21(const Notification& n){
	Slider2D& s2 = *(Slider2D *)n.sender();
	Slider& s = *(Slider *)n.receiver();
	s.value(s2.value(0));
}

void updateValue(const Notification& n){
	Slider& s = *(Slider *)n.sender();
	Label& l = *(Label *)n.receiver();
	char buf[16]; snprintf(buf, sizeof(buf), "%4.2f", s.value());
	l.label(buf);
}

void updateFocus(const Notification& n){
	Slider& s = *(Slider *)n.sender();
	Label& l = *(Label *)n.receiver();
	l.label(s.enabled(Focused) ? "get focus" : "lose focus");
}



int main(int argc, char ** argv){

	GLV glv;
	glv.colors().back.set(0);

	Window win(320, 240);
	win.setGLV(glv);

	Slider slider(Rect(10, 10, 100, 20));
	Slider2D slider2(Rect(10, 40, 100, 100));
	Label status("", Place::BL, 10, -10);
	
	slider.attach(updateValue, Update::Value, &status);
	slider.attach(updateFocus, Update::Focus, &status);
	slider.attach(updateSlider12, Update::Value, &slider2);
	slider2.attach(updateSlider21, Update::Value, &slider);

	glv << slider << slider2 << status;
	
	Application::run();		// Application is also an AL binding, but unlike Window, it is a singleton
}


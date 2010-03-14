/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

void updateSlider12(const Notification& n){
	Slider2D& s2 = *(Slider2D *)n.receiver();
	Slider& s = *(Slider *)n.sender();
	SliderChange v = *(SliderChange *)n.data();
	s2.value(s.value(), 0);
}

void updateSlider21(const Notification& n){
	Slider2D& s2 = *(Slider2D *)n.sender();
	Slider& s = *(Slider *)n.receiver();
	s.value(s2.value(0));
}

void updateFocus(const Notification& n){
	Slider& s = *(Slider *)n.sender();
	Label& l = *(Label *)n.receiver();
	l.label(s.enabled(Focused) ? "get focus" : "lose focus");
}

void buttonsUpdateLabel(const Notification& n){
	Label& l = *(Label *)n.receiver();
	ButtonChange v = *(ButtonChange *)n.data();
	char buf[16]; snprintf(buf, sizeof(buf), "[%2d] = %d", v.index(), v.value());
	l.label(buf);
}

void sliderUpdateLabel(const Notification& n){
	Label& l = *(Label *)n.receiver();
	SliderChange v = *(SliderChange *)n.data();
	char buf[16]; snprintf(buf, sizeof(buf), "[%2d] = %4.2f", v.index(), v.value());
	l.label(buf);
}

void dialerUpdateLabel(const Notification& n){
	Label& l = *(Label *)n.receiver();
	NumberDialerChange v = *(NumberDialerChange *)n.data();
	char buf[16]; snprintf(buf, sizeof(buf), "[%2d] = %4.2f", v.index(), v.value());
	l.label(buf);
}


int main(int argc, char ** argv){

	GLV glv;
	glv.colors().back.set(0);

	Window win(320, 400);
	win.setGLV(glv);

	// Make some widgets
	Buttons buttons(Rect(64), 4,4);
	Slider slider(Rect(10, 10, 100, 20), 0, false);
	Slider2D slider2(Rect(10, 40, 100, 100));
	SliderGrid<4> sliderGrid(Rect(100));
	//NumberDialer numberDialer(Rect(12*4, 12), 1,2);
	NumberDialer numberDialer(12,0,0, 1,2, 10,-10);
	numberDialer.padding(2);

	// Create labels for update notifications
	Label::Spec spec(Place::CR, 10, 0);
	Label lbButtons("", spec), lbSlider("", spec), 
		lbSlider2("", spec), lbSliderGrid("", spec),
		lbNumberDialer("", spec);

	// Attach some notifications
	buttons.attach(buttonsUpdateLabel, Update::Value, &lbButtons);
	slider.attach(sliderUpdateLabel, Update::Value, &lbSlider);
	slider2.attach(sliderUpdateLabel, Update::Value, &lbSlider2);
	sliderGrid.attach(sliderUpdateLabel, Update::Value, &lbSliderGrid);
	numberDialer.attach(dialerUpdateLabel, Update::Value, &lbNumberDialer);
	
	slider.attach(updateFocus, Update::Focus, &lbSlider);
	slider.attach(updateSlider12, Update::Value, &slider2);
	slider2.attach(updateSlider21, Update::Value, &slider);

	Placer placer(glv, Direction::S, Place::TL, 10, 10);

	placer	<< (buttons << lbButtons)
			<< (slider << lbSlider)
			<< (slider2 << lbSlider2)
			<< (sliderGrid << lbSliderGrid)
			<< (numberDialer << lbNumberDialer);
	
	Application::run();
}


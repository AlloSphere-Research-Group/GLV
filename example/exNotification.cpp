/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Using a Notification to set a Label

A Notification is used to set a label to a slider's value.

*/

#include "example.h"

void ntSetLabel(const Notification& n){
	Label& l = *(Label *)n.receiver();
	Slider& s = *(Slider *)n.sender();
	l.value(s.valueString());
}

int main(int argc, char ** argv){

	GLV glv;
	Slider slider(Rect(100,20));
	Label label;
	
	slider.attach(ntSetLabel, Update::Value, &label);

	Placer p(glv, Direction::E, Place::TL, 10, 10);
	
	p << slider << label;

	Window win(200, 40, "", &glv);
	Application::run();
}


/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */
/*
Example: 
*/

#include "example.h"

int main(){

	GLV top;
	Window win(800,800, "Grid", &top);
	
	top.colors().set(StyleColor::WhiteOnBlack);
	//top.colors().set(StyleColor::SmokeyGray);
	
	Grid v1(Rect(0,0));
	
	v1.range(1);			// set plot region
	v1.major(1);			// set major tick mark placement
	v1.minor(2);			// number of divisions per major ticks
	v1.equalizeAxes(true);
	
	v1.stretch(1,1);
	//v1.disable(CropSelf);
	top << v1;

	top.addHandler(Event::KeyDown, *new FullScreenToggler(win));

	Application::run();
}


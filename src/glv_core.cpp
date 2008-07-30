/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

namespace Event{

	const char * string(const Event::t e){
		#define CS(t) case Event::t: return #t;
		switch(e){
			CS(Null)
			CS(Quit)
			CS(GetFocus) CS(LoseFocus)
			CS(MouseDown) CS(MouseUp) CS(MouseMove) CS(MouseDrag) CS(MouseWheel)
			CS(KeyDown) CS(KeyUp) CS(KeyRepeat)
			CS(Unused)
			default: return "?";
		};
		#undef CS
	}
}



StyleColor::StyleColor()
:	back(0.5, 0.5), border(0.5, 1), fore(1, 1), text(1, 1)
{}



void StyleColor::set(int preset){
	switch(preset){
		case BlackOnWhite:	back.set(1  ); border.set(0); fore.set(0.5); text.set(0); break;
		case Gray:			back.set(0.2); border.set(1); fore.set(1); text.set(0); break;
		case WhiteOnBlack:	back.set(0  ); border.set(1); fore.set(0.5); text.set(1); break;
	}
}


void StyleColor::hsv(float h, float s, float v){
	Color c; c.setHSV(h,s,v);
	c.a = 0.2; back.set(c);
	c.a = 0.5; border.set(c);
	c.a = 1.0; fore.set(c);
}


} // glv::

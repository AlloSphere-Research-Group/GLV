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
			CS(WindowCreate) CS(WindowDestroy) CS(WindowResize)
			CS(FocusGained) CS(FocusLost)
			CS(MouseDown) CS(MouseUp) CS(MouseMove) CS(MouseDrag) CS(MouseWheel)
			CS(KeyDown) CS(KeyUp) CS(KeyRepeat)
			CS(Unused)
			default: return "?";
		};
		#undef CS
	}
}



StyleColor::StyleColor(){
	set(BlackOnWhite);
}



void StyleColor::set(Preset preset){
	switch(preset){
		case BlackOnWhite:	back.set(1.0); border.set(0); fore.set(0.5); 
							selection.set(0.7); text.set(0);
							break;
		case Gray:			back.set(0.6); border.set(0.1); fore.set(0.8);
							selection.set(0.8); text.set(0);
							break;
		case WhiteOnBlack:	back.set(0.0); border.set(1); fore.set(0.5);
							selection.set(0.2); text.set(1);
							break;
	}
}


void StyleColor::hsv(float h, float s, float v){
	Color c = HSV(h,s,v);

	fore.set(c);

	float vt;
	vt = v + (v < 0.5 ? 0.4 : -0.4);
	back.set(HSV(h,s,vt));

	vt = v + (v < 0.5 ? 0.2 : -0.2);
	selection.set(HSV(h,s,vt));
	
	text.set(c.inverse().blackAndWhite());
	border.set(fore);
}


} // glv::

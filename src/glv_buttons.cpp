/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_buttons.h"

namespace glv {

Buttons::Buttons(
	const Rect& r, int nx, int ny, bool toggles, bool mutExc, SymbolFunc on, SymbolFunc off
)
:	Widget(r, 2, toggles, mutExc, true), mSymOff(off), mSymOn(on)
{	
	data().resize(Data::BOOL, nx,ny);
	useInterval(false);
}

void Buttons::onDraw(GLV& g){
	Widget::onDraw(g);

	using namespace glv::draw;

	float xd = dx();
	float yd = dy();
	float padx = paddingX();
	float pady = paddingY();
	color(colors().fore);
	
	// TODO: small buttons hard to see when not antialiased
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	//draw::enable(PolygonSmooth);
	
	draw::stroke(1);

	for(int i=0; i<sizeX(); ++i){
		float x = xd*i + padx;

		for(int j=0; j<sizeY(); ++j){
			float y = yd*j + pady;
			if(getValue(i,j)){	if(mSymOn ) mSymOn (x, y, x+xd-padx*2, y+yd-pady*2); }
			else{				if(mSymOff) mSymOff(x, y, x+xd-padx*2, y+yd-pady*2); }
		}		
	}
	
	//draw::disable(PolygonSmooth);
}

bool Buttons::onEvent(Event::t e, GLV& g){
	if(!Widget::onEvent(e,g)) return false;

	switch(e){
	case Event::MouseDrag:
		if(enabled(SelectOnDrag)){
		
			int oldIdx = selected();
			selectFromMousePos(g);
			int idx = selected();
			
			if(idx != oldIdx &&  g.mouse().left()){
				if(enabled(Momentary)){
					setValue(false, oldIdx);
					setValue(true, idx);
				}
				else{
					setValue(getValue(oldIdx));
				}
			}
		}
		return false;

	case Event::MouseDown:
		if(g.mouse().left()){
			selectFromMousePos(g);
			setValue(enabled(Momentary) ? true : !getValue());
			return false;
		}
		break;

	case Event::KeyDown:
		switch(g.keyboard().key()){
		case ' ': setValue(enabled(Momentary) ? true : !getValue()); return false;
		}
		break;

	case Event::KeyUp:
		switch(g.keyboard().key()){
		case ' ': if(enabled(Momentary)) setValue(false); return false;
		}
		break;

	case Event::MouseUp: return false;

	default: break;
	}
	return true;
}


} // glv::

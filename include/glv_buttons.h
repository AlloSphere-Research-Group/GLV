#ifndef INC_GLV_BUTTONS_H
#define INC_GLV_BUTTONS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_widget.h"

namespace glv {


// LJP: The constructor for widgets takes a Rect as an argument rather than
// individual l,t,w,h components since it's more flexible for layout and 
// only requires writing one constructor.

/// Icon function type
typedef void (* iconFunc)(float l, float t, float r, float b);



template <class V>
class ButtonBase: public ValueWidget<V>{
public:
	GLV_INHERIT_VALUEWIDGET
	
	/// @param[in] r		geometry
	/// @param[in] nx		number along x (ignored by fixed size value types)
	/// @param[in] ny		number along y (ignored by fixed size value types)
	/// @param[in] toggles	whether the button toggles
	/// @param[in] mutExc	whether multiple buttons can be on
	/// @param[in] on		the on state icon
	/// @param[in] off		the off state icon
	ButtonBase(const Rect& r, int nx=1, int ny=1, bool toggles=true, bool mutExc=false, iconFunc on=draw::rect, iconFunc off=0)
	:	ValueWidget<V>(r, nx, ny, 3, toggles, mutExc, true),
		mIconOff(off), mIconOn(on)
	{
	}
	
	virtual void onDraw(){
		using namespace glv::draw;

		float dx = w/sizeX();
		float dy = h/sizeY();

		// draw the grid lines
//		if(drawGrid()){
//			color(colors().border); lineWidth(1);
//			begin(Lines);
//			for(int x=1; x<sizeX(); ++x){ vertex(x*dx,0); vertex(x*dx,h); }
//			for(int y=1; y<sizeY(); ++y){ vertex(0,y*dy); vertex(w,y*dy); }
//			end();
//		}
		ValueWidget<V>::drawGrid();

		float p1  = padding();
		float p_2 = padding()*0.5;
		color(colors().fore);
		
		for(int i=0; i<sizeX(); ++i){
			
			float x = dx*i + p_2;
		
			for(int j=0; j<sizeY(); ++j){
				int ind = index(i,j);
				float y = dy*j + p_2;
				if(value()[ind]){	if(mIconOn ) mIconOn (x, y, x+dx-p1, y+dy-p1); }
				else{				if(mIconOff) mIconOff(x, y, x+dx-p1, y+dy-p1); }
			}		
		}
		
	}
	
	virtual bool onEvent(Event::t e, GLV& g){
		switch(e){		
		case Event::MouseDown:
			if(g.mouse.left()){
				
				ValueWidget<V>::onSelectClick(g);
				
				if(enabled(MutualExc))	value().zero();
				if(enabled(Toggleable))	value()[selected()] ^= true;
				else value()[selected()]  = true;
				return false;
			}
			break;

//		case Event::MouseDrag:
//			if(g.mouse.left()){
//				
//				ValueWidget<V>::onSelectClick(g);
//				
//				if(enabled(MutualExc))		value().zero();
//				//if(!enabled(Toggleable))
//					value()[selected()]  = true;
//				return false;
//			}
//			break;
			
		case Event::MouseUp:
			if(g.mouse.button() == Mouse::Left){
				if(!enabled(Toggleable)) value()[selected()] = false;
			}
			break;
			
		default: break;
		}
		return true;
	}
	
protected:
	iconFunc mIconOff, mIconOn;	// state icons

};



/// Single button
struct Button : public ButtonBase<Values<bool> >{

	typedef ButtonBase<Values<bool> > super;

	Button(const Rect& r=Rect(20), bool toggles=true, iconFunc on=draw::rect, iconFunc off=0)
	:	super(r, 1, 1, toggles, false, on, off)
	{}
	
	bool value() const { return super::value()[0]; }
	Button& value(bool v){ super::value()[0] = v; return *this; }
};



/// Multiple buttons
typedef ButtonBase<Array<bool> >  Buttons;



} // glv::

#endif


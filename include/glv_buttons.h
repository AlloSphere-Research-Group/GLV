#ifndef INC_GLV_BUTTONS_H
#define INC_GLV_BUTTONS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_widget.h"

namespace glv {

/// Icon function type
typedef void (* iconFunc)(float l, float t, float r, float b);



/// One or more buttons on a grid
class Buttons:  public Widget {
public:

	/// @param[in] r		geometry
	/// @param[in] nx		number along x (ignored by fixed size value types)
	/// @param[in] ny		number along y (ignored by fixed size value types)
	/// @param[in] toggles	whether the button toggles
	/// @param[in] mutExc	whether multiple buttons can be on
	/// @param[in] on		the on state icon
	/// @param[in] off		the off state icon
	Buttons(
		const Rect& r=Rect(), int nx=1, int ny=1,
		bool toggles=true, bool mutExc=false,
		iconFunc on=draw::rect, iconFunc off=0
	)
	:	Widget(r, Data(Data::BOOL, nx,ny), 3, toggles, mutExc, true),
		mIconOff(off), mIconOn(on)
	{	useInterval(false); }

	const iconFunc& iconOff() const { return mIconOff; }
	const iconFunc& iconOn () const { return mIconOn; }

	Buttons& iconOff(const iconFunc& f){ mIconOff=f; return *this; }
	Buttons& iconOn (const iconFunc& f){ mIconOn =f; return *this; }

	virtual const char * className() const { return "Buttons"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& g);

	bool value(int i1=0, int i2=0) const { return model().at<bool>(i1, i2); }

protected:
	iconFunc mIconOff, mIconOn;	// state icons
};


inline void Buttons::onDraw(){
	using namespace glv::draw;

	float dx = w/sizeX();
	float dy = h/sizeY();

	// draw the grid lines
	drawGrid();

	float p1  = padding();
	float p_2 = padding()*0.5;
	color(colors().fore);
	
	for(int i=0; i<sizeX(); ++i){
		
		float x = dx*i + p_2;
	
		for(int j=0; j<sizeY(); ++j){
			int ind = index(i,j);
			float y = dy*j + p_2;
			if(value(i,j)){	if(mIconOn ) mIconOn (x, y, x+dx-p1, y+dy-p1); }
			else{			if(mIconOff) mIconOff(x, y, x+dx-p1, y+dy-p1); }
		}		
	}
	
}

inline bool Buttons::onEvent(Event::t e, GLV& g){
	switch(e){		
	case Event::MouseDown:
		if(g.mouse.left()){
			
			onSelectClick(g);

			if(enabled(Toggleable))	setValueSelected(!value(selected()));
			else					setValueSelected(true);
			return false;
		}
		break;

//	case Event::MouseDrag:
//		if(g.mouse.left()){
//			
//			ValueWidget<V>::onSelectClick(g);
//			
//			if(enabled(MutualExc))		value().zero();
//			//if(!enabled(Toggleable))
//				value()[selected()]  = true;
//			return false;
//		}
//		break;
		
	case Event::MouseUp:
		if(g.mouse.button() == Mouse::Left){
			if(!enabled(Toggleable)){
				//Base::setValueNotify(false);
				setValueSelected(false);
			}
		}
		break;
		
	default: break;
	}
	return true;
}


/// Single button
class Button : public Buttons {
public:
	/// @param[in] r		geometry
	/// @param[in] toggles	whether the button toggles
	/// @param[in] on		the on state icon
	/// @param[in] off		the off state icon
	Button(const Rect& r=Rect(20), bool toggles=true, iconFunc on=draw::rect, iconFunc off=0)
	:	Buttons(r, 1, 1, toggles, false, on, off)
	{}
	
	virtual const char * className() const { return "Button"; }
};


//
///// Generic base class for buttons
//template <class V>
//class ButtonBase:  public ValueWidget<V, bool>{
//public:
//	GLV_INHERIT_VALUEWIDGET(V,bool);
//
//	/// @param[in] r		geometry
//	/// @param[in] nx		number along x (ignored by fixed size value types)
//	/// @param[in] ny		number along y (ignored by fixed size value types)
//	/// @param[in] toggles	whether the button toggles
//	/// @param[in] mutExc	whether multiple buttons can be on
//	/// @param[in] on		the on state icon
//	/// @param[in] off		the off state icon
//	ButtonBase(
//		const Rect& r, int nx=1, int ny=1,
//		bool toggles=true, bool mutExc=false,
//		iconFunc on=draw::rect, iconFunc off=0
//	)
//	:	Base(r, nx, ny, 3, toggles, mutExc, true),
//		mIconOff(off), mIconOn(on)
//	{}
//
//	const iconFunc& iconOff() const { return mIconOff; }
//	const iconFunc& iconOn () const { return mIconOn; }
//
//	ButtonBase& iconOff(const iconFunc& f){ mIconOff=f; return *this; }
//	ButtonBase& iconOn (const iconFunc& f){ mIconOn =f; return *this; }
//
//	virtual const char * className() const { return "ButtonBase"; }
//	virtual void onDraw();
//	virtual bool onEvent(Event::t e, GLV& g);
//
////	bool value(int i) const { return model().at<bool>(i); }
////
////	ButtonBase& value(bool v, int i){ select(i); setValueNotify(v); return *this; }
////
////	ButtonBase& value(bool v, int ix, int iy){ select(ix, iy); setValueNotify(v); return *this; }
//
//
//protected:
//	iconFunc mIconOff, mIconOn;	// state icons
//
//	virtual void onSetValueNotify(const bool& v, int idx);
//};
//
//
//
///// Single button
//class Button : public ButtonBase<Values<bool> >{
//public:
//
//	typedef ButtonBase<Values<bool> > Base;
//
//	/// @param[in] r		geometry
//	/// @param[in] toggles	whether the button toggles
//	/// @param[in] on		the on state icon
//	/// @param[in] off		the off state icon
//	Button(const Rect& r=Rect(20), bool toggles=true, iconFunc on=draw::rect, iconFunc off=0)
//	:	Base(r, 1, 1, toggles, false, on, off)
//	{}
//
//	/// Get value
//	bool value() const { return Base::values()[0]; }
//
//	/// Set value and notify observers
//	Button& value(bool v){ select(0); setValueNotify(v); return *this; }
//	
//	virtual const char * className() const { return "Button"; }
//	virtual bool toString(std::string& s) const { glv::toToken(s, value()); return true; }
//	virtual bool fromString(const std::string& s){
//		bool v;	if(glv::fromToken(v,s)){ value(v); return true; } return false;
//	}
//};
//
//
//
//
///// Multiple buttons
//class Buttons : public ButtonBase<Array<bool> >{
//public:
//
//	typedef ButtonBase<Array<bool> > Base;
//
//	/// @param[in] r		geometry
//	/// @param[in] nx		number along x (ignored by fixed size value types)
//	/// @param[in] ny		number along y (ignored by fixed size value types)
//	/// @param[in] toggles	whether the button toggles
//	/// @param[in] mutExc	whether multiple buttons can be on
//	/// @param[in] on		the on state icon
//	/// @param[in] off		the off state icon
//	Buttons(
//		const Rect& r=Rect(20), int nx=1, int ny=1,
//		bool toggles=true, bool mutExc=false,
//		iconFunc on=draw::rect, iconFunc off=0
//	)
//	:	Base(r, nx, ny, toggles, mutExc, on, off)
//	{}
//	
//	/// Get value at 1D index
//	bool value(int i) const { return Base::values()[i]; }
//	
//	/// Set value at 1D index and notify observers
//	Buttons& value(bool v, int i){
//		select(i); setValueNotify(v);
//		return *this;
//	}
//
//	/// Set value at 2D index and notify observers
//	Buttons& value(bool v, int ix, int iy){
//		select(ix, iy); setValueNotify(v);
//		return *this;
//	}
//	
//	virtual const char * className() const { return "Buttons"; }
//	virtual bool toString(std::string& s) const { glv::toToken(s, &values()[0], size()); return true; }
//	virtual bool fromString(const std::string& s){
//		bool v[size()];
//		if(glv::fromToken(v,size(), s)){
//			for(int i=0; i<size(); ++i) value(v[i], i);
//			return true;
//		}
//		return false;
//	}
//};
//
//
//
//
//// Implementation ______________________________________________________________
//
//template <class V>
//void ButtonBase<V>::onDraw(){
//	using namespace glv::draw;
//
//	float dx = w/sizeX();
//	float dy = h/sizeY();
//
//	// draw the grid lines
//	Base::drawGrid();
//
//	float p1  = padding();
//	float p_2 = padding()*0.5;
//	color(colors().fore);
//	
//	for(int i=0; i<sizeX(); ++i){
//		
//		float x = dx*i + p_2;
//	
//		for(int j=0; j<sizeY(); ++j){
//			int ind = index(i,j);
//			float y = dy*j + p_2;
//			if(Base::values()[ind]){	if(mIconOn ) mIconOn (x, y, x+dx-p1, y+dy-p1); }
//			else{						if(mIconOff) mIconOff(x, y, x+dx-p1, y+dy-p1); }
//		}		
//	}
//	
//}
//
//template <class V>
//bool ButtonBase<V>::onEvent(Event::t e, GLV& g){
//	switch(e){		
//	case Event::MouseDown:
//		if(g.mouse.left()){
//			
//			Base::onSelectClick(g);
//			
//			//if(enabled(MutualExc))	value().zero();
//			if(enabled(Toggleable))	Base::setValueNotify(!Base::values()[selected()]);
//			else					Base::setValueNotify(true);
//			return false;
//		}
//		break;
//
////	case Event::MouseDrag:
////		if(g.mouse.left()){
////			
////			ValueWidget<V>::onSelectClick(g);
////			
////			if(enabled(MutualExc))		value().zero();
////			//if(!enabled(Toggleable))
////				value()[selected()]  = true;
////			return false;
////		}
////		break;
//		
//	case Event::MouseUp:
//		if(g.mouse.button() == Mouse::Left){
//			if(!enabled(Toggleable)){
//				Base::setValueNotify(false);
//			}
//		}
//		break;
//		
//	default: break;
//	}
//	return true;
//}
//
//
//template <class V>
//void ButtonBase<V>::onSetValueNotify(const bool& v, int idx){
//	if(v == Base::values()[idx]) return;
//	if(enabled(MutualExc)) Base::values().zero();
//	Base::values()[idx] = v;
//	notify(Update::Value, ButtonChange(v, idx));
//}


} // glv::

#endif


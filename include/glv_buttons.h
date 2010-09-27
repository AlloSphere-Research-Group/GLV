#ifndef INC_GLV_BUTTONS_H
#define INC_GLV_BUTTONS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_icon.h"
#include "glv_widget.h"

namespace glv {

/// Icon function type
typedef void (* iconFunc)(float l, float t, float r, float b);


/// One or more buttons on a grid
class Buttons : public Widget {
public:

	/// @param[in] r		geometry
	/// @param[in] nx		number along x
	/// @param[in] ny		number along y
	/// @param[in] toggles	whether the button toggles
	/// @param[in] mutExc	whether multiple buttons can be on
	/// @param[in] on		the on state icon
	/// @param[in] off		the off state icon
	Buttons(
		const Rect& r=Rect(), int nx=1, int ny=1,
		bool momentary=false, bool mutExc=false,
		iconFunc on=draw::rectangle, iconFunc off=0
	);

	const iconFunc& iconOff() const { return mIconOff; }
	const iconFunc& iconOn () const { return mIconOn; }

	Buttons& iconOff(const iconFunc& f){ mIconOff=f; return *this; }
	Buttons& iconOn (const iconFunc& f){ mIconOn =f; return *this; }

	virtual const char * className() const { return "Buttons"; }
	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& g);

	bool getValue() const { return Widget::getValue<bool>(); }
	bool getValue(int i) const { return Widget::getValue<bool>(i); }
	bool getValue(int i1, int i2) const { return Widget::getValue<bool>(i1, i2); }

protected:
	iconFunc mIconOff, mIconOn;	// state icons
//	Icon * mIconOff, mIconOn;
};



/// Single button
class Button : public Buttons {
public:
	/// @param[in] r		geometry
	/// @param[in] toggles	whether the button toggles
	/// @param[in] on		the on state icon
	/// @param[in] off		the off state icon
	Button(const Rect& r=Rect(20), bool toggles=true, iconFunc on=draw::rectangle, iconFunc off=0)
	:	Buttons(r, 1, 1, toggles, false, on, off)
	{}
	
	virtual const char * className() const { return "Button"; }
};


} // glv::

#endif


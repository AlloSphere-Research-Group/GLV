#ifndef INC_GLV_BUTTONS_H
#define INC_GLV_BUTTONS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_icon.h"
#include "glv_widget.h"

namespace glv {

/// One or more buttons on a grid
class Buttons : public Widget {
public:

	/// @param[in] r			geometry
	/// @param[in] nx			number along x
	/// @param[in] ny			number along y
	/// @param[in] momentary	whether the button state matches button press state
	/// @param[in] mutExc		whether multiple buttons can be on
	/// @param[in] on			the on state symbol
	/// @param[in] off			the off state symbol
	Buttons(
		const Rect& r=Rect(), int nx=1, int ny=1,
		bool momentary=false, bool mutExc=false,
		SymbolFunc on=draw::rectangle, SymbolFunc off=0
	);

	/// Get off state symbol
	const SymbolFunc& symbolOff() const { return mSymOff; }
	
	/// Get on state symbol
	const SymbolFunc& symbolOn () const { return mSymOn; }

	/// Set on/off symbols
	Buttons& symbol(const SymbolFunc& f){ symbolOff(f); return symbolOn(f); }

	/// Set off state symbol
	Buttons& symbolOff(const SymbolFunc& f){ mSymOff=f; return *this; }

	/// Set on state symbol
	Buttons& symbolOn (const SymbolFunc& f){ mSymOn =f; return *this; }

	virtual const char * className() const { return "Buttons"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

	bool getValue() const { return Widget::getValue<bool>(); }
	bool getValue(int i) const { return Widget::getValue<bool>(i); }
	bool getValue(int i1, int i2) const { return Widget::getValue<bool>(i1, i2); }

protected:
	SymbolFunc mSymOff, mSymOn;	// state symbols
//	Icon * mIconOff, mIconOn;
};



/// Single button
class Button : public Buttons {
public:
	/// @param[in] r			geometry
	/// @param[in] momentary	whether the button state matches button press state
	/// @param[in] on			the on state symbol
	/// @param[in] off			the off state symbol
	Button(const Rect& r=Rect(20), bool momentary=false, SymbolFunc on=draw::rectangle, SymbolFunc off=0)
	:	Buttons(r, 1,1, momentary, false, on, off)
	{}
	
	virtual const char * className() const { return "Button"; }
};


} // glv::

#endif


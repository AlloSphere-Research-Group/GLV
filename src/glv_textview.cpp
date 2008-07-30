/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <string>
#include <string.h>
#include "glv_textview.h"

namespace glv{

#define CTOR_LIST mSize(1), mVertical(false)

#define CTOR_BODY\
	disable(CropSelf | DrawBack | DrawBorder | HitTest);\
	label(str);\
	vertical(vert);

Label::Label(const std::string& str, bool vert)
:	View(0,0,0,0), CTOR_LIST
{	CTOR_BODY }

Label::Label(const std::string& str, space_t l, space_t t, bool vert)
:	View(l,t,0,0), CTOR_LIST
{	CTOR_BODY }

Label::Label(const std::string& str, Place::t p, space_t px, space_t py, bool vert)
:	View(0,0,0,0), CTOR_LIST
{	CTOR_BODY 
	pos(p, px, py).anchor(p);
}

#undef CTOR_LIST
#undef CTOR_BODY

Label& Label::label(const std::string& s){ mLabel = s; fitExtent(); return *this; }
Label& Label::size(float pixels){ mSize = pixels/8.; fitExtent(); return *this; }

Label& Label::vertical(bool v){
	if(v != mVertical){
		rotateRect();
		mVertical = v;
	}
	return *this;
}

void Label::onDraw(){
	using namespace glv::draw;
	lineWidth(1);
	color(colors().text);
	if(mVertical){ translate(0,h); rotate(0,0,-90); }
	scale(mSize, mSize);
	text(mLabel.c_str());
}


void Label::fitExtent(){
	space_t tw = 0, th = 8, mw = 0;
	const char * c = mLabel.c_str();
	
	while(*c){
		switch(*c++){
		case '\n': th += 16; tw = 0; break;
		case '\t': tw = ((int)(tw/32) + 1) * 32; if(tw > mw) mw=tw; break;
		default: tw += 8; if(tw > mw) mw=tw;
		}
	}
	extent(mw*mSize, th*mSize);
	if(mVertical) rotateRect();
}

void Label::rotateRect(){ t += w - h; transpose(); }


} // glv::

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

Label::Label(const std::string& str, const Spec& s)
:	View(0,0,0,0)
{
	disable(CropSelf | DrawBack | DrawBorder | HitTest);
	label(str);
	vertical(s.vert);
	size(s.size);
	pos(s.posAnch, s.dx, s.dy).anchor(s.posAnch);
}

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

Label& Label::label(const std::string& s){ mLabel = s; fitExtent(); notify(Update::Value); return *this; }
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
	
	space_t dw = mw*mSize - w;
	space_t dh = th*mSize - h;
	posAdd(-dw*mAnchorX, -dh*mAnchorY);
	
	extent(mw*mSize, th*mSize);
	if(mVertical) rotateRect();
}

void Label::rotateRect(){ t += w - h; transpose(); }





NumberDialer::NumberDialer(const Rect& r, int numInt, int numFrac)
:	View(r), mNI(0), mNF(0), mVal(0), mPad(2), mAcc(0)
{	
	resize(numInt, numFrac);
	mMax = pow(10, mNI+mNF)-1;
	mMin =-mMax;
	dig(mNI);
}

NumberDialer::NumberDialer(const Rect& r, int numInt, int numFrac, double max, double min)
:	View(r), mNI(0), mNF(0), mVal(0), mPad(2), mAcc(0)
{	
	resize(numInt, numFrac);
	range(max, min);
	dig(mNI);
}



NumberDialer& NumberDialer::range(double max, double min){
	mMin = convert(min);
	mMax = convert(max);
	return *this;
}

double NumberDialer::value() const{ return mVal * mValMul; }
NumberDialer& NumberDialer::value(double v){ valSet(convert(v)); return *this; }

void NumberDialer::onDraw(){ //printf("% g\n", value());
	using namespace glv::draw;
	float dx = w/size();
	
	// draw box at position (only if focused)
	if(enabled(Focused)){
		color(colors().fore, colors().fore.a*0.5);
		rect(dig()*dx, 0, (dig()+1)*dx, h);
	}
	
	// draw number
	int absVal = mVal < 0 ? -mVal : mVal;
	int msd = mNF;	// most significant digit
	
	if(absVal > 0){
		msd = (int)log10(absVal);
		msd = msd < mNF ? mNF : msd > size()-2 ? size()-2 : msd;
	}
	
	color(colors().text);
	draw::push();
		float sf = (h - 2*mPad)/Glyph::baseline();
		scale(sf, sf);

		float tdx = Glyph::width()*0.5*sf;
		float x = (w - dx*0.5)*sf - tdx;
		float y = mPad*sf;
		dx *= sf;
		
		int power = 1;
		bool drawChar = false; // don't draw until non-zero or past decimal point

		begin(Lines);
			if(mNF  > 0) character('.', dx*(mNI+1) - tdx, y);
			if(mVal < 0) character('-', dx*0.5 - tdx, y);
			
			for(int i=0; i<=msd; ++i){
				char c = '0' + (absVal % (power*10))/power;
				power *= 10;
				if(c!='0' || i>=mNF) drawChar = true;
				if(drawChar) character(c, x, y);
				x -= dx;
			}
		end();
	draw::pop();
}


bool NumberDialer::onEvent(Event::t e, GLV& g){

	Keyboard& k = g.keyboard;
	Mouse& m    = g.mouse;

	switch(e){
	case Event::MouseDown:
		mAcc = 0;
		int oldDig = dig();
		dig((int)(m.xRel()/w * size()));
		if(dig() == 0 && oldDig == 0) flipSign();
		return false;
	
	case Event::MouseDrag:
		if(onNumber()){
			mAcc += 0.25 * fabs(m.dy());
			if(mAcc > 1){
				int mul = (int)mAcc;
				valAdd((m.dy() > 0.f ? -mag() : mag())*mul);
				mAcc -= mul;
			}
		}
		return false;
	
	case Event::KeyDown:
	
		if(k.isNumber() && onNumber()){
			int v = mVal < 0 ? -mVal : mVal;
			int p = mag();
			v += (k.keyAsNumber() - ((v / p) % 10)) * p;
			valSet(mVal < 0 ? -v : v);
			return false;
		}
		else{
			switch(k.key()){
			case 'a': onNumber() ? valAdd( mag()) : flipSign(); return false;
			case 'z': onNumber() ? valAdd(-mag()) : flipSign(); return false;
			case '-': flipSign(); return false;
			case Key::Right: dig(dig()+1); return false;
			case Key::Left:  dig(dig()-1); return false;
			}
		}
		break;
		
	default:;
	}

	return true;
}





} // glv::

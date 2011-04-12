/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <math.h>
#include <string>
#include <string.h>
#include "glv_textview.h"

namespace glv{

#define CTOR_LIST mAlignX(0), mAlignY(0), mVertical(false)
#define CTOR_BODY\
	data().resize(Data::STRING);\
	useInterval(false);\
	disable(CropSelf | DrawBack | DrawBorder | HitTest);\
	setValue(str);\
	vertical(vert)

Label::Label(const std::string& str, const Spec& s)
:	Widget(Rect(0)), mAlignX(0), mAlignY(0)
{
	data().resize(Data::STRING);
	useInterval(false);
	disable(CropSelf | DrawBack | DrawBorder | HitTest);
	setValue(str);
	vertical(s.vert);
	size(s.size);
	pos(s.posAnch, s.dx, s.dy).anchor(s.posAnch);
}

Label::Label(const std::string& str, bool vert)
:	Widget(Rect(0)), CTOR_LIST
{	CTOR_BODY; }

Label::Label(const std::string& str, space_t l, space_t t, bool vert)
:	Widget(Rect(l,t,0,0)), CTOR_LIST
{	CTOR_BODY; }

Label::Label(const std::string& str, Place::t p, space_t px, space_t py, bool vert)
:	Widget(Rect(0)), CTOR_LIST
{	CTOR_BODY;
	pos(p, px, py).anchor(p);
}

#undef CTOR_LIST
#undef CTOR_BODY

Label& Label::align(float vx, float vy){ mAlignX=vx; mAlignY=vy; return *this; }

Label& Label::size(float pixels){
	font().size(pixels);
	fitExtent();
	return *this;
}

//Label& Label::setValue(const std::string& s){
////	model().set(s);
////	fitExtent();
////	if(numObservers(Update::Value)) notify(Update::Value, LabelChange(s));
//	Widget::setValue(s);
//	fitExtent();
//	return *this;
//}

Label& Label::vertical(bool v){
	if(v != mVertical){
		rotateRect();
		mVertical = v;
	}
	return *this;
}

void Label::onDraw(GLV& g){
	using namespace glv::draw;
	lineWidth(1);
	color(colors().text);
	if(mVertical){ translate(0,h); rotate(0,0,-90); }
	//font().render(value().c_str());
	font().render(data().toString().c_str());
	//scale(mSize, mSize);
	//text(value().c_str());
}


void Label::fitExtent(){

	float tw, th;
	font().getBounds(tw,th, data().toString().c_str());
	//th -= font().descent();

	space_t dw = tw - w;
	space_t dh = th - h;
	posAdd(-dw*mAlignX, -dh*mAlignY);
	extent(tw, th);
	if(mVertical) rotateRect();

/*
	space_t dx = 8;
	space_t tw = 0, th = 8, mw = 0;
	//const char * c = value().c_str();
	const char * c = data().toString().c_str();
	if(!c) return;
	while(*c){
		switch(*c++){
		case '\n': th += dx*2; tw = 0; break;
		case '\t': tw = ((int)(tw/32) + 1) * 32; if(tw > mw) mw=tw; break;
		case '\b': tw -= dx; break;
		default: tw += dx; if(tw > mw) mw=tw;
		}
	}
	
	float mul = font().scaleX();
	
	space_t dw = mw*mul - w;
	space_t dh = th*mul - h;
	posAdd(-dw*mAlignX, -dh*mAlignY);
	
	extent(mw*mul, th*mul);
	if(mVertical) rotateRect();
*/
}

void Label::rotateRect(){ t += w - h; transpose(); }



#define CTOR_LIST mNI(0), mNF(0), mAcc(0), mShowSign(true)
#define CTOR_BODY\
	font().letterSpacing(1./4);\
	enable(DrawSelectionBox)

NumberDialers::NumberDialers(int numInt, int numFrac, double max, double min, int nx, int ny)
:	Widget(Rect(0,0, (12-2)*(numInt+numFrac+1), 12), 6, false,false,true), CTOR_LIST
{
	CTOR_BODY;
	data().resize(Data::DOUBLE, nx, ny);
	resize(numInt, numFrac);
	dig(mNI);
	fitExtent();
	interval(max, min);
} 

NumberDialers::NumberDialers(const NumberDialers& v)
:	Widget(v,6, false,false,true), CTOR_LIST
{
	CTOR_BODY;
	data() = v.data();
	data().clone();
	dig(v.sizeInteger());
	resize(v.sizeInteger(), v.sizeFraction());
	interval(v.max(), v.min());
}

#undef CTOR_LIST
#undef CTOR_BODY

void NumberDialers::fitExtent(){
	extent(
		sizeX() * (paddingX()*2 + (numDigits() * font().advance('M'))),
		sizeY() * (paddingY()*2 + font().cap())
	);
//	print();
}

NumberDialers& NumberDialers::interval(double mx, double mn){
	mInterval.endpoints(mn, mx);
	double m = maxVal();	// do not allow numbers larger than can be displayed
	if(min() < -m) mInterval.min(-m);
	if(max() >  m) mInterval.max( m);
	showSign(mn < 0);
//	valSet(mVal);
	setValue(getValue());
	return *this;
}

NumberDialers& NumberDialers::resize(int numInt, int numFrac){
	mNI = numInt; mNF = numFrac;
//	mValMul = 1. / pow(10., mNF);
//	setWidth();
	fitExtent();
	return *this;
}

NumberDialers& NumberDialers::showSign(bool v){
	mShowSign=v;
//	setWidth();
	fitExtent();
	return *this;
}

int NumberDialers::sizeFraction() const { return mNF; }
int NumberDialers::sizeInteger() const { return mNI; }

void NumberDialers::onDraw(GLV& g){ //printf("% g\n", value());
	using namespace glv::draw;

	fitExtent();

	float dxCell= dx();
	float dyCell= dy();
	float dxDig = font().advance('M');

//	View::enable(DrawSelectionBox);
//	View::enable(DrawGrid);

	// draw box at position (only if focused)
	if(enabled(Focused)){
	
		float bx = dxCell*selectedX() + paddingX()/1;
		float by = dyCell*selectedY();// + paddingY()/2;
	
//		color(colors().fore, colors().fore.a*0.4);
		color(colors().selection);
		rectangle(bx + dig()*dxDig, by, bx + (dig()+1)*dxDig, by + dyCell-0.5f);
	}

	drawSelectionBox();
	drawGrid(g.graphicsData());

	lineWidth(1);

	for(int i=0; i<sizeX(); ++i){
		for(int j=0; j<sizeY(); ++j){
			
			float cx = dxCell*i;	// left edge of cell
			float cy = dyCell*j;	// top edge of cell
			
			// draw number
			int vali = valInt(i,j);
			int absVal = vali < 0 ? -vali : vali;
			int msd = mNF;	// position from right of most significant digit
			
			if(absVal > 0){
				msd = (int)log10((double)absVal);
				int p = numDigits() - (mShowSign ? 2:1);
				msd = msd < mNF ? mNF : (msd > p ? p : msd);
			}
			
			if(mNI == 0) msd-=1;

			// Determine digit string
			char str[32];
			int ic = numDigits();
			str[ic] = '\0';
			for(int i=0; i<numDigits(); ++i) str[i]=' ';
			
			int power = 1;
			bool drawChar = false; // don't draw until non-zero or past decimal point

			if(mShowSign && vali < 0) str[0] = '-';
			
			for(int i=0; i<=msd; ++i){
				char c = '0' + (absVal % (power*10))/power;
				power *= 10;
				if(c!='0' || i>=mNF) drawChar = true;
				--ic;
				if(drawChar) str[ic] = c;
			}

			// Draw the digit string
			float tx = int(cx + paddingX());
			float ty = int(cy + paddingY());

			color(colors().text);
		//	printf("%s\n", str);
			font().render(str, tx, ty);
			if(mNF>0) font().render(".", dxDig*(mNI+numSignDigits()-0.5f) + tx, ty);
		}
	}
}

bool NumberDialers::onEvent(Event::t e, GLV& g){

	if(!Widget::onEvent(e,g)) return false;

	const Keyboard& k = g.keyboard();
	const Mouse& m    = g.mouse();

	switch(e){
	case Event::MouseDown:{
		mAcc = 0;
		int oldDig = dig();

		selectFromMousePos(g);
		float dxDig = font().advance('M');
		int d = (m.xRel() - (dx() * selectedX() + paddingX())) / dxDig;
		
//		printf("%2d (%2d, %2d)\n", d, selectedX(), selectedY());
		dig(d);
		if(dig() == 0 && oldDig == 0 && mShowSign) flipSign();
		return false;
	}
	
	case Event::MouseDrag:
		if(onNumber()){
			mAcc += 0.25f * fabs(m.dy());
			if(mAcc > 1){
				int mul = (int)mAcc;
				valAdd((m.dy() > 0.f ? -mag() : mag())*mul);
				mAcc -= mul;
			}
		}
		return false;
	
	case Event::KeyDown:
	
		if(k.isNumber() && onNumber()){
			int vali = valInt(selected());
			int v = vali < 0 ? -vali : vali;
			int p = pow(10., numDigits()-1-dig());
			v += (k.keyAsNumber() - ((v / p) % 10)) * p;
			v = vali < 0 ? -v : v;
			setValue(v * pow(10., -mNF));
			return false;
		}
		else{
			switch(k.key()){
			case 'a': onNumber() ? valAdd( mag()) : flipSign(); return false;
			case 'z': onNumber() ? valAdd(-mag()) : flipSign(); return false;
			case '-': flipSign(); return false;
			case 'c': setValue(0); return false;
			case '.': dig(numDigits()-mNF); return false; // go to first fraction digit (if any)
			case Key::Left:
			case 's': dig(dig()-1); return false;
			case Key::Right:
			case 'd': dig(dig()+1); return false;
			}
		}
		break;
		
	default:;
	}

	return true;
}

// Old from NumberDialers:
//	int mVal;				// current fixed-point value
//	double mValMul;

//	void setWidth(){ w = (h-2)*size(); }
	
//	int convert(double v) const { return (v/mValMul) + (v>0. ? 0.5:-0.5); }
//	int mag() const { return pow(10., numDigits()-1-dig()); }

//	void valAdd(int v){	valSet(v + mVal); }
//	int valInt(int ix, int iy=0){ return convert(data().at<double>(ix,iy)); }
//	void flipSign(){
//		if((mVal>0 && -mVal>=convert(mMin)) || (mVal<0 && -mVal<=convert(mMax)))
//			valSet(-mVal);
//	}

//	void valSet(int v){	// converts fixed point to floating point value
////		mVal = glv::clip(v, convert(mMax), convert(mMin));
////		double val = mVal * mValMul;
////		Widget::setValue(val);
//
//		double val = v * mValMul;
//		setValue(val);
//	}

//	NumberDialers& setValue(double v){ valSet(convert(v)); return *this; }

//	virtual bool onAssignData(Data& d, int ind1, int ind2){
//	
////		double v = d.at<double>(ind1, ind2);		
////		int vi = convert(v);
//////		mVal = glv::clip(v, convert(mMax), convert(mMin));
//////		double val = mVal * mValMul;		
////		mVal = vi;
//	
//		if(Widget::onAssignData(d, ind1, ind2)){
//			double v = d.at<double>(ind1, ind2);
//			mVal = convert(v); //printf("%d\n", mVal);
//			return true;
//		}
//		return false;
//	}




TextView::TextView(const Rect& r, float textSize)
:	Widget(r), mSpacing(1), mPadX(4), mSel(0), mBlink(0)
{
	data().resize(Data::STRING);
	cursorPos(0);
	size(textSize);
}

TextView& TextView::size(float pixels){
	font().size(pixels);
	return *this;
}

//TextView& TextView::setValue(const std::string& v){
//	if(v != mText){
//		mText=v;
//		callNotify();
//	}
//	return *this;
//}

bool TextView::onAssignData(Data& d, int ind1, int ind2){
	if(Widget::onAssignData(d, ind1, ind2)){
		mText = getValue();
		return true;
	}
	return false;
}

void TextView::onDraw(GLV& g){
	using namespace draw;
	
	if(++mBlink==40) mBlink=0; // update blink interval

	float padX = mPadX;
	float padY = 4;
	float addY =-4;//was -2		// subtraction from top since some letters go above cap

	float tl = mPos * font().advance('M') + padX;
//	float tr = tl + font().advance('M');
	float tt = addY + padY;
	float tb = tt + fabs(font().descent()+font().cap()) - addY;
	float strokeWidth = 1;
	
	// draw selection
	if(selected()){
		float sl, sr;
		if(mSel>0){
			sl = tl;
			sr = sl + mSel*font().advance('M');
		}
		else{
			sr = tl;
			sl = sr + mSel*font().advance('M');
		}
		color(colors().selection);
		rectangle(sl, tt, sr, tb);
	}

	// draw cursor
	if(mBlink<20 && enabled(Focused)){
		color(colors().text);
		shape(Lines, tl, tt, tl, tb);
	}

	draw::lineWidth(strokeWidth);
	color(colors().text);
	font().render(mText.c_str(), padX, padY);
}

bool TextView::onEvent(Event::t e, GLV& g){

	const Keyboard& k = g.keyboard();
	int key = k.key();
	float mx = g.mouse().xRel();

	switch(e){
		case Event::KeyDown:
			if(isprint(key)){
				deleteSelected();
				mText.insert(mPos, 1, k.key()); setValue(mText);
				cursorPos(mPos+1);
				return false;
			}
			else{
				switch(key){
				case Key::Backspace:
					if(selected()) deleteSelected();
					else if(validPos()){
						deleteText(mPos-1, 1);
						cursorPos(mPos-1);
					}
					return false;
					
				case Key::Delete:
					if(selected()) deleteSelected();
					else if(mText.size()){
						deleteText(mPos, 1);
						cursorPos(mPos);
					}
					return false;
				
				case Key::Left:
					if(k.shift()) select(mSel-1);
					else cursorPos(mPos-1);
					return false;
					
				case Key::Right:
					if(k.shift()) select(mSel+1);
					else cursorPos(mPos+1);
					return false;
					
				case Key::Down:	cursorPos(mText.size()); return false;
				case Key::Up:	cursorPos(0); return false;
					
//				case Key::Enter:
//				case Key::Return:
//					return false;
				}
			}
			break;

		case Event::MouseDown:
			cursorPos(xToPos(mx));
		case Event::MouseUp:
			return false;

		case Event::MouseDrag:
			{
				int p = xToPos(mx);
				if(p >= mPos) select(p-mPos+1);
				else select(p-mPos);
				//printf("%d\n", mSel);
			}
			return false;

		default:;
	}

	return true;
}


void TextView::deleteSelected(){
	if(mSel>0){
		deleteText(mPos, mSel);
		cursorPos(mPos);
	}
	else if(mSel<0){
		deleteText(mPos+mSel, -mSel);
		cursorPos(mPos+mSel);
	}
}


void TextView::deleteText(int start, int num){
	mText.erase(start, num);
	setValue(mText);
}

void TextView::select(int v){
	int nt = mText.size();
	int end = mPos + v;
	if(end<0) end=0;
	if(end>nt) end = nt;
	mSel = end-mPos;
}

void TextView::cursorPos(int v){
	if(v<=int(mText.size()) && v>=0){
		mPos=v;
	}
	deselect();
	mBlink=0;
}

int TextView::xToPos(float x){
	float charw = font().advance('M');
	if(x<0) x=0;
	int p = (x-mPadX*1)/charw;
	if(p > (int)mText.size()) p = mText.size();
	if(p<0) p=0;
	return p;
}




} // glv::

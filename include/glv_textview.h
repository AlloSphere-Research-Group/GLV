#ifndef INC_GLV_TEXTVIEW_H
#define INC_GLV_TEXTVIEW_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <string>
#include <string.h>
#include "glv_core.h"
#include "glv_widget.h"

namespace glv{


typedef ChangedValue<std::string> LabelChange;
typedef ChangedValue<double> NumberDialerChange;


/// Basic label widget
class Label : public View{
public:

	/// Prototype for constructor
	struct Spec{
		Spec(Place::t posAnch, space_t dx, space_t dy, float size=8, bool vert=false)
		:	posAnch(posAnch), dx(dx), dy(dy), size(size), vert(vert)
		{}

		Spec& set(Place::t posAnch_, space_t dx_, space_t dy_, float size_=8, bool vert_=false){
			posAnch=posAnch_; dx=dx_; dy=dy_; size=size_; vert=vert_;
		}
		
		Place::t posAnch;
		space_t dx, dy;
		float size;
		bool vert;
	};

	Label(const std::string& str, const Spec& spec);

	/// @param[in] str		Label string
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& str, bool vert);

	/// @param[in] str		Label string
	/// @param[in] l		Left position
	/// @param[in] t		Top position
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& str="", space_t l=0, space_t t=0, bool vert=false);

	/// Ctor for adding label to inside of another View

	/// @param[in] str		Label string
	/// @param[in] posAnch	Position/anchor place relative to parent
	/// @param[in] px		X position offset
	/// @param[in] py		Y position offset
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& str, Place::t posAnch, space_t px, space_t py, bool vert=false);

	Label& align(float vx, float vy);		///< Set alignment factors for label area
	Label& label(const std::string& s);		///< Set label string
	Label& size(float pixels);				///< Set label size
	Label& vertical(bool v);				///< Set whether label is displayed vertically

	virtual void onDraw();

protected:
	std::string mLabel;		// The label string
	float mSize;
	float mAlignX, mAlignY;
	bool mVertical;
	
	void fitExtent();
	void rotateRect();		// swap between vertical and horizontal
};




/// Number editor with individually controllable digits

/// Each digit acts like a slider that can be scrolled up or down to increase
/// or decrease the value of the number. The keyboard can also be used to edit
/// the currently focused digit. The sign on the left side can be toggled by 
/// clicking it.
class NumberDialer : public View{
public:

	/// @param[in] r		Geometry
	/// @param[in] numInt	Number of places in integer part
	/// @param[in] numFrac	Number of places in fraction part
	NumberDialer(const Rect& r, int numInt, int numFrac);

	/// @param[in] r		Geometry
	/// @param[in] numInt	Number of places in integer part
	/// @param[in] numFrac	Number of places in fraction part
	/// @param[in] max		Maximum value
	/// @param[in] min		Minimum value
	NumberDialer(const Rect& r, int numInt, int numFrac, double max, double min=0);

	/// Get value
	double value() const;
	
	/// Set max and min output range
	NumberDialer& range(double max, double min=0);

	/// Set value
	NumberDialer& value(double v);

	virtual void onDraw();
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	int mNI, mNF, mPos;
	int mVal, mMin, mMax;
	float mPad, mAcc;
	double mValMul;

	int convert(double v) const { return v / mValMul + (v>0. ? 0.5:-0.5); }
	int mag() const { return pow(10, size()-1-dig()); }
	bool onNumber() const { return mPos != 0; }
	int dig() const { return mPos; }
	void dig(int v){ mPos = v < 0 ? 0 : v >= size() ? size()-1 : v; }
	int size() const { return mNI + mNF + 1; }
	void valAdd(int v){	valSet(v + mVal); }
	void valSet(int v){
		int prev = mVal;
		mVal = v < mMin ? mMin : v > mMax ? mMax : v;
		if(mVal != prev) notify(Update::Value, NumberDialerChange(value()));
	}
	void flipSign(){ valSet(-mVal); }
	
	void resize(int numInt, int numFrac){
		mNI = numInt; mNF = numFrac;
		mValMul = 1. / pow(10., mNF);
	}
};





// Base class for number displaying/editing box(es)

// Deprecated in favor of NumberDialer.
//template <class V>
//class NumberBoxBase : public ValueWidget<V>{
//public:
//	GLV_INHERIT_VALUEWIDGET
//
//	NumberBoxBase(const Rect& r, int nx=1, int ny=1, const char * format="% g")
//	:	ValueWidget<V>(r, nx, ny, 10, false, true, true),
//		mStep(1), mFormat(format)
//	{}
//	
//	NumberBoxBase& step(float v){ mStep=v; return *this; }
//
//	virtual void onDraw(){
//		using namespace glv::draw;
//
//		float dx = w/sizeX();
//		float dy = h/sizeY();
//
//		// draw the grid lines
//		//ValueWidget<V>::drawGrid(*this);
//
//		// draw selected frame
//		color(colors().fore);
//		float fx = dx*selectedX(), fy = dy*selectedY();
//		frame(fx, fy, fx+dx, fy+dy);
//
//		float p_2 = padding()*0.5;
//		float textScale = (dy-padding())/Glyph::baseline();
//		float rTextScale = 1./textScale;
//		
//		// Draw cursor
//		color(colors().text, colors().text.a*0.3);
//		float curx = mNumEnt.pos() * Glyph::width()*textScale + selectedX()*this->dx() + p_2;
//		float cury = selectedY() * this->dy();
//		draw::rect(curx, cury, curx+Glyph::width(), cury+this->dy());
//		
//		lineWidth(1);
//		
//		char buf[16]; // text buffer
//		
//		// Draw text
//		// TODO: turn this into a display list
//		color(colors().text);
//		for(int i=0; i<sizeX(); ++i){
//			
//			float x = dx*i + p_2;
//		
//			for(int j=0; j<sizeY(); ++j){
//				int ind = index(i,j);
//				float y = dy*j + p_2;
//
//				float v = value()[ind];
//				snprintf(buf, sizeof(buf), mFormat, v);
//				//float len = strlen(buf);
//				//text(buf, p - (8.f * len * 0.5f), 4); // center text
//				
//				push();
//				scale(textScale, textScale);
//				text(buf, pix(x)*rTextScale, pix(y)*rTextScale);
//				pop();
//			}
//		}
//		
//	}
//	
//	virtual bool onEvent(Event::t e, GLV& g){
//		switch(e){		
//		case Event::MouseDown:
//			ValueWidget<V>::onSelectClick(g);
//			mNumEnt.value(value()[selected()]);
//			return false;
//			
//		case Event::MouseUp:
//			break;
//			
//		case Event::KeyDown:
//		
//			ValueWidget<V>::onSelectKey(g);
//			#define SETVAL value()[selected()] = mNumEnt.value()
//
//			if(mNumEnt.read(g.keyboard.key())){
//				SETVAL;
//			}
//
//			switch(g.keyboard.key()){
//			case 'w':	mNumEnt.bwd1(); return false;
//			case 'e':	mNumEnt.fwd1(); return false;
//			case 'a':	mNumEnt.addAtPos( 1); SETVAL; return false;
//			case 'z':	mNumEnt.addAtPos(-1); SETVAL; return false;
//			
//			case Key::Delete:	mNumEnt.del(); SETVAL; return false;
////			case 'a': value()[selected()] += mStep; return false;
////			case 'z': value()[selected()] -= mStep; return false;
////			case '\\': mNumEnt.reset(); return false;
//			//case Key::Delete: mNumEnt.back1(); return false;
//			default:;
//			}
//		
//			break;
//			#undef SETVAL
//		default: break;
//		}
//		return true;
//	}
//
//
//protected:
//	float mStep;
//	const char * mFormat;
//	NumberEntry mNumEnt;
//};







} // glv::
#endif

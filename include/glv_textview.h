#ifndef INC_GLV_TEXTVIEW_H
#define INC_GLV_TEXTVIEW_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <string>
#include <string.h>
#include "glv_core.h"
#include "glv_widget.h"

namespace glv{

/// Basic label widget
class Label : public View{
public:

	/// @param[in] str		Label string
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& str, bool vert);

	/// @param[in] str		Label string
	/// @param[in] l		Left position
	/// @param[in] t		Top position
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& str, space_t l=0, space_t t=0, bool vert=false);

	/// Ctor for adding label to inside of another View

	/// @param[in] str		Label string
	/// @param[in] posAnch	Position/anchor place relative to parent
	/// @param[in] px		X position offset
	/// @param[in] py		Y position offset
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& str, Place::t posAnch, space_t px, space_t py, bool vert=false);

	Label& label(const std::string& s);
	Label& size(float pixels);
	Label& vertical(bool v);

	virtual void onDraw();

protected:
	std::string mLabel;		// The label string
	float mSize;
	bool mVertical;
	
	void fitExtent();
	void rotateRect();		// swap between vertical and horizontal
};



// Base class for number displaying/editing box(es)

//
//
template <class V>
class NumberBoxBase : public ValueWidget<V>{
public:
	GLV_INHERIT_VALUEWIDGET

	NumberBoxBase(const Rect& r, int nx=1, int ny=1, const char * format="% g")
	:	ValueWidget<V>(r, nx, ny, 10, false, true, true),
		mStep(1), mFormat(format)
	{}
	
	NumberBoxBase& step(float v){ mStep=v; return *this; }

	virtual void onDraw(){
		using namespace glv::draw;

		float dx = w/sizeX();
		float dy = h/sizeY();

		// draw the grid lines
		//ValueWidget<V>::drawGrid(*this);

		// draw selected frame
		color(colors().fore);
		float fx = dx*selectedX(), fy = dy*selectedY();
		frame(fx, fy, fx+dx, fy+dy);

		float p_2 = padding()*0.5;
		float textScale = (dy-padding())/Glyph::baseline();
		float rTextScale = 1./textScale;
		
		// Draw cursor
		color(colors().text, colors().text.a*0.3);
		float curx = mNumEnt.pos() * Glyph::width()*textScale + selectedX()*this->dx() + p_2;
		float cury = selectedY() * this->dy();
		draw::rect(curx, cury, curx+Glyph::width(), cury+this->dy());
		
		lineWidth(1);
		
		char buf[16]; // text buffer
		
		// Draw text
		// TODO: turn this into a display list
		color(colors().text);
		for(int i=0; i<sizeX(); ++i){
			
			float x = dx*i + p_2;
		
			for(int j=0; j<sizeY(); ++j){
				int ind = index(i,j);
				float y = dy*j + p_2;

				float v = value()[ind];
				snprintf(buf, sizeof(buf), mFormat, v);
				//float len = strlen(buf);
				//text(buf, p - (8.f * len * 0.5f), 4); // center text
				
				push();
				scale(textScale, textScale);
				text(buf, pix(x)*rTextScale, pix(y)*rTextScale);
				pop();
			}
		}
		
	}
	
	virtual bool onEvent(Event::t e, GLV& g){
		switch(e){		
		case Event::MouseDown:
			ValueWidget<V>::onSelectClick(g);
			mNumEnt.value(value()[selected()]);
			return false;
			
		case Event::MouseUp:
			break;
			
		case Event::KeyDown:
		
			ValueWidget<V>::onSelectKey(g);
			#define SETVAL value()[selected()] = mNumEnt.value()

			if(mNumEnt.read(g.keyboard.key())){
				SETVAL;
			}

			switch(g.keyboard.key()){
			case 'w':	mNumEnt.bwd1(); return false;
			case 'e':	mNumEnt.fwd1(); return false;
			case 'a':	mNumEnt.addAtPos( 1); SETVAL; return false;
			case 'z':	mNumEnt.addAtPos(-1); SETVAL; return false;
			
			case Key::Delete:	mNumEnt.del(); SETVAL; return false;
//			case 'a': value()[selected()] += mStep; return false;
//			case 'z': value()[selected()] -= mStep; return false;
//			case '\\': mNumEnt.reset(); return false;
			//case Key::Delete: mNumEnt.back1(); return false;
			default:;
			}
		
			break;
			#undef SETVAL
		default: break;
		}
		return true;
	}


protected:
	float mStep;
	const char * mFormat;
	NumberEntry mNumEnt;
};







} // glv::
#endif

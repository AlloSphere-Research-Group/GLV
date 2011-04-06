#ifndef INC_GLV_TEXTVIEW_H
#define INC_GLV_TEXTVIEW_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <string>
#include <ctype.h>
#include <string.h>
#include "glv_core.h"
#include "glv_font.h"
#include "glv_widget.h"

namespace glv{

/// Basic label widget
class Label : public Widget {
public:

	/// Prototype for constructor
	struct Spec{
		/// Constructor
		Spec(Place::t posAnch, space_t dx, space_t dy, float size=8, bool vert=false)
		:	posAnch(posAnch), dx(dx), dy(dy), size(size), vert(vert){}
		
		/// Set all members
		Spec& set(Place::t posAnch_, space_t dx_, space_t dy_, float size_=8, bool vert_=false){
			posAnch=posAnch_; dx=dx_; dy=dy_; size=size_; vert=vert_; return *this;
		}
		
		Place::t posAnch;
		space_t dx, dy;
		float size;
		bool vert;
	};

	/// Create label using a prototype

	/// @param[in] text		Label text
	/// @param[in] spec		Label spec prototype
	Label(const std::string& str, const Spec& spec);

	/// @param[in] str		Label text
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& text, bool vert);

	/// @param[in] text		Label text
	/// @param[in] l		Left position
	/// @param[in] t		Top position
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& text="", space_t l=0, space_t t=0, bool vert=false);

	/// Ctor for adding label to inside of another View

	/// @param[in] text		Label text
	/// @param[in] posAnch	Position/anchor place relative to parent
	/// @param[in] px		X position offset
	/// @param[in] py		Y position offset
	/// @param[in] vert		Whether to draw label vertically
	Label(const std::string& text, Place::t posAnch, space_t px, space_t py, bool vert=false);

	Label& align(float vx, float vy);		///< Set alignment factors for label area
	Label& size(float pixels);				///< Set label size
	Label& vertical(bool v);				///< Set whether label is displayed vertically

	/// Get value
	const std::string& getValue() const { return data().elems<std::string>()[0]; }

	virtual const char * className() const { return "Label"; }
	virtual void onDraw(GLV& g);

protected:
	float mAlignX, mAlignY;
	bool mVertical;
	
	void fitExtent();
	void rotateRect();		// swap between vertical and horizontal

	virtual bool onAssignData(Data& d, int ind1, int ind2){
		if(Widget::onAssignData(d, ind1, ind2)){
			fitExtent();
			return true;
		}
		return false;
	}
};




/// Number editor with individually controllable digits

/// Each digit acts like a slider that can be scrolled up or down to increase
/// or decrease the value of the number. The keyboard can also be used to edit
/// the currently focused digit. The sign on the left side can be toggled by 
/// clicking it.
/// The digits are scaled according to the dimensions of the view. The padding
/// amount determines the spacing around the digits. For best looking
/// characters use a Rect dimension ns x s where 'n' is the number of digits
/// and 's' is the character size plus padding amount.
class NumberDialers : public Widget{
public:

	/// @param[in] numInt	Number of places in integer part
	/// @param[in] numFrac	Number of places in fraction part
	/// @param[in] max		Maximum value
	/// @param[in] min		Minimum value
	/// @param[in] nx		Number of instances along x
	/// @param[in] ny		Number of instances along y
	NumberDialers(int numInt=3, int numFrac=2, double max=100, double min=0, int nx=1, int ny=1);

	/// Copy constructor
	NumberDialers(const NumberDialers& v);

	/// Get value
	double getValue() const { return Widget::getValue<double>(); }

	/// Get number of digits in fraction part
	int sizeFraction() const;
	
	/// Get number of digits in integer part
	int sizeInteger() const;

	/// Set size of font
	NumberDialers& fontSize(float pixels){ font().size(pixels); fitExtent(); return *this; }

	/// Set max and min output range. Values larger than displayable range will be clipped.
	NumberDialers& interval(double max, double min=0);

	/// Set padding amount
	NumberDialers& padding(double v){ Widget::padding(v); fitExtent(); return *this; }

	/// Set number of digits in integer and fraction parts
	NumberDialers& resize(int numInt, int numFrac);

	/// Set whether to show sign
	NumberDialers& showSign(bool v);

	virtual const char * className() const { return "NumberDialers"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	int mNI, mNF, mPos;		// # digits in integer, # digits in fraction, selected digit position
	float mAcc;
	bool mShowSign;

	void fitExtent();

	bool onNumber() const { return mPos!=signPos(); }
	int dig() const { return mPos; }
	void dig(int v){ mPos = v<0 ? 0 : v>=numDigits() ? numDigits()-1 : v; }
	double maxVal() const { return (pow(10., mNI+mNF)-1)/pow(10., mNF); }
	int numDigits() const { return mNI + mNF + numSignDigits(); }
	int numSignDigits() const { return mShowSign ? 1:0; }
	int signPos() const { return mShowSign ? 0 : -1; }
	int valInt(int ix, int iy=0) const {
		double v = data().at<double>(ix,iy);
		return (int)(v * pow(10., mNF) + (v>0. ? 0.5:-0.5));
	}

	double mag() const { return pow(10., numDigits()-1-dig() - mNF); }
	void valAdd(double v){ setValue(getValue() + v); }

	void flipSign(){
		double v = getValue();
		if((v>0 && -v>=mMin) || (v<0 && -v<=mMax))
			setValue(-v);
	}
};



/// Number editor with individually controllable digits
class NumberDialer: public NumberDialers{
public:

	/// @param[in] numInt	Number of places in integer part
	/// @param[in] numFrac	Number of places in fraction part
	/// @param[in] max		Maximum value
	/// @param[in] min		Minimum value
	NumberDialer(int numInt=3, int numFrac=2, double max=100, double min=0)
	:	NumberDialers(numInt,numFrac,max,min,1,1){ padding(2); }

	/// Copy constructor
	NumberDialer(const NumberDialer& v)
	:	NumberDialers(v){ padding(2); }

	virtual const char * className() const { return "NumberDialer"; }
};



/// View for editing text
class TextView : public Widget{
public:
	/// Constructor
	TextView(const Rect& r=glv::Rect(200,16), float textSize=8);

	/// Get value
	const std::string& getValue() const { return data().elems<std::string>()[0]; }

	/// Returns true if no text is present
	bool empty() const { return getValue().empty(); }

	/// Set cursor position
	void cursorPos(int v);
	
	/// Put cursor past end of text
	void cursorEnd(){ cursorPos(mText.size()); }

	/// Set size of font in pixels
	TextView& size(float pixels);

	void select(int v);
	void deselect(){ mSel=0; }

	virtual const char * className() const { return "TextView"; }
	virtual void onDraw(GLV& g);	
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	std::string mText;		// The text string

	float mSpacing;
	float mPadX;
	int mPos;
	int mSel;	// selection range (0==none)
	int mBlink;
	bool validPos(){ return mPos<=int(mText.size()) && mPos>0; }
	int xToPos(float x); // convert x pixel position to character position
	void deleteSelected();
	bool selected(){ return mSel!=0; }
	void deleteText(int start, int num);

	virtual bool onAssignData(Data& d, int ind1, int ind2);
};



class ListSelect : public View {
public:

	/// @param[in] r		geometry
	/// @param[in] pad		padding between items
	ListSelect(const Rect& r=Rect(140,16), space_t pad=4)
	:	View(r), mSelected(0), mPad(pad)
	{}

	/// @param[in] r		geometry
	/// @param[in] item1	item 1
	/// @param[in] item2	item 2
	/// @param[in] pad		padding between items
	ListSelect(
		const Rect& r,
		const std::string& item1, const std::string& item2,
		space_t pad=4
	)
	:	View(r), mSelected(0), mPad(pad)
	{	add(item1).add(item2); }

	/// @param[in] r		geometry
	/// @param[in] item1	item 1
	/// @param[in] item2	item 2
	/// @param[in] item3	item 3
	/// @param[in] pad		padding between items
	ListSelect(
		const Rect& r,
		const std::string& item1, const std::string& item2, const std::string& item3,
		space_t pad=4
	)
	:	View(r), mSelected(0), mPad(pad)
	{	add(item1).add(item2).add(item3); }

	/// @param[in] r		geometry
	/// @param[in] item1	item 1
	/// @param[in] item2	item 2
	/// @param[in] item3	item 3
	/// @param[in] pad		padding between items
	ListSelect(
		const Rect& r,
		const std::string& item1, const std::string& item2, const std::string& item3, const std::string& item4,
		space_t pad=2
	)
	:	View(r), mSelected(0), mPad(pad)
	{	add(item1).add(item2).add(item3).add(item4); }


	/// Get selected item
	int selected() const { return mSelected; }

	/// Get currently selected item's string
	const std::string& getValue() const { return mItems[selected()]; }

	/// Get index of an item
	
	/// \returns index of item or number of items if the item is not found
	///
	int indexOf(const std::string& item) const {
		for(unsigned i=0; i<mItems.size(); ++i){
			if(mItems[i] == item) return i;
		}
		return mItems.size();
	}

	/// Add new item to list
	ListSelect& add(const std::string& v){
		mItems.push_back(v); return *this;
	}

	/// Select item based on index
	ListSelect& select(int i){
		mSelected = glv::clip<int>(i,mItems.size()-1); return *this;
	}

	/// Select item based on name
	ListSelect& select(const std::string& v){
		int i = indexOf(v);
		if(i < int(mItems.size())) select(i);
		return *this;
	}

	virtual const char * className() const { return "ListSelect"; }

	virtual void onDraw(GLV& g){
		using namespace glv::draw;
		if(mItems.size() < 1) return;
		font().size(height() - 2*mPad);
		
		color(colors().text);
		stroke(1);
		font().render(mItems[selected()].c_str(), mPad, mPad);
	}

	virtual bool onEvent(Event::t e, GLV& g){
	
		const Keyboard& k = g.keyboard();
		const Mouse& m = g.mouse();
	
		switch(e){
		case Event::KeyDown:
			if(!k.ctrl() && !k.alt() && isgraph(k.key())){
				char c = tolower(k.key());
				for(unsigned i=0; i<mItems.size(); ++i){
					if(mItems[i][0] == c){
						select(i); break;
					}
				}
				return false;
			}
			else{
				switch(k.key()){
				case Key::Up:	select(selected()-1); return false;
				case Key::Down:	select(selected()+1); return false;
				default:;
				}
			}
			break;
		case Event::MouseDown:
			return false;
		case Event::KeyUp:
		case Event::MouseUp: return false;

		case Event::MouseDrag:{
			int dy = m.y() - m.y(m.button());
			int inc = ((dy+800000) % 8 == 0) * (dy<0?-1:1);
			select(selected()+inc);
			return false;
		}
			
		default:;
		}
		return true;
	}

private:
	int mSelected;
	space_t mPad;
	std::vector<std::string> mItems;
	
	/// Get data associated with the model, if any
	virtual const Data& getData() const {
		glv::Data& md = const_cast<glv::Data&>(data());
		md.set(mItems[selected()]);
		return data();
	}

	/// Set data associated with the model, if any
	virtual void setData(const Data& d){
		std::string s = d.at<std::string>(0);
		select(s);
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

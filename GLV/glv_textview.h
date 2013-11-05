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
		Spec(Place::t posAnch=Place::TL, space_t dx=0, space_t dy=0, float size=8, bool vert=false)
		:	posAnch(posAnch), dx(dx), dy(dy), size(size), stroke(1), vert(vert){}
		
//		/// Set all members
//		Spec& set(Place::t posAnch_, space_t dx_, space_t dy_, float size_=8, bool vert_=false){
//			posAnch=posAnch_; dx=dx_; dy=dy_; size=size_; vert=vert_; return *this;
//		}
		
		Place::t posAnch;
		space_t dx, dy;
		float size;
		float stroke;
		bool vert;
	};

	/// Create label using a prototype

	/// @param[in] text		Label text
	/// @param[in] spec		Label spec prototype
	Label(const std::string& text, const Spec& spec);

	/// @param[in] text		Label text
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

	float stroke() const { return mStroke/256.; }

	Label& align(float vx, float vy);		///< Set alignment factors for label area
	Label& size(float pixels);				///< Set label size
	Label& stroke(float pixels);			///< Set stroke width
	Label& vertical(bool v);				///< Set whether label is displayed vertically

	/// Get value
	const std::string& getValue() const { return data().elems<std::string>()[0]; }

	virtual const char * className() const { return "Label"; }
	virtual void onDraw(GLV& g);

protected:
	float mAlignX, mAlignY;
	unsigned short mStroke;
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

	virtual void onResize(space_t dx, space_t dy){
		Widget::onResize(dx,dy);
		fitExtent();
	}
};


/// View for editing text
class TextView : public Widget{
public:

	/// Character input filter predicate

	/// @param[in] text			the current text
	/// @param[in] pos			insertion position within text
	/// @param[in] newChar		the new character to be added to the string
	/// \returns whether the character should be inserted into the string
	typedef bool (*CharacterInputFilter)(const std::string& text, int pos, int newChar);

	/// Numeric character input filter
	static bool filterNumeric(const std::string& text, int pos, int newChar);


	/// Constructor
	TextView(const Rect& r=glv::Rect(200,16), float textSize=8);


	/// Get value
	const std::string& getValue() const { return data().elems<std::string>()[0]; }

	/// Returns true if no text is present
	bool empty() const { return getValue().empty(); }

	/// Get current cursor position
	int cursorPos() const { return mPos; }

	/// Get current selection range
	int selectionRange() const { return mSel; }


	/// Set cursor position
	void cursorPos(int v);
	
	/// Put cursor past end of text
	void cursorEnd(){ cursorPos(mText.size()); }

	/// Set the character input filter
	TextView& filter(CharacterInputFilter v){ mFilter=v; return *this; }

	/// Set size of font in pixels
	TextView& size(float pixels);

	/// Select a number of characters away from cursor position
	void select(int v);
	void selectAll();
	void deselect(){ mSel=0; }


	virtual const char * className() const { return "TextView"; }
	virtual void onAnimate(double dsec);
	virtual void onDraw(GLV& g);	
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	std::string mText;		// The text string

	float mSpacing;
//	float mPadX;
	int mPos;
	int mSel;	// selection range (0==none)
	float mBlink;	// cursor blink phase ( on = [0, 0.5); off = [0.5, 1) )
	CharacterInputFilter mFilter;

	bool validPos(){ return mPos<=int(mText.size()) && mPos>0; }
	int xToPos(float x); // convert x pixel position to character position
	void deleteSelected();
	bool textSelected(){ return mSel!=0; }
	void deleteText(int start, int num);

	virtual bool onAssignData(Data& d, int ind1, int ind2);
};



// Allows selection from a list of values
class ListView : public Widget{
public:

	ListView(const Rect& r=Rect(0), int nx=0, int ny=0);

	ListView& fitExtent();

	ListView& selectValue(const std::string& v);

	const std::string getValue() const { return Widget::getValue<std::string>(); }
	const std::string getValue(int i) const { return Widget::getValue<std::string>(i); }
	const std::string getValue(int i1, int i2) const { return Widget::getValue<std::string>(i1, i2); }

	virtual const Data& getData(Data& dst) const;
	virtual void setData(const Data& d);

	virtual const char * className() const { return "ListView"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
};



// Drop-down list
class DropDown : public TextView {
public:
	typedef std::vector<std::string> Items;

	/// @param[in] r			geometry
	/// @param[in] textSize		size of text, in pixels
	DropDown(const Rect& r=glv::Rect(200,16), float textSize=8)
	:	TextView(r,textSize), mItemList(*this)
	{	init(); }

	/// @param[in] r			geometry
	/// @param[in] item1		item 1
	/// @param[in] item2		item 2
	/// @param[in] textSize		size of text, in pixels
	DropDown(
		const Rect& r,
		const std::string& item1, const std::string& item2,
		float textSize=8
	)
	:	TextView(r,textSize), mItemList(*this)
	{	init(); addItem(item1).addItem(item2); }

	/// @param[in] r			geometry
	/// @param[in] item1		item 1
	/// @param[in] item2		item 2
	/// @param[in] item3		item 3
	/// @param[in] textSize		size of text, in pixels
	DropDown(
		const Rect& r,
		const std::string& item1, const std::string& item2, const std::string& item3,
		float textSize=8
	)
	:	TextView(r,textSize), mItemList(*this)
	{	init(); addItem(item1).addItem(item2).addItem(item3); }

	/// @param[in] r			geometry
	/// @param[in] item1		item 1
	/// @param[in] item2		item 2
	/// @param[in] item3		item 3
	/// @param[in] item4		item 4
	/// @param[in] textSize		size of text, in pixels
	DropDown(
		const Rect& r,
		const std::string& item1, const std::string& item2, const std::string& item3, const std::string& item4,
		float textSize=8
	)
	:	TextView(r,textSize), mItemList(*this)
	{	init(); addItem(item1).addItem(item2).addItem(item3).addItem(item4); }

	virtual ~DropDown();


	/// Get index of currently selected item
	int selectedItem() const { return mSelectedItem; } //return mItemList.selected(); }

	/// Get reference to items
	Items& items(){ return mItems; }

	/// Add an item to the list
	DropDown& addItem(const std::string& v);

	virtual const char * className() const { return "DropDown"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	struct ItemList : public ListView{
		ItemList(DropDown& v): dd(v){}
		virtual bool onEvent(Event::t e, GLV& g);
		DropDown& dd;
	} mItemList;

	Items mItems;		// list data that ItemList references
	int mSelectedItem;

	void init();
	void showList();
	void hideList(GLV& g);
	virtual bool onAssignData(Data& d, int ind1, int ind2);

/* TODO from ListSelect:
	/// Get index of an item
	
	/// \returns index of item or number of items if the item is not found
	///
	int indexOf(const std::string& item) const {
		for(unsigned i=0; i<mItems.size(); ++i){
			if(mItems[i] == item) return i;
		}
		return mItems.size();
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
*/
};


/// Search box
class SearchBox : public TextView{
public:
	typedef std::vector<std::string> Items;

	/// @param[in] r			geometry
	/// @param[in] textSize		size of text, in pixels
	SearchBox(const Rect& r=glv::Rect(200,16), float textSize=8);

	virtual ~SearchBox();

	/// Get whether results list is currently showing
	bool listShowing() const { return mItemList.visible(); }

	/// Get reference to items
	Items& items(){ return mItems; }

	/// Add item to search
	SearchBox& addItem(const std::string& v);

	virtual const char * className() const { return "SearchBox"; }
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	struct ItemList : public ListView{
		ItemList(SearchBox& v): sb(v){}
		virtual bool onEvent(Event::t e, GLV& g);
		SearchBox& sb;
	} mItemList;

	Items mItems;
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

	/// Get value at 1D index
	double getValue(int i) const { return Widget::getValue<double>(i); }
	
	/// Get value at 2D index
	double getValue(int i1, int i2) const { return Widget::getValue<double>(i1,i2); }


	/// Get whether the display is dimmed if the value is zero
	bool dimZero() const { return mDimZero; }

	/// Get number of digits in fraction part
	int sizeFraction() const;
	
	/// Get number of digits in integer part
	int sizeInteger() const;


	/// Set whether the display is dimmed if the value is zero
	NumberDialers& dimZero(bool v){ mDimZero=v; return *this; }

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

	virtual void onCellChange(int indexOld, int indexNew);

protected:
	//Lazy<TextView> mTextEntry;
	static TextView mTextEntry;
	int mNI, mNF, mPos;		// # digits in integer, # digits in fraction, selected digit position
	float mAcc;
	bool mShowSign, mOverwriteMode, mDimZero, mTextEntryMode;

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

	double mag(int digit) const { return pow(10., numDigits()-1-digit - mNF); }
	double mag() const { return mag(dig()); }
	void valAdd(double v){ setValue(getValue() + v); }

	void flipSign(){
		double v = getValue();
		if((v>0 && -v>=min()) || (v<0 && -v<=max()))
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

} // glv::
#endif

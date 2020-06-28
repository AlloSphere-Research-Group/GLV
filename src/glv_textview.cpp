/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <ctype.h> // tolower
#include <string.h> // strncmp
#include "glv_textview.h"

namespace glv{

#define CTOR_BODY(stroke_, vert_)\
	disable(CropSelf | DrawBack | DrawBorder | HitTest);\
	padding(0);\
	data().resize(Data::STRING);\
	useInterval(false);\
	setValue(str);\
	stroke(stroke_);\
	vertical(vert_);

Label::Label(const std::string& str, const Spec& s)
:	Widget(Rect(0))
{
	CTOR_BODY(s.stroke, s.vert);
	size(s.size);
	pos(s.posAnch, s.dx, s.dy).anchor(s.posAnch);
}

Label::Label(const std::string& str, bool vert)
:	Label(str, 0,0, vert)
{}

Label::Label(const std::string& str, space_t l, space_t t, bool vert)
:	Widget(Rect(l,t,0,0))
{
	CTOR_BODY(1,vert);
}

Label::Label(const std::string& str, Place::t p, space_t px, space_t py, bool vert)
:	Label(str, vert)
{
	pos(p, px, py).anchor(p);
}

#undef CTOR_BODY

Label& Label::align(float vx, float vy){ mAlignX=vx; mAlignY=vy; return *this; }

Label& Label::size(float pixels){
	font().size(pixels);
	fitExtent();
	return *this;
}

Label& Label::stroke(float pixels){
	mStroke = pixels*256.f;
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
	lineWidth(stroke());
	color(colors().text);
	if(mVertical){ translate(0,h); rotate(0,0,-90); }
	font().render(
		g.graphicsData(),
		data().toString().c_str(),
		paddingX(),
		paddingY()
	);
	//scale(mSize, mSize);
	//text(value().c_str());
}


void Label::fitExtent(){
	// get width/height of text
	float tw, th;
	font().getBounds(tw,th, data().toString().c_str());

	// align text by translating its current position
	auto dw = tw - (w - paddingX()*2);
	auto dh = th - (h - paddingY()*2);
	translate(-dw*mAlignX, -dh*mAlignY);

	tw += paddingX()*2;
	th += paddingY()*2;

	extent(tw, th);
	if(mVertical) rotateRect();
}

void Label::rotateRect(){
	t += w - h;
	transpose();
}

std::string Label::onDebug() const{
	std::string msg;
	if(getValue().empty()){
		msg += "\tString is empty.\n";
	}
	return msg;
}




bool TextView::filterNumeric(const std::string& text, int pos, int newChar){
	return
		((newChar >= '0') && (newChar <= '9'))
		|| newChar=='-' || newChar=='+' || newChar=='e' || newChar=='.'
	;
}

TextView::TextView(const Rect& r, float textSize)
:	Widget(r, 4)
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

void TextView::onAnimate(double dsec){
	mBlink += dsec * 0.8;
	if(mBlink >= 1) mBlink-=1;
}

void TextView::onDraw(GLV& g){
	using namespace draw;

	auto padX = paddingX();
	auto padY = paddingY();
	float addY = -4.f;//was -2 // subtraction from top since some letters go above cap

	auto tl = mPos * font().advance('M') + padX;
	//auto tr = tl + font().advance('M');
	auto tt = addY + padY;
	auto tb = tt + fabs(font().descent()+font().cap()) - addY;
	float strokeWidth = 1.f;
	
	// draw selection
	if(textSelected()){
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
	if(mBlink<0.5 && enabled(Focused)){
		stroke(1);
		color(colors().text);
		shape(Lines, pixc(tl), tt, pixc(tl), tb);
	}

	draw::lineWidth(strokeWidth);
	color(colors().text);
//	font().render(mText.c_str(), pixc(padX), pixc(padY-1));
	font().render(g.graphicsData(), mText.c_str(), padX, padY-1);
}

bool TextView::onEvent(Event::t e, GLV& g){

	const auto& k = g.keyboard();
	int key = k.key();
	float mx = g.mouse().xRel();

	switch(e){
		case Event::KeyDown:
			if(k.ctrl()){
				switch(key){
				case 'a': selectAll(); return false;
				}
			}
			else if(k.alt() || k.meta()){} // bubble if control key down

			else if(k.isPrint()){

				// preserve current text if something is selected
				std::string oldText;
				auto oldSel = mSel;
				auto oldPos = mPos;
				if(mSel){
					oldText = mText;
					deleteSelected();
				}

				// No filter assigned or filter returns true
				if(!mFilter || mFilter(getValue(), mPos, key)){
					mText.insert(mPos, 1, k.key());
					setValue(mText);
					cursorPos(mPos+1);
					return false;
				}

				// restore old text if the new character is invalid
				if(!oldText.empty()){
					mText = oldText;
					mPos = oldPos;
					mSel = oldSel;
				}
			}
			else{
				switch(key){
				case Key::Backspace:
					if(textSelected()) deleteSelected();
					else if(validPos()){
						deleteText(mPos-1, 1);
						cursorPos(mPos-1);
					}
					return false;
					
				case Key::Delete:
					if(textSelected()) deleteSelected();
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
					
				case Key::Down:
				case Key::End:	cursorPos(mText.size()); return false;
				case Key::Up:	
				case Key::Home:	cursorPos(0); return false;
					
				case Key::Enter:
				case Key::Return:
					notify(this, Update::Action);
					return false;
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

void TextView::selectAll(){
	//mPos = 0;
	//select(mText.size());
	cursorEnd();
	select(-mText.size());
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
	int p = (x-paddingX()*1)/charw;
	if(p > (int)mText.size()) p = mText.size();
	if(p<0) p=0;
	return p;
}




ListView::ListView(const Rect& r, int nx, int ny)
:	Widget(r)
{
	data().resize(Data::STRING, nx,ny);
	//font().size(18);
	disable(DrawGrid);
	//padding(8);
}

ListView& ListView::fitExtent(){
	float maxw = 0.f;//, maxh = 0;
	int nitems = data().size();

	for(int i=0; i<nitems; ++i){
		float x = font().advance(data().at<std::string>(i).c_str());
		if(x > maxw) maxw = x;
	}
	extent(
		draw::pix(data().size(0) * (maxw + paddingX()*2)),
		draw::pix(data().size(1) * (font().cap() + font().descent() + paddingY()*2))
	);
	return *this;
}

ListView& ListView::selectValue(const std::string& v){
	auto idx = data().indexOf(v);
	if(idx != Data::npos) select(idx);
	//printf("ListView::selectValue = %d\n", idx);
	return *this;
}

const Data& ListView::getData(Data& dst) const {
	dst = data().slice(selected(), 1);
	return dst;
}

void ListView::setData(const Data& d){
	auto idx = data().indexOf(d);
	if(idx != Data::npos){
		select(idx);
		ModelChange modelChange(data(), idx);
		notify(this, Update::Value, &modelChange);
	}
}

void ListView::onDraw(GLV& g){

	using namespace glv::draw;

	Indexer idx(data().size(0), data().size(1));
	float dx_ = dx(0);
	float dy_ = dy(1);
	
	while(idx()){
		int ix = idx[0];
		int iy = idx[1];
		
		float px = dx_ * ix;
		float py = dy_ * iy;

		if(selectedX() == ix && selectedY() == iy){
			color(colors().selection);
			draw::rectangle(px,py, px+dx_,py+dy_);
		}
		
		color(colors().text);
		lineWidth(1);
		
		//font().render(data().at<std::string>(ix,iy).c_str(), pixc(px+paddingX()), pixc(py+paddingY()));
		font().render(g.graphicsData(), data().at<std::string>(ix,iy).c_str(), px+paddingX(), py+paddingY());
	}
	
	Widget::onDraw(g);
}

bool ListView::onEvent(Event::t e, GLV& g){

	if(!Widget::onEvent(e,g)) return false;

	switch(e){
	case Event::MouseDown:
	case Event::MouseDrag:
		selectFromMousePos(g);
		return false;

//		case Event::MouseMove:
//			if(containsPoint(g.mouse().xRel(), g.mouse().yRel())){
//				selectFromMousePos(g);
//				return false;
//			}
//			break;
	
	case Event::KeyDown:
		switch(g.keyboard().key()){
			case Key::Enter:
			case Key::Return:
				notify(this, Update::Action);
//				printf("action on %d\n", selected());
				return false;
		default:;
		}
		break;
	
	default:;
	}
	

	return true;
}



DropDown::DropDown(const Rect& r, float textSize)
:	TextView(r,textSize)
{
	mItemList.disable(Visible);
	mItemList.font().size(font().size());
	
	// Set padding so text appears vertically centered
	//paddingX(font().size()/2 + h/2);
}

DropDown::DropDown(const Rect& r, std::initializer_list<std::string> items, float textSize)
:	DropDown(r,textSize)
{
	for(auto& s : items) addItem(s);
}

DropDown::DropDown(std::initializer_list<std::string> items, float textSize)
:	DropDown(Rect(), items, textSize)
{
	fitExtent();
}

DropDown::DropDown(
	const Rect& r,
	const std::string& item1, const std::string& item2,
	float textSize
)
:	DropDown(r, {item1,item2}, textSize)
{}

DropDown::DropDown(
	const Rect& r,
	const std::string& item1, const std::string& item2, const std::string& item3,
	float textSize
)
:	DropDown(r, {item1,item2,item3}, textSize)
{}

DropDown::DropDown(
	const Rect& r,
	const std::string& item1, const std::string& item2, const std::string& item3, const std::string& item4,
	float textSize
)
:	DropDown(r, {item1,item2,item3,item4}, textSize)
{}

DropDown::~DropDown(){
	mItemList.remove();
}

DropDown& DropDown::addItem(const std::string& v){
	items().push_back(v);

	mItemList.data().set(&items()[0], 1, items().size());
	mItemList.clipIndices();
	mItemList.fitExtent();

	if(items().size() == 1) setValue(items()[0]);

	return *this;
}

DropDown& DropDown::fitExtent(){
	if(mItems.size()){
		unsigned longest=0;
		for(unsigned i=0; i<mItems.size(); ++i){
			if(mItems[i].size() > mItems[longest].size()) longest=i;
		}
		float tw,th;
		font().getBounds(tw,th, mItems[longest].c_str());
		float texPad = 3.;
		extent(tw + th + 2.*texPad + 4., th + 2.*texPad);
	}
	return *this;
}

DropDown& DropDown::selectedItem(int i){
	if(i<(int)mItems.size() && i>=0) setValue(mItems[i]);
	return *this;
}

void DropDown::hideList(GLV& g){
	mItemList.disable(Visible);
	g.setFocus(this);
}

void DropDown::showList(){
	if(mItems.size()){
		// Must place list in topmost view to register clicks
		space_t ax = 0, ay = height();
		auto& top = *const_cast<View *>(toAbs(ax, ay));
		if(mItemList.parent == 0) top << mItemList;
		auto ayAdj = ay - (selectedItem()+1) * (mItemList.h / mItems.size());
		if(ayAdj < 0.) ayAdj = ay;
		//FIXME: if ax,ay not integer, list may not appear!
		mItemList.pos(draw::pix(ax), draw::pix(ayAdj));
		mItemList.bringToFront();
		mItemList.enable(Visible);
	}
}

void DropDown::onDraw(GLV& g){
	mBlink = 0.9;
	TextView::onDraw(g);
	
	draw::color(colors().fore);
	auto pad = 3.;
	draw::triangleD(w - h + pad, pad, w-pad, h-pad);
}

bool DropDown::onEvent(Event::t e, GLV& g){
//printf("DropDown::onEvent %s\n", toString(e));

/*
	From ListSelect:

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

*/

	const Keyboard& k = g.keyboard();

	switch(e){
	case Event::KeyDown:
		// bubble if modifier key down
		if(k.ctrl() || k.alt() || k.meta()){}
		
		// printable keys search list and select first matching item
		else if(k.isPrint()){
			unsigned char lkey = tolower(k.key());
			for(unsigned i=0; i<items().size(); ++i){
				if(lkey == tolower(items()[i][0])){
					mItemList.select(i);
					showList();
					break;
				}
			}
			return false;
		}
		
		// switch on other keys...
		else{
			switch(k.key()){
			case Key::Down:
			case Key::Up:
				if(!mItemList.visible()){
					mItemList.selectValue(getValue());
				}
				else{ // pass on up/down arrows to item list...
					mItemList.onEvent(e,g);
				}
				showList();
				return false;
			case Key::Enter:
			case Key::Return:
				if(mItemList.visible()) setValue(mItemList.getValue());
				hideList(g);
				return TextView::onEvent(e,g); // let base class handle it from here
			default:;
			}
		}
		break;
	case Event::MouseUp:
		return false;
	case Event::MouseDown:
		if(mItemList.visible())	hideList(g);
		else{					showList(); //g.setFocus(&mItemList); // momentary
		}
		return false;
	case Event::FocusLost:
		if(!mItemList.enabled(Focused)) mItemList.disable(Visible);
		return false;
	default:;
	}

	return true;
}

bool DropDown::onAssignData(Data& d, int ind1, int ind2){
	//printf("DropDown: onAssignData\n"); d.print();
	// ind1 and ind2 are always zero since TextView is one value
	
	// check if new string value is in our list
	std::string itemString = d.at<std::string>(0);
	int itemIndex = mItemList.data().indexOf(itemString);
	if(Data::npos != itemIndex){
		mSelectedItem = itemIndex;
		//printf("DropDown: selectedItem = %d\n", itemIndex);
		mItemList.selectValue(itemString);
	}
	TextView::onAssignData(d, ind1, ind2);
	return true;
}

bool DropDown::ItemList::onEvent(Event::t e, GLV& g){
	const Keyboard& k = g.keyboard();
	const Mouse& m = g.mouse();

	switch(e){
	case Event::KeyDown:
		switch(k.key()){
		case Key::Enter:
		case Key::Return:
			dd.setValue(getValue());
			dd.onEvent(e,g);
			break;
		default:;
		}
		break;
	case Event::FocusLost:
		disable(Visible);
		break;
	case Event::MouseUp:
		//printf("setValue\n");
		//if(Rect(w,h).containsPoint(m.xRel(), m.yRel())){
		if(containsPoint(m.x(), m.y())){
			dd.setValue(getValue());
			dd.notify(&dd, Update::Action);
			//dd.onEvent(e,g);
		}
		disable(Visible);
		g.setFocus(&dd);
		return false;
	default:;
	}
	return ListView::onEvent(e,g);
}




SearchBox::SearchBox(const Rect& r, float textSize)
:	TextView(r,textSize)
{}

SearchBox::~SearchBox(){ mItemList.remove(); }

SearchBox& SearchBox::addItem(const std::string& v){
	items().push_back(v);
	return *this;
}

bool SearchBox::onEvent(Event::t e, GLV& g){

//	printf("SearchBox::onEvent %s\n", toString(e));

	bool showList = true;
	const Keyboard& k = g.keyboard();

	switch(e){
	case Event::KeyDown:
		switch(k.key()){
		case Key::Down:	// pass on up/down arrows to search list...
		case Key::Up:
			mItemList.onEvent(e,g);
			return false;
		case Key::Enter:
		case Key::Return:
			if(mItemList.visible()){
				setValue(mItemList.getValue());
				cursorEnd();
			}
			mItemList.disable(Visible);
			showList = false;
			break; // pass event on to TextView
			//return false;
		case Key::Tab:
			if(!empty()){
				const std::string& tstr = getValue();
				for(const auto& s : mItems){
					int r = strncmp(s.c_str(), tstr.c_str(), tstr.size());
					if(0 == r){
						setValue(s);
						cursorEnd();
						break;
					}
				}
			}
			return false;
		default:;
		}
		break;
	case Event::FocusLost:
		if(!mItemList.enabled(Focused)) mItemList.disable(Visible);
		return false;
	default:;
	}
	
	bool res = TextView::onEvent(e,g);

	if(Event::KeyDown == e || Event::MouseDown == e){

		mItemList.disable(Visible);
		if(showList && !empty()){
			std::vector<std::string> listItems;
			const std::string& tstr = getValue();
			for(const auto& s : mItems){
				//int r = strncasecmp(s.c_str(), tstr.c_str(), tstr.size());
				auto pos = s.find(tstr);
				if(pos != std::string::npos){
					//printf("%s, %d\n", s.c_str(), pos);
					listItems.push_back(s);
				}
			}

			if(listItems.size()){
				if(!((listItems.size() == 1) && (listItems[0].size() == tstr.size()))){
					space_t ax = 0, ay = height();
					const View& top = *toAbs(ax, ay);
					
					mItemList.data().resize(1, listItems.size());
					mItemList.data().assignFromArray(&listItems[0], listItems.size());
					mItemList.clipIndices();
					mItemList.fitExtent();

					if(ay > top.height()/2){
						ay -= mItemList.height() + height();
					}

					mItemList.pos(ax,ay);
					mItemList.enable(Visible);
					mItemList.bringToFront();
				}
			}
			if(mItemList.parent == 0) root() << mItemList;			
		}
	}

	return res;
}

bool SearchBox::ItemList::onEvent(Event::t e, GLV& g){
	const auto& k = g.keyboard();
	const auto& m = g.mouse();

	switch(e){
	case Event::KeyDown:
		switch(k.key()){
		case Key::Enter:
		case Key::Return:
			//sb.setValue(getValue());
			sb.onEvent(e,g);
			break;
		default:;
		}
		break;

	case Event::FocusLost:
		disable(Visible);
		break;

	case Event::MouseUp:
		// ItemList is in absolute coordinates
		if(containsPoint(m.x(), m.y())){
			sb.setValue(getValue());
			sb.cursorEnd();
			sb.notify(&sb, Update::Action);
			sb.onEvent(e,g);
		}
		disable(Visible);
		g.setFocus(&sb);
		return false;

	default:;
	}
	return ListView::onEvent(e,g);
}


NumberDialers::TextEntry::TextEntry(){
	filter(TextView::filterNumeric);
	paddingY(3);
}

bool NumberDialers::TextEntry::onEvent(Event::t e, GLV& g){
	auto * nd = dynamic_cast<NumberDialers*>(parent);

	switch(e){
	case Event::FocusLost:
		if(nd){
			unbind(*nd);
			return false;
		}
		break;

	case Event::KeyDown:
		switch(g.keyboard().key()){
		case Key::Escape:
			setValue(""); // prevents NumberDialer from being set
		case Key::Return:
		case Key::Enter:
			g.setFocus(nd);
			unbind(*nd);
			return false;
		}
		break;

	default:;
	}

	return TextView::onEvent(e,g);
}

void NumberDialers::TextEntry::bind(NumberDialers& nd){
	nd << (*this);
	setValue(toString(nd.getValue()));
	selectAll();
}

void NumberDialers::TextEntry::unbind(NumberDialers& nd){
	remove();
	if(!getValue().empty()){				
		nd.setValue(data().at<double>(0));
	}
}

decltype(NumberDialers::mTextEntry) NumberDialers::mTextEntry;


#define CTOR_BODY\
	font().letterSpacing(1./4);\
	enable(DrawSelectionBox)

NumberDialers::NumberDialers(int numInt, int numFrac, double max, double min, int nx, int ny)
:	Widget(Rect(0,0, (12-2)*(numInt+numFrac+1), 12), 2, false,false,true)
{
	CTOR_BODY;
	data().resize(Data::DOUBLE, nx, ny);
	resize(numInt, numFrac);
	interval(max, min);
} 

NumberDialers::NumberDialers(const std::string& range, double initVal, int nx, int ny)
:	NumberDialers(1,0, 1.,0., nx,ny)
{
	bool neg=false, pos=false;
	int numInt=0;
	int numFrac=0;
	bool dec = false;

	if(range.size()>2 && range[0]=='+' && range[1]=='-'){
		neg=pos=true;
	} else if(range[0]=='-'){
		neg=true;
	}

	for(auto c : range){
		if('0' <= c && c <= '9'){
			if(dec) ++numFrac;
			else    ++numInt;
		} else if(c=='.'){
			dec=true;
		}
	}
	if(!(neg || pos)) pos=true;
	resize(numInt, numFrac);
	double maxVal; glv::fromToken(maxVal, neg&&pos ? range.substr(1) : range);
	maxVal = std::abs(maxVal);
	interval(pos ? maxVal:0, neg ? -maxVal:0);
	for(int i=0; i<size(); ++i){ setValue(initVal, i); }
}

NumberDialers::NumberDialers(const NumberDialers& v)
:	Widget(v,2, false,false,true)
{
	CTOR_BODY;
	data() = v.data();
	data().clone();
	dig(v.sizeInteger());
	resize(v.sizeInteger(), v.sizeFraction());
	interval(v.max(), v.min());
}

#undef CTOR_BODY

void NumberDialers::fitExtent(){
	extent(
		draw::pix(sizeX() * (paddingX()*2 + (numDigits() * font().advance('M'))) + 1),
		draw::pix(sizeY() * (paddingY()*2 + font().cap()) + 1)
	);
//	print();
}

NumberDialers& NumberDialers::interval(double mx, double mn){
	Widget::interval(mn,mx);
	double m = maxVal();	// do not allow numbers larger than can be displayed
	if(min() < -m) mInterval.min(-m);
	if(max() >  m) mInterval.max( m);
	showSign((min() < 0.) || (max() < 0.));
	//valSet(mVal);
	for(int i=0; i<data().size(); ++i) setValue(getValue(i), i);
	return *this;
}

NumberDialers& NumberDialers::resize(int numInt, int numFrac){
	mNI = numInt; mNF = numFrac;
	//mValMul = 1. / pow(10., mNF);
	//setWidth();
	dig(mNI); // go to first integer (or fraction) digit
	fitExtent();
	return *this;
}

NumberDialers& NumberDialers::showSign(bool v){
	mShowSign=v;
	//setWidth();
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

		float x = dxCell*selectedX() + paddingX()/1 - 1;
		//float y = dyCell*selectedY() + paddingY()/2;
		float y = dyCell*(selectedY()+0.5);
		float ty= font().cap()/2. + 3;

//		color(colors().fore, colors().fore.a*0.4);
		color(colors().selection);
		//rectangle(bx + dig()*dxDig, by, bx + (dig()+1)*dxDig, by + dyCell-0.5f);
		rectangle(x + dig()*dxDig, y-ty, x + (dig()+1)*dxDig, y+ty);
	}

	drawSelectionBox();
	drawGrid(g.graphicsData());

	lineWidth(1);

	if(textEntryMode()){
		mTextEntry.extent(dxCell, dyCell);
		mTextEntry.pos(dxCell*selectedX(), dyCell*selectedY());
	}

	for(int i=0; i<sizeX(); ++i){
		for(int j=0; j<sizeY(); ++j){

			float cx = dxCell*i;	// left edge of cell
			float cy = dyCell*j;	// top edge of cell

			// draw number
			long long vali = valInt(i,j);
			unsigned long absVal = vali < 0 ? -vali : vali;
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

			if(mShowSign && vali < 0) str[0] = '-';

			unsigned long long power = 1;
			bool drawChar = false; // don't draw until non-zero or past decimal point

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

			if(vali || !dimZero()){
				color(colors().text);
			} else {
				color(colors().text.mix(colors().back, 0.8));
			}
		//	printf("%s\n", str);
//			font().render(g.graphicsData(), str, pixc(tx), pixc(ty));
//			if(mNF>0) font().render(g.graphicsData(), ".", pixc(dxDig*(mNI+numSignDigits()-0.5f) + tx), pixc(ty));
			font().render(g.graphicsData(), str, tx, ty);
			if(mNF>0) font().render(g.graphicsData(), ".", dxDig*(mNI+numSignDigits()-0.5f) + tx, ty);
		}
	}
}

bool NumberDialers::onEvent(Event::t e, GLV& g){

	const Keyboard& k = g.keyboard();
	const Mouse& m    = g.mouse();

	switch(e){
	case Event::MouseDown:{
		mAcc = 0;
		int oldDig = dig();

		selectFromMousePos(g);
		float dxDig = font().advance('M');
		int d = (m.xRel() - (dx() * selectedX() + paddingX())) / dxDig;
		
		//printf("%2d (%2d, %2d)\n", d, selectedX(), selectedY());
		dig(d);
		if(dig() == 0 && oldDig == 0 && mShowSign) flipSign();
		return false;
	}
	
	case Event::MouseDrag:
		if(onNumber()){
			mAcc += 0.25f * fabs(m.dy());
			if(mAcc > 1){
				int mul = (int)mAcc;
				int digit = dig();
				/*if(m.left() && m.right() && digit < (numDigits()-1)){
					++digit;
				}*/
				valAdd((m.dy() > 0.f ? -mag(digit) : mag(digit))*mul);
				mAcc -= mul;
			}
		}
		return false;
	
	case Event::KeyDown:
		if(k.ctrl() || k.alt() || k.meta()){}
		else if(k.isNumber() && onNumber()){
			long vi = valInt(selected());
			auto absvi = vi < 0 ? -vi : vi;
			unsigned long p = pow(10., numDigits()-1-dig()) + 0.5;
			//printf("%lu\n", p);
			absvi += (k.keyAsNumber() - ((absvi / p) % 10)) * p;
			auto newvi = vi < 0 ? -absvi : absvi;
			setValue(newvi * pow(10., -mNF));
			if(!mOverwriteMode) dig(dig()+1);
			return false;
		}
		else{
			switch(k.key()){
			case 'a': onNumber() ? valAdd( mag()) : flipSign(); return false;
			case 'z': onNumber() ? valAdd(-mag()) : flipSign(); return false;
			case '-': flipSign(); return false;
			case 'c': setValue(0); return false;
			case '=':
			case Key::Enter:
			case Key::Return:
				if(!textEntryMode()){ // bring up the text entry box
					mTextEntry.bind(*this);
					g.setFocus(&mTextEntry);
				}
				else{				// set value from text entry box, then remove it
					g.setFocus(this);
					mTextEntry.unbind(*this);
				}
				return false;
			case '.': dig(numDigits()-mNF); return false; // go to first fraction digit (if any)
			case Key::Left:
			case 's': dig(dig()-1); return false;
			case Key::Right:
			case 'd': dig(dig()+1); return false;
			case 'o': mOverwriteMode^=true; return false;
			}
		}
		break;

	default:;
	}

	return Widget::onEvent(e,g);
}


void NumberDialers::onCellChange(int indexOld, int indexNew){
//	mTextEntryMode=false;
//	mTextEntry.remove();
}

std::string NumberDialers::onDebug() const {
	std::string msg;
	if(sizeInteger() < 0){
		msg += "\tSize of integer part is negative.\n";
	}
	else if(sizeInteger() > 9){
		msg += "\tSize of integer part is " + toString(sizeInteger()) + ".\n";
	}

	if(sizeFraction() < 0){
		msg += "\tSize of fraction part is negative.\n";
	}
	else if(sizeFraction() > 9){
		msg += "\tSize of fraction part is " + toString(sizeFraction()) + ".\n";
	}
	return msg;
}


NumberDialer::NumberDialer(int numInt, int numFrac, double max, double min)
:	NumberDialers(numInt,numFrac,max,min,1,1)
{
	padding(2);
}

NumberDialer::NumberDialer(const std::string& range, double initVal)
:	NumberDialers(range,initVal, 1,1)
{
	padding(2);
}

NumberDialer::NumberDialer(const NumberDialer& v)
:	NumberDialers(v)
{	
	padding(2);
}


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

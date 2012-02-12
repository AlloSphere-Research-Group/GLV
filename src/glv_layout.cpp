/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_layout.h"

namespace glv{


Divider::Divider(float thickness, float strokeWidth, bool vertical)
:	mStrokeWidth(strokeWidth), mIsVertical(vertical)
{
	if(vertical){
		stretch(0,1);
		width(thickness);
	}
	else{
		stretch(1,0);
		height(thickness);		
	}
}

void Divider::onDraw(GLV& g){
	using namespace glv::draw;
	if(mStrokeWidth <= 0) return;

	lineWidth(mStrokeWidth);
	color(colors().fore);
	if(mIsVertical){
		float p = pixc(w/2);
		shape(Lines, p,0, p,h);		
	}
	else{
		float p = pixc(h/2);
		shape(Lines, 0,p, w,p);
	}
}




Placer::Placer(space_t absX, space_t absY)
:	x(absX), y(absY), rx(0), ry(0), ax(0), ay(0), fw(0), fh(0), mAnchor((Place::t)0), parent(0)
{}

Placer::Placer(View& parent, Direction dir, Place::t ali, space_t x, space_t y, space_t pad)
:	x(x), y(y), rx(1), ry(1), mAnchor((Place::t)0), parent(&parent)
{
	flow(dir, pad);
	align(ali);
}

Placer& Placer::operator<< (View& v){
	v.Rect::pos(x - v.w * fw, y - v.h * fh);
	
	x += rx * v.w + ax;
	y += ry * v.h + ay;
	
	if(mAnchor) v.anchor(mAnchor);
	if(parent) (*parent) << v;
	return *this;
}

Placer& Placer::operator<< (View* v){
	return v ? (*this)<<*v : *this;
}

Placer& Placer::abs(space_t vx, space_t vy){ return absX(vx).absY(vy); }
Placer& Placer::absX(space_t v){ ax=v; return *this; }
Placer& Placer::absY(space_t v){ ay=v; return *this; }

Placer& Placer::align(space_t vx, space_t vy){ fw=vx; fh=vy; return *this; }

Placer& Placer::align(Place::t p){
	switch(p){
		case Place::TL: return align(0.0, 0.0);
		case Place::TC: return align(0.5, 0.0);
		case Place::TR: return align(1.0, 0.0);
		case Place::CL: return align(0.0, 0.5);
		case Place::CC: return align(0.5, 0.5);
		case Place::CR: return align(1.0, 0.5);
		case Place::BL: return align(0.0, 1.0);
		case Place::BC: return align(0.5, 1.0);
		case Place::BR: return align(1.0, 1.0);
		default: return *this;
	}
}

Placer& Placer::anchor(Place::t p){ mAnchor = p; return *this; }

Placer& Placer::flow(Direction d){
	ax < 0 ? ax = -ax : 0;
	ay < 0 ? ay = -ay : 0;
	rx < 0 ? rx = -rx : 0;
	ry < 0 ? ry = -ry : 0;
	return flow(d, ax > ay ? ax : ay, rx > ry ? rx : ry);
}

Placer& Placer::flow(Direction d, space_t va, space_t vr){
	switch(d){
		case Direction::S: return abs(   0, va).rel(  0, vr);
		case Direction::E: return abs( va,   0).rel(  vr, 0);
		case Direction::N: return abs(   0,-va).rel(  0,-vr);
		case Direction::W: return abs(-va,   0).rel(-vr,  0);
		default: return *this;
	}
}

Placer& Placer::pos(space_t vx, space_t vy){ x=vx; y=vy; return *this; }
Placer& Placer::posX(space_t v){ x=v; return *this; }
Placer& Placer::posY(space_t v){ y=v; return *this; }
Placer& Placer::rel(space_t vx, space_t vy){ rx=vx; ry=vy; return *this; }




LayoutFunc::LayoutFunc(space_t w, space_t h, const Rect& bounds)
:	elem(0,0,w,h), bounds(bounds), parent(0)
{}

LayoutFunc::~LayoutFunc()
{}

void LayoutFunc::layoutChildren(View& pv){
	View * cv = pv.child;
	while(cv){
		cv->set((*this)());
		cv = cv->sibling;
	}
}



LayoutGrid::LayoutGrid(space_t w, space_t h, const Rect& b, int numV, int numH)
:	LayoutFunc(w,h, b), numV(numV), numH(numH), cntV(0), cntH(0)
{}

LayoutGrid::LayoutGrid(space_t pad, const Rect& b, int numV, int numH)
:	LayoutFunc((b.w - pad)/numH - pad, (b.h - pad)/numV - pad, b), 
	numV(numV), numH(numH), cntV(0), cntH(0)
{
	bounds.l += pad;
	bounds.t += pad;
	bounds.w -= pad;
	bounds.h -= pad;
}

LayoutGrid::LayoutGrid(View& p, int numH, int numV, space_t pad)
:	LayoutFunc((p.w - pad)/numH - pad, (p.h - pad)/numV - pad, Rect(p.w, p.h)), 
	numV(numV), numH(numH), cntV(0), cntH(0)
{
	bounds.l += pad;
	bounds.t += pad;
	bounds.w -= pad;
	bounds.h -= pad;
	parent = &p;
}

LayoutGrid::~LayoutGrid()
{}

Rect& LayoutGrid::operator()(){
	
	elem.l = cntH/(float)numH * bounds.w + bounds.l;
	elem.t = cntV/(float)numV * bounds.h + bounds.t;
	
	if(++cntH == numH){
		cntH=0;
		if(++cntV == numV) cntV=0;
	}
	
	return elem;
}



Table::Table(const char * a, space_t padX, space_t padY, const Rect& r)
:	Group(r), mSize1(0), mSize2(0), mPad1(padX), mPad2(padY), mRepeatRow(-1)
{	arrangement(a); }


void Table::getCellDim(int idx, space_t& pl, space_t& pt, space_t& pr, space_t& pb){
	const Cell& c = mCells[idx];
	space_t padl = mPad1*(c.x+1);
	space_t padt = mPad2*(c.y+1);
	space_t padr = (c.w-1)*mPad1;
	space_t padb = (c.h-1)*mPad2;
	pl = sumSpan(&mColWs[0], c.x) + padl;
	pt = sumSpan(&mRowHs[0], c.y) + padt;
	pr = sumSpan(&mColWs[0], c.x+c.w, c.x) + pl+padr;
	pb = sumSpan(&mRowHs[0], c.y+c.h, c.y) + pt+padb;
}


Table& Table::arrange(){

	View * vp = child;
	int ind = 0;

	// Check if we have more children than the arrangement string accounts for.
	// If so, then "fix" the string by creating additional copies.
	int numChildren=0;
	while(vp){ ++numChildren; vp=vp->sibling; }
	
	if(numChildren > (int)mCells.size()){
		if(mRepeatRow < 0){
			std::string a = mAlign;
			//int sizeCells = mCells.size();
			int numCopies = (numChildren-1)/mCells.size();
			for(int i=0; i<numCopies; ++i){ a+=","; a+=mAlign; }
			arrangement(a.c_str());
		}
		else{
//			int c 
//			for(int i=0; i<mAlign.size(); ++i){
//				
//			}
		}
	}
	

	// compute extent of table
	mColWs.resize(mSize1);
	mRowHs.resize(mSize2);
	mColWs.assign(mColWs.size(), 0);
	mRowHs.assign(mRowHs.size(), 0);
	
	// resize table according to non-spanning cells
	vp = child; ind=0;
	while(vp){
		View& v = *vp;		
		Cell& c = mCells[ind];

		int i1=c.x, i2=c.y;
		c.view = vp;

		// c.w or c.h equal to 1 mean contents span 1 cell
		if(c.w == 1 && v.w > mColWs[i1]) mColWs[i1] = v.w;
		if(c.h == 1 && v.h > mRowHs[i2]) mRowHs[i2] = v.h;

		vp = vp->sibling; ++ind;
	}


	// resize table according to spanning cells
	for(unsigned i=0; i<mCells.size(); ++i){
		Cell& c = mCells[i];
		if(0 == c.view) continue;
		View& v = *c.view;

		if(c.w != 1){
			int beg = c.x;
			int end = c.x + c.w;
			space_t cur = sumSpan(&mColWs[0], end, beg) + (c.w-1)*mPad1;
			
			if(v.w > cur){
				space_t add = (v.w - cur)/c.w;
				for(int j=beg; j<end; ++j) mColWs[j] += add;			
			}
		}

		if(c.h != 1){
			int beg = c.y;
			int end = c.y + c.h;
			space_t cur = sumSpan(&mRowHs[0], end, beg) + (c.h-1)*mPad2;
			
			if(v.h > cur){
				space_t add = (v.h - cur)/c.h;
				for(int j=beg; j<end; ++j) mRowHs[j] += add;
			}
		}
	}


	// We need to compute the actual number of rows in the table here, because
	// mSize2 may actually be larger...
	// search for first non-zero row height from back
	int ny=mSize2-1;
	for(; ny>=0; --ny){
		if(mRowHs[ny] != 0) break;
	}
	++ny;
	//printf("%d %d\n", ny+1, mSize2);
	//mSize2 = ny+1;	// this causes a crash next time arrange is called???

	space_t accW = sumSpan(&mColWs[0], mSize1) + mPad1*(mSize1+1);
	//space_t accH = sumSpan(mRowHs, mSize2) + mPad2*(mSize2+1);
	space_t accH = sumSpan(&mRowHs[0], ny) + mPad2*(ny+1);
	extent(accW, accH);


	// position child views	
	for(unsigned i=0; i<mCells.size(); ++i){

		Cell& c = mCells[i];
		if(0 == c.view) continue;
		View& v = *c.view;

		space_t pl,pt,pr,pb;
		getCellDim(i,pl,pt,pr,pb);
		space_t cx = (pr-pl)*0.5;
		space_t cy = (pb-pt)*0.5;

		// Fit in cell if view stretches. We subtract off dimensions exterior
		// to the cell, including padding.
		v.extent(v.w - (w-(pr-pl))*v.stretchX(), v.h - (h-(pb-pt))*v.stretchY());

		#define CS(c, p, x, y) case c: v.anchor(Place::p).pos(Place::p, x, y); break;
		switch(c.code){
		CS('x', CC, pl+cx,	pt+cy)
		CS('<', CL, pl,		pt+cy)
		CS('>', CR, pr,		pt+cy)
		CS('^', TC, pl+cx,	pt)
		CS('v', BC, pl+cx,	pb)
		CS('p', TL, pl,		pt)
		CS('q', TR, pr,		pt)
		CS('b', BL, pl,		pb)
		CS('d', BR, pr,		pb)
		default:;
		};
		#undef CS
	}
	return *this;
}


Table& Table::arrangement(const char * va){
	
	mAlign = va;
	mCells.clear();
	
	mSize1=0; mSize2=1;
	
	bool count1=true;
	
	// derive the number of rows and columns from the arrangement string
	const char * v = va;
	while(*v){
		
		char c = *v++;
		
		if(isAlignCode(c) || ('.' == c) || ('-' == c) || ('|' == c)){
			if(count1) ++mSize1;
		}
		else if(c==','){
			++mSize2;
			count1=false;
		}
	}
	
	//printf("%d %d\n", mSize1, mSize2);

	// compute and store geometry of table cells
	v = va;
	int ind=-1, indCell=-1;
	while(*v){
	
		if(isAlignCode(*v)){
			++ind;
			++indCell;
			int ix = ind % mSize1;
			int iy = (ind / mSize1) % mSize2;
			mCells.push_back(Cell(ix, iy, 1, 1, *v));
		}
		else if('.' == *v){
			++ind;
		}
		else if('-' == *v && indCell>=0){
			mCells[indCell].w++;
			++ind;
		}
		else if('|' == *v){
		
			++ind;
			int col = ind % mSize1;
		
			// find first cell matching column number and inc its y span
			for(unsigned i=0; i<mCells.size(); ++i){
				Cell& c = mCells[i];
				if(c.x == col && isAlignCode(c.code)) c.h++;
			}
		}
		
		++v;
	}
	
	return *this;
}


void Table::onDraw(GLV& g){
//	for(unsigned i=0; i<mColWs.size(); ++i) printf("%g ", mColWs[i]); printf("\n");
//	for(unsigned i=0; i<mRowHs.size(); ++i) printf("%g ", mRowHs[i]); printf("\n\n");

	using namespace glv::draw;
	if(enabled(DrawGrid)){
		color(colors().border);
		lineWidth(1);
		for(unsigned i=0; i<mCells.size(); ++i){
			space_t cl,ct,cr,cb;
			getCellDim(i, cl,ct,cr,cb);
			cl -= mPad1/2;
			cr += mPad1/2;
			ct -= mPad2/2;
			cb += mPad2/2;
			frame(cl,ct,cr,cb);
		}
	}
}



Scroll::Scroll(const Rect& r, float scrollBarWidth)
:	View(r),
	mSliderX(Rect(-scrollBarWidth, scrollBarWidth)), 
	mSliderY(Rect( scrollBarWidth,-scrollBarWidth)),
	mSliderXY(Rect(-scrollBarWidth,-scrollBarWidth, scrollBarWidth, scrollBarWidth)),
	mMode(HORIZONTAL | VERTICAL)
{
	enable(CropChildren);
	mSliderX.anchor(0,1).stretch(1,0).pos(Place::BL);
	mSliderY.anchor(1,0).stretch(0,1).pos(Place::TR);
	mSliderXY.anchor(1,1).pos(Place::BR);
	mSliderX.disable(FocusHighlight);
	mSliderY.disable(FocusHighlight);
	mSliderXY.disable(FocusHighlight);
	
	mSliderXY.knobSize(0);
	mSliderXY.knobSymbol(draw::circle<8>);
	mSliderXY.enable(Momentary);
	
	(*this) << mSliderX << mSliderY << mSliderXY;
}


void Scroll::onDraw(GLV& g){
	using namespace glv::draw;
	mSliderX.bringToFront();	// do not change order of these!
	mSliderY.bringToFront();
	mSliderXY.bringToFront();

	// hide scrollbars by default
	mSliderX.disable(Visible);
	mSliderY.disable(Visible);
	mSliderXY.disable(Visible);

	if(child == &mSliderX) return;

	Rect r = child->rect();

//		Rect r(0,0,0,0);
//		
//		{
//			View * c = child;
//			while(c){
//				if(c != &mSliderX && c != &mSliderY){
//					r.unionOf(*c, r);
//				}
//				c = c->sibling;
//			}
//		}
////		r.print();


	// slider units are in pixels

	float xpos = mSliderX.getValue(0);
	float ypos = mSliderY.getValue(1);
	child->pos(-xpos, ypos);
	mSliderX.interval(0, r.width());
	mSliderY.interval(0,-r.height()); // use negative range so 0 is at top

	if(r.width() > width()){
		if(mMode & HORIZONTAL){
			mSliderX.enable(Visible);
			mSliderXY.enable(Visible);
		}
		// subtracting y slider width to fit content
		float sr = width() - mSliderY.width();
		mSliderX.endpoints(xpos, xpos+sr);
		mSliderX.jump(sr/(mSliderX.max()-mSliderX.min()));
	}

	if(r.height() > height()){
		if(mMode & VERTICAL){
			mSliderY.enable(Visible);
			mSliderXY.enable(Visible);
		}
		// subtracting x slider height to fit content
		float sr = height() - mSliderX.height();
		mSliderY.endpoints(ypos, ypos-sr);
		mSliderY.jump(sr/(mSliderY.max()-mSliderY.min()));
//		printf("%g %g\n", mSliderY.getValue(0), mSliderY.getValue(1));
	}
	
	if(mMode & ALWAYS){
		if(mMode & HORIZONTAL) mSliderX.enable(Visible);
		if(mMode & VERTICAL  ) mSliderY.enable(Visible);
	}
}

bool Scroll::onEvent(Event::t e, GLV& g){
		
	const Keyboard& k = g.keyboard();
//	const Mouse& m = g.mouse();
	
	switch(e){
	case Event::KeyDown:
		switch(k.key()){
		case Key::PageDown:	pageY(-1); return false;
		case Key::PageUp:	pageY( 1); return false;
		default:;
		}
		break;
	default:;
	}
	return true;
}


} // glv::

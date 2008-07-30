/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_layout.h"

namespace glv{


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

Placer& Placer::abs(space_t vx, space_t vy){ ax=vx; ay=vy; return *this; }

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

} // glv::

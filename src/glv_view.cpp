/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

#define VIEW_INIT\
	Notifier(),\
	parent(0), child(0), sibling(0), \
	draw(cb),\
	mFlags(Visible | DrawBack | DrawBorder | CropSelf | FocusHighlight | HitTest | Controllable), \
	mStyle(&(Style::standard())), mAnchorX(0), mAnchorY(0), mStretchX(0), mStretchY(0)

View::View(space_t left, space_t top, space_t width, space_t height, drawCallback cb)
:	Rect(left, top, width, height), VIEW_INIT
{}

View::View(const Rect& rect, Place::t anch, drawCallback cb)
:	Rect(rect), VIEW_INIT
{
	anchor(anch);
}

#undef VIEW_INIT


View::~View(){

	// If you get a double-free warning, it's probably because you set a
	// pointer to the address of statically allocated data.
	mStyle->smartDelete();
	
	// remove myself from the view hierarchy
	if (parent) remove();
	
	// TODO: make sure this isn't causing a space leak
	if (child) {
		// delete children
		while (child->sibling) 
		{
			// note that delete View also calls remove() automatically
			delete child->sibling;
		}
		
		delete child;
	}
}

// Add a child view to myself
void View::add(View& newchild)
{

	View * op = newchild.parent;
	
	// This is problematic when the child does not have a parent
	newchild.reanchor(op ? w-op->w : w, op ? h-op->h : h);
	//if(op) newchild.reanchor(w - op->w, h - op->h);

	newchild.parent = this;
	newchild.sibling = 0;
	
	//newchild.constrainWithinParent();	// keep within the bounds of the parent's rect
	
	if (child == 0) // I didn't have any children until now
	{
		child = &newchild;
	} else {
		// I have children already... so go to the end and add there
		// default behaviour is to add at the end of the list, to be drawn last
		View * lastChild = child;
		while (lastChild->sibling != 0) lastChild = lastChild->sibling;
		lastChild->sibling = &newchild;
	}
}

void View::remove()
{
	// note that this doesn't delete the view, it just removes it from the hierarchy
	if (parent && parent->child)	// sanity check: don't try to remove a window or an unattached view
	{
		if (parent->child == this)
		{
			// I'm my parent's first child 
			// - remove my reference, but keep the sibling list healthy
			parent->child = sibling;
			
		} else {
			// I must be one of parent->child's siblings
			View * temp = parent->child;
			while (temp->sibling)
			{
				if (temp->sibling == this) {
					// I'm temp's sibling
					// - remove my reference, keep the sibling list healthy
					temp->sibling = this->sibling; 
					break; 
				}
				temp = temp->sibling;
			}
		}
	}
	
	// keep a reference to this 'lost' view in the removedViews list
//	View::removedViews.push_back(this);
}


void View::makeLastSibling(){
	if(parent && sibling){
		View * p = parent;
		remove();
		p->add(*this);
	}
}


// Compute translation from this View to target's parent
// We do this by traversing the tree upwards and accumulating positions.
bool View::absToRel(View * v, space_t & x, space_t & y) const{

	v = v ? v->parent : 0;	// go up one View
	space_t dx = 0, dy = 0;

	while(v){
		if(v == this){	// found target's top parent
			x -= dx; y -= dy;
			return true;			
		}
		dx += v->l; dy += v->t;	// accum relative offsets
		v = v->parent;
	}
	return false;
}


View& View::anchor(space_t mx, space_t my){
	mAnchorX = mx; mAnchorY = my; return *this;
}


View& View::anchor(Place::t p){
	#define CS(p,x,y) case Place::p: anchor(x,y); break;
	switch(p){
	CS(TL, 0, 0  ) CS(TC, 0.5, 0  ) CS(TR, 1, 0)
	CS(CL, 0, 0.5) CS(CC, 0.5, 0.5) CS(CR, 1, 0.5)
	CS(BL, 0, 1  ) CS(BC, 0.5, 1  ) CS(BR, 1, 1)
	}
	#undef CS
	return *this;
}


void View::appendCallback(Event::t e, eventCallback cb){
	callbackLists[e].push_back(cb);
}


void View::cloneStyle(){
	if(mStyle){		
		style(new Style(*mStyle));
		mStyle->deletable(true);
	}
	else{
		mStyle = new Style(true);
	}
}


StyleColor& View::colors() const { return style().color; }


void View::constrainWithinParent(){
	if (parent) {
		// LJP: maybe this is too restrictive for top level children?
//		l = l < 0 ? 0 : l > (parent->w - w) ? (parent->w - w) : l;
//		t = t < 0 ? 0 : t > (parent->h - h) ? (parent->h - h) : t;

		space_t d = w < 20 ? w : 20;	// allow this many pixels to remain on a side
		l = l < d - w ? d - w : l > parent->w - d ? parent->w - d : l;
		
		d = h < 20 ? h : 20;
		t = t < d - h ? d - h : t > parent->h - d ? parent->h - d : t;
	}
}


void View::drawBack() const{
	using namespace glv::draw;
	
	if(enabled(DrawBack)){
		color(colors().back);
		rect(0, 0, pix(w), pix(h));
	}
}


void View::drawBorder() const{
	using namespace glv::draw;
	
	if(enabled(DrawBorder)){
		float borderWidth = 1.0;
		//draw::color((mFocused && focusHighlight) ? colors().fore : colors().border);
		
		if(enabled(Focused) && enabled(FocusHighlight)){
			HSV hsv(colors().border);
			hsv.v > 0.5 ? hsv.v -= 0.2 : hsv.v += 0.2;
			color(Color(hsv));
		}
		else{
			color(colors().border);
		}
		lineWidth(borderWidth);
		frame(0, 0, pix(w), pix(h));
	}
}


// This returns the last child and sibling View containing point.
View * View::findTarget(space_t &x, space_t &y){

	// start search from current focus (if it contains x,y) or top level window
	View * n = this;		// node for walking tree
	View * target = this;	// our hit
	
	space_t rx = x, ry = y;
	
	while(n->child){
	
		// target may be the child or one of its siblings
		View * sib = n->child;
		View * match = 0;
		
		// Iterate through siblings
		while(sib){
			if(sib->containsPoint(x,y) && sib->visible()) match = sib;
			sib = sib->sibling;
		}
		
		// we found a sibling target; update the relative x & y & run the main while() again
		if(match){
			n = match;
			rx -= n->l; ry -= n->t;	// compute relative coords
			
			if(match->enabled(HitTest)){
				target = match;
				x = rx; y = ry;	// relativize x&y to new context
			}
		} 
		else{
			// if we reached the end of the sibling chain, end the target search
			break; 
		} 
	}

	return target;
}


void View::focused(bool b){
	property(Focused, b);
	if(b) makeLastSibling(); // move to end of chain, so drawn last
	notify();
}


void View::move(space_t x, space_t y){
	posAdd(x, y);
	constrainWithinParent();
}


int View::numEventCallbacks(Event::t e) const{	
	const std::map<Event::t, eventCallbackList>::const_iterator it = callbackLists.find(e);
	return it != callbackLists.end() ? it->second.size() : 0;
}


void View::on(Event::t e, eventCallback cb){	
	if(!callbackLists[e].empty())
		callbackLists[e].pop_front();
	
	callbackLists[e].push_front(cb);
}


void View::onDraw(){ if(draw) draw(this); }

bool View::onEvent(Event::t e, GLV& glv){ return false; }


void View::onResize(space_t dx, space_t dy){

	// move anchored children
	View * v = child;	
	while(v){
		v->reanchor(dx, dy);
		v = v->sibling;
	}
}


View& View::pos(Place::t p, space_t x, space_t y){
	using namespace glv::Place;
	
	switch(p){
		case TL: Rect::pos(x	, y    ); break;
		case TC: Rect::pos(x-w/2, y    ); break;
		case TR: Rect::pos(x-w	, y    ); break;
		case CL: Rect::pos(x	, y-h/2); break;
		case CC: Rect::pos(x-w/2, y-h/2); break;
		case CR: Rect::pos(x-w	, y-h/2); break;
		case BL: Rect::pos(x	, y-h  ); break;
		case BC: Rect::pos(x-w/2, y-h  ); break;
		case BR: Rect::pos(x-w	, y-h  ); break;
	}
	
	return *this;
}


void View::printFlags() const{
	//printf("\n%p: (%x)\n", this, mFlags);
	#define DO(flag) printf("%d: %s\n", enabled(flag) != 0, #flag);
	DO(Visible) DO(DrawBack) DO(DrawBorder) DO(CropChildren)
	DO(CropSelf) DO(FocusHighlight) DO(HitTest)
	#undef DO
}
	

void View::reanchor(space_t dx, space_t dy){
	posAdd(dx * mAnchorX, dy * mAnchorY);
	extent(w + dx * mStretchX, h + dy * mStretchY);
}


View& View::stretch(space_t mx, space_t my){ mStretchX=mx; mStretchY=my; return *this; }


void View::style(Style * style){
	mStyle->smartDelete();
	mStyle = style;
}


} // glv::

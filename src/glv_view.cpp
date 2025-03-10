/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <algorithm>
#include <ctype.h>		// isalnum
#include "glv_core.h"

namespace glv{

#define VIEW_INIT\
	Notifier(), SmartObject<View>(),\
	parent(0), child(0), sibling(0), \
	mFlags(Visible | DrawBack | DrawBorder | CropSelf | FocusHighlight | FocusToTop | HitTest | Controllable | Animate), \
	mStyle(&(Style::standard())), mAnchorX(0), mAnchorY(0), mStretchX(0), mStretchY(0)

View::View(const Rect& rect, Place::t anch)
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
	remove();
	
	// delete children
	if(child){
	
		// remove all child's siblings first
		while(child->sibling){
			// note that delete View also calls remove() automatically
			//delete child->sibling;
			// If View was allocated with 'new', then delete...
			if(child->sibling->dynamicAlloc()) delete child->sibling;
			// ... otherwise just remove from hierarchy. Compiler frees memory...
			else child->sibling->remove();
			
		}
		
		// then remove the child
		//delete child;
		if(child->dynamicAlloc()) delete child;
		else child->remove();
	}
}


View& View::add(View& newChild){

	View * op = newChild.parent; // old parent
	
	// TODO: This is problematic when the child does not already have a parent
	newChild.reanchor(op ? w-op->w : w, op ? h-op->h : h);
	//if(op) newchild.reanchor(w - op->w, h - op->h);

	// remove from previous network
	newChild.remove();
	
	// add to new network
	newChild.parent = this;
	
	//newChild->constrainWithinParent();	// keep within the bounds of the parent's rect
	
	if(!child){ // I didn't have any children until now
		child = &newChild;
	}
	else{
		// I have children already... so go to the end and add there
		// default behaviour is to add at the end of the list, to be drawn last
		View * lastChild = child;
		while(lastChild->sibling) lastChild = lastChild->sibling;
		lastChild->sibling = &newChild;
	}
	return *this;
}


View& View::add(View * newChild){
	if(newChild){	// valid address?
		add(*newChild);
	}
	return *this;
}


void View::remove(){
	// TODO: what to do if a node has a sibling, but no parent?

	// note that this doesn't delete the view, it just removes it from the hierarchy
	if(parent && parent->child){	// sanity check: don't try to remove a window or an unattached view

		// re-patch parent's child?
		if(parent->child == this){
			// I'm my parent's first child 
			// - remove my reference, but keep the sibling list healthy
			parent->child = sibling;
		}
		
		// re-patch the sibling chain?
		else{
			// I must be one of parent->child's siblings
			View * temp = parent->child;
			while(temp->sibling){
				if(temp->sibling == this) {
					// I'm temp's sibling
					// - remove my reference, keep the sibling list healthy
					temp->sibling = this->sibling; 
					break; 
				}
				temp = temp->sibling;
			}
		}
		
		parent=0; sibling=0; // no more parent or sibling, but child is still valid
	}

	// keep a reference to this 'lost' view in the removedViews list
//	View::removedViews.push_back(this);
}


void View::makeLastSibling(){
	if(parent && sibling){
//		View * p = parent;
//		remove();
//		p->add(this);
		parent->add(this);
	}
}


void View::getChildren(std::vector<View*>& children, TraversalAction& predicate){
	children.clear();
	View * c = child;
	while(c){
		if(predicate(c, 1)) children.push_back(c);
		c = c->sibling;
	}
}

void View::getDescendents(std::vector<View*>& descendents, TraversalAction& predicate){
	descendents.clear();
	
	struct Action : TraversalAction {
		std::vector<View*>& desc;
		TraversalAction& pred;
		
		Action(std::vector<View*>& d, TraversalAction& p): desc(d), pred(p){}
		bool operator()(View * v, int depth) override {
			if(pred(v,depth)) desc.push_back(v);
			return true;
		}
	} action(descendents, predicate);
	traverseDepth(action);
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

//void View::addCallback(Event::t e, eventCallback cb){
//	if(!hasCallback(e, cb)){
//		callbackLists[e].push_back(cb);
//	}
//}
//
//bool View::hasCallback(Event::t e, eventCallback cb) const {
//	if(hasCallbacks(e)){
//		const eventCallbackList& l = callbackLists.find(e)->second;
//		return find(l.begin(), l.end(), cb) != l.end();
//	}
//	return false;
//}
//
//
//bool View::hasCallbacks(Event::t e) const {
//	return 0!=callbackLists.count(e);
//}
//
//int View::numCallbacks(Event::t e) const{	
//	const std::map<Event::t, eventCallbackList>::const_iterator it = callbackLists.find(e);
//	return it != callbackLists.end() ? it->second.size() : 0;
//}
//
//void View::on(Event::t e, eventCallback cb){
//	if(hasCallbacks(e) && !callbackLists[e].empty())
//		callbackLists[e].pop_front();
//	
//	callbackLists[e].push_front(cb);
//}
//
//void View::removeCallback(Event::t e, eventCallback cb){
//	if(hasCallbacks(e)){
//		std::list<eventCallback>::iterator it;
//		for(it = callbackLists[e].begin(); it != callbackLists[e].end(); ){
//			if(*it == cb)	it = callbackLists[e].erase(it);
//			else			it++;
//		}
//	}
//}
//
//void View::removeAllCallbacks(Event::t e){
//	if(hasCallbacks(e)){
//		while(!callbackLists[e].empty())
//			callbackLists[e].pop_front();
//	}
//}


View& View::addHandler(DrawHandler& v){
	mDrawHandlers.push_back(&v);
	return *this;
}

View& View::addHandler(Event::t e, EventHandler& h){
	mEventHandlersMap(); // create map, if not already created
	if(!hasEventHandler(e, h)){
		mEventHandlersMap()[e].push_back(&h);
	}
	return *this;
}

void View::removeHandler(DrawHandler& v){
	DrawHandlers::iterator it = mDrawHandlers.begin();
	while(it != mDrawHandlers.end()){
		if(*it == &v) mDrawHandlers.erase(it++);
		else ++it;
	}
}

void View::removeHandler(Event::t e, EventHandler& h){
	if(hasEventHandlers(e)){
		EventHandlers& hs = mEventHandlersMap()[e];
		EventHandlers::iterator it = hs.begin();
		while(it != hs.end()){
			if(*it == &h) hs.erase(it++);
			else ++it;
		}
	}
}

bool View::hasEventHandler(Event::t e, const EventHandler& h) const {
	if(hasEventHandlers(e)){
		const EventHandlers& hs = mEventHandlersMap().find(e)->second;
		return find(hs.begin(), hs.end(), &h) != hs.end();
	}
	return false;
}

bool View::hasEventHandlers(Event::t e) const {
	return mEventHandlersMap.created() && 0!=mEventHandlersMap().count(e);
}

int View::numEventHandlers(Event::t e) const {
	if(hasEventHandlers(e)){
		const EventHandlersMap& handlers = mEventHandlersMap();
		const EventHandlersMap::const_iterator it = handlers.find(e);
		return it != handlers.end() ? it->second.size() : 0;
	}
	return 0;
}


void View::addModels(ModelManager& mm){
	struct Add : TraversalAction{
		Add(ModelManager& v): m(v){}
		bool operator()(View * v, int depth) override {
			if(v->hasName()) m.add(v->name(), *v);
			return true;
		}
		ModelManager& m;	
	} add(mm);

	traverseDepth(add);
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


View& View::bringToFront(){ makeLastSibling(); return *this; }


View& View::cloneStyle(){
	if(mStyle){		
		style(new Style(*mStyle));
		mStyle->deletable(true);
	}
	else{
		mStyle = new Style(true);
	}
	return *this;
}


StyleColor& View::colors() const { return style().color; }


void View::constrainWithinParent(){
	if(parent){
		// LJP: maybe this is too restrictive for top level children?
//		l = l < 0 ? 0 : l > (parent->w - w) ? (parent->w - w) : l;
//		t = t < 0 ? 0 : t > (parent->h - h) ? (parent->h - h) : t;

		space_t d = w < 20 ? w : 20;	// allow this many pixels to remain on a side
		l = l < d - w ? d - w : l > parent->w - d ? parent->w - d : l;
		
		d = h < 20 ? h : 20;
		t = t < d - h ? d - h : t > parent->h - d ? parent->h - d : t;
	}
	rectifyGeometry();
}


void View::doDraw(GLV& g){
	using namespace glv::draw;

	if(enabled(DrawBack)){
		color(colors().back);
		rectangle(0,0, w,h);
		/*float verts[4*2] = { 0,0, 0,h, w,0, w,h };
		Color& colbg = colors().back;
		//Color& colfg = colors().fore;
		Color& colbd = colors().border;
		Color colsh = colbg*0.9 + colbd*0.1;
		Color cols[4] = { colbg, colsh, colbg, colsh };
		paint(TriangleStrip, (Point2 *)verts, cols, 4);*/
	}

	bool drawNext = true;
	DrawHandlers::iterator it = mDrawHandlers.begin();
	while(it != mDrawHandlers.end()){
		DrawHandlers::iterator itnext = ++it; --it;
		g.graphicsData().reset();
		drawNext = (*it)->onDraw(*this, g);
		if(!drawNext) break;
		it = itnext;
	}

	if(drawNext){
		g.graphicsData().reset();
		onDraw(g);
	}

	if(enabled(DrawBorder)){
		float borderWidth = 1.0;

		// double border thickness if focused
		if(enabled(Focused) && enabled(FocusHighlight)){
			borderWidth *= 2;
		}

		lineWidth(borderWidth);
		color(colors().border);
		const float ds = 0.5; // OpenGL suggests 0.375, but smears with AA enabled
		frame(ds, ds, pix(w)-ds, pix(h)-ds);
		//frame(0,0, pix(w)-ds, pix(h)-ds); // hack to give bevelled look
		
		/*
		// This uses slightly different border colors to give bevelled look
		color(colors().border);
		shape(draw::LineStrip, 0.5f,h-0.5f, w-0.5f,h-0.5f, w-0.5f,0.5f);
		color(colors().border.mixRGB(colors().back, 0.5));
		shape(draw::LineStrip, w-0.5f,0.5f, 0.5f,0.5f, 0.5f,h-0.5f);
		*/
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
		
		space_t cx = rx, cy = ry;
		
		// Iterate through siblings
		while(sib){
			if(sib->visible() && sib->containsPoint(x,y)){
				match = sib;
			}
//			if(sib->visible()){
//				if(!sib->enabled(CropChildren) && sib->child){
//					View * v = sib->child;
//					
//					do {
//						space_t ax = rx - sib->l, ay = ry - sib->t;
//						if(v != v->findTarget(ax, ay)){
//							match = v;
//							cx = ax + match->l;
//							cy = ay + match->t;
//							match->print();
//							//printf("%g %g\n", cx, cy);
//						}
//						//printf("%g %g\n", ax, ay);
//
//						v = v->sibling;
//					} while(v);
//				}
//				else if(sib->containsPoint(x,y)){
//					match = sib;
//					cx = rx; cy = ry;
//				}
//			}
			sib = sib->sibling;
		}
		
		// we found a sibling target; update the relative x & y & run the main while() again
		if(match){
			n = match;
			rx = cx - n->l; ry = cy - n->t;	// compute relative coords
			
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


void View::fit(bool moveChildren){
	if(child){			
		Rect r = unionOfChildren();
		//printf("%s: ", name().c_str()); r.print(); print();
		
		if(moveChildren){
			View * v = child;
			while(v){
				v->l -= r.l;
				v->t -= r.t;
				v = v->sibling;
			}
			extent(r.width(), r.height());
		}
		else{
			set(r);
		}
	}
}


void View::focused(bool b){
//	property(Focused, b);
	if(b && enabled(FocusToTop)){
		// move all nodes in branch to end of sibling chain so drawn last
		View * v = this;
		do{
			v->bringToFront();
			v = v->parent;
		} while(v && v->parent);
	}
	notify(this, Update::Focus);
}


Font& View::font(){
	return mFont();
}


View& View::maximize(){
	if(!enabled(Maximized)){
		enable(Maximized);
		mRestoreRect().set(*this);
		reanchor(0,0);
	}
	return *this;
}


//void View::modelToString(std::string& v, const std::string& modelName) const {
//
//	ModelManager mm;
//
//	struct Add : ConstTraversalAction{
//		Add(ModelManager& v): m(v){}
//		bool operator()(const View * v, int depth) override {
//			if(v->hasName()) m.add(v->name(), *v);
//			return true;
//		}
//		ModelManager& m;	
//	} add(mm);
//
//	traverseDepth(add);
//	mm.stateToToken(v, modelName);
//}
//
//
//int View::modelFromString(const std::string& v){
//
//	ModelManager mm;
//
//	struct Add : TraversalAction{
//		Add(ModelManager& v): m(v){}
//		bool operator()(View * v, int depth) override {
//			if(v->hasName()) m.add(v->name(), *v);
//			return true;
//		}
//		ModelManager& m;	
//	} add(mm);
//
//	traverseDepth(add);
//	return mm.stateFromToken(v);
//}



void View::move(space_t x, space_t y){
	translate(x, y);
	constrainWithinParent();
}


View& View::name(const std::string& v){
//printf("View::name(%s)\n", v.c_str());
//	if(isalpha(v[0]) || '_' == v[0]){
//		unsigned i=1;
//		for(; i<v.size(); ++i){
//			if(!(isalnum(v[i]) || '_' == v[i])) break;
//		}
//		if(v.size()==i) mName=v;
//	}

	if(glv::isIdentifier(v)) mName=v;
	return *this;
}


void View::onResizeRect(space_t dx, space_t dy){
	onResize(dx,dy);
	// Move/resize anchored children
	// This will recursively call onResize's through the entire descendency tree
	View * v = child;	
	while(v){
		v->reanchor(dx, dy);
		v = v->sibling;
	}
}


View& View::pos(Place::t p){ return pos(p,0,0); }

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
		default:;
	}
	
	return *this;
}


const View * View::posAbs(space_t& al, space_t& at) const{
	al=0; at=0;
	return toAbs(al, at);
}


void View::debug() const {
	struct Action : ConstTraversalAction{
		bool operator()(const View * v, int depth) const override {
			std::string msg;
			if(v->width() == 0 || v->height() == 0){
				msg += "\tWidth or height is zero. View will not be visible.\n";
			}
			if(v->width() < 0 || v->height() < 0){
				msg += "\tWidth or height is negative.\n";
			}
			/* This tends to print out too much
			if(v->parent && (!v->intersects(*v->parent))){
				msg += "\tRect does not intersect with parent.\n";
			}//*/
			msg += v->onDebug();
			if(!msg.empty()){
				fprintf(stderr, "%s%s(%p):\n%s\n",
					v->className(),
					v->name().empty() ? " " : (" \"" + v->name() + "\" ").c_str(),
					v,
					msg.c_str()
				);
			}
			return true;
		}
	} action;

	traverseDepth(action);
}

void View::printDescendents() const {

	struct A : ConstTraversalAction{
		bool operator()(const View * v, int depth) const override {
			for(int i=0; i<depth; ++i) printf("|   ");
			const std::string& nm = v->name();
			printf("%s %p %s\n", v->className(), v, (nm[0] ? "\"" + nm + "\"" : nm).c_str());
			return true;
		}	
	};
	
	traverseDepth(A());
}

void View::printFlags() const{
	//printf("\n%p: (%x)\n", this, mFlags);
	#define DO(flag) printf("%d: %s\n", enabled(flag) != 0, #flag);
	DO(Visible) DO(DrawBack) DO(DrawBorder) DO(CropChildren)
	DO(CropSelf) DO(FocusHighlight) DO(HitTest)
	#undef DO
}
	

void View::reanchor(space_t dx, space_t dy){
	
	if(!enabled(Maximized)){
		translate(dx * mAnchorX, dy * mAnchorY);
		extent(w + dx * mStretchX, h + dy * mStretchY);
//		//printf("%s (%p): % g % g d(% g, % g) s(% g, % g)\n", className(), this, w,h, dx,dy, mStretchX, mStretchY);
	}
	else{
		Rect& RR = mRestoreRect();
		RR.translate(dx * mAnchorX, dy * mAnchorY);
		RR.extent(RR.w + dx * mStretchX, RR.h + dy * mStretchY);
		if(parent) set(0,0, parent->w, parent->h);
	}
}


void View::rectifyGeometry(){
	fixNegativeExtent();

	if(enabled(KeepWithinParent) && parent){
		space_t maxw = parent->width();
		space_t maxh = parent->height();
		
		extent(w>maxw?maxw:w, h>maxh?maxh:h);
	
		if(left() < 0) left(0);
		if(top() < 0) top(0);
		if(right() > maxw) right(maxw);
		if(bottom() > maxh) bottom(maxh);
	}
}


View& View::restore(){
	if(enabled(Maximized)){
		disable(Maximized);
		set(mRestoreRect());
	}
	return *this;
}


bool View::showing() const {
	const View * v = this;
	do{
		if(!v->visible()) return false;
		v = v->parent;
	} while(v);
	
	return true;
}


View& View::stretch(space_t mx, space_t my){ mStretchX=mx; mStretchY=my; return *this; }


View& View::style(Style * style){
	mStyle->smartDelete();
	mStyle = style;
	return *this;
}


const View * View::toAbs(space_t& x, space_t& y) const {
	const View * v = this;
 	while(v->parent){
		x += v->l;
		y += v->t;
		v = v->parent;
 	}
 	return v;
}

View& View::root(){
	View * v = this;
	while(v->parent) v = v->parent;
	return *v;
}


#define TRAVERSE_DEPTH(Qual, qual)\
void View::traverseDepth(qual Qual##TraversalAction& action) qual {\
	qual View * const root = this;\
	qual View * n = root;\
	int depth = 0;\
	if(!n->child){				/* Early exit if leaf */\
		action(n, depth);\
		return;\
	}\
	while(n){\
		action(n, depth);\
		if(n->child){			/* Down to child */\
			++depth;\
			n = n->child;\
		}\
		else if(n->sibling){	/* Across to sibling */\
			n = n->sibling;\
		}\
		else{					/* Up and over to next branch */\
			if(n == root || !n->parent) return; /* Triggers if only node in tree */\
			while(n->parent){\
				--depth;\
				n = n->parent;\
				if(n == root) return;\
				else if(n->sibling){ n = n->sibling; break; }\
			}\
		}\
	}\
}
TRAVERSE_DEPTH(,)
TRAVERSE_DEPTH(Const, const)


Rect View::unionOfChildren() const{
	View * c = child;
	if(c){			
		Rect r(*c);
		
		while(c->sibling){
			c = c->sibling;
			r.unionOf(*c, r);
		}
		return r;
	}
	else{
		return Rect(0,0,0,0);
	}	
}


Rect View::visibleRegion() const {

	const View * v = this;
	Rect r(0,0,w,h);
	space_t lr=0, tr=0; // relative coords as we go towards root
	
	while(true){
		if(v->enabled(Visible)){

			lr -= v->left();
			tr -= v->top();

			if(v->parent){

				v = v->parent;
				
				if(v->enabled(CropChildren) || !v->parent){
					r.intersection(Rect(lr, tr, v->w, v->h), r);
					if(r.w <= 0 || r.h <= 0){
						return Rect(0);
					}
				}
			}
			else{
				break;
			}
		}
		else{
			return Rect(0);
		}
	}
	return r;
}

} // glv::

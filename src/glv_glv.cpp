/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

GLV::GLV(drawCallback cb, space_t width, space_t height)
:	View(0, 0, width, height, cb), mFocusedView(this)
{
	disable(DrawBorder | FocusHighlight);
	cloneStyle();
}



void GLV::broadcastEvent(Event::t e){ 

	View * const root = this;
	View * cv = root;
	
	doEventCallbacks(*cv, e);
	
	while(true){
		if(cv->child) cv = cv->child;
		
		else if(cv->sibling) cv = cv->sibling;
		
		// retrace upwards until a parent's sibling is found
		else{
			while(cv != root && cv->sibling == 0) cv = cv->parent;
			
			if(cv->sibling) cv = cv->sibling;

			else break; // break the loop when the traversal returns to the root
		}
		
		doEventCallbacks(*cv, e);
	}
}


// Since we possibly have multiple event callbacks, we will bubble the event if any
// one of them has bubbling set to true.
bool GLV::doEventCallbacks(View& v, Event::t e){

	if(!v.enabled(Controllable)) return false;

	bool bubble = v.onEvent(e, *this);					// Execute virtual callback

	const eventCallbackList& cbl = v.callbackLists[e];
	
	// Execute callbacks in list
	for(eventCallbackList::const_iterator it = cbl.begin(); it != cbl.end(); it++){
		if(*it) bubble |= (*it)(&v, *this);
	}
	
	return bubble;
}

void GLV::doFocusCallback(bool get){

	Event::t e = get ? Event::GetFocus : Event::LoseFocus;

	if(mFocusedView){
		mFocusedView->focused(get);
		
		if(mFocusedView->numEventCallbacks(e) ){
			eventType(e);
			doEventCallbacks(*mFocusedView, e);
		}
	}
}

void GLV::drawGLV(unsigned int w, unsigned int h){
	preamble(w, h);
	drawWidgets(w, h);
}


static void drawContext(float tx, float ty, View * v, float& cx, float& cy, View *& c){
	using namespace glv::draw;

	cx += tx; cy += ty;	// update absolute coordinates of drawing context

	identity();								// clear model matrix (assumed set already)

	// these weird calls are necessary so that we draw on the center of pixels
	// rather than on the boundaries
	translate(pix(cx) + 0.5, pix(cy) + 0.5);	// offset to center of top-left pixel
	//scale(v->w/(v->w+1), v->h/(v->h+1));	// scale to lose 1 pixel on each edge

	c = v;
}

static void computeCrop(std::vector<Rect>& cr, int lvl, space_t ax, space_t ay, View * v){
	if(v->enabled(CropChildren)){
		cr[lvl].set(ax, ay, v->w, v->h);	// set absolute rect
		cr[lvl].intersection(cr[lvl-1], cr[lvl]);
	}
}

// Views are drawn depth-first from leftmost to rightmost sibling
void GLV::drawWidgets(unsigned int w, unsigned int h){
	using namespace draw;

	float cx = 0, cy = 0; // drawing context absolute position
	View * const root = this;
	View * cv = root;

	// The crop region is the intersection of all parent rects up to the top 
	// view. The intersections also need to be done in absolute coordinates.	
	std::vector<Rect> cropRects(16, Rect(w, h));	// index is hierarchy level
	int lvl = 0;	// start at root = 0
	
	push2D(w, h);	// initialise the OpenGL renderer for our 2D GUI world
	onDraw();		// draw myself
	push();			// push model matrix because of transformations in drawContext()
	
	draw::enable(ScissorTest);
	
	while(true){
		// find the next view to draw
		
		// go to child node if exists and I'm drawable
		if(cv->child && cv->visible()){
			drawContext(cv->child->l, cv->child->t, cv->child, cx, cy, cv);
			computeCrop(cropRects, ++lvl, cx, cy, cv);
		}
		
		// go to sibling node if exists
		else if(cv->sibling){
			drawContext(cv->sibling->l - cv->l, cv->sibling->t - cv->t, cv->sibling, cx, cy, cv);
			computeCrop(cropRects, lvl, cx, cy, cv);
		}
		
		// retrace upwards until a parent's sibling is found
		else{
			while(cv != root && cv->sibling == 0){
				drawContext(-cv->l, -cv->t, cv->parent, cx, cy, cv);
				lvl--;
			}
			
			if(cv->sibling){
				drawContext(cv->sibling->l - cv->l, cv->sibling->t - cv->t, cv->sibling, cx, cy, cv);
				computeCrop(cropRects, lvl, cx, cy, cv);
			}
			else break; // break the loop when the traversal returns to the root
		}
		
		
		// draw the current view
		if(cv->visible()){

			Rect r = cropRects[lvl-1];	// cropping region comes from parent context
			if(cv->enabled(CropSelf)) r.intersection(Rect(cx, cy, cv->w, cv->h), r); // crop my own draw?

			// LJP: using some weird numbers here, seems to work right though...
			//scissor(r.l, h - r.bottom() - 1, r.w+2, r.h+1);
			scissor(pix(r.l), pix(h - r.bottom() - 1.499), pix(r.w+1), pix(r.h+1.499));

			cv->drawBack();
			push(); cv->onDraw(); pop();		// push/pop model cuz user might forget to...
			cv->drawBorder();
		}
	}
	
	pop();
	
	// this weird call is necessary so that raster calls get scissored properly
	// not entirely sure why this works, but it does.
	scissor(0,0,(GLint)w,(GLint)h);

	pop2D();
}

void GLV::preamble(unsigned int w, unsigned int h){
	using namespace draw;
	glDrawBuffer(GL_BACK);
	clearColor(colors().back.r, colors().back.g, colors().back.b, colors().back.a);
	clear(ColorBufferBit | DepthBufferBit);	// TODO: this needs to be coordinated with the display settings
}

void GLV::propagateEvent(){ //printf("GLV::propagateEvent(): %s\n", Event::getName(eventtype));
	View * v = mFocusedView;
	Event::t e = eventType();
	while(v && doEventCallbacks(*v, e)) v = v->parent;
}

void GLV::setFocus(View * v){

	// save current event since we do not want to propagate GetFocus and LoseFocus
	Event::t currentEvent = eventType();	
											
	doFocusCallback(false);	// Call current focused View's LoseFocus callback
	mFocusedView = v;		// Set the currently focused View
	doFocusCallback(true);	// Call newly focused View's GetFocus callback

	eventType(currentEvent);
}

void GLV::setKeyDown(int keycode){
	eventType(Event::KeyDown);
	keyboard.mKeycode = keycode;
	keyboard.mIsDown = true;
}

void GLV::setKeyUp(int keycode){
	eventType(Event::KeyUp);
	keyboard.mKeycode = keycode;
	keyboard.mIsDown = false;
}

void GLV::setMouseDown(space_t& x, space_t& y, int button, int clicks){
	eventType(Event::MouseDown);	
	//if(button == Mouse::Left)
		setFocus(findTarget(x, y));
	mouse.updateButton(button, true, clicks);
}

void GLV::setMouseMove(space_t& x, space_t& y){
	eventType(Event::MouseMove);
	if(absToRel(mFocusedView, x, y)){
		x -= mFocusedView->l;
		y -= mFocusedView->t;
	}
}

void GLV::setMouseDrag(space_t& x, space_t& y){
	eventType(Event::MouseDrag);
	if(absToRel(mFocusedView, x, y)){
		x -= mFocusedView->l;
		y -= mFocusedView->t;
	}
}

// MouseDrag doesn't call setFocus because that messes up the dragging of a view.
// x, y coords are passed in as absolute (i.e. relative to top GLV View)
void GLV::setMouseDrag(space_t& x, space_t& y, int button, int clicks){
	eventType(Event::MouseDrag);
	
	// LJP: This operates on all children, rather than just the focused View.
	//findTarget(x, y);			//for setting relative coordinates
	
	if(absToRel(mFocusedView, x, y)){
		x -= mFocusedView->l;
		y -= mFocusedView->t;
	}
	
	// LJP: Why update button state during a drag event?
	mouse.updateButton(button, true, clicks);
}

void GLV::setMousePos(int x, int y, space_t relx, space_t rely){
	mouse.pos(x, y);
	mouse.posRel(relx, rely);
}

void GLV::setMouseUp(space_t& x, space_t& y, int button, int clicks){
	eventType(Event::MouseUp);
	mouse.updateButton(button, false, clicks);
}

void GLV::setMouseWheel(int wheelDelta){
	eventType(Event::MouseWheel);
	mouse.bufferPos(mouse.mW[0] + (space_t)wheelDelta, mouse.mW);
}


} // glv::

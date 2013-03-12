/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

GLV::GLV(space_t width, space_t height)
:	View(Rect(width, height)), mFocusedView(this)
{
	disable(DrawBorder | FocusHighlight);
//	cloneStyle();
	instances().push_back(this);
}

GLV::~GLV(){ //printf("~GLV\n");
	for(unsigned i=0; i<instances().size(); ++i){
		if(instances()[i] == this){
			instances().erase(instances().begin() + i);
		}
	}
}


void GLV::broadcastEvent(Event::t e){ 

	struct A : TraversalAction{
		GLV& glv; Event::t event;
		A(GLV& g, Event::t e): glv(g), event(e){}
		bool operator()(View * v, int depth){
			glv.doEventCallbacks(*v, event);
			return true;
		}
		bool operator()(const View * v, int depth){ return false; }
	} a(*this, e);
	
	traverseDepth(a);
}



// The bubbling return values from the virtual and function pointer callbacks
// are ANDed together.
bool GLV::doEventCallbacks(View& v, Event::t e){
//	printf("doEventCallbacks: %s %d\n", v.className(), e);

	// TODO: which is better?
//	if(!v.enabled(Controllable)) return false;	// cancels all events w/o handling
	if(!v.enabled(Controllable)) return true;	// bubbles all events w/o handling

//	bool bubble = v.onEvent(e, *this);					// Execute virtual callback
//	
//	if(bubble){
//		if(v.hasCallbacks(e)){
//			const eventCallbackList& cbl = v.callbackLists[e];
//			
//			// Execute callbacks in list
//			for(eventCallbackList::const_iterator it = cbl.begin(); it != cbl.end(); it++){
//				//if(*it) bubble |= (*it)(&v, *this);
//				if(*it){
//					bool r = (*it)(&v, *this);
//					bubble &= r;
//					if(!r) break;
//				}
//			}
//		}
//	}
//	
//	return bubble | v.enabled(AlwaysBubble);

//	bool bubble = true;
//
//	if(v.hasCallbacks(e)){
//		const eventCallbackList& cbl = v.callbackLists[e];
//		
//		// Execute callbacks in list
//		for(eventCallbackList::const_iterator it = cbl.begin(); it != cbl.end(); it++){
//			//if(*it) bubble |= (*it)(&v, *this);
//			if(*it){
//				bool r = (*it)(&v, *this);
//				bubble &= r;
//				if(!bubble) goto end;
//			}
//		}
//	}
//	
//	bubble &= v.onEvent(e, *this);
//	end:
//	return bubble || v.enabled(AlwaysBubble);

	bool bubble = true;

	if(v.hasEventHandlers(e)){
		const EventHandlers& hs = v.mEventHandlersMap()[e];

		// Execute callbacks in list
		for(EventHandlers::const_iterator it = hs.begin(); it != hs.end(); ++it){
			if(*it){
				bubble = (*it)->onEvent(v, *this);
				if(!bubble) break;
			}
		}
	}

	if(bubble) bubble = v.onEvent(e, *this);
	
	return bubble || v.enabled(AlwaysBubble);
}

void GLV::doFocusCallback(bool get){

	Event::t e = get ? Event::FocusGained : Event::FocusLost;

	if(mFocusedView){
		mFocusedView->focused(get);
		
//		if(mFocusedView->numEventHandlers(e)){
//			printf("GLV::doFocusCallback(%d) on a %s\n", get, mFocusedView->className());
//			eventType(e);
			doEventCallbacks(*mFocusedView, e);
//		}
	}
}

void GLV::drawGLV(unsigned ww, unsigned wh, double dsec){
#ifdef GLV_OPENGL_ES1
	// glDrawBuffer is not necessary according to opengles v1.1 spec
#else
	glDrawBuffer(GL_BACK);
#endif
	drawWidgets(ww, wh, dsec);
}


static void drawContext(float tx, float ty, View * v, float& cx, float& cy, View *& c){
	using namespace glv::draw;

	cx += tx; cy += ty;	// update absolute coordinates of drawing context

	identity();								// clear model matrix (assumed set already)

	// The offsets are necessary so that we draw on the center of pixels
	// rather than on the boundaries
//	translate(pixc(cx), pixc(cy));	// offset to center of top-left pixel
	translate(pix(cx), pix(cy));	// round position to nearest pixel coordinates
//	translate(cx, cy);

	c = v;
}

static void computeCrop(std::vector<Rect>& cr, int lvl, space_t ax, space_t ay, View * v){
	if(v->enabled(CropChildren)){
		cr[lvl].set(ax, ay, v->w, v->h);	// set absolute rect
		
		// get intersection with myself and previous level
		if(lvl>0){
			Rect r = cr[lvl];
			r.resizeEdgesBy(-1);	// shrink area to save the borders
			r.intersection(cr[lvl-1], cr[lvl]);
			//cr[lvl].intersection(cr[lvl-1], cr[lvl]);
		}
	}
	
	// if no child cropping, then inherit previous level's crop rect
	else{ cr[lvl] = cr[lvl-1]; }
}

// Views are drawn depth-first from leftmost to rightmost sibling
void GLV::drawWidgets(unsigned int ww, unsigned int wh, double dsec){
	using namespace draw;

	// TODO: Perhaps the View tree should be serialized into a separate list
	//		used for rendering?
	//		This will permit a user to change the graph structure during a draw 
	//		callback. Currently if this is attempted, we crash and burn.

	enter2D(ww, wh);		// initialise the OpenGL renderer for our 2D GUI world

	// Render all primitives at integer positions, ref: OpenGL Redbook
	// NOTE: This is a comprise to get almost pixel-perfection for both lines 
	// (half-integers) and polygons (integers). We'll do it "by hand" due to all
	// the exceptions and to get exact pixel-perfect accuracy.
//	translate(0.375f, 0.375f);

	float cx = 0, cy = 0; // drawing context absolute position
	View * const root = this;
	View * cv = root;

	// The crop region is the intersection of all parent rects up to the top 
	// view. The intersections also need to be done in absolute coordinates.	
	std::vector<Rect> cropRects(16, Rect(ww, wh));	// index is hierarchy level
	int lvl = 0;	// start at root = 0
	
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY); // note: enabling this messes up glColor, so leave it off
	//glColorPointer(4, GL_FLOAT, 0, 0);

	// Animate all the views
	struct AnimateViews : public TraversalAction{
		AnimateViews(double dt_): dt(dt_){}
		virtual bool operator()(View * v, int depth){
			if(v->enabled(Animate)) v->onAnimate(dt);
			return true;
		}
		double dt;
	} animateViews(dsec);
	traverseDepth(animateViews);

	graphicsData().reset();
	//if(enabled(Animate)) onAnimate(dsec);
	doDraw(*this);

	draw::enable(ScissorTest);

	while(true){

		cv->onDataModelSync();		// update state based on attached model variables
		cv->rectifyGeometry();

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
		
		// animate current view
		//if(cv->enabled(Animate)) cv->onAnimate(dsec);
		
		// draw current view
		if(cv->visible()){
			Rect r = cropRects[lvl-1];	// cropping region comes from parent context
			if(cv->enabled(CropSelf)) r.intersection(Rect(cx, cy, cv->w, cv->h), r); // crop my own draw?

			// bypass if drawing area outside of crop region
			if(r.h<=0.f || r.w <= 0.f) continue;

			int sx = pix(r.l);
			int sy = wh - (pix(r.t) + pix(r.h)) + 0.99;
			int sw = pix(r.w);
			int sh = r.h + 0.5;
//			if(sx < int(ww) && sy < int(wh)){	// LJP: this might be too paranoid
//				if(sx < 0) sx=0;
				if(sy < 0) sy=0;
//				if(sx+sw > int(ww)) sw = ww - sx;
//				if(sy+sh > int(wh)) sh = wh - sy;
				//printf("[%d %d] -> %d %d %d %d\n", ww,wh, sx,sy,sw,sh);
				scissor(sx, sy, sw, sh);

				graphicsData().reset();
				//if(cv->enabled(Animate)) cv->onAnimate(dsec);
				cv->doDraw(*this);
//			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	draw::disable(ScissorTest);
}

std::vector<GLV *>& GLV::instances(){
	static std::vector<GLV *> * sInstances = new std::vector<GLV *>;
	return *sInstances;
}

bool GLV::propagateEvent(){ //printf("GLV::propagateEvent(): %s\n", Event::getName(eventtype));
	View * v = mFocusedView;
	Event::t e = eventType();
	while(v && doEventCallbacks(*v, e)) v = v->parent;
	return v != 0;
}

void GLV::refreshModels(bool clearExistingModels){
	if(clearExistingModels) mMM.clearModels();
	addModels(mMM);
}

void GLV::setFocus(View * v){

	// save current event since we do not want to propagate GetFocus and LoseFocus
//	Event::t currentEvent = eventType();	

	// do nothing if already focused
	if(v == mFocusedView) return;

	// update states before calling event callbacks
	if(mFocusedView)	mFocusedView->disable(Focused);
	if(v)				v->enable(Focused);

	doFocusCallback(false);	// Call current focused View's LoseFocus callback
	mFocusedView = v;		// Set the currently focused View
	doFocusCallback(true);	// Call newly focused View's GetFocus callback

//	eventType(currentEvent);
}

void GLV::setKeyDown(int keycode){
	eventType(Event::KeyDown);
	mKeyboard.mKeycode = keycode;
	mKeyboard.mIsDown = true;
}

void GLV::setKeyUp(int keycode){
	eventType(Event::KeyUp);
	mKeyboard.mKeycode = keycode;
	mKeyboard.mIsDown = false;
}

void GLV::setKeyModifiers(bool shift, bool alt, bool ctrl, bool caps, bool meta){
	mKeyboard.shift(shift);
	mKeyboard.alt(alt);
	mKeyboard.ctrl(ctrl);
	mKeyboard.caps(caps);
	mKeyboard.meta(meta);
}

void GLV::setMouseDown(space_t& x, space_t& y, int button, int clicks){
	eventType(Event::MouseDown);
	if(!mMouse.isDownAny()) // only change focus if no other buttons down
		setFocus(findTarget(x, y));
	mMouse.posRel(x,y);
	mMouse.updateButton(button, true, clicks);
}

void GLV::setMouseUp(space_t& x, space_t& y, int button, int clicks){
	eventType(Event::MouseUp);
	if(absToRel(mFocusedView, x, y)){
		x -= mFocusedView->l;
		y -= mFocusedView->t;
	}
	mMouse.posRel(x,y);
	mMouse.updateButton(button, false, clicks);
}

void GLV::setMouseMotion(space_t& x, space_t& y, Event::t e){
	eventType(e);
	if(absToRel(mFocusedView, x, y)){
		x -= mFocusedView->l;
		y -= mFocusedView->t;
	}
}

void GLV::setMouseMove(space_t& x, space_t& y){
	setMouseMotion(x,y,Event::MouseMove);
}

void GLV::setMouseDrag(space_t& x, space_t& y){
	setMouseMotion(x,y,Event::MouseDrag);
}

void GLV::setMousePos(int x, int y, space_t relx, space_t rely){
	mMouse.pos(x, y);
	mMouse.posRel(relx, rely);
}

void GLV::setMouseWheel(int wheelDelta){
	eventType(Event::MouseWheel);
	mMouse.bufferPos(mMouse.mW[0] + (space_t)wheelDelta, mMouse.mW);
}

bool GLV::valid(const GLV * g){
	for(unsigned i=0; i<instances().size(); ++i){
		if(instances()[i] == g) return true;
	}
	return false;
}


} // glv::

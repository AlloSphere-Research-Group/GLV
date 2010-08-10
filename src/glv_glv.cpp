/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

GLV::GLV(drawCallback cb, space_t width, space_t height)
:	View(0, 0, width, height, cb), mFocusedView(this)
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

	struct Action : View::TraversalAction{
		GLV& glv; Event::t event;
		Action(GLV& g, Event::t e): glv(g), event(e){}
		bool operator()(View * v, int depth){
			glv.doEventCallbacks(*v, event);
			return true;
		}
		bool operator()(const View * v, int depth){ return false; }
	} action(*this, e);
	
	traverseDepth(this, action);
}



// The bubbling return values from the virtual and function pointer callbacks
// are ANDed together.
bool GLV::doEventCallbacks(View& v, Event::t e){
//	printf("doEventCallbacks: %s %d\n", v.className(), e);
	if(!v.enabled(Controllable)) return false;

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

	bool bubble = true;

	if(v.hasCallbacks(e)){
		const eventCallbackList& cbl = v.callbackLists[e];
		
		// Execute callbacks in list
		for(eventCallbackList::const_iterator it = cbl.begin(); it != cbl.end(); it++){
			//if(*it) bubble |= (*it)(&v, *this);
			if(*it){
				bool r = (*it)(&v, *this);
				bubble &= r;
				if(!bubble) goto end;
			}
		}
	}
	
	bubble &= v.onEvent(e, *this);
	end:
	return bubble | v.enabled(AlwaysBubble);
}

void GLV::doFocusCallback(bool get){

	Event::t e = get ? Event::FocusGained : Event::FocusLost;

	if(mFocusedView){
		mFocusedView->focused(get);
		
		if(mFocusedView->numCallbacks(e) ){
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
		
		// get intersection with myself and previous level
		if(lvl>0) cr[lvl].intersection(cr[lvl-1], cr[lvl]);
	}
	
	// if no child cropping, then inherit previous level's crop rect
	else{ cr[lvl] = cr[lvl-1]; }
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
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glColorPointer(4, GL_FLOAT, 0, 0);
	
	push2D(w, h);	// initialise the OpenGL renderer for our 2D GUI world
	onDraw();		// draw myself
	push();			// push model matrix because of transformations in drawContext()
	
	draw::enable(ScissorTest);

	while(true){

		cv->onModelSync();	// update state based on attached model variables
		cv->validate();		// validate geometry
	
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

			cv->drawPre();
			push(); cv->onDraw(); pop();		// push/pop model cuz user might forget to...
			cv->drawPost();
		}
	}
	
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	
	pop();
	
	// this weird call is necessary so that raster calls get scissored properly
	// not entirely sure why this works, but it does.
	scissor(0,0,(GLint)w,(GLint)h);

	pop2D();
}

std::vector<GLV *>& GLV::instances(){
	static std::vector<GLV *> * sInstances = new std::vector<GLV *>;
	return *sInstances;
}

void GLV::preamble(unsigned int w, unsigned int h){
	using namespace draw;
	glDrawBuffer(GL_BACK);
	//colors().back.print();
	clearColor(colors().back.r, colors().back.g, colors().back.b, colors().back.a);
	clear(ColorBufferBit | DepthBufferBit);	// TODO: this needs to be coordinated with the display settings
}

bool GLV::propagateEvent(){ //printf("GLV::propagateEvent(): %s\n", Event::getName(eventtype));
	View * v = mFocusedView;
	Event::t e = eventType();
	while(v && doEventCallbacks(*v, e)) v = v->parent;
	return v != 0;
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
	mouse.posRel(x,y);
	mouse.updateButton(button, true, clicks);
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

bool GLV::valid(const GLV * g){
	for(unsigned i=0; i<instances().size(); ++i){
		if(instances()[i] == g) return true;
	}
	return false;
}


} // glv::

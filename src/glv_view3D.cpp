#include "glv_draw.h"
#include "glv_view3D.h"


namespace glv{

View3D::View3D(const Rect& r)
: View(r), mNear(0.1), mFar(100), mFOVY(45), mOrtho(false)
{
	resetModelView();
//	double rxy = 0.1; // rotates opposite direction because its modelview?
//	mMatrix[0] = cos(rxy);
//	mMatrix[1] =-sin(rxy);
//	mMatrix[4] =-mMatrix[1];
//	mMatrix[5] = mMatrix[0];	
//	mMatrix[12] = 1;
//	mMatrix[13] = 1;
//	mMatrix[14] = 1;
}

void View3D::resetModelView(){
	for(int j=0; j<4; ++j){
	for(int i=0; i<4; ++i){
		int k = i + j*4; 
		modelView()[k] = (i==j);
	}}
}

void View3D::onDraw(GLV& g){

	using namespace glv::draw;

	// Viewport needs to be in absolute screen coordinates, 
	// so we need to convert from relative...
	space_t ax,ay;
	const View * top = posAbs(ax,ay);

	// y starts at top in GLV, but at bottom in screen coords, so need to flip
	ay = top->h - (ay + h);
	
	// If View is outside window, just return...
	if(ax > top->w || ay > top->h) return;


	// Create viewport just at widget location
	viewport((int)ax, (int)ay, (int)w, (int)h);

	draw::enable(DepthTest);
	draw::disable(Blend);

	// Set up scissor region
	/* Use same scissor region set by 2D renderer.
	{
		space_t sl=ax, sb=ay, sr=w+sl, st=h+sb;		
		if(sl<0) sl=0;
		if(sr>top->w) sr=top->w;
		if(sb<0) sb=0;
		if(st>top->h) st=top->h;
		scissor(sl, sb, sr-sl, st-sb); // use scissor set by main GUI renderer
	}
	*/

	clearColor(colors().back);
	clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Push 3D transforms
	push(Projection);
	identity();
	if(mOrtho){
		float t = 1.04; // 4% padding
		float r = t*float(w)/h;
		draw::ortho(-r,r, -t,t, mNear, mFar);
	}
	else{
		perspective(mFOVY, float(w)/h, mNear, mFar);
	}
	
	push(ModelView);
	identity();
	glMultMatrixf(modelView());

	// Do all 3D drawing
	onDraw3D(g);

	// Restore GUI transforms
	pop(Projection);
	pop(ModelView);


	// Set 2D attributes
	// TODO: factor into draw::attrib2D(w,h)
	draw::disable(DepthTest);
	draw::enable(LineSmooth, Blend);
	blendTrans();
	viewport(0, 0, top->w, top->h);

	// Do 2D drawing
	onDraw2D(g);
}

bool View3D::onEvent(Event::t e, GLV& g){

	const Mouse& m = g.mouse();
	const Keyboard& k = g.keyboard();
	switch(e){
		/*case Event::MouseDown:
			return false;

		case Event::MouseDrag:
			return false;
		*/

		case Event::KeyDown:
			switch(k.key()){
				case 'i': resetModelView(); return false;
				case 'o': mOrtho^=true; return false;
				default:;
			}
		default:;
	}

	return true;
}

} // glv::


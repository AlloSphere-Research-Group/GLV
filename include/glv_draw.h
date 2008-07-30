#ifndef INC_GLV_DRAW_H
#define INC_GLV_DRAW_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <math.h>
#include "glv_conf.h"
#include "glv_color.h"

namespace glv {

/// Drawing routines.
namespace draw{

const double C_PI = 4. * atan(1.);
const double C_2PI = 2. * C_PI;

#ifdef check
	#undef check
#endif

// primitives
enum{
	LineLoop      = GL_LINE_LOOP,
	LineStrip     = GL_LINE_STRIP,
	Lines         = GL_LINES,
	Points        = GL_POINTS,
	TriangleFan   = GL_TRIANGLE_FAN,
	TriangleStrip = GL_TRIANGLE_STRIP,
	Triangles     = GL_TRIANGLES,
	QuadStrip     = GL_QUAD_STRIP,
	Quads         = GL_QUADS
};

// capabilities (for disable() and enable())
enum{
	Blend			= GL_BLEND,
	DepthTest		= GL_DEPTH_TEST,
	Fog				= GL_FOG,
	LineSmooth		= GL_LINE_SMOOTH,
	LineStipple		= GL_LINE_STIPPLE,
	PolygonSmooth	= GL_POLYGON_SMOOTH,
	PointSmooth		= GL_POINT_SMOOTH,
	ScissorTest		= GL_SCISSOR_TEST,
	Texture2D		= GL_TEXTURE_2D
};

// attribute masks
enum{
	ColorBufferBit	= GL_COLOR_BUFFER_BIT,
	DepthBufferBit	= GL_DEPTH_BUFFER_BIT,
	EnableBit		= GL_ENABLE_BIT,
	ViewPortBit		= GL_VIEWPORT_BIT
};

enum{
	Model			= GL_MODELVIEW,
	Projection		= GL_PROJECTION,
	Texture			= GL_TEXTURE
};

struct Glyph{
	enum{ Once=(1<<7), Dot1=(1<<5), Dot2=(2<<5), MaskDots=96, MaskSize=31 };
	
	const unsigned char field;
	float x[8], y[8];
	
	static int cap()		{ return 0; }
	static int median()		{ return 3; }
	static int baseline()	{ return 8; }
	static int descent()	{ return 11; }
	static int width()		{ return 8; }
	
	unsigned char dots() const { return (field & MaskDots) >> 5; }
	unsigned char once() const { return (field           ) >> 7; }
	unsigned char size() const { return (field & MaskSize)     ; }
};



// Basic rendering commands
void begin(int primitive);							///< Begin vertex group delimitation
void blendFunc(int sfactor, int dfactor);			///< Set blending function
void blendTrans();									///< Set blending function to transparent
void blendAdd();									///< Set blending function to additive
void clear(int mask);								///< Clear drawing buffers
void clearColor(float r, float g, float b, float a=1);	///< Set clear color
void color(float gray, float a=1);					///< Set current draw color
void color(float r, float g, float b, float a=1);		///< Set current draw color
void color(const Color& c);							///< Set current draw color
void color(const Color& c, float a);				///< Set current draw color, but override alpha component
void end();											///< End vertex group delimitation
void fog(float end, float start, float density=1);	///< Set linear fog parameters
void identity();									///< Load identity transform matrix
void lineStipple(char factor, short pattern);		///< Specify line stipple pattern
void lineWidth(float val);							///< Set width of lines
void matrixMode(int mode);							///< Set current transform matrix
void ortho(float l, float r, float b, float t);		///< Set orthographic projection mode
void pointSize(float val);							///< Set size of points
void pop();											///< Pop current transform matrix stack
void pop(int matrixMode);							///< Pop a transform matrix stack also setting as current matrix
void pop2D();										///< Pop 2-D pixel space
void pop3D();										///< Pop 3-D signed normalized cartesian space
void popAttrib();									///< Pop last pushed attributes from stack
void push();										///< Push current transform matrix stack 
void push(int matrixMode);							///< Push a transform matrix stack also setting as current matrix
template <class T> void push2D(T w, T h);			///< Push 2-D pixel space
template <class T> void push3D(T w, T h, T near=0.1, T far=100);	///< Push 3-D signed normalized cartesian space
void pushAttrib(int attribs);						///< Push current attributes onto stack
void rotate(float degx, float degy, float degz);
void rotateX(float deg);
void rotateY(float deg);
void rotateZ(float deg);
void scale(float x, float y, float z=1.f);
void scissor(float x, float y, float w, float h);
void texCoord(float x, float y);
void translate(float x, float y, float z=0.f);
void translateX(float x);
void translateY(float y);
void translateZ(float z);
void viewport(float x, float y, float w, float h);

template <class V3> void vertex(const V3& v);		///< Send single vertex given 3-element array accessible object
void vertex(float x, float y, float z=0);			///< Send single vertex

template <class T>
void vertexY(T * ys, unsigned long len, T xInc=1, int prim = LineStrip);

template <class T>
void vertex(T * xs, T * ys, unsigned long len, int prim = LineStrip);


// icons
void check		(float l, float t, float r, float b);
template <int N> void dot(float l, float t, float r, float b);
void frame		(float l, float t, float r, float b);
void minus		(float l, float t, float r, float b);
void plus		(float l, float t, float r, float b);
void rect		(float l, float t, float r, float b);
void triangleR	(float l, float t, float r, float b);
void triangleL	(float l, float t, float r, float b);
void triangleU	(float l, float t, float r, float b);
void triangleD	(float l, float t, float r, float b);
void x			(float l, float t, float r, float b);

/// Parallel horizontal and vertical lines
void grid(float l, float t, float w, float h, float divx, float divy, bool incEnds=true);

void linePattern(float l, float t, float w, float h, int n, const char * pat="/");

/// Parallel horizontal lines
void linesH(float l, float t, float w, float h, int n);

/// Converts a float to its pixel value.
inline int pix(float v);

/// Regular polygon
void pgon(float l, float t, float w, float h, int sides, float angleNorm=0);

void shape(int prim, float x0, float y0, float x1, float y1);
void shape(int prim, float x0, float y0, float x1, float y1, float x2, float y2);
void shape(int prim, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

void spokes(float l, float t, float w, float h, int sides, float angleNorm=0);

/// Sends vertex calls for a printable ascii character.

/// This function must be called within a begin(Lines) and end().  
/// The coordinate system is an 8x8 grid with top-left at (0,0).
bool character(int c, float dx=0, float dy=0);

/// Draws a text string, including new lines and tabs.

/// The spacing between characters is fixed at 8 units.
///
void text(const char * s, float l=0, float t=0, float lineSpacing=1, unsigned int tabSpaces=4);


/// Functor for disabling rendering capabilities
struct Disable{
	const Disable& operator() (int cap) const { glDisable(cap); return *this; }
	const Disable& operator<< (int cap) const { return (*this)(cap); }
};

static Disable disable;

/// Functor for enabling rendering capabilities
struct Enable{
	const Enable& operator() (int cap) const { glEnable(cap); return *this; }
	const Enable& operator<< (int cap) const { return (*this)(cap); }
};

static Enable enable;



// Implementation ______________________________________________________________

inline void check(float l, float t, float r, float b){
	shape(LineStrip, l,0.5*(t+b), l+(r-l)*0.3,b, r,t);
}

template <int N>
void dot(float l, float t, float r, float b){
	float px=1, py=0, rx=cos(6.28318530718/N), ry=sin(6.28318530718/N);
	float mx=0.5*(l+r), my=0.5*(t+b), sx=(r-l)*0.5, sy=(b-t)*0.5;
	
	begin(TriangleFan);
	vertex(mx, my);
	for(int i=0; i<N+1; ++i){	
		vertex(mx+px*sx, my+py*sy);
		float tx=px;
		px = px*rx - py*ry;
		py = tx*ry + py*rx;
	}
	end();
}

inline void frame(float l, float t, float r, float b){
	shape(LineLoop, l, t, l, b, r, b, r, t);
}

inline void minus(float l, float t, float r, float b){
	float my = 0.5*(t+b);
	shape(Lines, l,my, r,my);
}

// [-2,-1) -> -1.5
// [-1, 0) -> -0.5
// [ 0, 1) ->  0.5
// [ 1, 2) ->  1.5
inline int pix(float v){ return v>=0 ? (int)(v+0.5) : (int)(v-0.5); }

inline void plus(float l, float t, float r, float b){
	float mx = 0.5*(l+r);
	float my = 0.5*(t+b);
	shape(Lines, mx,t, mx,b, l,my, r,my);
}

inline void rect(float l, float t, float r, float b){
	glRectf(r, b, l, t);
}

inline void shape(int primitive, float x0, float y0, float x1, float y1){
	begin(primitive);
		vertex(x0, y0); vertex(x1, y1);
	end();
}

inline void shape(int primitive, float x0, float y0, float x1, float y1, float x2, float y2){
	begin(primitive);
		vertex(x0, y0); vertex(x1, y1); vertex(x2, y2);
	end();
}

inline void shape(int primitive, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3){
	begin(primitive);
		vertex(x0, y0); vertex(x1, y1); vertex(x2, y2); vertex(x3, y3);
	end();
}

inline void triangleD(float l, float t, float r, float b){	
	shape(Triangles, 0.5*(l+r),b, r,t, l,t);
}

inline void triangleL(float l, float t, float r, float b){	
	shape(Triangles, l,0.5*(t+b), r,b, r,t);
}

inline void triangleR(float l, float t, float r, float b){
	shape(Triangles, r,0.5*(t+b), l,t, l,b);
}

inline void triangleU(float l, float t, float r, float b){
	shape(Triangles, 0.5*(l+r),t, l,b, r,b);
}

inline void x(float l, float t, float r, float b){
	shape(Lines, l,t, r,b, l,b, r,t);
}






// core drawing routines
inline void color(float v, float a){ color(v,v,v,a); }

inline void color(const Color& c){ color(c.r, c.g, c.b, c.a); }

inline void color(const Color& c, float a){ color(c.r, c.g, c.b, a); }

inline void pop(int mode){ matrixMode(mode); pop(); }
inline void push(int mode){ matrixMode(mode); push(); }

inline void rotate(float degx, float degy, float degz){
	rotateX(degx); rotateY(degy); rotateZ(degz);
}

inline void translateX(float x){ translate(x, 0, 0); }
inline void translateY(float y){ translate(0, y, 0); }
inline void translateZ(float z){ translate(0, 0, z); }

template <class V3>
inline void vertex(const V3& v){ vertex(v[0], v[1], v[2]); }

template <class T>
void vertexY(T * ys, unsigned long len, T xInc, int prim){
	begin(prim);
	for(unsigned long i=0; i<len; ++i) vertex((float)(xInc * i), (float)ys[i]);
	end();
}

template <class T>
void vertex(T * xs, T * ys, unsigned long len, int prim){
	begin(prim);
	for(unsigned long i=0; i<len; ++i) vertex((float)xs[i], (float)ys[i]);
	end();
}



// platform dependent

inline void begin(int primitive){ glBegin(primitive); }
inline void clear(int mask){ glClear(mask); }
inline void clearColor(float r, float g, float b, float a){ glClearColor(r,g,b,a); }
inline void color(float r, float g, float b, float a){ glColor4f(r,g,b,a); }
inline void end(){ glEnd(); }

// we need to push and pop matrices and viewport bit
template <class T>
void push2D(T w, T h){
							// to ensure polygon edges blend properly
	disable << DepthTest << PolygonSmooth;
	enable << Blend << LineSmooth;
	blendTrans();

	push(Projection); pushAttrib(ViewPortBit); identity();
		viewport(0, 0, w, h);
		ortho(0, w, h, 0);		// flat 2D world dimension L,R,B,T
	
	push(Model); identity();
}


inline void pop2D(){
	popAttrib();					// for popping GL_VIEWPORT_BIT
	pop(Projection);
	pop(Model);
}


template <class T>
void push3D(T w, T h, T near, T far){
	//pushAttrib(DepthBufferBit);
	pushAttrib(ColorBufferBit | DepthBufferBit | EnableBit);
	enable(DepthTest);
	
	push(Projection); identity();
		gluPerspective(45, w/(GLfloat)h, near, far);
	
	push(Model); identity();
		translate(0, 0, -2.42);
}


inline void pop3D(){
	popAttrib();					// for popping GL_DEPTH_BUFFER_BIT
	pop(Projection);
	pop(Model);
}

inline void blendFunc(int sfactor, int dfactor){ glBlendFunc(sfactor, dfactor); }
inline void blendTrans(){ blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
inline void blendAdd(){ blendFunc(GL_SRC_COLOR, GL_ONE); }

inline void fog(float end, float start, float density){
	glFogi(GL_FOG_MODE, GL_LINEAR); 
	glFogf(GL_FOG_DENSITY, density); glFogf(GL_FOG_START, start); glFogf(GL_FOG_END, end);
}

inline void identity(){ glLoadIdentity(); }
inline void lineStipple(char factor, short pattern){ glLineStipple(factor, pattern); }
inline void lineWidth(float v){ glLineWidth(v); }
inline void matrixMode(int mode){ glMatrixMode(mode); }
inline void ortho(float l, float r, float b, float t){ gluOrtho2D(l,r,b,t); }
inline void pointSize(float v){ glPointSize(v); }
inline void push(){ glPushMatrix(); }
inline void pushAttrib(int attribs){ glPushAttrib(attribs); }
inline void pop() { glPopMatrix(); }
inline void popAttrib(){ glPopAttrib(); }
inline void rotateX(float deg){ glRotatef(deg, 1.f, 0.f, 0.f); }
inline void rotateY(float deg){ glRotatef(deg, 0.f, 1.f, 0.f); }
inline void rotateZ(float deg){ glRotatef(deg, 0.f, 0.f, 1.f); }
inline void scale(float x, float y, float z){ glScalef(x,y,z); }
inline void scissor(float x, float y, float w, float h){ glScissor((GLint)x,(GLint)y,(GLsizei)w,(GLsizei)h); }
inline void texCoord(float x, float y){ glTexCoord2f(x,y); }
inline void translate(float x, float y, float z){ glTranslatef(x,y,z); }
inline void viewport(float x, float y, float w, float h){ glViewport((GLint)x,(GLint)y,(GLsizei)w,(GLsizei)h); }
inline void vertex(float x, float y, float z){ glVertex3f(x,y,z); }

} // end namespace draw

} // end namespace glv

#endif


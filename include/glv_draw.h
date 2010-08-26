#ifndef INC_GLV_DRAW_H
#define INC_GLV_DRAW_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <math.h>
#include <stdio.h>
#include "glv_conf.h"
#include "glv_color.h"

#define GLV_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

namespace glv {

struct Point2{
	Point2(){}
	Point2(float x_, float y_): x(x_), y(y_){}
	void operator()(float x_, float y_){x=x_; y=y_;}
	union{
		struct{ float x,y; };
		float elems[2];
	};
};

struct Point3{
	Point3(){}
	Point3(float x_, float y_, float z_): x(x_), y(y_), z(z_){}
	void operator()(float x_, float y_, float z_){x=x_; y=y_; z=z_;}
	union{
		struct{ float x,y,z; };
		float elems[3];
	};
};


/// Drawing routines.
namespace draw{

const double C_PI = 4. * atan(1.);
const double C_2PI = 2. * C_PI;



#ifdef check
	#undef check
#endif

// primitives
enum{
	Points        = GL_POINTS,
	Lines         = GL_LINES,
	LineStrip     = GL_LINE_STRIP,
	LineLoop      = GL_LINE_LOOP,
	Triangles     = GL_TRIANGLES,
	TriangleStrip = GL_TRIANGLE_STRIP,
	TriangleFan   = GL_TRIANGLE_FAN,
#ifdef GLV_USE_OPENGL_ES
	Quads         = GL_POINTS,
	QuadStrip     = GL_TRIANGLE_STRIP,
#else
	Quads         = GL_QUADS,
	QuadStrip     = GL_QUAD_STRIP,
#endif
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
	ModelView		= GL_MODELVIEW,
	Projection		= GL_PROJECTION,
	Texture			= GL_TEXTURE
};


// Basic rendering commands
void begin(int primitive);							///< Begin vertex group delimitation
void blendFunc(int sfactor, int dfactor);			///< Set blending function
void blendTrans();									///< Set blending function to transparent
void blendAdd();									///< Set blending function to additive
void clear(int mask);								///< Clear drawing buffers
void clearColor(float r, float g, float b, float a=1);	///< Set clear color
void clearColor(const Color& c);					///< Set clear color
void color(float gray, float a=1);					///< Set current draw color
void color(float r, float g, float b, float a=1);		///< Set current draw color
void color(const Color& c);							///< Set current draw color
void color(const Color& c, float a);				///< Set current draw color, but override alpha component
void end();											///< End vertex group delimitation
void fog(float end, float start, const Color& c=Color(0));	///< Set linear fog parameters
void identity();									///< Load identity transform matrix
void lineStipple(char factor, short pattern);		///< Specify line stipple pattern
void lineWidth(float val);							///< Set width of lines
void matrixMode(int mode);							///< Set current transform matrix
void ortho(float l, float r, float b, float t);		///< Set orthographic projection mode
void paint(int prim, Point2 * verts, int numVerts);	///< Draw array of 2D vertices
void paint(int prim, Point2 * verts, Color * cols, int numVerts);
void paint(int prim, Point2 * verts, unsigned * indices, int numIndices); ///< Draw indexed array of 2D vertices
void paint(int prim, Point3 * verts, int numVerts);	///< Draw array of 3D vertices
void paint(int prim, Point3 * verts, Color * cols, int numVerts);
void paint(int prim, Point3 * verts, unsigned * indices, int numIndices); ///< Draw indexed array of 3D vertices
void paint(int prim, Point3 * verts, Color * cols, unsigned * indices, int numIndices); 
void pointSize(float val);							///< Set size of points
void pointAtten(float c2=0, float c1=0, float c0=1); ///< Set distance attenuation of points. The scaling formula is clamp(size * sqrt(1/(c0 + c1*d + c2*d^2)))
void pop();											///< Pop current transform matrix stack
void pop(int matrixMode);							///< Pop a transform matrix stack also setting as current matrix
void pop2D();										///< Pop 2-D pixel space
void pop3D();										///< Pop 3-D signed normalized cartesian space
void popAttrib();									///< Pop last pushed attributes from stack
int printError(const char * pre="", bool verbose=true, FILE * out=stdout); ///< Print rendering errors to file
void push();										///< Push current transform matrix stack 
void push(int matrixMode);							///< Push a transform matrix stack also setting as current matrix
void push2D(float w, float h);						///< Push 2-D pixel space
void push3D(float w, float h, float near=0.1, float far=100, float fovy=45);	///< Push 3-D signed normalized cartesian space
void pushAttrib(int attribs);						///< Push current attributes onto stack
void rotate(float degx, float degy, float degz);
void rotateX(float deg);
void rotateY(float deg);
void rotateZ(float deg);
void scale(float v);								///< Scale all dimensions by amount
void scale(float x, float y, float z=1.f);
void scissor(float x, float y, float w, float h);
void stroke(float w);								///< Sets width of lines and points
void texCoord(float x, float y);
void translate(float x, float y, float z=0.f);
void translateX(float x);
void translateY(float y);
void translateZ(float z);
void viewport(float x, float y, float w, float h);

template <class V3> void vertex(const V3& v);		///< Send single vertex given 3-element array accessible object
void vertex(float x, float y);						///< Send single xy vertex
void vertex(float x, float y, float z);				///< Send single xyz vertex

template <class T>
void vertexY(T * ys, unsigned long len, T xInc=1, int prim = LineStrip);

template <class T>
void vertex(T * xs, T * ys, unsigned long len, int prim = LineStrip);


// icons
void check		(float l, float t, float r, float b);			///< Check mark
template <int N> void disc(float l, float t, float r, float b);	///< Disc with N edges
void frame		(float l, float t, float r, float b);			///< Rectangular frame
void minus		(float l, float t, float r, float b);			///< Minus
void plus		(float l, float t, float r, float b);			///< Plus
void rect		(float l, float t, float r, float b);			///< Solid rectangle
void triangleR	(float l, float t, float r, float b);			///< Right pointing triangle
void triangleL	(float l, float t, float r, float b);			///< Left pointing triangle
void triangleU	(float l, float t, float r, float b);			///< Upward pointing triangle
void triangleD	(float l, float t, float r, float b);			///< Downward pointing triangle
void x			(float l, float t, float r, float b);			///< X mark

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


/// Draws a text string, including new lines and tabs.

/// The spacing between characters is fixed.
///
void text(const char * s, float l=0, float t=0, unsigned fontSize=8, float lineSpacing=1, unsigned tabSpaces=4);



/// Disables rendering capabilities
struct Disable{

	Disable(){}
	Disable(int cap){ *this << cap; }
	Disable(int cap1, int cap2){ *this << cap1 << cap2; }

	/// Disable a capability
	const Disable& operator()(int cap) const { return *this << cap; }
	const Disable& operator()(int cap1, int cap2) const { return *this << cap1<<cap2; }
	const Disable& operator()(int cap1, int cap2, int cap3) const { return *this << cap1<<cap2<<cap3; }
	
	/// Disable a capability
	const Disable& operator<< (int cap) const { glDisable(cap); return *this; }
};

/// Global Disable functor
static Disable disable;



/// Enables rendering capabilities
struct Enable{

	Enable(){}
	Enable(int cap){ *this << cap; }
	Enable(int cap1, int cap2){ *this << cap1 << cap2; }

	/// Enable a capability
	const Enable& operator()(int cap) const { return *this << cap; }
	const Enable& operator()(int cap1, int cap2) const { return *this << cap1<<cap2; }
	const Enable& operator()(int cap1, int cap2, int cap3) const { return *this << cap1<<cap2<<cap3; }
	
	/// Enable a capability
	const Enable& operator<< (int cap) const { glEnable(cap); return *this; }
};

/// Global Enable functor
static Enable enable;





// Implementation ______________________________________________________________

inline void paint(int prim, Point2 * verts, int numVerts){
	//glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(prim, 0, numVerts);
	//glDisableClientState(GL_VERTEX_ARRAY);
}

inline void paint(int prim, Point2 * verts, Color * cols, int numVerts){
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_FLOAT, 0, cols);
	glDrawArrays(prim, 0, numVerts);
	glDisableClientState(GL_COLOR_ARRAY);
}

inline void paint(int prim, Point2 * verts, unsigned * indices, int numIndices){
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawElements(prim, numIndices, GL_UNSIGNED_INT, indices);
}

inline void paint(int prim, Point3 * verts, int numVerts){
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glDrawArrays(prim, 0, numVerts);
}

inline void paint(int prim, Point3 * verts, Color * cols, int numVerts){
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_FLOAT, 0, cols);
	glDrawArrays(prim, 0, numVerts);
	glDisableClientState(GL_COLOR_ARRAY);
}

inline void paint(int prim, Point3 * verts, unsigned * indices, int numIndices){
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glDrawElements(prim, numIndices, GL_UNSIGNED_INT, indices);
}

inline void paint(int prim, Point3 * verts, Color * cols, unsigned * indices, int numIndices){
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_FLOAT, 0, cols);
	glDrawElements(prim, numIndices, GL_UNSIGNED_INT, indices);
	glDisableClientState(GL_COLOR_ARRAY);
}

inline void check(float l, float t, float r, float b){ shape(LineStrip, l,0.5*(t+b), l+(r-l)*0.3,b, r,t); }

template <int N>
void disc(float l, float t, float r, float b){
	static const double theta = C_2PI/N;
	float px=1, py=0, rx=cos(theta), ry=sin(theta);
	float mx=0.5*(l+r), my=0.5*(t+b), sx=(r-l)*0.5, sy=(b-t)*0.5;
	
	Point2 pts[N+2];
	
	pts[0](mx,my);

	for(int i=1; i<N+1; ++i){
		pts[i](mx+px*sx, my+py*sy);
		float tx=px;
		px = px*rx - py*ry;
		py = tx*ry + py*rx;
	}
	
	pts[N+1] = pts[1];
	
	paint(TriangleFan, pts, GLV_ARRAY_SIZE(pts));
}

inline void frame(float l, float t, float r, float b){ shape(LineLoop, l, t, l, b, r, b, r, t); }
inline void minus(float l, float t, float r, float b){ float my=0.5*(t+b); shape(Lines, l,my, r,my); }

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

inline void shape(int prim, float x0, float y0, float x1, float y1){
	float v[] = {x0,y0,x1,y1};
	paint(prim, (Point2 *)v, 2);
}

inline void shape(int prim, float x0, float y0, float x1, float y1, float x2, float y2){
	float v[] = {x0,y0,x1,y1,x2,y2};
	paint(prim, (Point2 *)v, 3);
}

inline void shape(int prim, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3){
	float v[] = {x0,y0,x1,y1,x2,y2,x3,y3};
	paint(prim, (Point2 *)v, 4);
}

inline void stroke(float w){ lineWidth(w); pointSize(w); }
inline void triangleD(float l, float t, float r, float b){ shape(Triangles, 0.5*(l+r),b, r,t, l,t); }
inline void triangleL(float l, float t, float r, float b){ shape(Triangles, l,0.5*(t+b), r,b, r,t); }
inline void triangleR(float l, float t, float r, float b){ shape(Triangles, r,0.5*(t+b), l,t, l,b); }
inline void triangleU(float l, float t, float r, float b){ shape(Triangles, 0.5*(l+r),t, l,b, r,b); }
inline void x(float l, float t, float r, float b){ shape(Lines, l,t, r,b, l,b, r,t); }


// core drawing routines
inline void clearColor(const Color& c){ clearColor(c.r, c.g, c.b, c.a); }
inline void color(float v, float a){ color(v,v,v,a); }
inline void color(const Color& c){ color(c.r, c.g, c.b, c.a); }
inline void color(const Color& c, float a){ color(c.r, c.g, c.b, a); }
inline void pop(int mode){ matrixMode(mode); pop(); }
inline void push(int mode){ matrixMode(mode); push(); }
inline void rotate(float degx, float degy, float degz){	rotateX(degx); rotateY(degy); rotateZ(degz); }
inline void scale(float v){ scale(v,v,v); }
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
inline void begin(int prim){
#ifndef GLV_USE_OPENGL_ES
	glBegin(prim);
#endif
}
inline void blendFunc(int sfactor, int dfactor){ glBlendFunc(sfactor, dfactor); }
inline void blendAdd(){ glBlendEquation(GL_FUNC_ADD); blendFunc(GL_SRC_COLOR, GL_ONE); }
inline void blendTrans(){ glBlendEquation(GL_FUNC_ADD); blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
inline void clear(int mask){ glClear(mask); }
inline void clearColor(float r, float g, float b, float a){ glClearColor(r,g,b,a); }
inline void color(float r, float g, float b, float a){ glColor4f(r,g,b,a); }

inline void end(){
#ifndef GLV_USE_OPENGL_ES
	glEnd();
#endif
}

inline void identity(){ glLoadIdentity(); }
inline void lineStipple(char factor, short pattern){ glLineStipple(factor, pattern); }
inline void lineWidth(float v){ glLineWidth(v); }
inline void matrixMode(int mode){ glMatrixMode(mode); }
inline void ortho(float l, float r, float b, float t){ gluOrtho2D(l,r,b,t); }
inline void pointSize(float v){ glPointSize(v); }
inline void pointAtten(float c2, float c1, float c0){
	GLfloat att[3] = {c0, c1, c2};
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
}
inline void push(){ glPushMatrix(); }
inline void pushAttrib(int attribs){ glPushAttrib(attribs); }
inline void pop() { glPopMatrix(); }
inline void popAttrib(){ glPopAttrib(); }
inline void rect(float l, float t, float r, float b){ glRectf(r, b, l, t); }
inline void rotateX(float deg){ glRotatef(deg, 1.f, 0.f, 0.f); }
inline void rotateY(float deg){ glRotatef(deg, 0.f, 1.f, 0.f); }
inline void rotateZ(float deg){ glRotatef(deg, 0.f, 0.f, 1.f); }
inline void scale(float x, float y, float z){ glScalef(x,y,z); }
inline void scissor(float x, float y, float w, float h){ glScissor((GLint)x,(GLint)y,(GLsizei)w,(GLsizei)h); }
inline void texCoord(float x, float y){ glTexCoord2f(x,y); }
inline void translate(float x, float y, float z){ glTranslatef(x,y,z); }
inline void viewport(float x, float y, float w, float h){ glViewport((GLint)x,(GLint)y,(GLsizei)w,(GLsizei)h); }

inline void vertex(float x, float y){
#ifndef GLV_USE_OPENGL_ES
	glVertex2f(x,y);
#endif
}

inline void vertex(float x, float y, float z){
#ifndef GLV_USE_OPENGL_ES
	glVertex3f(x,y,z);
#endif
}

} // draw::

} // glv::

#endif


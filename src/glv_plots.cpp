/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <float.h>
#include <string.h>
#include "glv_util.h"
#include "glv_plots.h"

namespace glv{

/*
Requirements:
Multiple graphs viewable within same plotting region.
Support for different plotting styles- line, bar, scatter, polar
Support for 1D and 2D plots

J
	basic plot types:
	surface/wire, line, symbol/dot
	
	array is set of points where dim=0 dependent, dim>0 independent

Dimension=0 compact dimension
Dimension>0 dimension of embedding space

Shape
(1, Nx,  1, 1)	1D curve (dependent x, independent y)
(1,  1, Ny, 1)	1D curve (dependent y, independent x)
(2, Nx,  1, 1)	2D curve
(2,  1, Ny, 1)	2D curve
(3, Nx,  1, 1)	3D curve

(1, Nx, Ny, 1)	2D density or 3D surface
(2, Nx, Ny, 1)	2D density colored
(3, Nx, Ny, 1)	2D density colored

All plotters present multidimensional arrays in a consistent way. Dimensions
are treated as follows:

0: inner dimensions (i.e., color, position, glyph shape)
1: x dimensions
2: y dimensions
3: z dimensions

Type		Shape			Memory
1D x		(1, n, 1)		(x1, x2, x3)
1D y		(1, 1, n)		((y1, y2, y3))
2D Scatter	(2, n)			((x1, y1), (x2, y2), (x3, y3))				
3D Scatter	(3, n)			((x1, y1, z1), (x2, y2, z2), (x3, y3, z3))
xy			(n, 2)			((x1, x2, x3), (y1, y2, y3))
xyz			(n, 3)			((x1, x2, x3), (y1, y2, y3), (z1, z2, z3))

Density		(C, n, m)		((v11, v21, v31), (v12, v22, v32))
Surface z	(1, n, m, 1)	((v11, v21, v31), (v12, v22, v32))
Surface y	(1, n, 1, m)	((v11, v21, v31), (v12, v22, v32))
Surface x	(1, 1, n, m)	((v11, v21, v31), (v12, v22, v32))

*/

Plottable::Plottable(const Plottable& src){
	*this = src;
}

Plottable::Plottable(int prim, float stroke)
:	mPrim(prim), mStroke(stroke), mBlendMode(TRANSLUCENT), mLineStipple(-1),
	mDrawUnder(false), mUseStyleColor(true), mActive(true)
{
	updateRegion(0,0,-1,-1);
	add(*this);
}

Plottable::Plottable(int prim, float stroke, const Color& col)
:	Plottable(prim, stroke)
{
	color(col);
}

Plottable& Plottable::operator= (const Plottable& src){
	if(this == &src) return *this;

	// Assign members with pointers
	mGraphicsMaps = src.mGraphicsMaps;
	mData = src.mData;

	// Deep copy data members
	memcpy(&mPOD1, &(src.mPOD1), (char*)&mPOD2 - (char*)&mPOD1);

	// Re-assign any graphics maps that were assigned from parent object
	const auto * srcGraphicsMap = dynamic_cast<const GraphicsMap*>(&src);
	for(auto& gmap : mGraphicsMaps){
		if(srcGraphicsMap == gmap){
			gmap = dynamic_cast<GraphicsMap*>(this);
		}
	}

	return *this;
}

Plottable& Plottable::add(GraphicsMap& v){
	mGraphicsMaps.push_back(&v);
	return *this;
}

Plottable& Plottable::remove(GraphicsMap& v){
	mGraphicsMaps.erase(
		std::remove(mGraphicsMaps.begin(), mGraphicsMaps.end(), &v),
		mGraphicsMaps.end()
	);
	return *this;
}

Plottable& Plottable::updateRegion(int x, int y, int w, int h){
	mUpdateRegion[0]=x;
	mUpdateRegion[1]=y;
	mUpdateRegion[2]=w;
	mUpdateRegion[3]=h;
	return *this;
}

void Plottable::doPlot(GraphicsData& gd, const Data& d){
	if(!d.hasData() || !active()) return;
	draw::color(color());
	draw::stroke(stroke());
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	draw::enable(draw::PointSmooth);
	draw::enable(draw::LineSmooth);

	bool doLineStipple = (-1 != mLineStipple);

	if(doLineStipple){
		draw::lineStipple(1, mLineStipple);
		draw::lineStippling(true);
	}

	int ux = mUpdateRegion[0];
	int uy = mUpdateRegion[1];
	int uw = mUpdateRegion[2];
	int uh = mUpdateRegion[3];

	int endIndices[3] = {
		ux + uw + (uw < 0 ? d.size(1)+1 : 0),
		uy + uh + (uh < 0 ? d.size(2)+1 : 0),
		d.size(3)
	};
	int startIndices[3] = { ux, uy, 0 };
	Indexer ind(endIndices, startIndices);

	for(auto& gmap : mGraphicsMaps){
		ind.reset();
		gmap->onMap(gd, d, ind);
	}

	switch(mBlendMode){
		case TRANSLUCENT: break;
		case ADDITIVE:
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case SUBTRACTIVE:
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		case SCREEN:
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			break;
		case MULTIPLICATIVE:
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		default:;
	}

	onDraw(gd, d);

	// Restore GLV's default blending function
	switch(mBlendMode){
		case TRANSLUCENT: break;
		case ADDITIVE:
		case SUBTRACTIVE:
		case SCREEN:
		case MULTIPLICATIVE:
			draw::blendTrans();
			break;
		default:;
	}

    if(doLineStipple) draw::lineStippling(false);
}



PlotDensity::PlotDensity(const Color& c, float hueSpread, int ipol)
:	Plottable(draw::Triangles, 1, c), mTex(0,0, GL_RGBA, GL_FLOAT),
	mHueSpread(hueSpread), mIpol(ipol)
{
	mRegion[0].endpoints(-1, 1);
	mRegion[1].endpoints(-1, 1);
//	add(defaultColorMap());
}

PlotDensity& PlotDensity::plotRegion(const Interval<double>& vx, const Interval<double>& vy){
	mRegion[0] = vx; mRegion[1] = vy; return *this;
}

PlotDensity& PlotDensity::updateRegion(int x, int y, int w, int h){
	Plottable::updateRegion(x,y,w,h);
	mTex.updateRegion(x,y,w,h);
	return *this;
}

void PlotDensity::onMap(GraphicsData& gd, const Data& d, const Indexer& i){

	int N0 = d.size(0);	// number of "internal" dimensions

	Color col = gd.colors()[0];
	HSV hsv = col;

	Color col1 = HSV(hsv).rotateHue( mHueSpread);
	Color col2 = HSV(hsv).rotateHue(-mHueSpread);

	switch(N0){
	case 1:
		while(i()){
			float w0 = d.at<float>(0,i[0],i[1],i[2]);

			Color c((w0 > 0 ? col1*w0 : col2*-w0), col.a);
			
			//Color c(col * w0, col.a);			
			gd.addColor(c);
		}
		break;

	case 2:
		while(i()){
			float w0 = d.at<float>(0,i[0],i[1],i[2]);
			float w1 = d.at<float>(1,i[0],i[1],i[2]);
			Color c = HSV(hsv.h, hsv.s*w1, hsv.v*w0);
			gd.addColor(c);
		}
		break;

	default:
		while(i()){
			float w0 = d.at<float>(0,i[0],i[1],i[2]);
			float w1 = d.at<float>(1,i[0],i[1],i[2]);
			float w2 = d.at<float>(2,i[0],i[1],i[2]);
			Color c = Color(w0, w1, w2);
			gd.addColor(c);
		}
	}
}

void PlotDensity::onContextCreate(){
	// note: texture created in onPlot since we don't know the size
	//mTex.create();
	mTex.recreate();
}

void PlotDensity::onContextDestroy(){
	mTex.destroy();
	mTex.dealloc();
}

// polar coloring
//			float w0 = d.at<float>(0,i[0],i[1],i[2]);
//			float w1 = d.at<float>(1,i[0],i[1],i[2]);
//			
//			double m = hypot(w0,w1);
//			double a = atan2(w1,w0)/(-2*M_PI); if(a<0) a=1+a;
//			Color c = HSV(a, 1, m);

void PlotDensity::onDraw(GraphicsData& b, const Data& d){
	// (re)creates texture on GPU only when size changes
	mTex.create(d.size(1), d.size(2), NULL);

	mTex.magFilter(mIpol ? GL_LINEAR : GL_NEAREST);
	draw::enable(draw::Texture2D);
	draw::color(1,1,1,1);
	mTex.begin();
	mTex.send(&b.colors()[0]);
	mTex.draw(mRegion[0].min(), mRegion[1].max(), mRegion[0].max(), mRegion[1].min());
	mTex.end();
	draw::disable(draw::Texture2D);
}


PlotFunction1D::PlotFunction1D(const Color& c, float stroke, int prim, PathStyle path)
:	Plottable(prim, stroke,c), mPathStyle(path), mDomainOffset(0)
{
}

template <class R>
void getFX(float& x, float& y, const Data& d, const Indexer& i, int offset){
	x = i[0] + offset;
	y = d.elem<R>(0, i[0]);
}

template <class R>
void getFY(float& x, float& y, const Data& d, const Indexer& i, int offset){
	x = d.elem<R>(0, 0, i[1]);
	y = i[1] + offset;
}

void PlotFunction1D::onMap(GraphicsData& g, const Data& d, const Indexer& i){
//	Indexer j(i.size());
//	while(j()){
//		double x = j[0];
//		double y = d.at<double>(0, j[0]);
//		g.addVertex(x, y);
//	}

	/*
	dim		mapping
	0		position
	1		number of points along x
	2		number of points along y
	*/

	//int N0 = d.size(0); // only first component used
	int N1 = d.size(1);
	int N2 = d.size(2);
	
	// N1 == 1,	domain along y, f(y) = ...
	// N2 == 1,	domain along x, f(x) = ...

	#define PLOT(Type)\
	switch(mPathStyle){\
		case PlotFunction1D::DIRECT:\
			if(1==N2){\
				while(i()){\
					float x,y; getFX<Type>(x,y, d,i, mDomainOffset);\
					g.addVertex(x, y);\
				}\
			}\
			else if(1==N1){\
				while(i()){\
					float x,y; getFY<Type>(x,y, d,i, mDomainOffset);\
					g.addVertex(x, y);\
				}\
			}\
			break;\
		case PlotFunction1D::ZIGZAG:\
			if(1==N2){\
				while(i()){\
					float x,y; getFX<Type>(x,y, d,i, mDomainOffset);\
					g.addVertex(x, 0);\
					g.addVertex(x, y);\
				}\
			}\
			else if(1==N1){\
				while(i()){\
					float x,y; getFY<Type>(x,y, d,i, mDomainOffset);\
					g.addVertex(0, y);\
					g.addVertex(x, y);\
				}\
			}\
			break;\
		default:;\
	}\
	
	switch(d.type()){
	case Data::FLOAT:	PLOT(float);	break;
	case Data::DOUBLE:	PLOT(double);	break;
	case Data::INT:		PLOT(int);		break;
	default:;
	}
}



PlotFunction2D::PlotFunction2D(const Color& c)
:	Plottable(draw::LineStrip, 1,c)
{
}

void PlotFunction2D::onMap(GraphicsData& g, const Data& d, const Indexer& i){
	if(d.size(0) < 2) return;
	while(i()){
		auto x = d.at<float>(0, i[0], i[1]);
		auto y = d.at<float>(1, i[0], i[1]);
		g.addVertex(x, y);
	}
}



struct EvPlotCreateContext : public EventHandler{
	bool onEvent(View& v, GLV& g){
		auto& p = *static_cast<Plot *>(&v);
		auto it = p.plottables().begin();
		while(it != p.plottables().end()){
			(**it).onContextCreate();
			++it;
		}
		return true;
	}
} evPlotCreateContext;

struct EvPlotDestroyContext : public EventHandler{
	bool onEvent(View& v, GLV& g){
//		printf("destroy\n");
		auto& p = *static_cast<Plot *>(&v);
		auto it = p.plottables().begin();
		while(it != p.plottables().end()){
			(**it).onContextDestroy();
			++it;
		}
		return true;
	}
} evPlotDestroyContext;

Plot::Plot(const Rect& r)
:	Grid(r)
{
	data().type(Data::FLOAT);
	addHandler(Event::WindowCreate, evPlotCreateContext);
	addHandler(Event::WindowDestroy, evPlotDestroyContext);
	addHandler(Event::Quit, evPlotDestroyContext);
}

Plot::Plot(const Rect& r, Plottable& p)
:	Plot(r)
{
	add(p);
}

Plot::Plot(const Rect& r, Plottable& p1, Plottable& p2)
:	Plot(r)
{
	add(p1); add(p2);
}

void Plot::onDraw(GLV& g){

	auto& gd = g.graphicsData();
	draw::color(colors().fore);

	{ pushGrid();
		for(auto& plottable : plottables()){
			auto& p = *plottable;
			if(p.active() && p.drawUnderGrid()){
				gd.reset();
				gd.colors()[0] = p.color();
				p.doPlot(gd, p.data().hasData() ? p.data() : data());
			}
		}

	popGrid(); }

	Grid::onDraw(g);

	draw::color(colors().fore);

	// push into grid space and call attached plottables
	{ pushGrid();
		for(auto& plottable : plottables()){
			auto& p = *plottable;
			if(p.active() && !p.drawUnderGrid()){
				gd.reset();
				gd.colors()[0] = p.color();
				p.doPlot(gd, p.data().hasData() ? p.data() : data());
			}
		}
	popGrid(); }
}

bool Plot::onEvent(Event::t e, GLV& g){
	if(Grid::onEvent(e,g)){
		
		const auto& k = g.keyboard();
		
		if(e == Event::KeyDown){
			if(k.isNumber()){
				unsigned i = k.keyAsNumber();
				i = i>0 ? i-1 : 10;
				if(i < plottables().size() && plottables()[i]){
					auto& p = *plottables()[i];
					p.active(!p.active());

				}
				return false;
			}
			else{
				auto strokeInc = [this](double ds){
					for(auto& p:plottables()){
						auto newStroke = p->stroke()+ds;
						if(newStroke >= 1) p->stroke(newStroke);
					}
				};
				switch(k.key()){
					case '>': strokeInc( 1); return false;
					case '<': strokeInc(-1); return false;
				}
			}
		}
		
		return true;
	}
	return false;
}

Plot& Plot::add(Plottable& v){
	if(std::find(mPlottables.begin(), mPlottables.end(), &v) == mPlottables.end()){
		mPlottables.push_back(&v);
	}
	return *this;
}

Plot& Plot::remove(Plottable& v){
	// why the f*!@ doesn't this work!
	//std::remove(mPlottables.begin(), mPlottables.end(), &v);

	for(unsigned i=0; i<plottables().size(); ++i){
		auto * p = plottables()[i];
		if(p == &v){
			mPlottables.erase(mPlottables.begin() + i);
			break;
		}		
	}

	return *this;
}

//
///// Density plot
//
///// Model data should have a shape of (nc, nx, ny) where 'nc' si the number of
///// components of the value (e.g., a scalar is 1) and 'nx' and 'ny' are the size
///// of the x and y dimensions, respectively.
//class DensityPlot : public Plot {
//public:
//	DensityPlot(const Rect& r=Rect());
//
//	DensityPlot& color(const Color& v){ mColor1=v; return *this; }
//
//	virtual const char * className() const { return "DensityPlot"; }
//	virtual void onDraw(GLV& g);
//
//protected:
//	Color mColor1;
////	float val(int i, int j, int k){ return to01(data().at<float>(i,j,k)); }
//	float val(int i, int j, int k){ return (data().at<float>(i,j,k)); }
//};


//DensityPlot::DensityPlot(const Rect& r)
//:	Plot(r)
//{
//	data().type(Data::DOUBLE);
//}
//
//void DensityPlot::onDraw(GLV& g){
//	using namespace glv::draw;
//	if(data().size() == 0) return;
//
//	float xd = dx(1);
//	float yd = dy(2);
//	int N0 = data().size(0);
//	int N1 = data().size(1);
//	int N2 = data().size(2);
////	int order = model().order();
//	HSV hsv = mColor1;
//
///*
//  .   .   .
//1        
//  .   .   .
//0        
//  .   .   .
//    0   1
//
//prim			Nv	Ni
//Quads			4N	4N
//Triangles		4N	6N
//TriangleStrip	4N	6N
//*/
//
////	int Np = (N1+1)*(N2+1);
////	Color cols[Np];
////	Point2 pts[Np];
////	unsigned inds[N1*2*N2 + 2*N2];
////	int ii=-1;
////	int iv=0;
////
////	glShadeModel(GL_FLAT);
////
////	for(int j=0; j<N2+1; ++j){
////		for(int i=0; i<N1+1; ++i){
////			float x = xd*i;
////			float y = yd*j;
////			int i1 = i!=N1 ? i : i-1;
////			int i2 = j!=N2 ? j : j-1;
////
////			Color c;
////			switch(N0){
////			case 1: c = HSV(hsv.h, hsv.s, hsv.v*val(0,i1,i2)); break;
////			case 2: c = HSV(hsv.h, hsv.s*val(1,i1,i2), hsv.v*val(0,i1,i2)); break;
////			case 3:	c = Color(val(0,i1,i2), val(1,i1,i2), val(2,i1,i2), 1); break;
////			default:c = Color(val(0,i1,i2), val(1,i1,i2), val(2,i1,i2), val(3,i1,i2));
////			}
////			
////			cols[iv] = c;
////			pts[iv](x,y);
////			++iv;
////			
////			if(j < N2){
////				inds[++ii] = (j  )*(N1+1) + i;
////				inds[++ii] = (j+1)*(N1+1) + i;
////				
////				if(i == N1){
////					inds[ii+1] = inds[ii]; ++ii;
////					inds[++ii] = (j+1)*(N1+1);
////				}
////			}
////		}
////	}
////
////	draw::paint(draw::TriangleStrip, pts, cols, inds, ii);
////
////	glShadeModel(GL_SMOOTH);
//
//#ifdef GLV_USE_OPENGL_ES
//	GraphicsData& b = g.graphicBuffers();
//	b.reset();
//
//	for(int j=0; j<N2; ++j){
//		float y = yd*j;
//		for(int i=0; i<N1; ++i){
//			float x = xd*i;
//
//			Color c;
//			switch(N0){
//			case 1: c = HSV(hsv.h, hsv.s, hsv.v*val(0,i,j)); break;
//			case 2: c = HSV(hsv.h, hsv.s*val(1,i,j), hsv.v*val(0,i,j)); break;
//			case 3:	c = Color(val(0,i,j), val(1,i,j), val(2,i,j), 1); break;
//			default:c = Color(val(0,i,j), val(1,i,j), val(2,i,j), val(3,i,j));
//			}
//			
//			int ind = b.vertices2().size();
//			b.addColor(c,c,c,c);			
//			b.addVertex2(x,y, x+xd,y, x+xd,y+yd, x,y+yd);
//			b.addIndex(ind+0, ind+1, ind+3);
//			b.addIndex(ind+1, ind+2, ind+3);
//		}
//	}
//	
//	paint(Triangles, b);
//#else
//	GraphicsData& b = g.graphicsData();
//	b.reset();
//
//	for(int j=0; j<N2; ++j){
//		float y = yd*j;
//		for(int i=0; i<N1; ++i){
//			float x = xd*i;
//
//			Color c;
//			switch(N0){
//			case 1: c = HSV(hsv.h, hsv.s, hsv.v*val(0,i,j)); break;
//			case 2: c = HSV(hsv.h, hsv.s*val(1,i,j), hsv.v*val(0,i,j)); break;
//			case 3:	c = Color(val(0,i,j), val(1,i,j), val(2,i,j), 1); break;
//			default:c = Color(val(0,i,j), val(1,i,j), val(2,i,j), val(3,i,j));
//			}
//			
//			b.addColor(c,c,c,c);			
//			b.addVertex2(x,y, x+xd,y, x+xd,y+yd, x,y+yd);
//		}
//	}
//	paint(Quads, b);
//#endif
//}


} // glv::

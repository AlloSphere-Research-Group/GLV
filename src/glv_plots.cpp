/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <float.h>
#include "glv_util.h"
#include "glv_plots.h"

namespace glv{

#define CTOR_INIT_LIST(size)\
	mSize(0), mIMin(0), mIMax(size),\
	mColor(0), mColors(0),\
	mDrawPrim(draw::LineStrip), mStroke(1),\
	mDotEnds(false), mDotPoints(false), mInterpolate(false)

#define CTOR_INIT\
	for(int i=0; i<DIMS; ++i) mPoints[i]=mPointsRef[i]=0;

PlotData::PlotData()
:	CTOR_INIT_LIST(0)
{
	CTOR_INIT
}

PlotData::PlotData(int size, PlotDim::t dim)//, const Color& plotColor)
:	CTOR_INIT_LIST(size)
{
	CTOR_INIT
	resize(size, dim);
}

#undef CTOR_INIT_LIST
#undef CTOR_INIT

PlotData::~PlotData(){
	free();
	if(mColor) delete mColor;
}

PlotData& PlotData::drawType(int primitive){ mDrawPrim=primitive; return *this; }

PlotData& PlotData::interval(int mn, int mx){
	sort(mn,mx);
	if(mn<0) mn=0;
	if(mx>size()) mx=size();
	mIMin=mn; mIMax=mx;
	return *this;
}

PlotData& PlotData::color(const Color& c){
	if(!mColor) mColor = new Color(c);
	else mColor->set(c);
	return *this;
}

PlotData& PlotData::dotEnds(bool v){ mDotEnds=v; return *this; }
PlotData& PlotData::dotPoints(bool v){ mDotPoints=v; return *this; }
PlotData& PlotData::interpolate(bool v){ mInterpolate=v; return *this; }

//PlotData& PlotData::plotDim(PlotDim::t v){
//	mPlotDim=v;
//	switch(v){
//	case PlotDim::X:
//		for(int i=0; i<size(); ++i){ mPoints[i].y = i; }
//		break;
//	case PlotDim::Y:
//		for(int i=0; i<size(); ++i){ mPoints[i].x = i; }
//		break;
//	default:;
//	}
//	return *this;
//}

PlotData& PlotData::resize(int n, PlotDim::t dim){
	if(n != size()){
		mSize = n;
		free();
		for(int i=0; i<DIMS; ++i){
			if(dim & (1<<i)){ mPoints[i] = new float[n]; }
		}
		interval(mIMin, mIMax);
	}
	return *this;
}

PlotData& PlotData::stroke(float v){ mStroke=v; return *this; }

PlotData& PlotData::zero(){
	for(int j=0; j<DIMS; ++j){
		if(mPoints[j]){
			for(int i=0; i<size(); ++i) mPoints[j][i] = 0;
		}
	}
	return *this;
}

float * PlotData::points(int dim) const {
	if(mPointsRef[dim]) return mPointsRef[dim];
	return mPoints[dim];
}

Color * PlotData::colors(){ if(0 == mColors) allocColors(); return mColors; }





#define CTOR_INIT_LIST(view) \
	View(view),\
	mTickMajorX(1), mTickMajorY(1),\
	mShowAxes(true), mPreserveAspect(false)

#define CTOR_INIT\
	for(int i=0; i<DIM; ++i){ mMin[i]=-1; mMax[i]=1; }

Plot::Plot(const Rect& r)
:	CTOR_INIT_LIST(r)
{	CTOR_INIT }

Plot::Plot(const Rect& r, const Color& c)
:	CTOR_INIT_LIST(r)
{
	CTOR_INIT
	createData(0, PlotDim::X)->color(c);
}


Plot::Plot(const Rect& r, int size, PlotDim::t dim)
:	CTOR_INIT_LIST(r)
{
	CTOR_INIT
	createData(size, dim);
}

Plot::Plot(const Rect& r, int size, PlotDim::t dim, const Color& c)
:	CTOR_INIT_LIST(r)
{
	CTOR_INIT
	createData(size, dim)->color(c);
}

#undef CTOR_INIT_LIST

Plot::~Plot(){
	std::set<PlotData *>::iterator it = mOwnData.begin();
	for(; it!=mOwnData.end(); ++it){
		delete *it;
	}
}

Plot& Plot::addData(PlotData& v){
	mData.push_back(&v);
	return *this;
}

PlotData * Plot::createData(int size, PlotDim::t dim){
	PlotData * r = new PlotData(size, dim);
	mOwnData.insert(r);
	mData.push_back(r);
	return r;
}

Plot& Plot::center(){
	rangeX((mMax[0]-mMin[0])*0.5);
	rangeY((mMax[1]-mMin[1])*0.5);
	return *this;
}

Plot& Plot::equalizeAxes(){
	// make extents in each dimension equal to max
	float dx=mMax[0]-mMin[0], cx=mean(mMin[0], mMax[0]);
	float dy=mMax[1]-mMin[1], cy=mean(mMin[1], mMax[1]);
	
	float mx = max(dx,dy)*0.5;
	mMin[0] = cx - mx;
	mMax[0] = cx + mx;
	mMin[1] = cy - mx;
	mMax[1] = cy + mx;

	return *this;
}

Plot& Plot::preserveAspect(bool v){ mPreserveAspect=v; return *this; }

Plot& Plot::rangeFit(float padPercentage){

	if(mData.size() != 0 && data(0).size() != 0){
	
		float mn[DIM], mx[DIM];
		
		for(int k=0; k<DIM; ++k){
			mn[k] = FLT_MAX;
			mx[k] =-FLT_MAX;
		}

		for(unsigned j=0; j<mData.size(); ++j){
			for(int i=0; i<data(j).size(); ++i){
				for(int k=0; k<DIM; ++k){
					float * pts = data(j).points(k);
					float v = pts ? pts[i] : i;	// if independent, use index
					if(v < mn[k]) mn[k]=v;
					if(v > mx[k]) mx[k]=v;
				}
			}
		}
		
		float a[DIM];
		for(int k=0; k<DIM; ++k){
			a[k] = (mx[k]-mn[k]) * (padPercentage/2./100.);
		}
		rangeX(mn[0]-a[0], mx[0]+a[0]);
		rangeY(mn[1]-a[1], mx[1]+a[1]);
//		printf("%f %f %f %f\n", xmn,xmx,ymn,ymx);
	}
	return *this;
}

Plot& Plot::range(float v){ rangeX(v); return rangeY(v); }
Plot& Plot::rangeX(float v){ return rangeX(-v, v); }
Plot& Plot::rangeY(float v){ return rangeY(-v, v); }
Plot& Plot::rangeX(float mn, float mx){ sort(mn, mx); mMax[0]=mx; mMin[0]=mn; return *this; }
Plot& Plot::rangeY(float mn, float mx){ sort(mn, mx); mMax[1]=mx; mMin[1]=mn; return *this; }
Plot& Plot::showAxes(bool v){ mShowAxes=v; return *this; }	
Plot& Plot::tickMajor(float v){ return tickMajorX(v).tickMajorY(v); }
Plot& Plot::tickMajorX(float v){ mTickMajorX=v; return *this; }
Plot& Plot::tickMajorY(float v){ mTickMajorY=v; return *this; }

PlotData& Plot::data(int i){ return *mData[i]; }

//Color * Plot::colors(int i){ return data(i).colors(); }
//Point2 * Plot::points(int i){ return data(i).points(); }

void Plot::plotExtent(float& pw, float& ph){
	pw=w, ph=h;
	if(mPreserveAspect){// && mPlotDim == PlotDim::XY){
		w>h ? pw=h : ph=w;
	}
}


void Plot::draw(const PlotData& d, float mulX, float addX, float mulY, float addY, const Color& defaultCol){
	using namespace glv::draw;

	bool drawEndpoints = d.dotEnds() && !d.dotPoints() && d.isXY();
	int B=d.mIMin, E=d.mIMax;					// begin/end plotting indices
	int N = E-B;

	// copy points over
	if((int)mPoints.size() < N){
		mPoints.resize(N);
	}
	
	for(int j=0; j<d.DIMS; ++j){
		int s = d.stride(j);
		float * pts = d.points(j);
		if(pts){
			for(int i=0; i<N; ++i){
				mPoints[i].elems[j] = pts[i*s];
			}
		}
		
		// if independent, use index
		else{
			for(int i=0; i<N; ++i){
				mPoints[i].elems[j] = i;
			}			
		}
	}

	draw::push(ModelView);
		draw::translate(addX, addY, 0);
		draw::scale(mulX, mulY, 1);
		draw::color(d.mColor ? *d.mColor : defaultCol);
		
		// draw first endpoint
		if(drawEndpoints){
			pointSize(d.stroke()*2);
			if(d.mColors) draw::color(d.mColors[B]);
			draw::paint(Points, &mPoints[0], 1);
		}
	
	// draw buffer

//		#define DRAW_BUFFERS\
//			if(mBufX && mBufY){			/* xy-plot */\
//				const float h1=-0.0625, h2=0.5625;\
//				int im1=0, ip1=1, ip2=2;\
//				for(int i=B; i<E; ++i){\
//					if(mBufCol) color(mBufCol[i]);\
//					vertex(mBufX[i]*mulX+addX, mBufY[i]*mulY+addY);\
//					if(mInterpolate){\
//						ip2 = i+2; if(ip2>=E) ip2 = E-B-1;\
//						vertex(\
//							(mBufX[im1]*h1 + mBufX[i]*h2 + mBufX[ip1]*h2 + mBufX[ip2]*h1)*mulX+addX,\
//							(mBufY[im1]*h1 + mBufY[i]*h2 + mBufY[ip1]*h2 + mBufY[ip2]*h1)*mulY+addY\
//						);\
//						im1=i; ip1=ip2;\
//					}\
//				}\
//			}\
//			else if(mBufX && !mBufY){	/* x-axis plot */\
//				double dx = w/(E-B-1), x=0;\
//				for(int i=B; i<E; ++i){\
//					if(mBufCol) color(mBufCol[i]);\
//					vertex(x, (mBufX[i]*mulY+addY)); x+=dx;\
//				}\
//			}\
//			else if(mBufY && !mBufX){	/* y-axis plot*/\
//				double dy = h/(E-B-1), y=0;\
//				for(int i=B; i<E; ++i){\
//					if(mBufCol) color(mBufCol[i]);\
//					vertex(mBufY[i]*mulX+addX, y); y+=dy;\
//				}\
//			}\

//	#define DRAW_BUFFERS\
//	{\
//		const float h1=-0.0625, h2=0.5625;\
//		int im1=0, ip1=1, ip2=2;\
//		for(int i=B; i<E; ++i){\
//			if(mBufCol) color(mBufCol[i]);\
//			vertex(mBufX[i]*mulX+addX, mBufY[i]*mulY+addY);\
//			if(mInterpolate){\
//				ip2 = i+2; if(ip2>=E) ip2 = E-B-1;\
//				vertex(\
//					(mBufX[im1]*h1 + mBufX[i]*h2 + mBufX[ip1]*h2 + mBufX[ip2]*h1)*mulX+addX,\
//					(mBufY[im1]*h1 + mBufY[i]*h2 + mBufY[ip1]*h2 + mBufY[ip2]*h1)*mulY+addY\
//				);\
//				im1=i; ip1=ip2;\
//			}\
//		}\
//	}

		#define DRAW_BUFFERS(prim)\
			if(d.mColors){\
				draw::paint(prim, &mPoints[0], d.mColors+B, N);\
			}\
			else{\
				draw::paint(prim, &mPoints[0], N);\
			}

		draw::stroke(d.stroke());

		DRAW_BUFFERS(d.mDrawPrim);
		if(d.dotPoints()){
			draw::stroke(d.stroke()*2);
			DRAW_BUFFERS(Points);
		}
	
		// draw last endpoint
		if(drawEndpoints){
			pointSize(d.stroke()*2);
			if(d.mColors) draw::color(d.mColors[E-1]);
			draw::paint(Points, &mPoints[N-1], 1);
		}

	draw::pop(ModelView);
}



void Plot::onDraw(GLV& g){
	using namespace glv::draw;

	draw::enable(PointSmooth);
	
	// In XY mode, these variables are used to keep the axes equalized
	float pw,ph;
	plotExtent(pw,ph);
	
	float xyAddX=(w-pw)/2;
	float xyAddY=(h-ph)/2;
	float mulX = pw / (mMax[0] - mMin[0]);	// pixels/distance along x
	float addX =-mulX*mMin[0] + xyAddX;		// pixel offset along x
	float mulY =-ph / (mMax[1] - mMin[1]);	// pixels/distance along y
	float addY = mulY*-mMax[1] + xyAddY;	// pixel offset along y
											// min and max must be flipped around zero

	// draw axes
	if(mShowAxes){
		color(colors().text);
		lineWidth(1);

//		if(hasX()){ shape(Lines, 0,addY, w,addY); }
//		if(hasY()){ shape(Lines, addX,0, addX,h); }
		shape(Lines, 0,addY, w,addY, addX,0, addX,h);
		
		struct DrawTicks{
			DrawTicks(float tick, float min, float max, float mul1, float add1, float add2, bool isX){
				if(tick > 0){
					int n = (max - min)/tick + 2;

					if(n < 128){
						// find closest multiple of mTickMajor to mMinX
						float p1 = (int(min/tick) * tick)*mul1+add1;
						float d1 = tick*mul1;
						float d2 = 4, d2m = add2-d2, d2p = add2+d2;
						
						// TODO: replace immediate mode
						begin(Lines);
						if(isX) for(int i=0; i<n; ++i){ vertex(p1, d2m); vertex(p1, d2p); p1 += d1; }
						else    for(int i=0; i<n; ++i){ vertex(d2m, p1); vertex(d2p, p1); p1 += d1; }
						end();
					}
				}			
			}
		};
	
		DrawTicks(mTickMajorX, mMin[0]-xyAddX/mulX, mMax[0]+xyAddX/mulX, mulX, addX, addY, true);
		DrawTicks(mTickMajorY,-mMax[1]+xyAddY/mulY,-mMin[1]-xyAddY/mulY,-mulY, addY, addX, false);
	}

	for(unsigned i=0; i<mData.size(); ++i)
		draw(*mData[i], mulX, addX, mulY, addY, colors().fore);

}

void Plot::zoom(float px, float py, float zm){
	float pw,ph; plotExtent(pw,ph);
	float mul = pow(2, zm*0.1);

	// These complicated linear maps are needed for XY mode when w!=h.
	// Most of this code was copied from onDraw().
	// There might be a cleaner way to do this, but for now it works...
	float xyAddX=((w-pw)/2);
	float xyAddY=((h-ph)/2);
	float mulX = pw / (mMax[0] - mMin[0]);		// pixels/distance along x
	float mulY = ph / (mMax[1] - mMin[1]);		// pixels/distance along y

	// True plotting intervals (could be extended beyond user-specified)
	float mnx = mMin[0]-xyAddX/mulX;
	float mxx = mMax[0]+xyAddX/mulX;
	float mny = mMin[1]-xyAddY/mulY;
	float mxy = mMax[1]+xyAddY/mulY;

	// Map pixel coords onto plot position
	float cx = mnx + (px/w)*(mxx-mnx);
	float cy = mny + (py/h)*(mxy-mny);
	
	// Update user-specified plotting intervals
	rangeX((mMin[0]-cx)*mul+cx, (mMax[0]-cx)*mul+cx);
	rangeY((mMin[1]-cy)*mul+cy, (mMax[1]-cy)*mul+cy);
}


bool Plot::onEvent(Event::t e, GLV& g){
	Keyboard& k = g.keyboard;
	Mouse& m    = g.mouse;

	switch(e){
	case Event::MouseDown:
		return false;
	
	case Event::MouseDrag:{
		float dx = m.dx();
		float dy = m.dy();

		if(m.left()){
			float pw,ph; plotExtent(pw,ph);
			float ax = ((mMax[0] - mMin[0])/pw) * dx;
			float ay =-((mMax[1] - mMin[1])/ph) * dy;
// TODO: constrain plot movement
//			if(hasY()) rangeX(mMinX - ax, mMaxX - ax);
//			if(hasX()) rangeY(mMinY - ay, mMaxY - ay);
			rangeX(mMin[0] - ax, mMax[0] - ax);
			rangeY(mMin[1] - ay, mMax[1] - ay);
		}
		
		if(m.right()){
			float px = m.xRel(Mouse::Right);
			float py = h-m.yRel(Mouse::Right);
			zoom(px, py, dy*0.1);
		}
		
		return false;
	}
		
	
	case Event::KeyDown:
		switch(k.key()){
			case 'c': center(); return false;
			case 'f': rangeFit(); return false;
			case 'e': equalizeAxes(); return false;
// TODO: keys for changing plot style
//			case 'd': dotPoints(!dotPoints()); return false;
//			case 'e': dotEnds(!dotEnds()); return false;
			case Key::Up: zoom(m.xRel(Mouse::Left), h-m.yRel(Mouse::Left), -1); return false;
			case Key::Down: zoom(m.xRel(Mouse::Left), h-m.yRel(Mouse::Left),1); return false;
		}

	default:;
	}

	return true;
}


DensityPlot::DensityPlot(const Rect& r)
:	Widget(r)
{
	data().type(Data::DOUBLE);
}

void DensityPlot::onDraw(GLV& g){
	using namespace glv::draw;
	if(data().size() == 0) return;

	float xd = dx(1);
	float yd = dy(2);
	int N0 = data().size(0);
	int N1 = data().size(1);
	int N2 = data().size(2);
//	int order = model().order();
	HSV hsv = mColor1;

/*
  .   .   .
1        
  .   .   .
0        
  .   .   .
    0   1

prim			Nv	Ni
Quads			4N	4N
Triangles		4N	6N
TriangleStrip	4N	6N
*/

//	int Np = (N1+1)*(N2+1);
//	Color cols[Np];
//	Point2 pts[Np];
//	unsigned inds[N1*2*N2 + 2*N2];
//	int ii=-1;
//	int iv=0;
//
//	glShadeModel(GL_FLAT);
//
//	for(int j=0; j<N2+1; ++j){
//		for(int i=0; i<N1+1; ++i){
//			float x = xd*i;
//			float y = yd*j;
//			int i1 = i!=N1 ? i : i-1;
//			int i2 = j!=N2 ? j : j-1;
//
//			Color c;
//			switch(N0){
//			case 1: c = HSV(hsv.h, hsv.s, hsv.v*val(0,i1,i2)); break;
//			case 2: c = HSV(hsv.h, hsv.s*val(1,i1,i2), hsv.v*val(0,i1,i2)); break;
//			case 3:	c = Color(val(0,i1,i2), val(1,i1,i2), val(2,i1,i2), 1); break;
//			default:c = Color(val(0,i1,i2), val(1,i1,i2), val(2,i1,i2), val(3,i1,i2));
//			}
//			
//			cols[iv] = c;
//			pts[iv](x,y);
//			++iv;
//			
//			if(j < N2){
//				inds[++ii] = (j  )*(N1+1) + i;
//				inds[++ii] = (j+1)*(N1+1) + i;
//				
//				if(i == N1){
//					inds[ii+1] = inds[ii]; ++ii;
//					inds[++ii] = (j+1)*(N1+1);
//				}
//			}
//		}
//	}
//
//	draw::paint(draw::TriangleStrip, pts, cols, inds, ii);
//
//	glShadeModel(GL_SMOOTH);

#ifdef GLV_USE_OPENGL_ES
	GraphicBuffers& b = g.graphicBuffers();
	b.reset();

	for(int j=0; j<N2; ++j){
		float y = yd*j;
		for(int i=0; i<N1; ++i){
			float x = xd*i;

			Color c;
			switch(N0){
			case 1: c = HSV(hsv.h, hsv.s, hsv.v*val(0,i,j)); break;
			case 2: c = HSV(hsv.h, hsv.s*val(1,i,j), hsv.v*val(0,i,j)); break;
			case 3:	c = Color(val(0,i,j), val(1,i,j), val(2,i,j), 1); break;
			default:c = Color(val(0,i,j), val(1,i,j), val(2,i,j), val(3,i,j));
			}
			
			int ind = b.vertices2().size();
			b.addColor(c,c,c,c);			
			b.addVertex2(x,y, x+xd,y, x+xd,y+yd, x,y+yd);
			b.addIndex(ind+0, ind+1, ind+3);
			b.addIndex(ind+1, ind+2, ind+3);
		}
	}
	
	paint(Triangles, b);
#else
	GraphicBuffers& b = g.graphicBuffers();
	b.reset();

	for(int j=0; j<N2; ++j){
		float y = yd*j;
		for(int i=0; i<N1; ++i){
			float x = xd*i;

			Color c;
			switch(N0){
			case 1: c = HSV(hsv.h, hsv.s, hsv.v*val(0,i,j)); break;
			case 2: c = HSV(hsv.h, hsv.s*val(1,i,j), hsv.v*val(0,i,j)); break;
			case 3:	c = Color(val(0,i,j), val(1,i,j), val(2,i,j), 1); break;
			default:c = Color(val(0,i,j), val(1,i,j), val(2,i,j), val(3,i,j));
			}
			
			b.addColor(c,c,c,c);			
			b.addVertex2(x,y, x+xd,y, x+xd,y+yd, x,y+yd);
		}
	}
	paint(Quads, b);
#endif
}


} // glv::

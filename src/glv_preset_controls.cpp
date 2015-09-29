/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <algorithm>
#include "glv_preset_controls.h"

namespace glv{

PresetControl::PresetControl()
:	mSearchBox(*this), mMM(NULL), mPrompt(false)
{
	init();
}

PresetControl::PresetControl(ModelManager& m)
:	mSearchBox(*this), mMM(&m), mPrompt(false)
{
	init();
}

PresetControl::~PresetControl(){}

void PresetControl::init(){
//	mTextEntry.addHandler(Event::KeyDown, mTextKeyDown);
	mStatus.extent(mSearchBox.h);
	mStatus.disable(Controllable | HitTest | DrawBorder | DrawBack);
	mStatus.symbol(draw::magnifier);
	
//	mBtnPanel.addHandler(Event::MouseDown, mPanelButtonClick);
	mBtnPanel.extent(mStatus.h);
	mBtnPanel.symbol(draw::combo<draw::frameTrunc<1,1,1,1>, draw::crosshatch<2,2> >);
	mBtnPanel.enable(Momentary);
	mBtnPanel.padding(4,0).padding(8,1);
	paddingX(0).paddingY(0);
	(*this) << mStatus << mSearchBox;// << mBtnPanel;
	fit();
}


bool PresetControl::PresetSearchBox::onEvent(Event::t e, GLV& g){

	if(NULL != pc.mMM){

	ModelManager& mm = *pc.mMM;
	const Keyboard& k = g.keyboard();

	if(Event::KeyDown == e){
		items().clear();
		ModelManager::Snapshots::const_iterator it = mm.snapshots().begin();
		while(it != mm.snapshots().end()){
			addItem(it->first);
			++it;
		}
	}

	switch(e){
	case Event::KeyDown:
		pc.mStatus.symbol(draw::magnifier);
		
		if((k.key() != 's' && k.key() != Key::Delete && k.key() != Key::Backspace) || !k.ctrl()){
			pc.mPrompt=false;
		}

		switch(k.key()){
		case 's':
			if(k.ctrl()){
				if(!empty()){
					const std::string& name = getValue();
					if(!pc.mPrompt && mm.snapshots().count(name)){
						pc.mPrompt = true;
						pc.mStatus.symbol(draw::question);
						//pc.mStatus.symbol(draw::polygonCut<16,45>);
						//pc.mStatus.symbol(draw::spokes<8,90>);
					}
					else{
						pc.mPrompt = false;
						pc.mStatus.symbol(draw::fileSave);
						mm.saveSnapshot(name);
						mm.snapshotsToFile();
					}
				}
				return false;
			}
			break;
		case Key::Backspace:
		case Key::Delete:
			if(k.ctrl()){
				if(!empty()){
					const std::string& name = getValue();
					if(mm.snapshots().count(name)){
						if(!pc.mPrompt){
							pc.mPrompt = true;
							pc.mStatus.symbol(draw::question);
						}
						else{
							pc.mPrompt = false;
							pc.mStatus.symbol(draw::x);
							mm.snapshots().erase(name);
							mm.snapshotsToFile();
						}					
					}

				}
				return false;
			}
			break;
		case Key::Enter:
		case Key::Return:
			if(!listShowing()){ // only attempt load if not selecting from list
				if(!mm.loadSnapshot(getValue())){
					printf("Could not load snapshot \"%s\"\n", getValue().c_str());
				}
				else{
					pc.mStatus.symbol(draw::fileLoad);
				}
			}
			break;
		default:;
		}
	default:;
	}
	
	} // NULL != mMM

	bool res = SearchBox::onEvent(e,g);

	return res;
}


bool PresetControl::setPreset(const std::string& v){
	if(NULL != mMM && mMM->snapshots().count(v)){
		mMM->loadSnapshot(v);
		mSearchBox.setValue(v);
		mSearchBox.cursorEnd();
		return true;		
	}
	return false;
}


bool PresetControl::loadFile(){
	if(NULL == mMM){
		fprintf(stderr, "From PresetControl::loadFile: Attempt to load file without a ModelManager\n");
	}
	else if(mMM->snapshotsFromFile()){
		setPreset("default");
		return true;
	}
	return false;
}

void PresetControl::onDraw(GLV& g){
	using namespace glv::draw;

//	if(mPrompt)	mStatus.symbol(draw::polygonCut<16,45>);
//	else			mStatus.symbol(0);
	
	//mPanel.property(Visible, mBtnPanel.getValue());

}

bool PresetControl::onEvent(Event::t e, GLV& g){

	switch(e){
	case Event::MouseDown:
	case Event::MouseDrag:
		return false;
	
	case Event::KeyDown:
		switch(g.keyboard().key()){
		default:;
		}
		break;
	
	default:;
	};

	return true;
}




template <class T>
static void insertCopy(std::vector<T>& src, int to, int from){
	if(src.empty()){
		src.push_back(T());
	}
	else{
		src.insert(src.begin() + to, src[from]);
	}
}

template <int D>
static void pointerLine(float l, float t, float r, float b){
	float h2 = (t+b)/2;
	float pts[] = {
		D, h2,
		0, h2-D,
		0, h2+D,
		D, h2,
		r, h2
	};
	draw::paint(draw::LineStrip, (Point2*)pts, GLV_ARRAY_SIZE(pts)/2);
}



PathView::PathView(space_t width)
:	Widget(Rect(width, 20)),
	mStates(0),
	mDurModel(*this),
	mDur(3,3, 990,0), mCrv(2,1, 90,-90), mSmt(1,1, 8,-8), // mName(Rect(200,12), 6)
	mPos(0), mPlaying(false)
{
//	data().resize(Data::STRING);
//	data().resize(Data::NONE).shape(1,8);
//	data().print();
	disable(DrawSelectionBox);
//	enable(DrawSelectionBox);
	enable(CropChildren);
	disable(DrawGrid);

	mDur.l = seqRight();
	mCrv.l = mDur.right();
	mSmt.l = mCrv.right();
	mName.l= mSmt.right()+8;
	
	mDur.disable(DrawBorder | DrawBack);
	mCrv.disable(DrawBorder | DrawBack);
	mSmt.disable(DrawBorder | DrawBack);
	mName.disable(DrawBorder | DrawBack);
	mName.searchBox().disable(DrawBorder | DrawBack);
	
	mCrv.attach(ntUpdatePlot, Update::Value, this);
	mSmt.attach(ntUpdatePlot, Update::Value, this);
	
	//mPlotWarp.disable(DrawBorder);
	
//	mPathMM.add("dur", mDur);
//	mPathMM.add("crv", mCrv);
//	mPathMM.add("smt", mSmt);
//	mPathMM.add("name", mName);

	(*this) << mDur << mCrv << mSmt << mName;
}

void PathView::ntUpdatePlot(const Notification& n){
	n.receiver<PathView>()->updatePlot();
}


float PathView::duration() const {
	float r = 0;
	for(unsigned i=0; i<mPath.size(); ++i){
		r += mPath[i].dur;
	}
	return r;
}


PathView& PathView::modelManager(ModelManager& v){
	mStates = &v;
	mName.modelManager(v);
	return *this;
}

void PathView::onAnimate(double dsec){
	if(mPlaying && mPath.size()>0){

		double max = mPath.size()-1;

		if(mPos >= max){		// clip current position
			mPos = max;
			mPlaying = false;	// stop playing when end is reached
		}
		else if(mStates){		// are there presets assigned?
			
			// okay, updating time is complicated...
			int idx = int(mPos);			// current keyframe index
			double dur = mPath[idx].dur;	// current keyframe duration
			double frac = mPos - idx;		// fraction through keyframe
			double timeNow = frac * dur;	// relative time within keyframe
			double timeNext= timeNow + dsec;// 
			
			// time update leaves us in the same keyframe
			if(timeNext < dur){
				mPos = idx + timeNext/dur;
				loadCurrentPos();
			}
			
			// time update puts us beyond current keyframe
			else{
				//printf("went over!\n");
				double timeOver = timeNext - dur;
				mPos = idx + 1;
				loadCurrentPos();
				onAnimate(timeOver);
			}
		}
	}
	else { // for when mPath.size() <= 0
		mPlaying = false;
	}
}


void PathView::drawHeader(float x, float y){
//	float y =-10 + 0.;
//	float x = 0.;
	float textSize = 6;
	draw::stroke(1);
	draw::text("dur" , mDur.l + x, y, textSize);
	draw::text("crv" , mCrv.l + x, y, textSize);
	draw::text("smt" , mSmt.l + x, y, textSize);
	draw::text("name", mName.l+ x, y, textSize);
}


void PathView::onDraw(GLV& g){

	if(NULL == mStates) return;

	int Nk = mPath.size();

	data().shape(1,Nk);
	
	{
		//drawHeader(0, 0);

		// draw selected row highlight
		draw::color(Color(colors().text.mix(colors().back, 0.8)));
		draw::frame(0, selected()*dy(), right(), (selected()+1)*dy());

		// draw start cursor
		draw::stroke(2);
		draw::color(colors().selection);
		float pixStart = draw::pixc(mStart*dy());
		pointerLine<6>(l,pixStart-8, right(),pixStart+8);

		// draw position indicator
		//draw::color(colors().selection.mix(colors().back, 0.8)); // row hilite bar
		//draw::rectTrunc<2,2,2,2>(2, dy()*mPos, w-2, dy()*(mPos+1));
		draw::color(colors().selection);
		draw::rectTrunc<2,2,2,2>(2, dy()*mPos, seqRight()-4, dy()*(mPos+1));

		Rect vrect = visibleRegion();
//		vrect.print();

		int jbeg = vrect.top()   /dy();
		int jend = (vrect.bottom()/dy()) + 1;
		if(jend > sizeY()) jend = sizeY();

//		draw::color(1,0,0);
//		draw::x(vrect.left(), vrect.top(), vrect.right(), vrect.bottom());			
//		printf("%d %d\n", jbeg, jend);

		draw::stroke(1);
		std::string s;
//		float px = 0.375, py = 0.375;
		float px = 0.5, py = 0.5;

		Color seqCol = colors().text;
		seqCol = seqCol.mixRGB(colors().back, 0.2);

		/*printf("%7.6g\n",   1.);
		printf("%7.6g\n",  10.);
		printf("%7.6g\n", 100.);
		printf("%7.6g\n", 100.025);*/

		for(int j=jbeg; j<jend; ++j){
			float y = getY(j) + py;

			draw::color(seqCol);
			toString(s, j, "%03i");	draw::text(s.c_str(), px+startRight(), y+2, 6);

			if(selected() != j){
				draw::color(colors().text);
				Keyframe& kf = mPath[j];
				toString(s, kf.dur);		draw::text(s.c_str(), mDur.l + px, y);
				toString(s, kf.crv, "% g");	draw::text(s.c_str(), mCrv.l + px, y);
				toString(s, kf.smt);		draw::text(s.c_str(), mSmt.l + px, y);
				draw::text(kf.name.c_str(), mName.l, y);
			}
		}
	}
	
//	int numBytes = data().resize(1, Nk);
//	if(0 != numBytes){
//		
//	}

//	const ModelManager::Snapshots& keys = mPathMM.snapshots();
//	int Ns = keys.size();
//	
//	int numBytes = data().resize(Data::STRING, 1, Ns);
//	if(0 != numBytes){
//		
//		ModelManager::Snapshots::const_iterator it = keys.begin();
//		int idx=0; // local array index
//		
//		while(keys.end() != it){
//			data().assign<std::string>(it->first, 0, idx);
//			++idx;
//			++it;
//		}
//	}
	
	//printf("resize: %d\n", numBytes);

	fitExtent();

	Widget::onDraw(g);
}

bool PathView::onEvent(Event::t e, GLV& g){
	//if(Event::MouseDown == e) selectFromMousePos(g);

	const Keyboard& k = g.keyboard();
	const Mouse& m = g.mouse();
	const float mx = m.xRel();
	const float my = m.yRel();
	const float cx = m.xRel(Mouse::Left);

	switch(e){
	case Event::KeyDown:
		switch(k.key()){
		case 'd':
			if(k.ctrl()){
				// duplicate currently selected frame
				insertCopy(mPath, selected()+1, selected());
				return false;
			}
			break;
		case 's':
			if(k.ctrl()){
				saveFile();
				return false;
			}
			break;
		case 'l':
			if(k.ctrl()){
				loadFile();
				return false;
			}
			break;
		case ' ':	// toggle playback
			mPlaying ^= 1;
			return false;
		case Key::Enter:
		case Key::Return:
			mPos = mStart;
			return false;
		case Key::Delete:
		case Key::Backspace:
			if(k.ctrl()){
				mPath.erase(mPath.begin()+selected());
				data().size(1, mPath.size());
				if(mPath.size()){
					select(selected());
					onCellChange(selected(), selected());
				}
				return false;
			}
			break;
		case Key::Up:
			if(k.shift()){ // move up
				if(selected()>0 && mPath.size()>1){
					std::swap(mPath[selected()], mPath[selected()-1]);
					select(selected()-1);
				}
				return false;
			}
			break;
		case Key::Down:
			if(k.shift()){ // move down
				if(selected()<((int)mPath.size()-1) && mPath.size()>1){
					std::swap(mPath[selected()], mPath[selected()+1]);
					select(selected()+1);
				}
				return false;
			}
			break;
		default:;
		}
		break;

	case Event::MouseDown:
		if(mx > seqRight()){
			selectFromMousePos(g);
		}
		//else if(mx <= startRight()){
		else if(Mouse::Right == m.button()){
			int yi = my / dy();
			mStart = glv::clip<int>(yi, mPath.size()-1);
		}
		else{
			return onEvent(Event::MouseDrag, g);
		}
		return false;

	case Event::MouseDrag:
//		//printf("%d\n", yi);
//		if(e == Event::MouseDown && mx >= seqRight()){
//			selectFromMousePos(g);
//		}
//		else if(mx <= startRight()){
//			int yi = my / dy();
//			mStart = glv::clip<int>(yi, mPath.size()-1);
//		}
//		else{
//			float y = my / dy();
//			mPos = glv::clip<float>(y, mPath.size()-1);
//			loadCurrentPos();			
//		}
		//break;
		if(cx > startRight() && cx <= seqRight()){
			float y = my / dy();
			mPos = glv::clip<float>(y, mPath.size()-1);
			loadCurrentPos();
		}
		return false;

	default:;
	}
	
	bool res = Widget::onEvent(e,g);
	return res;
}

void PathView::onCellChange(int iOld, int iNew){
	float y = iNew * dy();
	mDur.t = getY(iNew);
	mCrv.t = getY(iNew);
	mSmt.t = getY(iNew);
	mName.t= y;

	Keyframe& kf = mPath[iNew];

	mDur.data().set(kf.dur);
	mCrv.data().set(kf.crv);
	mSmt.data().set(kf.smt);
	mName.searchBox().data().set(kf.name);
	mName.searchBox().setValue(mName.searchBox().getValue());
	//mName.searchBox().selectAll();

	updatePlot();
}

void PathView::updatePlot(){
	if(0 == mPath.size()) return;

	const int N = mPlotWarp.w/2;	
	mPlotWarp.range(0,N-1, 0);
	mPlotWarp.range(-1/mPlotWarp.h,1+1/mPlotWarp.h, 1);
	mPlotWarp.showAxis(false).showGrid(false);
	mPlotWarp.disable(Controllable);

	float crv = mCrv.getValue();
	float smt = mSmt.getValue();
	Data& warpPoints = mPlotWarp.data();
	warpPoints.resize(Data::FLOAT, 1,N);

	for(int i=0; i<N; ++i){
		float phs = float(i)/(N-1);
		warpPoints.elem<float>(i) = warp(phs, crv, smt);
	}
}

std::string PathView::pathsName() const {
	return (mStates ? mStates->name() : "") + "Paths";
}

void PathView::loadFile(){
	const std::string snapshotName("default");

	ModelManager& mm = mPathMM;

	mm.clearModels();
	mm.clearSnapshots();
	mm.name(pathsName());

	DataModel len, dur, crv, smt, name;

	len.data().resize(Data::INT, 1);	
	
	mm.add("len", len);
	
	//len.data().elem<int>(0) = 5;
	//len.data().print();

	mm.snapshotsFromFile();
//	printf("num snapshots = %d\n", mm.snapshots().size());

//	mm.printSnapshots();

	//(mm.snapshots()["path"]["len"])->data().print();
	if(!mm.loadSnapshot(snapshotName)){
		printf("Could not load snapshot \"%s\".\n", snapshotName.c_str());
	}

//	len.data().print();

	int N = len.data().elem<int>(0);

//	printf("%s\n", mm.snapshotsToString().c_str());
//	printf("%d\n", N);

	mm.add("dur", dur);
	mm.add("crv", crv);
	mm.add("smt", smt);
	mm.add("name", name);

	dur.data().resize(Data::FLOAT, N);
	crv.data().resize(Data::FLOAT, N);
	smt.data().resize(Data::FLOAT, N);
	name.data().resize(Data::STRING, N);

	mm.snapshotsFromFile();
	mm.loadSnapshot(snapshotName);
//	mm.printSnapshots();

	mPath.resize(N);
	for(int i=0; i<N; ++i){
		Keyframe& kf = mPath[i];
		kf.dur = dur.data().elem<float>(i);
		kf.crv = crv.data().elem<float>(i);
		kf.smt = smt.data().elem<float>(i);
		kf.name=name.data().elem<std::string>(i);
	}
	
	//Data temp;
	//mDurModel.getData(temp);
	//temp.print();
}

void PathView::saveFile(){
	ModelManager& mm = mPathMM;

	mm.name(pathsName());

	// Copy path keyframes into model data structures
	DataModel len, dur, crv, smt, name;

	int N = mPath.size();

	len.data().resize(Data::INT, 1);
	dur.data().resize(Data::FLOAT, N);
	crv.data().resize(Data::FLOAT, N);
	smt.data().resize(Data::FLOAT, N);
	name.data().resize(Data::STRING, N);

	len.data().assign(N);
	for(int i=0; i<N; ++i){
		const Keyframe& kf = mPath[i];
		dur.data().assign(kf.dur, i);
		crv.data().assign(kf.crv, i);
		smt.data().assign(kf.smt, i);
		name.data().assign(kf.name, i);
	}

	mm.clearModels();
	mm.clearSnapshots();

	mm.add("len", len);
	mm.add("dur", dur);
	mm.add("crv", crv);
	mm.add("smt", smt);
	mm.add("name", name);

	mm.saveSnapshot("default");
	mm.snapshotsToFile();
}


void PathView::fitExtent(){
	int N = mPath.size();
	if(!N) N=1;
	extent(w, N*mName.h);
}


int PathView::loadCurrentPos(){
	int ix = int(mPos);
	int iend = mPath.size()-1;
	
	if(ix >= iend){ // on the last keyframe?
		mStates->loadSnapshot(mPath[ix].name);
	}
	else{
		int iy = ix + 1;
		float f = mPos - ix;
		
		int iw = ix-1; if(iw<0) iw=0;
		int iz = ix+2; if(iz>iend) iz=iend;

		// warp fraction
		float crv = mPath[ix].crv;
		float smt = mPath[ix].smt;
		f = warp(f, crv, smt);

		const std::string& namex = mPath[ix].name;
		const std::string& namey = mPath[iy].name;
		const std::string& namew = mPath[iw].name;
		const std::string& namez = mPath[iz].name;
		//printf("%s -> %s, %g\n", namex.c_str(), namey.c_str(), frac);
		//mStates->loadSnapshot(namex, namey, 1-frac, frac);
		//mStates->loadSnapshot(name0, name1, cos(frac*M_PI/2), sin(frac*M_PI/2));

		
		double w[4];
		{			
			// Cardinal spline coefs
			
			// smoothness is (1 - tension)
			// Smoothness > 0 results in a more gradual curve, but
			// with increased overshooting. Smoothness of 0 results
			// in a line-like curve with sharp transitions at breakpoints, but
			// minimal ripple. Smoothness < 0 creates fastest curve at the
			// expense of increased undershooting.
			float c = smt*(1-f) + mPath[iy].smt*f; //1;
			
			float x[4]; // absolute times
			x[0] = 0;
			x[1] =        mPath[iw].dur;
			x[2] = x[1] + mPath[ix].dur;
			x[3] = x[2] + mPath[iy].dur;

			c *= (x[2]-x[1]);	// make domain [t[1], t[2]]
			
			// evaluate the Hermite basis functions
			float h00 = (1 + 2*f)*(f-1)*(f-1);
			float h10 = f*(f-1)*(f-1);
			float h01 = f*f*(3-2*f);
			float h11 = f*f*(f-1);	
			w[0] = (    - c*h10/(x[2]-x[0]));
			w[1] = (h00 - c*h11/(x[3]-x[1]));
			w[2] = (h01 + c*h10/(x[2]-x[0]));
			w[3] = (    + c*h11/(x[3]-x[1]));
		}

		mStates->loadSnapshot(namew, namex, namey, namez, w[0],w[1],w[2],w[3]);
	}
	return ix;
}

float PathView::warp(float x, float crv, float smt){
	static const float eps = 1e-8;
	if(crv>eps || crv<-eps){	// avoid divide by zero when c is near 0
		x = (1.f - exp(-crv*x))/(1.f - exp(-crv));
	}
	if(smt>0){
		//x = x*x*(3.f - 2.f*x);

		//float c = 1/smt;
		//x = (2*x-1)/(c + fabs(2*x-1))*(c+1)*0.5 + 0.5;
	}
	
//	if(smt <= -1)		smt =-0.999;
//	else if(smt >= 1)	smt = 0.999;
//	x = -cos(M_PI*x)/(1 + smt*cos(2*M_PI*x))*(1+smt)*0.5+0.5;
	
	/*
		// truncated rational sigmoid
		float c = 1/smt;
		x = (2*x-1)/(c + abs(2*x-1))*(c+1)*0.5 + 0.5
		
		// smooth cosine rational
		// c controls "knee" amount
		// c in [0, 0.36)	flat tops (square-like)
		// c in [0.36, 1)	overshoot
		// c in [-0.2, 0)	smooth line (triangle-like)
		// c in [x, -0.2)	mid-shelf
		x = cos(PI*x)/(1 + c*cos(2*PI*x))*(1+c)*0.5+0.5
	*/
	return x;
}


//static void ntPresetControlAction(const Notification& n){
//	printf("action 52!\n");
//}


PathEditor::PathEditor(const Rect& r)
:	Table("<<<,<--,<--")
{
	enable(DrawBack | DrawBorder);

	struct TransportSymbols{
		static void play(float l, float t, float r, float b){
			draw::shape(draw::Triangles, l,b, r,(t+b)/2, l,t);
		}

		static void pause(float l, float t, float r, float b){
			float w = r-l;
			float pts[] = {
				l,t, l,b, l+w*0.4f,t, l+w*0.4f,b,
				l+w*0.6f,t, l+w*0.6f,b, r,t, r,b
			};
			index_t idx[] = {
				0,1,2,3,
				3,4, // degenerate
				4,5,6,7
			};
			draw::paint(draw::TriangleStrip, (Point2*)pts, idx, GLV_ARRAY_SIZE(idx));
		}
	};

	mPathPresetControl.modelManager(mPathView.mPathMM);

	mTransportPlay.disable(FocusHighlight);
	mTransportPlay.symbolOn (TransportSymbols::pause);
	mTransportPlay.symbolOff(TransportSymbols::play);
	mTransportPlay.attachVariable(mPathView.mPlaying);

	mPathView.w = 0;
	mPathView.stretch(1,0);
	mPathView.disable(DrawBorder);
	mPathView.disable(DrawBack);
	mPathView.attach(ntSelection, Update::Selection, this);

	//mPathPresetControl.searchBox().attach(ntPresetControlAction, Update::Action, this);
	
	mPathView.mPlotWarp.add(
		(*new PlotFunction1D(Color(0),1,draw::TriangleStrip,PlotFunction1D::ZIGZAG)).useStyleColor(true)
	);

	const float topBarH = mPathPresetControl.h;
	mTransportPlay.extent(topBarH);
	mPathView.mPlotWarp.extent(60, topBarH);
	
	mHeader.h = 10;

	mScroll.extent(0, r.h);
	mScroll.stretch(1, 0);
	mScroll.mode(Scroll::VERTICAL);
	(*this) 
		<< mPathPresetControl << mTransportPlay << mPathView.mPlotWarp
		<< mHeader 
		<< (mScroll << mPathView);
	arrange();
}

void PathEditor::onDraw(GLV& g){
	draw::color(colors().text);
	mPathView.drawHeader(0, mHeader.top()+2);
}

void PathEditor::ntSelection(const Notification& n){
	PathView& S = *n.sender<PathView>();
	PathEditor& R = *n.receiver<PathEditor>();
	const ChangedSelection& D = *n.data<ChangedSelection>();

	Rect vrect = S.visibleRegion();
	int itop = vrect.top() / S.dy();
	int ibot = vrect.bottom() / S.dy();
	//int isel = snd.selected();

//	printf("%d %d\n", itop, ibot);

	if(D.newIndex <= itop || D.newIndex >= ibot){
		//mScroll.pageY(-1);
		//rcv.mScroll.scrollY(-snd.dy());
		R.mScroll.scrollTopTo(D.newIndex * S.dy());
		R.mScroll.pageY(0.5);
	}
}



ParamPanel::ParamPanel()
:	Table("><")
{
	*this << mPresetControl << (new Label("preset"))->size(6);
}

ParamPanel& ParamPanel::addParam(
	View& v, const std::string& label, bool nameViews
){
	*this << v << (new Label(label))->size(6);
	if(nameViews) v.name(label);
	return *this;
}

ParamPanel& ParamPanel::addParamGroup(
	View * views[], const std::string * labels[], unsigned count,
	const std::string& groupName,
	bool prefixGroup, bool nameViews
){
	std::string layout(count, 'v');
	Table& table = *new Table(layout.c_str());
	table.padding(2);
	for(unsigned i=0; i<count; ++i) table << views[i];
	for(unsigned i=0; i<count; ++i) table << (new Label(*labels[i]))->size(6);
	if(nameViews){
		for(unsigned i=0; i<count; ++i){
			const std::string& label = *labels[i];
			views[i]->name(
				(prefixGroup && groupName[0]) ? groupName + "_" + label : label
			);
		}
	}
	table.arrange();
	*this << table << (new Label(groupName))->size(6);
	return *this;
}

ParamPanel& ParamPanel::addParamGroup(
	View& v1, const std::string& l1,
	const std::string& groupName, bool prefixGroup, bool nameViews
){
	View * v[] = {&v1};
	const std::string * l[] = {&l1};
	return addParamGroup(v,l,1, groupName, prefixGroup, nameViews);
}

ParamPanel& ParamPanel::addParamGroup(
	View& v1, const std::string& l1,
	View& v2, const std::string& l2,
	const std::string& groupName, bool prefixGroup, bool nameViews
){
	View * v[] = {&v1, &v2};
	const std::string * l[] = {&l1, &l2};
	return addParamGroup(v,l,2, groupName, prefixGroup, nameViews);
}

ParamPanel& ParamPanel::addParamGroup(
	View& v1, const std::string& l1,
	View& v2, const std::string& l2,
	View& v3, const std::string& l3,
	const std::string& groupName, bool prefixGroup, bool nameViews
){
	View * v[] = {&v1, &v2, &v3};
	const std::string * l[] = {&l1, &l2, &l3};
	return addParamGroup(v,l,3, groupName, prefixGroup, nameViews);
}

ParamPanel& ParamPanel::addParamGroup(
	View& v1, const std::string& l1,
	View& v2, const std::string& l2,
	View& v3, const std::string& l3,
	View& v4, const std::string& l4,
	const std::string& groupName, bool prefixGroup, bool nameViews
){
	View * v[] = {&v1, &v2, &v3, &v4};
	const std::string * l[] = {&l1, &l2, &l3, &l4};
	return addParamGroup(v,l,4, groupName, prefixGroup, nameViews);
}

ParamPanel& ParamPanel::addParamGroup(
	View& v1, const std::string& l1,
	View& v2, const std::string& l2,
	View& v3, const std::string& l3,
	View& v4, const std::string& l4,
	View& v5, const std::string& l5,
	const std::string& groupName, bool prefixGroup, bool nameViews
){
	View * v[] = {&v1, &v2, &v3, &v4, &v5};
	const std::string * l[] = {&l1, &l2, &l3, &l4, &l5};
	return addParamGroup(v,l,5, groupName, prefixGroup, nameViews);
}

} // glv::

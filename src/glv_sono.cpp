/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <cstring> // memcpy
#include "glv_sono.h"

namespace glv{

inline float linLog2(float v, float recMin = 1./16){
	static const float lnToLog2 = 1./std::log(2.);
	v = std::log(std::abs(v) + 0.000001f)*lnToLog2;// offset to avoid -inf
	v *= recMin;
	return v > -1.f ? v + 1.f : 0.f;
}


enum{
	SYNC_WAIT = -1,		// sync on; found sync point
	SYNC_FIND = -2,		// sync on; searching for sync point
	SYNC_OFF  = -3		// sync off
};


TimeScope::TimeScope(const glv::Rect& r, int frames, int chans)
:	mSamples(NULL), mFill(0), mSync(SYNC_FIND), mLocked(false)
{
	range(-1,1, 1);
	//range( 0,1, 0);
	showAxis(false);
	major(1, 1);
	minor(4, 1);
	
	resize(frames, chans);
	
	//showAxes(false, 0);
	//rangeY(0,1);
	//showAxes(false);
	//mPlot1D.pathStyle(PlotFunction1D::ZIGZAG);
	//mPlot1D.prim(draw::TriangleStrip);
	//add(mPlot1D);
}

TimeScope::~TimeScope(){
	delete[] mSamples;
	mSamples=NULL;
}


TimeScope& TimeScope::sync(bool v){
	mSync = v ? SYNC_FIND : SYNC_OFF;
	return *this;
}


void TimeScope::resize(int frames, int chans){

	if(0 == frames || 0 == chans) return;

	while(mLocked){} // wait for locks on data (from audio thread)

	mLocked = true;

	range(-1, frames+1, 0); // offset by 1 so wave isn't hidden by borders
	major(frames, 0);
	minor(1, 0);

	if(channels() != chans){
		for(int i=0; i<channels(); ++i) remove(mGraphs[i]);
		mGraphs.resize(chans);
		for(int i=0; i<chans; ++i){
			mGraphs[i].useStyleColor(true);
			//mGraphs[i].pathStyle(PlotFunction1D::ZIGZAG);
			//mGraphs[i].prim(draw::TriangleStrip);
			add(mGraphs[i]);
		}
	}
	
	data().resize(Data::FLOAT, frames, chans);
	
	// set each plottable's slice
	for(int i=0; i<chans; ++i){
		mGraphs[i].data() = data().slice(frames*i, frames).shape(1, frames);
	}
	
	delete[] mSamples;
	mSamples = new float[frames*chans];

	mLocked = false;
}


void TimeScope::update(const float * buf, int bufFrames, int bufChans){

	if(!enabled(glv::Animate)) return;
	if(!data().hasData() || !mSamples) return;
	if(mLocked) return; // if data is locked, then just return without blocking

	mLocked = true;

	// copy new audio data into internal buffer
	const int left = frames() - mFill;
	int Nf = left < bufFrames ? left : bufFrames;
	const int Nc = bufChans <= channels() ? bufChans : channels();

	// search for sync point
	if(SYNC_FIND == mSync){
		//float maxSlope = 0;
		for(int i=1; i<Nf-1; ++i){
			float prev = buf[i-1];
			float curr = buf[i  ];
			//float next = buf[i+1];
			if(prev <= 0.f && curr > 0.f){
				//float slope = curr - prev;
				//float slope = -prev;
				//float slope = (next-curr)-(curr-prev); slope *= slope;
				//float slope = (next-prev)*0.5; slope *= slope;
				//if(slope > maxSlope){
				//	maxSlope = slope;
					mSync = i-1;
				//}
				break;
			}
		}
		if(SYNC_FIND == mSync) goto end;	// no sync found, so bail...
	}

	if(mSync >= 0){				// sync found
		for(int i=0; i<Nc; ++i){
			std::memcpy(
				mSamples + frames()*i + mFill,
				buf + bufFrames*i + mSync,
				(Nf-mSync)*sizeof(*mSamples)
			);
		}
		mFill += bufFrames - mSync;
		mSync = SYNC_WAIT;
	}
	else{
		for(int i=0; i<Nc; ++i){
			std::memcpy(mSamples + frames()*i + mFill, buf + bufFrames*i, Nf*sizeof(*mSamples));
		}
		mFill += bufFrames;
	}

	// Is our sample buffer is full? If so, copy to draw buffer...
	// Note that this is more or less thread-safe since we are merely assigning
	// floating point values.
	if(mFill >= frames()){
		data().assignFromArray(mSamples, samples());
		mFill = 0;
		if(mSync > SYNC_OFF) mSync = SYNC_FIND;
	}
	
	end:
	mLocked = false;
}


bool TimeScope::onEvent(Event::t e, GLV& g){
	const Keyboard& k = g.keyboard();
	switch(e){
	case Event::KeyDown:
		switch(k.key()){
		case ' ': toggle(glv::Animate); return false;
		default:;
		}
	default:;
	}
	
	return Plot::onEvent(e,g);
}


PeakMeters::PeakMeters(const Rect& r, int chans)
:	View(r)
{
	channels(chans);
}

PeakMeters& PeakMeters::channels(int v){
	if(v != (int)mMonitors.size())
		mMonitors.resize(v);
	return *this;
}

int PeakMeters::getMeter(float x, float y){
	int r = y / height() * channels();
	if(r<0) return 0;
	else if(r>=channels()) return channels()-1;
	return r;
}

void PeakMeters::onDraw(GLV& g){

	float dy = height() / channels();
	
	for(int i=0; i<channels(); ++i){
		// MUST be read-only for thread-safety
		const Monitor& M = mMonitors[i];
		
		float vlog = linLog2(M.lastAbs);
		float vmax = linLog2(M.max);
		
		float y0 = dy*i;
		float y1 = y0 + dy - 1;
		
		Color colGood(colors().fore);
		Color colClip(1,0,0);
		
		if(M.max > 1){
			draw::color(colClip);
		} else {
			draw::color(colGood);
		}
		draw::rectangle(0,y0, width()*vlog,y1);
		draw::color(colGood);
		draw::shape(draw::Lines, width()*vmax,y0, width()*vmax,y1);
	}
	
}

bool PeakMeters::onEvent(Event::t e, GLV& g){
	const Mouse& m = g.mouse();

	switch(e){
	case Event::MouseDrag:
	case Event::MouseDown:{
		int idx = getMeter(m.xRel(), m.yRel());
		mMonitors[idx].max = 0;
	}
		return false;
	default:;
	}
	return true;
}

} // glv::

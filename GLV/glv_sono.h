#ifndef INC_GLV_SONO_H
#define INC_GLV_SONO_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>
#include <math.h>
#include "glv_core.h"
#include "glv_plots.h"

namespace glv{


class TimeScope : public Plot{
public:

	TimeScope(const glv::Rect& r=glv::Rect(200,100), int frames=0, int chans=1);
	
	~TimeScope();

	int   frames() const { return data().size(0); }
	int channels() const { return data().size(1); }
	int  samples() const { return data().size(0,1); }


	/// Resize plot display
	
	/// This should not be called from the audio thread.
	///
	void resize(int plotFrames, int plotChans);

	/// Update scope with new audio data
	
	/// Multi-channel audio data should be in a non-interleaved format.
	/// This can be safely called from the audio thread.
	void update(float * buf, int bufFrames, int bufChans);

	/// Set whether to synchronize waveform to first positive slope zero-crossing
	TimeScope& sync(bool v);

	virtual bool onEvent(Event::t e, GLV& g);
	virtual const char * className() const { return "TimeScope"; }

protected:
	std::vector<PlotFunction1D> mGraphs;
	float * mSamples;
	int mFill;
	int mSync;
	bool mLocked;
};



/// Group of peak meters for monitoring audio
class PeakMeters : public View{
public:

	/// @param[in] r			geometry
	PeakMeters(const Rect& r=Rect(100), int chans=4);

	/// Set number of channels
	PeakMeters& channels(int v);

	/// Get number of channels
	int channels() const { return mMonitors.size(); }

	void inputSample(float v, int chan){
		mMonitors[chan](v);
	}

	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);
	virtual const char * className() const { return "PeakMeters"; }

protected:
	struct Monitor{
		float lastAbs;
		float runSum;
		float max;
		
		void operator()(float v){
			float absv = ::fabsf(v);
			lastAbs = absv;
			runSum += v;
			if(absv > max) max = absv;
		}
	};

	std::vector<Monitor> mMonitors;

	int getMeter(float x, float y);
};

} // glv::
#endif

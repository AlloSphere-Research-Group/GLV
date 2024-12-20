#ifndef INC_GLV_SONO_H
#define INC_GLV_SONO_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>
#include "glv_core.h"
#include "glv_plots.h"

namespace glv{

/// Plots time-domain signal
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

	/// This can be safely called from the audio thread.
	///
	void update(const float * buf, int bufFrames, int bufChans, bool interleaved);

	/// Set whether to synchronize waveform to first positive slope zero-crossing
	TimeScope& sync(bool v);

	bool onEvent(Event::t e, GLV& g) override;
	const char * className() const override { return "TimeScope"; }

protected:
	std::vector<PlotFunction1D> mGraphs;
	float * mSamples = nullptr;
	int mFill = 0;
	int mSync;
	bool mLocked = false;
};



/// Group of peak meters for monitoring audio
class PeakMeters : public View{
public:

	/// \param[in] r			geometry
	PeakMeters(const Rect& r=Rect(100), int chans=4);

	/// Set number of channels
	PeakMeters& channels(int v);

	/// Get number of channels
	int channels() const { return mMonitors.size(); }

	void inputSample(float v, int chan){
		mMonitors[chan](v);
	}

	void onDraw(GLV& g) override;
	bool onEvent(Event::t e, GLV& g) override;
	const char * className() const override { return "PeakMeters"; }

protected:
	struct Monitor{
		float lastAbs;
		float runSum;
		float max;
		
		void operator()(float v){
			float absv = v>0.f?v:-v;
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

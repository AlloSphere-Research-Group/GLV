#ifndef INC_GLV_PRESET_CONTROLS_H
#define INC_GLV_PRESET_CONTROLS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_behavior.h"
#include "glv_layout.h"
#include "glv_buttons.h"
#include "glv_plots.h"
#include "glv_textview.h"

namespace glv{


/// Control for searching, saving and loading presets
class PresetControl : public Box {
public:
	PresetControl();
	PresetControl(ModelManager& m);

	virtual ~PresetControl();

	bool setPreset(const std::string& v);

	bool loadFile();
	PresetControl& modelManager(ModelManager& v){ mMM = &v; return *this; }

	SearchBox& searchBox(){ return mSearchBox; }

	virtual const char * className() const { return "PresetControl"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:

//	struct PanelButtonClick : public EventHandler{
//		PanelButtonClick(PresetControl& p): pc(p){}
//
//		virtual bool onEvent(View& v, GLV& g);
//
//		PresetControl& pc;
//	} mPanelButtonClick;


//	struct SearchBox : public TextView{
//		SearchBox(PresetControl& p): pc(p), mSearchList(*this){}
//
//		virtual ~SearchBox(){ mSearchList.remove(); }
//
//		virtual bool onEvent(Event::t e, GLV& g);
//
//		PresetControl& pc;
//
//		struct SearchList : public ListView{
//			SearchList(SearchBox& v): sb(v){}
//			virtual bool onEvent(Event::t e, GLV& g);
//			SearchBox& sb;
//		} mSearchList;
//	} mSearchBox;

	struct PresetSearchBox : public SearchBox{
		PresetSearchBox(PresetControl& p): pc(p){}
		virtual bool onEvent(Event::t e, GLV& g);
		PresetControl& pc;
	} mSearchBox;

	ModelManager * mMM;
	Button mStatus, mBtnPanel;
//	PresetView mPanel;
	bool mPrompt;

private:
	void init();
};




class PathView : public Widget{
public:

	struct Keyframe{
		Keyframe(): dur(1), crv(0), smt(0), name(""){}
		float dur, crv, smt;
		std::string name;
	};


	PathView();

	PathView& modelManager(ModelManager& v);

	ModelManager& pathModelManager(){ return mPathMM; }

	void drawHeader(float x, float y);
	void loadFile();
	void saveFile();

	virtual void onAnimate(double dsec);
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);
	virtual void onCellChange(int iOld, int iNew);
	virtual const char * className() const { return "PathView"; }

protected:
	friend class PathEditor;
	// Data is an array of preset names? or nothing, just dimensions?

	ModelManager * mStates;		// States (from GUI)
	ModelManager mPathMM;		// Animation path keyframes
	
	// Animation controls
	NumberDialer mDur, mCrv, mSmt;
	PresetControl mName;
	Plot mPlotWarp;
	std::vector<Keyframe> mPath;
	double mPos;	// current sequence playback position
	double mStart;	// sequence start
	bool mPlaying;

	void updatePlot();
	void fitExtent();
	
	// Get y position of ith element
	float getY(double i) const { return dy()*i + 2.; }
	float seqRight() const { return 32; }
	float startRight() const { return 8; }

	int loadCurrentPos();

	static void ntUpdatePlot(const Notification& n);
	static float warp(float x, float crv, float smt);
	std::string pathsName() const;
};



class PathEditor : public Table{
public:
	PathEditor(space_t height=180);
	
	PathView& pathView(){ return mPathView; }
//	PresetControl& presetControl(){ return mPresetControl; }

	ModelManager& pathModelManager(){ return pathView().pathModelManager(); }

	PathEditor& stateModelManager(ModelManager& v){
		mPathView.modelManager(v); return *this;
	}
	
	virtual void onDraw(GLV& g);
	virtual const char * className() const { return "PathEditor"; }

protected:
	PresetControl mPathPresetControl;
	Scroll mScroll;
	Group mHeader;
	PathView mPathView;
	
	static void ntSelection(const Notification& n);
};






class PresetView : public View {
public:
	PresetView(ModelManager& mm);

	virtual const char * className() const { return "PresetView"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:
	ModelManager * mMM;

private:
	friend class PresetControl;
	PresetView(){}
};



/// A simple parametric control panel
class ParamPanel : public Table {
public:

	ParamPanel()
	:	Table("><")
	{
		(*this) << mPresetControl << (new Label("preset"))->size(6);
	}


	/// Add a control widget with label
	
	/// @param[in] v					The control widget
	/// @param[in] label				The control's label
	/// @param[in] nameViewFromLabel	Whether to set the control's name to the label
	ParamPanel& addParam(
		View& v, const std::string& label, bool nameViewFromLabel=true
	){
		(*this) << v << (new glv::Label(label))->size(6);
		if(nameViewFromLabel) v.name(label);
		return *this;
	}
	
	/// Get preset control
	PresetControl& presetControl(){ return mPresetControl; }

private:
	PresetControl mPresetControl;
};


} // glv::
#endif

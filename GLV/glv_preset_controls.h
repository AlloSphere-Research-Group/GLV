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


	PathView(space_t width=400);

	PathView& modelManager(ModelManager& v);


	float duration() const;
	bool isPlaying() const { return mPlaying; }

	ModelManager& pathModelManager(){ return mPathMM; }

	PathView& play(){ mPlaying=true; return *this; }
	PathView& pause(){ mPlaying=false; return *this; }

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

	template <class T, int Offset>
	struct KeyframeElemModel : public Model {
	
		PathView& pv;
		
		KeyframeElemModel(PathView& v): pv(v){}
	
		virtual const Data& getData(Data& temp) const {
			const int N = pv.mPath.size();
			temp.resize(Data::getType<T>(), N);
			for(int i=0; i<N; ++i){
				temp.elem<T>(i) = *(T*)((char*)(&pv.mPath[i]) + Offset);
			}
			return temp;
		}

		virtual void setData(const Data& d){
//			const int N = d.size();
//			if(pv.mPath.size() != N) pv.mPath.resize(N);
//			for(int i=0; i<N; ++i){
//				*(T*)((char*)(&pv.mPath[i]) + Offset) = d.at<T>(i);
//			}
		}
	};

	ModelManager * mStates;		// States (from GUI)
	ModelManager mPathMM;		// Animation path keyframes

	KeyframeElemModel<float,0> mDurModel;
	//KeyframeElemModel<float,4> mCrvModel;

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

	/// @param[in] r	geometry
	PathEditor(const Rect& r = Rect(400,180));
	
	/// Get PathView component
	PathView& pathView(){ return mPathView; }
//	PresetControl& presetControl(){ return mPresetControl; }

	/// Get path model manager
	ModelManager& pathModelManager(){ return pathView().pathModelManager(); }

	/// Set state model manager
	PathEditor& stateModelManager(ModelManager& v){
		mPathView.modelManager(v); return *this;
	}
	
	virtual void onDraw(GLV& g);
	virtual const char * className() const { return "PathEditor"; }

protected:
	PresetControl mPathPresetControl;
	Button mTransportPlay;
	Scroll mScroll;
	Group mHeader;
	PathView mPathView;
	
	static void ntSelection(const Notification& n);
};



/// A simple parametric control panel
class ParamPanel : public Table {
public:

	ParamPanel();


	/// Add a control widget with label
	
	/// @param[in] v					The control widget
	/// @param[in] label				The control's label
	/// @param[in] nameViewFromLabel	Whether to set the control's name to the 
	///									label (so it's a preset parameter)
	ParamPanel& addParam(
		View& v, const std::string& label, bool nameViewFromLabel=true
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View * views[], const std::string * labels[], unsigned count,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true
	);

	/// Add two control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const std::string& label1,
		View& view2, const std::string& label2,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true
	);

	/// Add three control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const std::string& label1,
		View& view2, const std::string& label2,
		View& view3, const std::string& label3,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true
	);

	/// Add four control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const std::string& label1,
		View& view2, const std::string& label2,
		View& view3, const std::string& label3,
		View& view4, const std::string& label4,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true
	);

	/// Add five control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const std::string& label1,
		View& view2, const std::string& label2,
		View& view3, const std::string& label3,
		View& view4, const std::string& label4,
		View& view5, const std::string& label5,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true
	);
	
	/// Get preset control
	PresetControl& presetControl(){ return mPresetControl; }

	virtual const char * className() const { return "ParamPanel"; }

private:
	PresetControl mPresetControl;
};


} // glv::
#endif

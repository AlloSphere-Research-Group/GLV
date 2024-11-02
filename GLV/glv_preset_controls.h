#ifndef INC_GLV_PRESET_CONTROLS_H
#define INC_GLV_PRESET_CONTROLS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <initializer_list>
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

	/// Load a given preset
	bool setPreset(const std::string& name);

	/// Load presets from the file configured through the model manager
	bool loadFile();

	/// Set the model manager from which to access preset data
	PresetControl& modelManager(ModelManager& v){ mMM = &v; return *this; }

	/// Bind to a top-level GLV interface

	/// This prepares the GUI model manager and then attaches it to the preset
	/// control. All widgets that are to be saved in the preset must be named
	/// (through the View::name method) and added to the GUI before calling this
	/// method.
	///
	/// @param[in] gui		GUI from which to attach its model manager
	/// @param[in] name		A name given to the model manager and preset file.
	///						An empty string bypasses this option.
	void bind(GLV& gui, const char * name = "");

	SearchBox& searchBox(){ return mSearchBox; }

	const char * className() const override { return "PresetControl"; }
	void onDraw(GLV& g) override;
	bool onEvent(Event::t e, GLV& g) override;

protected:
	struct PresetSearchBox : public SearchBox{
		PresetSearchBox(PresetControl& p): pc(p){}
		bool onEvent(Event::t e, GLV& g) override;
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

	void onAnimate(double dsec) override;
	void onDraw(GLV& g) override;
	bool onEvent(Event::t e, GLV& g) override;
	void onCellChange(int iOld, int iNew) override;
	const char * className() const override { return "PathView"; }

protected:
	friend class PathEditor;
	// Data is an array of preset names? or nothing, just dimensions?

	template <class T, int Offset>
	struct KeyframeElemModel : public Model {
	
		PathView& pv;
		
		KeyframeElemModel(PathView& v): pv(v){}
	
		const Data& getData(Data& temp) const override {
			const int N = pv.mPath.size();
			temp.resize(Data::getType<T>(), N);
			for(int i=0; i<N; ++i){
				temp.elem<T>(i) = *(T*)((char*)(&pv.mPath[i]) + Offset);
			}
			return temp;
		}

		void setData(const Data& d) override {
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
	float seqRight() const { return 34; }
	float startRight() const { return 6; }

	int loadCurrentPos();
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
	PathEditor& stateModelManager(ModelManager& v);
	
	void onDraw(GLV& g) override;
	const char * className() const override { return "PathEditor"; }

protected:
	PresetControl mPathPresetControl;
	Button mTransportPlay;
	Scroll mScroll;
	Group mHeader;
	PathView mPathView;
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
		std::initializer_list<std::pair<View&,const char *>> viewLabelPairs,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		View& view3, const char * label3,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		View& view3, const char * label3,
		View& view4, const char * label4,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		View& view3, const char * label3,
		View& view4, const char * label4,
		View& view5, const char * label5,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		View& view3, const char * label3,
		View& view4, const char * label4,
		View& view5, const char * label5,
		View& view6, const char * label6,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		View& view3, const char * label3,
		View& view4, const char * label4,
		View& view5, const char * label5,
		View& view6, const char * label6,
		View& view7, const char * label7,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Add control widgets grouped in a row with labels underneath
	ParamPanel& addParamGroup(
		View& view1, const char * label1,
		View& view2, const char * label2,
		View& view3, const char * label3,
		View& view4, const char * label4,
		View& view5, const char * label5,
		View& view6, const char * label6,
		View& view7, const char * label7,
		View& view8, const char * label8,
		const std::string& groupName = "",
		bool prefixViewNamesWithGroupName=true,
		bool nameViewsFromLabels=true,
		Direction labelDir = Direction::S
	);

	/// Remove single parameter or group by its name
	ParamPanel& removeParam(const char * name);

	/// Get preset control
	PresetControl& presetControl(){ return mPresetControl; }

	const char * className() const override { return "ParamPanel"; }

private:
	PresetControl mPresetControl;
};


} // glv::
#endif

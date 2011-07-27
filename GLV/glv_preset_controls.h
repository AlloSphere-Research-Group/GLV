#ifndef INC_GLV_PRESET_CONTROLS_H
#define INC_GLV_PRESET_CONTROLS_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"
#include "glv_behavior.h"
#include "glv_layout.h"
#include "glv_buttons.h"
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


} // glv::
#endif

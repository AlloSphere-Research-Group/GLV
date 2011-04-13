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



class PresetControl : public Box {
public:
	PresetControl(ModelManager& m);

	void loadFile();
	PresetControl& modelManager(ModelManager& v){ mMM = &v; return *this; }

	virtual const char * className() const { return "PresetControl"; }
	virtual void onDraw(GLV& g);
	virtual bool onEvent(Event::t e, GLV& g);

protected:

	struct PanelButtonClick : public EventHandler{
		PanelButtonClick(PresetControl& p): pc(p){}

		virtual bool onEvent(View& v, GLV& g);

		PresetControl& pc;
	} mPanelButtonClick;

	struct TextKeyDown : public EventHandler{
		TextKeyDown(PresetControl& p): pc(p){}

		virtual bool onEvent(View& v, GLV& g);

		PresetControl& pc;
	} mTextKeyDown;

	ModelManager * mMM;

	TextView mTextEntry;
	Button mStatus, mBtnPanel;
	PresetView mPanel;

	bool mOverwrite;

private:
	// ensure that model manager is always set to valid object
	PresetControl(): mPanelButtonClick(*this), mTextKeyDown(*this){}
};

} // glv::
#endif

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_preset_controls.h"

namespace glv{

PresetView::PresetView(ModelManager& mm)
:	mMM(&mm)
{
	extent(200);
	addHandler(Event::MouseDrag, Behavior::mouseMove);
}

void PresetView::onDraw(GLV& g){
	using namespace glv::draw;
}

bool PresetView::onEvent(Event::t e, GLV& g){

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


PresetControl::PresetControl(ModelManager& m)
:	mPanelButtonClick(*this), mTextKeyDown(*this),
	mMM(&m), mPanel(m), mOverwrite(false)
{
	mTextEntry.addHandler(Event::KeyDown, mTextKeyDown);
	mBtn.extent(mTextEntry.h);
	mBtn.disable(Controllable | HitTest);
	
	mBtnPanel.addHandler(Event::MouseDown, mPanelButtonClick);
	mBtnPanel.extent(mTextEntry.h);
	mBtnPanel.symbol(draw::combo<draw::frameTrunc<1,1,1,1>, draw::crosshatch<2,2> >);
	mBtnPanel.enable(Momentary);
	mBtnPanel.padding(4,0).padding(8,1);
	(*this) << mBtn << mTextEntry << mBtnPanel;
	fit();
}


bool PresetControl::PanelButtonClick::onEvent(View& v, GLV& g){
	//printf("%d\n", pc.mBtnPanel.getValue());
//	if(!pc.mBtnPanel.getValue()){ // flipped since value gets set after event
//		g << pc.mPanel;
//	}
//	else{
//		pc.mPanel.remove();
//	}
	if(pc.mPanel.parent){ // flipped since value gets set after event
		pc.mPanel.remove();
	}
	else{
		g << pc.mPanel;
	}

	return true;
}


bool PresetControl::TextKeyDown::onEvent(View& v, GLV& g){
	ModelManager& mm = *pc.mMM;
	TextView& te = pc.mTextEntry;

	const Keyboard& k = g.keyboard();
	
	if(k.key() != 's' || !k.ctrl()){
		pc.mOverwrite=false;
	}
	
	switch(k.key()){
	case 's':
		if(k.ctrl()){
			if(!te.empty()){
				const std::string& name = te.getValue();
				if(!pc.mOverwrite && mm.snapshots().count(name)){
					pc.mOverwrite = true;
				}
				else{
					pc.mOverwrite = false;
					mm.saveSnapshot(name);
					mm.snapshotsToFile();
				}
			}
			return false;
		}
		return true;
	case Key::Enter:
	case Key::Return:
		if(!mm.loadSnapshot(te.getValue())){
			printf("Could not load snapshot \"%s\"\n", te.getValue().c_str());
		}
		return false;
	case Key::Tab:
		if(!te.empty()){
			const std::string& tstr = te.getValue();
			ModelManager::Snapshots::const_iterator it = mm.snapshots().begin();
			while(it != mm.snapshots().end()){
				const std::string& s = it->first;
				int r = strncmp(s.c_str(), tstr.c_str(), tstr.size());
				if(0 == r){
					te.setValue(s);
					te.cursorEnd();
					break;
				}
				++it;
			}
		}
		return false;
	}
	return true;
}


void PresetControl::loadFile(){
	mMM->snapshotsFromFile();
	const std::string name = "default";
	if(mMM->snapshots().count(name)){
		mMM->loadSnapshot(name);
		mTextEntry.setValue(name);
		mTextEntry.cursorEnd();
	}
}

void PresetControl::onDraw(GLV& g){
	using namespace glv::draw;

	if(mOverwrite)	mBtn.symbol(draw::polygonCut<16,45>);
	else			mBtn.symbol(0);
	
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

} // glv::

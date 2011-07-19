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
	(*this) << mStatus << mSearchBox;// << mBtnPanel;
	fit();
}


PresetControl::~PresetControl(){
}

//bool PresetControl::PanelButtonClick::onEvent(View& v, GLV& g){
//	//printf("%d\n", pc.mBtnPanel.getValue());
////	if(!pc.mBtnPanel.getValue()){ // flipped since value gets set after event
////		g << pc.mPanel;
////	}
////	else{
////		pc.mPanel.remove();
////	}
//	if(pc.mPanel.parent){ // flipped since value gets set after event
//		pc.mPanel.remove();
//	}
//	else{
//		g << pc.mPanel;
//	}
//
//	return true;
//}


//bool PresetControl::SearchBox::onEvent(Event::t e, GLV& g){
//
//	ModelManager& mm = *pc.mMM;
////	TextView& te = pc.mTextEntry;
//	const Keyboard& k = g.keyboard();
//
//	switch(e){
//	case Event::KeyDown:
//		pc.mStatus.symbol(draw::magnifier);
//		
//		if(k.key() != 's' || !k.ctrl()){
//			pc.mPrompt=false;
//		}
//
//		switch(k.key()){
//		case Key::Down:	// pass on up/down arrows to search list...
//		case Key::Up:
//			mSearchList.onEvent(e,g);
//			return false;
//		case 's':
//			if(k.ctrl()){
//				if(!empty()){
//					const std::string& name = getValue();
//					if(!pc.mPrompt && mm.snapshots().count(name)){
//						pc.mPrompt = true;
//						pc.mStatus.symbol(draw::polygonCut<16,45>);
//						//pc.mStatus.symbol(draw::spokes<8,90>);
//					}
//					else{
//						pc.mPrompt = false;
//						pc.mStatus.symbol(draw::fileSave);
//						mm.saveSnapshot(name);
//						mm.snapshotsToFile();
//					}
//				}
//				return false;
//			}
//			break;
//		case Key::Enter:
//		case Key::Return:
//			if(mSearchList.visible()) setValue(mSearchList.getValue());
//			if(!mm.loadSnapshot(getValue())){
//				printf("Could not load snapshot \"%s\"\n", getValue().c_str());
//			}
//			else{
//				pc.mStatus.symbol(draw::fileLoad);
//			}
//			mSearchList.disable(Visible);
//			return false;
//		case Key::Tab:
//			if(!empty()){
//				const std::string& tstr = getValue();
//				ModelManager::Snapshots::const_iterator it = mm.snapshots().begin();
//				while(it != mm.snapshots().end()){
//					const std::string& s = it->first;
//					int r = strncmp(s.c_str(), tstr.c_str(), tstr.size());
//					if(0 == r){
//						setValue(s);
//						cursorEnd();
//						break;
//					}
//					++it;
//				}
//			}
//			return false;
//		default:;
//		}
//	default:;
//	}
//	
//	bool res = TextView::onEvent(e,g);
//
//	if(Event::KeyDown == e){
//
//		mSearchList.disable(Visible);
//		if(!empty()){
//			std::vector<std::string> items;
//			const std::string& tstr = getValue();
//			ModelManager::Snapshots::const_iterator it = mm.snapshots().begin();
//			while(it != mm.snapshots().end()){
//				const std::string& s = it->first;
//				//int r = strncasecmp(s.c_str(), tstr.c_str(), tstr.size());
//				size_t res = s.find(tstr);
//				if(res != std::string::npos){
////					printf("%s, %d\n", s.c_str(), res);
//					items.push_back(s);
//				}
//				/*
//				The strcasecmp() and strncasecmp() return an integer greater than,
//				equal to, or less than 0, according as s1 is lexicographically
//				greater than, equal to, or less than s2 after translation of each
//				corresponding character to lower-case.  The strings themselves are
//				not modified.  The comparison is done using unsigned characters, so
//				that `\200' is greater than `\0'.
//				*/
////				if(0 == r){
//////					items.push_back(s);
////				}
////				printf("%s\n", s.c_str());
//				++it;
//			}
//
//			if(items.size()){
//				space_t ax = 0, ay = height();
//				toAbs(ax, ay);
//				
//				mSearchList.data().resize(1, items.size());
//				mSearchList.data().assignFromArray(&items[0], items.size());
//				mSearchList.clipIndices();
//				mSearchList.fitExtent();
//				mSearchList.pos(ax,ay);
//				mSearchList.enable(Visible);
//				mSearchList.bringToFront();
//			}
//			if(mSearchList.parent == 0) pc.root() << mSearchList;			
//		}
//	}	
//
//	return res;
//}
//
//bool PresetControl::SearchBox::SearchList::onEvent(Event::t e, GLV& g){
//	const Keyboard& k = g.keyboard();
//
//	switch(e){
//	case Event::KeyDown:
//		switch(k.key()){
//		case Key::Enter:
//		case Key::Return:
//			sb.setValue(getValue());
//			sb.onEvent(e,g);
//			break;
//		default:;
//		}
//		break;
//	case Event::FocusLost:
//		disable(Visible);
//		break;
//	default:;
//	}
//	return ListView::onEvent(e,g);
//}

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
		
		if((k.key() != 's' && k.key() != Key::Delete) || !k.ctrl()){
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


bool PresetControl::loadFile(){
	if(NULL == mMM){
		fprintf(stderr, "From PresetControl::loadFile: Attempt to load file without a ModelManager\n");
	}
	else if(mMM->snapshotsFromFile()){
		const std::string name = "default";
		if(mMM->snapshots().count(name)){
			mMM->loadSnapshot(name);
			mSearchBox.setValue(name);
			mSearchBox.cursorEnd();
		}
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

} // glv::

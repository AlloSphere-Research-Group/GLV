#include "glv_draw.h"
#include "glv_widget.h"

namespace glv{

Widget::Widget(
	const Rect& r, const Data& data,
	space_t pad, bool toggles, bool mutExc, bool drawGrid
)
:	View(r), mPadding(pad), sx(0), sy(0), mMin(0), mMax(1), mUseInterval(true)
{
	setModel(data);
	model().clone();
	if(model().isNumerical()){ model().assignAll(0); }

	property(DrawGrid, drawGrid);
	property(MutualExc, mutExc);
	property(Toggleable, toggles);
}

void Widget::drawGrid(){
	
	if(enabled(DrawGrid)){
		using namespace glv::draw;

		float xd = dx();
		float yd = dy();

		color(colors().border); lineWidth(1);

		Point2 pts[(sizeX()+sizeY()-2)*2];
		int i=0;

		for(int x=1; x<sizeX(); ++x){
			pts[i++](x*xd, 0);
			pts[i++](x*xd, h);
		}
		for(int y=1; y<sizeY(); ++y){
			pts[i++](0, y*yd);
			pts[i++](w, y*yd);
		}
		paint(Lines, pts, GLV_ARRAY_SIZE(pts));
	}
}

void Widget::onModelSync(){
	IndexDataMap::iterator it = variables().begin();

	for(; it!=variables().end(); ++it){
		int idx = it->first;

		if(validIndex(idx)){
			const Data& data = it->second;
			if(model().slice(idx, model().size()-idx) != data){
				assignModel(data, idx);
			}
		}
	}
}

// note: indices passed in are always valid
bool Widget::onAssignModel(Data& d, int ind1, int ind2){

	if(enabled(MutualExc)){
		double v = 0;
		if(useInterval()) v = glv::clip(v, mMax, mMin);
		model().assignAll(v);
	}

	if(useInterval()){
		for(int i=0; i<d.size(); ++i){
			double v = d.at<double>(i);
			v = glv::clip(v, max(), min());
			d.put(v, i);
		}
	}

	int idx = model().indexFlat(ind1,ind2);	// starting index of model

	// Update any attached variables containing this index
	IndexDataMap::iterator it = variables().begin();
	for(; it != variables().end(); ++it){
		Data& v = it->second;
		
		// get destination/source intervals in terms of model indices
		int id0 = it->first;
		int id1 = id0 + v.size();
		int is0 = idx;
		int is1 = is0 + d.size();
		
		// the intersection
		int i0 = glv::max(id0, is0);
		int i1 = glv::min(id1, is1);
		
		if(i0 < i1){
//			printf("[%d, %d), [%d, %d), [%d, %d)\n", id0, id1, is0, is1, i0, i1);
			v.slice(i0-id0, i1-i0).assign(
				d.slice(i0-is0, i1-i0)
			);
		}
	}
	
//	if(variables().count(idx)){
//		Data& v = variables()[idx];
//		printf("1: %s %s\n", v.toToken().c_str(), d.toToken().c_str());
//		v.assign(d);
//		printf("2: %s %s\n", v.toToken().c_str(), d.toToken().c_str());
//	}

	Data modelOffset = model().slice(idx, model().size()-idx);

	if(d != modelOffset){
		model().assign(d, ind1, ind2);
		ModelChange modelChange(model(), idx);
		notify(this, Update::Value, &modelChange);
	}

	return true;
}

void Widget::onSelectClick(GLV& g){
	sx = (int)((g.mouse.xRel() / w) * sizeX());
	sy = (int)((g.mouse.yRel() / h) * sizeY());
	clipIndices();
}

void Widget::onSelectKey(GLV& g){
	//printf("shift %d\n", g.keyboard.shift());
	//printf("shift %d\n", g.keyboard.key());
	switch(g.keyboard.key()){
	case Key::Down:	sy++; break;
	case Key::Up:	sy--; break;
	case Key::Right:sx++; break;
	case Key::Left:	sx--; break;
	case Key::Tab:
		
		if(!g.keyboard.shift()){
			if(sx == sizeX()-1){
				sx=0;
				if(sy == sizeY()-1)	sy=0;
				else				sy++;
			}
			else sx++;
		}
		else{	// doesn't work with GLUT; key == 25 (end-of-medium) with shift down
			if(sx == 0){
				sx=sizeX()-1;
				if(sy == 0) sy=sizeY()-1;
				else		sy--;
			}
			else sx--;
		}
		break;
	}
	clipIndices();
}

Widget& Widget::setValueMax(){
	for(int i=0; i<size(); ++i){ setValue(max(), i); } return *this;
}

Widget& Widget::setValueMid(){
	for(int i=0; i<size(); ++i){ setValue(mid(), i); } return *this;
}

} // glv::

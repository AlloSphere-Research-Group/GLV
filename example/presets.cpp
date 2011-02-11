/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Preset management

This example shows how to save and recall the model state of a GUI.

*/

#include "example.h"


class ListView : public Widget{
public:

	ListView(const Rect& r=Rect(0), int nx=0, int ny=0)
	:	Widget(r)
	{
		data().resize(Data::STRING, nx,ny);
		//font().size(18);
		disable(DrawGrid);
		//padding(8);
	}

	ListView& fitExtent(){
		float maxw = 0;//, maxh = 0;
		int nitems = data().size();

		for(int i=0; i<nitems; ++i){
			float x = font().advance(data().at<std::string>(i).c_str());
			if(x > maxw) maxw = x;
		}
		extent(
			data().size(0) * (maxw + paddingX()*2),
			data().size(1) * (font().cap() + font().descent() + paddingY()*2)
		);
		return *this;
	}

	const std::string getValue() const { return Widget::getValue<std::string>(); }
	const std::string getValue(int i) const { return Widget::getValue<std::string>(i); }
	const std::string getValue(int i1, int i2) const { return Widget::getValue<std::string>(i1, i2); }

	virtual const Data& getData() const {
		mDataSelected = data().slice(selected(), 1);
		return mDataSelected;
	}

	// read only...
	virtual void setData(const Data& d){}

	virtual void onDraw(GLV& g){
	
		using namespace glv::draw;

		Indexer idx(data().size(0), data().size(1));
		float dx_ = dx(0);
		float dy_ = dy(1);
		
		while(idx()){
			int ix = idx[0];
			int iy = idx[1];
			
			float px = dx_ * ix;
			float py = dy_ * iy;

			if(selectedX() == ix && selectedY() == iy){
				color(colors().selection);
				draw::rectangle(px,py, px+dx_,py+dy_);
			}
			
			color(colors().text);
			lineWidth(1);
			font().render(data().at<std::string>(ix,iy).c_str(), px+paddingX(), py+paddingY());
		}
		
		Widget::onDraw(g);
	}


	virtual bool onEvent(Event::t e, GLV& g){
	
		if(!Widget::onEvent(e,g)) return false;

		switch(e){
		case Event::MouseDown:
		case Event::MouseDrag:
			selectFromMousePos(g);
			return false;

//		case Event::MouseMove:
//			if(containsPoint(g.mouse().xRel(), g.mouse().yRel())){
//				selectFromMousePos(g);
//				return false;
//			}
//			break;
		
		case Event::KeyDown:
			switch(g.keyboard().key()){
			default:;
			}
			break;
		
		default:;
		}
		
	
		return true;
	}
	

protected:
	mutable Data mDataSelected;
};

class MyGLV : public GLV{
public:
	virtual bool onEvent(Event::t e, GLV& g){
		//printf("%s\n", toString(e));
		
		switch(e){
		case Event::KeyDown:
			if(g.keyboard().key() == 's'){
				modelManager().saveSnapshot("preset " + toString(presetNum++));
				printf("\n%s\n", modelManager().snapshotsToString().c_str());
			}
			break;
		default:;
		}
		
		return true;
	}
	
	int presetNum;
};

int main(){

	MyGLV top; top.presetNum=1;

	Table gui("<"); //gui.enable(DrawGrid);

	ListView lv(Rect(100));
	char * fruits[] = {
		"kiwi", "apple", "banana", "pomegranite", "strawberry", "cherry",
		"canteloupe", "watermelon", "blueberry", "coconut", "pear", "grapefruit",
		"green bean", "spinach", "potato", "brussel sprout", "broccoli", "bell pepper",
		"corn", "celery", "onion", "lettuce", "carrot", "mushroom"
	};
	int Nfruit = sizeof(fruits)/sizeof(fruits[0]);
	lv.data().resize(2, Nfruit/2);
	
	for(int i=0; i<Nfruit; ++i){
		//lv.addItem(fruits[i]);
		lv.data().elem<std::string>(i) = fruits[i];
	}
//	lv.data().print();
	lv.fitExtent();
	gui << lv;

//	Label l;
//	TextView tv;
//	Button b;
//	Buttons bs(Rect(), 2, 2);
//	Slider s;
//	Sliders ss(Rect(), 1, 4);
//	Slider2D s2D;
//	NumberDialer nd(1,4,1,-1);
//	
//	//std::string strings[] = {"test1", "test2", "test3"};
//
//	//bs.disable(DrawBorder);
//	//bs.disable(DrawGrid);
//
//	l.setValue("Hello Label!").name("l");
//	tv.setValue("Hello TextView!").name("tv");
//	b.setValue(true).name("b");
//	bs.setValue(true, 0,1).name("bs");
//	s.setValue(0.5).name("s");
//	ss.setValue(0.4, 1).name("ss");
//	s2D.setValue(0.5, 0).setValue(0.1, 1).name("s2D");
//	nd.setValue(-0.5494).name("nd");
//
//	View * views[] = {&l, &tv, &b, &bs, &s, &ss, &s2D, &nd};
//	for(unsigned i=0; i<sizeof(views)/sizeof(View *); ++i){
//		gui << views[i];
////		const Data& d = views[i]->model();
////		printf("%p: %s\n", &d, d.toString().c_str());
////		views[i]->model().print(); printf("\n");
//	}

	top << gui.arrange();
	
	top.refreshModels();

	top.modelManager().name("presets");
//	top.modelManager().saveSnapshot("preset 1");
//	s.setValue(0.77);
//	top.modelManager().saveSnapshot("preset 2");

//	top.modelManager().snapshotsToFile("snapshots.txt");
//		
//	top.modelManager().clearSnapshots();
//	top.modelManager().snapshotsFromFile("snapshots.txt");

	//printf("\n%s\n", top.modelManager().snapshotsToString().c_str());
	
	Window win(0,0, "Presets", &top);
	win.fit();
	Application::run();
}


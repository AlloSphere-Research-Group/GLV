/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: Preset management

This example shows how to save and recall the model state of a GUI.

*/

#include "example.h"


class MyGLV : public GLV{
public:
	virtual bool onEvent(Event::t e, GLV& g){
		//printf("%s\n", toString(e));
		
		switch(e){
		case Event::KeyDown:
			if(g.keyboard().key() == 's'){
				modelManager().saveSnapshot("preset " + toString(presetNum++));
				modelManager().snapshotsToFile();
				//printf("\n%s\n", modelManager().snapshotsToString().c_str());
			}
			break;
		default:;
		}
		
		return true;
	}
	
	int presetNum;
};

int main(){

	MyGLV top;
	top.presetNum=1;

	Table gui("<"); //gui.enable(DrawGrid);

	Label l;
	TextView tv;
	DropDown dd;
	Button b;
	Buttons bs(Rect(), 2, 2);
	Slider s;
	Sliders ss(Rect(), 1, 4);
	Slider2D s2D;
	SliderRange sr;
	NumberDialer nd(1,4,1,-1);
	
	//std::string strings[] = {"test1", "test2", "test3"};

	//bs.disable(DrawBorder);
	//bs.disable(DrawGrid);

	l.setValue("Hello Label!").name("l");
	tv.setValue("Hello TextView!").name("tv");
	dd.addItem("apple").addItem("orange").name("dd");
	b.setValue(true).name("b");
	bs.setValue(true, 0,1).name("bs");
	s.setValue(0.5).name("s");
	ss.setValue(0.4, 1).name("ss");
	s2D.setValue(0.5, 0).setValue(0.1, 1).name("s2D");
	sr.centerRange(0.5, 0.1).name("sr");
	nd.setValue(-0.5494).name("nd");

	View * views[] = {&l, &tv, &dd, &b, &bs, &s, &ss, &s2D, &sr, &nd};
	for(unsigned i=0; i<sizeof(views)/sizeof(View *); ++i){
		gui << views[i];
//		const Data& d = views[i]->model();
//		printf("%p: %s\n", &d, d.toString().c_str());
//		views[i]->model().print(); printf("\n");
	}

	top << gui.arrange();
	
	top.refreshModels();

	top.modelManager().name("GLVPresetsExample");
//	top.modelManager().saveSnapshot("preset 1");
//	s.setValue(0.77);
//	top.modelManager().saveSnapshot("preset 2");

//	top.modelManager().snapshotsToFile();
//		
//	top.modelManager().clearSnapshots();
//	top.modelManager().snapshotsFromFile("snapshots.txt");

	//printf("\n%s\n", top.modelManager().snapshotsToString().c_str());
	
	Window win(0,0, "Presets", &top);
	win.fit();
	Application::run();
}


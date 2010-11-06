/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: 
*/

#include "example.h"

class PresetView : public View{
public:

	enum ViewMode{
		LIST,
		ICON
	};

	PresetView(ModelManager& m, const Rect& r=Rect())
	:	View(r), mMM(&m)
	{}

	virtual void onDraw(GLV& g){
	
	}

	virtual bool onEvent(Event::t e, GLV& g){
		return true;
	}

protected:
	ModelManager * mMM;
	ViewMode mMode;
};

class MyGLV : public GLV{
public:
	virtual bool onEvent(Event::t e, GLV& g){
		//printf("%s\n", toString(e));
		
		switch(e){
		case Event::KeyDown:
			if(g.keyboard.key() == 's'){
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

	MyGLV top; top.presetNum=10;
	Table gui("<"); gui.enable(DrawGrid);
	Label l;
	TextView tv;
	Button b;
	Buttons bs(Rect(), 2, 2);
	Slider s;
	Sliders ss(Rect(), 1, 4);
	Slider2D s2D;
	NumberDialer nd(1,4,1,-1);
	//std::string strings[] = {"test1", "test2", "test3"};

	l.setValue("Hello Label!").name("l");
	tv.setValue("Hello TextView!").name("tv");
	b.setValue(true).name("b");
	bs.setValue(true, 0,1).name("bs");
	s.setValue(0.5).name("s");
	ss.setValue(0.4, 1).name("ss");
	s2D.setValue(0.5, 0).setValue(0.1, 1).name("s2D");
	nd.setValue(-0.54941).name("nd");

	top << gui.arrange();
	
	top.refreshModels();

	top.modelManager().name("presets");
	top.modelManager().saveSnapshot("preset 1");
	s.setValue(0.77);
	top.modelManager().saveSnapshot("preset 2");

	top.modelManager().snapshotsToFile("snapshots.txt");
		
	top.modelManager().clearSnapshots();
	top.modelManager().snapshotsFromFile("snapshots.txt");

	//printf("\n%s\n", top.modelManager().snapshotsToString().c_str());
	
	Window win(0,0, "Presets", &top);
	win.fit();
	Application::run();
}


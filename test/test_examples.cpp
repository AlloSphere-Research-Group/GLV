/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

const int pages = 9;
Buttons tabs(Rect(110, pages*20), 1, pages, true, true);
Group groups[pages];

Button bt1(Rect(20), true), bt2(bt1, true, draw::x, draw::minus);
Buttons bts14(Rect(60, 60), 1, 4, true, true);
Buttons bts41(Rect(120, 15), 4, 1, true, true);
Buttons bts44(Rect(60), 4, 4);
Slider sl1H(Rect(100, 20)), sl1V(Rect(20, 100));
Slider2D sl2(Rect(100));
SliderGrid<3> sg3(Rect(100));
SliderGrid<4> sg4(Rect(100));
Sliders sliders1(Rect(100), 10, 1, true), sliders2(sliders1, 1, 10, true);

ButtonBase<Values<bool, 4, 4> > btv(Rect(80));
ButtonBase<Array<bool> > bta(Rect(80), 1, 1);
NumberDialer nd1(12,0,0, 4,0, 9999,-9999), nd2(16,0,0, 1,3, 8,0);

FunctionGraph fg(Rect(200, 100), 4, 10);

void drawCB(View * v){
	using namespace glv::draw;

	for(int i=0; i<tabs.size(); ++i) groups[i].property(Visible, tabs.value()[i]);

	push3D(v->w, v->h);		// push into 3-D mode passing in width and height of view
	
	pop3D();
}


int main (int argc, char ** argv){
	
	GLV top(drawCB);
	Window win(600, 400, "GLV Examples", &top);
	
	top.colors().back.set(1);
	
	glv::Style::standard().color.set(StyleColor::BlackOnWhite);
	
	for(int i=0; i<pages; ++i){
		top << groups[i].disable(Visible).stretch(1,1);
	}
	
	fg.tension(1.);
	
	groups[0] << bt1.pos(Place::BL).anchor(Place::CC)(Event::MouseDrag, Behavior::mouseResize);
	groups[0] << bt2.pos(Place::BR, -4,0).anchor(Place::CC)(Event::MouseDrag, Behavior::mouseResize);
	groups[1] << bts14.pos(Place::BR, -4,0).anchor(Place::CC);
	groups[1] << bts41.pos(Place::TL, 0, 4).anchor(Place::CC);
	groups[1] << bts44.pos(Place::BL).anchor(Place::CC);
	groups[2] << fg.pos(Place::BL).anchor(Place::CC);
	groups[3] << sl1H.pos(Place::BL).anchor(Place::CC);
	groups[3] << sl1V.pos(Place::BR, -4,0).anchor(Place::CC);
	groups[4] << sl2.pos(Place::BL).anchor(Place::CC);
	groups[5] << sg3.pos(Place::BL).anchor(Place::CC);
	groups[5] << sg4.pos(Place::TL, 0, 4).anchor(Place::CC);
	groups[6] << sliders1.pos(Place::BR).anchor(Place::CC); 
	groups[6] << sliders2.pos(Place::BL, 4,0).anchor(Place::CC);
	groups[7] << (new Label("Horizontal"))->pos(Place::BL).anchor(Place::CC);
	groups[7] << (new Label("Vertical", true))->pos(Place::BR,-16,0).anchor(Place::CC);
	groups[8] << nd1.pos(Place::BL).anchor(Place::CC);
	groups[8] << nd2.pos(Place::TL, 0, 4).anchor(Place::CC);

	tabs << (new Label("Button"		  ))->anchor(0.5, 1./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("Buttons"	  ))->anchor(0.5, 3./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("FunctionGraph"))->anchor(0.5, 5./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("Slider"		  ))->anchor(0.5, 7./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("Slider2D"	  ))->anchor(0.5, 9./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("SliderGrid"	  ))->anchor(0.5,11./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("Sliders"	  ))->anchor(0.5,13./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("Label"	      ))->anchor(0.5,15./(tabs.size()*2)).pos(Place::CC);
	tabs << (new Label("NumberDialer" ))->anchor(0.5,17./(tabs.size()*2)).pos(Place::CC);
	top << tabs;
	
	tabs(Event::MouseDrag, Behavior::mouseResize);

	//#define PRINT_SIZE(x) printf("sizeof(%s) = %d\n", #x, sizeof(x));
	//PRINT_SIZE(View) PRINT_SIZE(Notifier) PRINT_SIZE(Rect)

	Application::run();
}


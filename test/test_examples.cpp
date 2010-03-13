/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

const int pages = 14;
Buttons tabs(Rect(110, pages*20), 1, pages, true, true);
Group groups[pages];

Button bt1(Rect(20), true), bt2(bt1, true, draw::x, draw::minus);
Buttons bts14(Rect(60, 60), 1, 4, true, true);
Buttons bts41(Rect(120, 15), 4, 1, true, true);
Buttons bts44(Rect(60), 4, 4);
ButtonBase<Values<bool, 4, 4> > btv(Rect(80));
ButtonBase<Array<bool> > bta(Rect(80), 1, 1);
FunctionGraph fg(Rect(200, 100), 4, 10);

const int plotSize=64;
FunctionPlot	plotXY(Rect(150), plotSize, Color(0,0,1)),
				plotX(Rect(150,60), plotSize, Color(1,0,0)),
				plotY(Rect(60,150), plotSize, Color(0,1,0));

NumberDialer nd1(12,0,0, 4,0, 9999,-9999), nd2(16,0,0, 1,3, 8,0);
Slider sl1H(Rect(100, 20)), sl1V(Rect(20, 100));
Slider2D sl2(Rect(100));
SliderGrid<3> sg3(Rect(100));
SliderGrid<4> sg4(Rect(100));
SliderRange slRH(Rect(100,20)), slRV(Rect(20,100));
Sliders sliders1(Rect(100), 10, 1, true), sliders2(sliders1, 1, 10, true);
Table table(	". v - -,"
				"> p ^ q,"
				"| < x >,"
				"| b v d", 12, 6, Rect(0));
TextView tv1(Rect(200,16));

struct SubView3D : View3D{
	SubView3D(const Rect& r): View3D(r){}
	
	virtual void onDraw3D(){
		using namespace glv::draw;
		static unsigned r=0; r+=2;
		translateZ(-3);
		rotateY(r);
		begin(Triangles);
		color(1,0,0); vertex(-1,-1,0);
		color(0,1,0); vertex( 1,-1,0);
		color(0,0,1); vertex( 0, 1,0);
		end();
	}
} v3D(Rect(200));


void drawCB(View * v){
	using namespace glv::draw;

	for(int i=0; i<tabs.size(); ++i) groups[i].property(Visible, tabs.value()[i]);

	push3D(v->w, v->h);		// push into 3-D mode passing in width and height of view
	
	pop3D();
}


int main(int argc, char ** argv){
	
	GLV top(drawCB);
	Window win(600, 400, "GLV Examples", &top);
	
	top.colors().back.set(1);
	
	glv::Style::standard().color.set(StyleColor::BlackOnWhite);
	
	for(int i=0; i<pages; ++i){
		top << groups[i].disable(Visible|HitTest).stretch(1,1);
	}
	
	fg.tension(1.);

	table	<< new Label("col span") << new Label("row span", true)
			<< new Label("top-left") << new Label("top-center") << new Label("top-right") 
			<< new Label("center-left") << new Label("center-center") << new Label("center-right")
			<< new Label("bottom-left") << new Label("bottom-center") << new Label("bottom-right");
	table.arrange();
	//table.enable(DrawBorder);
	
	const float ffreq = 2*M_PI/(plotSize-1);
	plotXY.stroke(2);
	for(int i=0; i<plotSize; ++i){
		float phs = i*ffreq;
		float sigX = (cos(phs) + 0.5*cos(2*phs))*0.6;
		float sigY = (sin(phs) + 0.5*sin(2*phs))*0.6;
		plotXY.bufferX()[i] = sigX;
		plotXY.bufferY()[i] = sigY;
		plotX.bufferX()[i] = sigX;
		plotY.bufferY()[i] = sigY;
	}
	
	v3D(Event::MouseDrag, Behavior::mouseMove)
		(Event::MouseDrag, Behavior::mouseResize);

	int i=-1;
	groups[++i]<< bt1.pos(Place::BL).anchor(Place::CC)(Event::MouseDrag, Behavior::mouseResize);
	groups[  i]<< bt2.pos(Place::BR, -4,0).anchor(Place::CC)(Event::MouseDrag, Behavior::mouseResize);
	groups[++i]<< bts14.pos(Place::BR, -4,0).anchor(Place::CC);
	groups[  i]<< bts41.pos(Place::TL, 0, 4).anchor(Place::CC);
	groups[  i]<< bts44.pos(Place::BL).anchor(Place::CC);
	groups[++i]<< fg.pos(Place::BL).anchor(Place::CC);
	groups[++i]<< (new Label("Horizontal"))->pos(Place::BL).anchor(Place::CC);
	groups[  i]<< (new Label("Vertical", true))->pos(Place::BR,-16,0).anchor(Place::CC);
	groups[++i]<< nd1.pos(Place::BL).anchor(Place::CC);
	groups[  i]<< nd2.showSign(false).pos(Place::TL, 0, 4).anchor(Place::CC);
	groups[++i]<< sl1H.pos(Place::BL).anchor(Place::CC);
	groups[  i]<< sl1V.pos(Place::BR, -4,0).anchor(Place::CC);
	groups[++i]<< sl2.pos(Place::BL).anchor(Place::CC);
	groups[++i]<< sg3.pos(Place::BL).anchor(Place::CC);
	groups[  i]<< sg4.pos(Place::TL, 0, 4).anchor(Place::CC);
	groups[++i]<< slRH.pos(Place::BL).anchor(Place::CC);
	groups[  i]<< slRV.pos(Place::BR, -4,0).anchor(Place::CC);
	groups[++i]<< sliders1.pos(Place::BR).anchor(Place::CC); 
	groups[  i]<< sliders2.pos(Place::BL, 4,0).anchor(Place::CC);
	groups[++i]<< table.pos(Place::BC).anchor(Place::CC);
	groups[++i]<< plotXY.pos(Place::BL).anchor(Place::CC);
	groups[  i]<< plotX.pos(Place::TL, 0,4).anchor(Place::CC);
	groups[  i]<< plotY.pos(Place::BR,-4,0).anchor(Place::CC);
	groups[++i]<< tv1.pos(Place::CC, 0,0).anchor(Place::CC);
	groups[++i]<< v3D.pos(Place::CC, 0,0).anchor(Place::CC);

	float y = 1./(tabs.size()*2), dy=2*y;
	tabs << (new Label("Button"			))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("Buttons"		))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("FunctionGraph"	))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("Label"			))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("NumberDialer"	))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("Slider"			))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("Slider2D"		))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("SliderGrid"		))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("SliderRange"	))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("Sliders"		))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("Table"			))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("FunctionPlot"	))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("TextView"		))->anchor(0.5, y).pos(Place::CC); y+=dy;
	tabs << (new Label("View3D"			))->anchor(0.5, y).pos(Place::CC); y+=dy;
	top << tabs;
	
	tabs(Event::MouseDrag, Behavior::mouseResize);

	//#define PRINT_SIZE(x) printf("sizeof(%s) = %d\n", #x, sizeof(x));
	//PRINT_SIZE(View) PRINT_SIZE(Notifier) PRINT_SIZE(Rect)

	Application::run();
}


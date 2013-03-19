/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "example.h"

int main(){
	using namespace std;
	const float W = 100;

	Label::Spec caption; caption.size=6;
	Label::Spec h1; h1.stroke=1.5; h1.size=8;

	GLV top;
	top.colors().set(StyleColor::BlackOnWhite);
	top.colors().fore.set(0.5);
	top.colors().selection.set(0.8);
	top.colors().border.set(0);
	Table gui("pppppp");

	Table tblView;
	View v0(Rect(W,W-16));
	View v10(Rect( 8, 8,W/2,W/2));
	View v11(Rect(40,24,W/2,W/2));
	View v110(Rect( 8, 8,W/4,W/4));
	View v111(Rect(16,16,W/4,W/4));
	Scroll scr(Rect(W,W));
	scr.padding(4);
	Label scrText(
		"Scroll provides a subview\n"
		"of its children whose\n"
		"total dimensions are\n"
		"generally too large to\n"
		"fit within a limited\n"
		"amount of space.\n\n"
		"The subview can be\n"
		"scrolled around by\n"
		"dragging the sliders on\n"
		"the edges. The size of\n"
		"the sliders indicates the\n"
		"relative amount of content\n"
		"being displayed in its\n"
		"respective dimension."
	);

	tblView
		<< new Label("View", h1)
		<< new Divider
		<< (v0 << v10 << (v11 << v110 << v111)) << new Label("View hierarchy", caption)
		<< (scr<<scrText) << new Label("Scroll", caption)
	;

	ColorPicker cpk;
	cpk.setValue(HSV(0.7,0.7,0.7));
	ColorSliders csl;
	csl.setDataFromString("{0.5,1,0.7}");
	tblView
		<< new Divider(16,0)
		<< new Label("Color", h1)
		<< new Divider
		<< cpk << new Label(cpk.className(), caption)
		<< csl << new Label(csl.className(), caption)
	;
	gui << tblView.arrange();



	Table tbl1;
	Button btn;
	Buttons btnsH(Rect(W, W/5), 5,1);
	Buttons btnsV(Rect(W/5, W), 1,5);
	Buttons btns2(Rect(W, W), 5,5);
	tbl1
		<< new Label("Button", h1)
		<< new Divider
		<< btn		<< (new Label(btn.className(), caption))->paddingY(4)
		<< btnsH	<< new Label(btnsH.className()+string(" (5x1)"), caption)
		//<< btnsV
		<< btns2	<< new Label(btns2.className()+string(" (5x5)"), caption)
	;
	//gui << tbl1.arrange();

	//Table tblText;
	TextView tv(Rect(W,16));
	tv.setValue("Hello GLV!");
	NumberDialer nd; nd.setValue(12.34);
	ListView lv(Rect(W,16*3), 2,3);
	for(int i=0; i<lv.data().size(); ++i){
		lv.setValue("item"+toString(i+1), i);
	}
	DropDown dd(Rect(W,16), "Item 1", "Item 2", "Item 3", "Item 4");
	tbl1
		<< new Divider(16,0)
		<< new Label("Text", h1)
		<< new Divider
		//<< new Label("a label")
		<< tv	<< new Label(tv.className(), caption)
		<< nd	<< new Label(nd.className(), caption)
		<< lv	<< new Label(lv.className(), caption)
		<< dd	<< new Label(dd.className(), caption)
	;
	gui << tbl1.arrange();


	Table tblSliders;
	SliderGrid<4> sl4;
	Slider sl;
	sl.setValue(0.4);
	Slider slc; slc.interval(-1,1);
	Sliders slsH(Rect(W,W/2), 10,1);
	for(int i=0;i<slsH.size(); ++i) slsH.setValue(cos(i)*0.5+0.5, i);
	SliderRange slr;
	Slider2D sl2;
	sl2.setValue(0.4,0);
	sl2.setValue(0.3,1);
	tblSliders
		<< new Label("Slider", h1)
		<< new Divider
		<< sl	<< new Label(sl  .className(), caption)
		<< slc	<< new Label(slc .className(), caption)
		<< slsH	<< new Label(slsH.className() + string(" (10x1)"), caption)
		<< slr	<< new Label(slr .className(), caption)
		<< sl2	<< new Label(sl2 .className(), caption)
		<< sl4	<< new Label(sl4 .className() + string(" (n=4)"), caption)
	;
	gui << tblSliders.arrange();


	Table tblPlot;
	Plot p1Dc(Rect(W,60), *new PlotFunction1D);
	p1Dc.data().resize(1,60);
	p1Dc.range(0,p1Dc.data().size(1), 0);
	p1Dc.range(-1.1, 1.1, 1);
	for(int i=0; i<p1Dc.data().size(); ++i){
		p1Dc.data().assign(cos(i*0.2)*cos(i*0.8), i);
	}
	Plot p1Dd(Rect(W,60),
		*new PlotFunction1D(Color(0), 1, draw::Lines, PlotFunction1D::ZIGZAG),
		*new PlotFunction1D(Color(0), 2, draw::Points)
	);
	p1Dd.data().resize(1,16);
	p1Dd.range(0,p1Dd.data().size(1), 0);
	p1Dd.range(-1.1, 1.1, 1);
	for(int i=0; i<p1Dd.data().size(1); ++i){
		p1Dd.data().assign(cos(i*0.2)*cos(i*0.8), i);
	}
	Plot p2D(Rect(W,W), *new PlotFunction2D);
	p2D.data().resize(2,60);
	for(int i=0; i<p2D.data().size(1); ++i){
		p2D.data().assign(cos(i*0.2)*cos(i*0.8), 0, i);
		p2D.data().assign(sin(i*0.2)*sin(i*0.8), 1, i);
	}
	Plot pdn(Rect(W,W), *new PlotDensity(Color(1,0.9,0.5), 0.02, 1));
	pdn.data().resize(1,64,64);
	for(int j=0; j<pdn.data().size(2); ++j){
	for(int i=0; i<pdn.data().size(1); ++i){
		float x = i*0.1;
		float y = j*0.1;
		float v = cos(3*hypot(x-5, y-3)) + cos(3*hypot(x+5, y+3));
		pdn.data().assign(v/2, 0,i,j);
	}}

	tblPlot
		<< new Label("Plot", h1)
		<< new Divider
		<< p1Dc	<< new Label(p1Dc.className() + string(" (cont.)"), caption)
		<< p1Dd	<< new Label(p1Dd.className() + string(" (discrete)"), caption)
		<< p2D	<< new Label(p2D .className() + string(" (curve)"), caption)
		<< pdn	<< new Label(pdn .className() + string(" (density)"), caption)
	;
	gui << tblPlot.arrange();

	//gui.enable(DrawBorder);
	//gui.enable(DrawGrid);
	gui.arrange();
	top << gui;
	Window win(800,600, "GLV Showcase", &top);
	win.fit();
	Application::run();
}


/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

Window win(320, 240);

struct MyView3D : public View3D{

	MyView3D(): count(0){
		stretch(1,1);
		disable(DrawBorder);
	}

	virtual void onDraw3D(GLV& g){
		using namespace draw;

		if(100==count) win.show(); ++count;

		static float ang=0;
		translate(0,0,-3);
		rotate(0,ang,ang*0.31); ++ang;
		
		GraphicsData& gr = g.graphicsData();
		gr.addColor(Color(1));
		gr.addColor(Color(0.7,0.3,0));
		gr.addColor(Color(0.7,0.3,0));
		gr.addColor(Color(0.7,0.3,0));

		gr.addVertex3(-1,-1, 0);
		gr.addVertex3(-1, 1, 0);
		gr.addVertex3( 1,-1, 0);
		gr.addVertex3( 1, 1, 0);

		paint(TriangleStrip, gr);
	}
	
	virtual void onDraw2D(GLV& g){
		using namespace draw;
		color(0);
		text(
			"esc  toggle full screen\n"
			"  g  toggle game mode\n"
			"  h  hide\n"
			"  i  iconify\n"
			"  c  show/hide cursor\n"
			"  q  quit",
			8, 8
		);
	}
	
	bool onEvent(Event::t e, GLV& g){
		switch(e){
		case Event::KeyDown:
			switch(g.keyboard().key()){
				case Key::Escape: win.fullScreenToggle(); break;
				case 'c': win.hideCursor(!win.hideCursor()); break;
				case 'g': win.gameModeToggle(); break;
				case 'h': win.hide(); count=0; break;
				case 'i': win.iconify(); count=0; break;
				case 's': win.show(); break;
				case 'q': Application::quit(); break;
				default:;
			}
			break;
		
		case Event::WindowResize: printf("Event::WindowResize\n"); break;
		case Event::WindowCreate: printf("Event::WindowCreate\n"); break;
		case Event::WindowDestroy: printf("Event::WindowDestroy\n"); break;
		case Event::Quit: printf("Event::Quit\n"); break;
		default:;
		}
		return false;
	}

	int count;
};

bool evWinResize(View * v, GLV& g){ printf("Event::WindowResize\n"); return false; }
bool evWinCreate(View * v, GLV& g){ printf("Event::WindowCreate\n"); return false; }
bool evWinDestroy(View * v, GLV& g){ printf("Event::WindowDestroy\n"); return false; }
bool evQuit(View * v, GLV& g){ printf("Event::Quit\n"); return false; }

int main(){
	GLV glv;
	MyView3D v3D;
	v3D.colors().set(StyleColor::Gray);
//	glv(Event::KeyDown, evKeyDown);
//	glv(Event::WindowResize, evWinResize);
//	glv(Event::WindowCreate, evWinCreate);
//	glv(Event::WindowDestroy, evWinDestroy);
//	glv(Event::Quit, evQuit);

	glv << v3D;
	win.setGLV(glv);
	Application::run();
}


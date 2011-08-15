/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

struct EventView : public View{
	bool onEvent(Event::t e, GLV& g){
		
		const Keyboard& k = g.keyboard();
		const Mouse& m = g.mouse();
		
		switch(e){
		case Event::KeyDown: printf("Event::KeyDown\n");
			k.print();
			break;
		case Event::KeyUp: printf("Event::KeyUp\n");
			k.print();
			break;
		case Event::MouseDown: printf("Event::MouseDown\n");
			m.print();
			break;
		case Event::MouseUp: printf("Event::MouseUp\n");
			m.print();
			break;
		case Event::MouseDrag: printf("Event::MouseDrag\n");
			m.print();
			break;
		case Event::MouseMove: break;
		case Event::WindowResize: printf("Event::WindowResize\n"); break;
		case Event::WindowCreate: printf("Event::WindowCreate\n"); break;
		case Event::WindowDestroy: printf("Event::WindowDestroy\n"); break;
		case Event::Quit: printf("Event::Quit\n"); break;
		default:;
		}
		return false;
	}
};


int main(){
	Window win(200,200);
	GLV glv;
	
	EventView v1, v2;

	v1.rect().set(0,0,200,200);
	v2.rect().set(100,100,100,100);

	glv << (v1 << v2);

	win.setGLV(glv);
	Application::run();
}


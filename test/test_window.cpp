#include "test_glv.h"

using namespace glv;

Window win(320, 240);
bool cursor = false;
bool show = false;

void drawCB(View * v){

	GLV& g = *(GLV *)v;
	Mouse& m = g.mouse;
	Keyboard& k = g.keyboard;

	using namespace draw;
	
	text(
		"Esc  toggle fullscreen\n"
		"  c  show/hide cursor\n",
		8, 8
	);
}


bool keydown(View * v, GLV& g){
	g.keyboard.print();
	switch(g.keyboard.key()){
		case Key::Escape: win.fullscreenToggle(); break;
		case 'c': win.hideCursor(cursor^=true); break;
		case 's': win.show(show^=true); break;
		default:;
	}
	return false;
}


int main (int argc, char ** argv){

	GLV glv(drawCB);
	glv(Event::KeyDown, keydown);
	glv.colors().back.set(0);

	win.setGLV(glv);
	
	Application::run();		// Application is also an AL binding, but unlike Window, it is a singleton
}


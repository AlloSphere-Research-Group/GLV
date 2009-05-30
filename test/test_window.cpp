/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

Window win(320, 240);
int count=0;

void drawCB(View * v){

	using namespace draw;

	if(200==count) win.show(); ++count;

	push3D(v->w, v->h);
		static float ang=0;
		translate(0,0,-1);
		rotate(0,ang,ang*0.31); ++ang;
		begin(Quads);
			color(1); vertex(-1,-1,0);
			color(0.7,0.3,0); vertex(-1,1,0); vertex(1,1,0); vertex(1,-1,0);
		end();
	pop3D();

	color(1);
	text(
		"Esc  toggle full screen\n"
		"  g  toggle game mode\n"
		"  h  hide\n"
		"  i  iconify\n"
		"  c  show/hide cursor",
		8.5, 8.5
	);
}


bool keydown(View * v, GLV& g){
	switch(g.keyboard.key()){
		case Key::Escape: win.fullScreenToggle(); break;
		case 'c': win.hideCursor(!win.hideCursor()); break;
		case 'g': win.gameModeToggle(); break;
		case 'h': win.hide(); count=0; break;
		case 'i': win.iconify(); count=0; break;
		case 's': win.show(); break;
		default:;
	}
	return false;
}


int main(){
	GLV glv(drawCB);
	glv(Event::KeyDown, keydown);
	win.setGLV(glv);
	Application::run();
}


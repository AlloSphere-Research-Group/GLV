#include "test_glv.h"

using namespace glv;

void drawCB(View * v){
	using namespace glv::draw;
	float numIcons = 10;
	float x = 20, y = 20, inc = pix((v->w - 2*x) / numIcons);
	float d = pix((v->w - 2*x - 10 * (numIcons-1)) / numIcons);
	
	color(0.2);
	lineWidth(2);
	rect		(x, y, x+d, y+d); x += inc;
	frame		(x, y, x+d, y+d); x += inc;
	triangleR	(x, y, x+d, y+d); x += inc;
	triangleL	(x, y, x+d, y+d); x += inc;
	triangleU	(x, y, x+d, y+d); x += inc;
	triangleD	(x, y, x+d, y+d); x += inc;
	draw::x		(x, y, x+d, y+d); x += inc;
	plus		(x, y, x+d, y+d); x += inc;
	minus		(x, y, x+d, y+d); x += inc;
	check		(x, y, x+d, y+d); x += inc;
	
	x = 20; y += d + 10;
	for(int i=3; i<8; ++i){ pgon  (x, y, d, d, i); x += inc; }
	for(int i=3; i<8; ++i){ spokes(x, y, d, d, i); x += inc; }
	
	x = 20; y += d + 10;
	for(int i=2; i<5; ++i){ for(int j=2; j<5; ++j){ grid(x,y,d,d,i,j); x+=inc; }}
	
	x = 20.5; y += d + 10; y = (int)y + 0.5;
	color(0); lineWidth(1);
	text("Amazingly few discotheques provide jukeboxes.", x, y);
	
	x = 20.5; y += d + 10;
	y = (int)y + 0.5;
	begin(Lines);
	for(int i=0; i<(127 - '!'); ++i){
		float dx = (i%16) * 8;
		float dy = (i/16) * 16;
		character(i + '!', x + dx, y + dy);
	}
	end();
}


int main (int argc, char ** argv){
	GLV top(drawCB);
	top.colors().back.set(1);
	
	Window win(400, 300, "GLV", &top);
	Application::run();
}


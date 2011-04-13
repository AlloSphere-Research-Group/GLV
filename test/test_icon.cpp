/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"

using namespace glv;

struct MyGLV : public GLV {
	void onDraw(GLV& g){
		using namespace glv::draw;
		float numIcons = 10;
		float x = 20, y = 20, inc = pix((width() - 2*x) / numIcons);
		float d = pix((width() - 2*x - 10 * (numIcons-1)) / numIcons);
		
		color(0.2);
		lineWidth(2);
		rectangle		(x, y, x+d, y+d); x+=inc;
		frame			(x, y, x+d, y+d); x+=inc;
		triangleR		(x, y, x+d, y+d); x+=inc;
		triangleL		(x, y, x+d, y+d); x+=inc;
		triangleU		(x, y, x+d, y+d); x+=inc;
		triangleD		(x, y, x+d, y+d); x+=inc;
		draw::x			(x, y, x+d, y+d); x+=inc;
		plus			(x, y, x+d, y+d); x+=inc;
		minus			(x, y, x+d, y+d); x+=inc;
		check			(x, y, x+d, y+d); x+=inc;

		x = 20; y += d + 10;
		frameTrunc<4,0,0,0>	(x,y, x+d,y+d); x+=inc;
		frameTrunc<0,4,0,0>	(x,y, x+d,y+d); x+=inc;
		frameTrunc<0,0,4,0>	(x,y, x+d,y+d); x+=inc;
		frameTrunc<0,0,0,4>	(x,y, x+d,y+d); x+=inc;

		frameTrunc<4,0,0,4>	(x,y, x+d,y+d); x+=inc;
		frameTrunc<0,0,4,4>	(x,y, x+d,y+d); x+=inc;
		frameTrunc<4,4,4,4>	(x,y, x+d,y+d); x+=inc;
		frameTrunc<2,2,2,2>	(x,y, x+d,y+d); x+=inc;
		//frameTrunc<2,2,2,2>	(x,y, x+d*2,y+d); x+=inc;
		
		frameTrunc<1,4,14,4>(x,y, x+d*2,y+d); x+=inc;
		
		x = 20; y += d + 10;
		polygon<3,1,0>	(x,y, x+d,y+d); x+=inc;
		polygon<4,1,0>	(x,y, x+d,y+d); x+=inc;
		polygon<5,1,0>	(x,y, x+d,y+d); x+=inc;
		polygon<6,1,0>	(x,y, x+d,y+d); x+=inc;
		polygon<7,1,0>	(x,y, x+d,y+d); x+=inc;
		
		spokes<3,90>	(x,y, x+d,y+d); x+=inc;
		spokes<4,45>	(x,y, x+d,y+d); x+=inc;
		spokes<5,0>		(x,y, x+d,y+d); x+=inc;
		spokes<6,0>		(x,y, x+d,y+d); x+=inc;
		spokes<7,0>		(x,y, x+d,y+d); x+=inc;

		x = 20; y += d + 10;
		polygon< 5,2,0>	(x,y, x+d,y+d); x+=inc;
		polygon< 7,2,0>	(x,y, x+d,y+d); x+=inc;
		polygon< 7,3,0>	(x,y, x+d,y+d); x+=inc;
		polygon< 8,3,0>	(x,y, x+d,y+d); x+=inc;
		polygon< 9,2,0>	(x,y, x+d,y+d); x+=inc;
		polygon< 9,4,0>	(x,y, x+d,y+d); x+=inc;
		polygon<10,3,0>	(x,y, x+d,y+d); x+=inc;

		x = 20; y += d + 10;
		disc<3>			(x, y, x+d, y+d); x+=inc;
		disc<4>			(x, y, x+d, y+d); x+=inc;
		disc<5>			(x, y, x+d, y+d); x+=inc;
		disc<6>			(x, y, x+d, y+d); x+=inc;
		disc<7>			(x, y, x+d, y+d); x+=inc;
		disc<20>		(x, y, x+d, y+d); x+=inc;

		x = 20; y += d + 10;
		circle<30>			(x, y, x+d, y+d); x+=inc;
		polygonCut< 4, 0>	(x, y, x+d, y+d); x+=inc;
		polygonCut< 4,45>	(x, y, x+d, y+d); x+=inc;
		polygonCut< 4,90>	(x, y, x+d, y+d); x+=inc;
		polygonCut< 6, 0>	(x, y, x+d, y+d); x+=inc;
		polygonCut< 8, 0>	(x, y, x+d, y+d); x+=inc;
		polygonCut<20, 0>	(x, y, x+d, y+d); x+=inc;
		polygonCut<20,45>	(x, y, x+d, y+d); x+=inc;
		polygonCut<20,90>	(x, y, x+d, y+d); x+=inc;
		polygonCut<20,-45>	(x, y, x+d, y+d); x+=inc;


		x = 20; y += d + 10;
		rose<60, 1,2,0>		(x,y, x+d,y+d); x+=inc;
		rose<60, 1,3,0>		(x,y, x+d,y+d); x+=inc;
		rose<60,-1,2,0>		(x,y, x+d,y+d); x+=inc;
		rose<60, 1,4,0>		(x,y, x+d,y+d); x+=inc;
		rose<60,-1,3,0>		(x,y, x+d,y+d); x+=inc;
		rose<60, 1,5,0>		(x,y, x+d,y+d); x+=inc;
		rose<60,-2,3,0>		(x,y, x+d,y+d); x+=inc;
		rose<60,-1,4,0>		(x,y, x+d,y+d); x+=inc;
		rose<60, 1,6,0>		(x,y, x+d,y+d); x+=inc;

		x = 20; y += d + 10;
		fileLoad			(x,y, x+d,y+d); x+=inc;
		fileSave			(x,y, x+d,y+d); x+=inc;
		magnifier			(x,y, x+d,y+d); x+=inc;

		//linesH(x,y,d,d, 5);				x += inc;
		//linePattern(x,y,d,d, 9, "1 1");	x += inc;
		
		x = 20; y += d + 10;
		GraphicsData gd;
		for(int i=2; i<5; ++i){ for(int j=2; j<5; ++j){ grid(gd, x,y,d,d,i,j); x+=inc; }}
		
		x = 20; y += d + 10; y = (int)y;
		color(0); lineWidth(1);
		text("Amazingly few discotheques provide jukeboxes.", x, y);
		
		x = 20; y += d + 10;
		y = (int)y;

		text(
			"!\"#$%&'()*+,-./0\n"
			"123456789:;<=>?@\n"
			"ABCDEFGHIJKLMNOP\n"
			"QRSTUVWXYZ[\\]^_`\n"
			"abcdefghijklmnop\n"
			"qrstuvwxyz{|}~",
			x,y
		);

//		for(int i=0; i<(127 - '!'); ++i){
//			float dx = (i%16) * 8;
//			float dy = (i/16) * 16;
//			character(i + '!', x + dx, y + dy);
//		}

//		{	Icon icon = Rectangle();
//			icon.draw(0,0,100,100);
//		}
//		{	Icon& icon = *new Rectangle();
//			icon.draw(100,0,200,100);
//		}
//		{	Icon * icon = new Rectangle;
//			icon->draw(200,0,300,100);
//		}
	}
};


int main (int argc, char ** argv){
	MyGLV top;
	top.colors().back.set(1);
	
	Window win(400, 500, "GLV", &top);
	Application::run();
}


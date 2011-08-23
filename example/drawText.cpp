/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: draw::text

This demonstrates how to quickly render text using the draw::text function.

*/

#include "example.h"

struct MyGLV : public GLV{
	void onDraw(GLV& g){
		using namespace glv::draw;

		color(0);
		lineWidth(1);
		
		float y = 10;

		// example sentence using all letters
		text("Amazingly few discotheques provide jukeboxes.", 10, y);

		y += 20;

		// a paragraph
		text(
			"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut\n"
			"labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco\n"
			"laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in\n"
			"voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat\n"
			"non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
			10, y, 6, 1.5
		);

		y += 70;
		
		// the printable ASCII set
		text(
			"!\"#$%&'()*+,-./0\n"
			"123456789:;<=>?@\n"
			"ABCDEFGHIJKLMNOP\n"
			"QRSTUVWXYZ[\\]^_`\n"
			"abcdefghijklmnop\n"
			"qrstuvwxyz{|}~",
			10, y
		);

		// tabbing is supported
		text("\
		zero	one		two		three\n\
zero	0		0		0		0\n\
one		0		1		2		3\n\
two		0		2		4		6\n\
three	0		3		6		9",
			200, y
		);

		y += 110;
		text("20 pixels", 10, y, 20);

		y += 30;
		text("12 pixels", 10, y, 12);

		y += 20;
		lineWidth(2);
		text("bold face", 10, y);

		lineWidth(1);
		color(HSV(0./3));
		text("red", 200, y);

		y += 12;
		color(HSV(1./3));
		text("green", 200, y);

		y += 12;
		color(HSV(2./3));
		text("blue", 200, y);
	}
};

int main(){
	MyGLV glv;			
	Window win(600,300, "Example: draw::text", &glv);
	Application::run();
}


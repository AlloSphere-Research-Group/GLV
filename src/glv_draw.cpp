/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <ctype.h>
#include "glv_draw.h"

namespace glv{
namespace draw{

// graphical ASCII characters lie in decimal range [33, 126]
static Glyph glyphs[94] = {
	{Glyph::Once | Glyph::Dot1 | 2, {3,4,4}, {7,0,5}},		// !
	{Glyph::Once | 4, {3,3,5,5}, {0,3,0,3}},				// "
	{Glyph::Once | 8, {1,7,1,7,3,3,5,5}, {3,3,5,5,1,7,1,7}},// #
	{0},	// $
	{Glyph::Once | Glyph::Dot2 | 2, {1,6,7,1}, {0,7,0,8}},	// %
	{7, {7,3,4,5,1,4,7}, {8,1,0,1,6,8,5}},					// &
	{Glyph::Once | 2, {5,3}, {0,3}},						// '
	{4, {5,3,3,5}, {0,2,6,8}},								// (
	{4, {3,5,5,3}, {0,2,6,8}},								// )
	{Glyph::Once | 6, {1,7,4,4,7,1}, {2,6,0,8,2,6}},		// *
	{Glyph::Once | 4, {1,7,4,4},{4,4,1,7}},					// +
	{Glyph::Once | 2, {4,2}, {7,10}},						// ,
	{2, {1,7}, {5,5}},										// -	
	{Glyph::Dot1, {3}, {7}},								// .
	{Glyph::Once | 2, {6,2}, {0,8.5}},						// /
	{6, {6,1,1,6,6,1}, {0,0,8,8,0,8}},						// 0
	{3, {2,4,4}, {1,0,8.5}},								// 1
	{6, {1,2,5,6,1,6}, {2,0,0,2,8,8}},						// 2
	{7, {1,6,6,2,6,6,1}, {0,0,2,3,4,7,8}},					// 3
	{4, {6,1,5,5}, {6,6,0,8.5}},							// 4
	{6, {6,1,1,6,6,1}, {0,0,2,3,7,8}},						// 5
	{6, {6,1,1,6,6,1}, {0,0,8,8,4,4}},						// 6
	{3, {1,6,3}, {0,0,8.5}},								// 7
	{7, {6,1,1,6,6,1,1}, {4,4,0,0,8,8,4}},					// 8
	{6, {6,1,1,6,6,1}, {4,4,0,0,8,8}},						// 9
	{Glyph::Dot2, {3,3}, {1,7}},							// :
	{Glyph::Once | Glyph::Dot1 | 2, {3,4,2}, {1,7,10}},		// ;
	{3, {7,1,7}, {0,4,8.5}},								// <
	{Glyph::Once | 4, {1,7,1,7}, {3,3,6,6}},				// =
	{3, {1,7,1}, {0,4,8.5}},								// >
	{Glyph::Dot1 | 5, {3,1,6,6,3,3}, {7,0,0,3,4,5}},		// ?
	{8, {7,3,3,7,7,1,1,7}, {2,3,6,6,0,0,8,8}},				// @
	{Glyph::Once | 6, {1,4,4,7,2,6}, {8.5,0,0,8.5,5,5}},	// A
	{8, {1,1,5,5,1,6,6,1}, {8,0,1,4,4,4,8,8}},				// B
	{4, {6,1,1,6}, {0,1,7,8}},								// C
	{5, {1,6,6,1,1}, {0,2,7,8,0}},							// D
	{Glyph::Once | 8, {1,1,1,6,1,5,1,6}, {0,8,0,0,4,4,8,8}},// E
	{Glyph::Once | 6, {1,1,1,6,1,5}, {0,8,0,0,4,4}},		// F
	{6, {6,1,1,6,6,4}, {0,1,7,8,5,5}},						// G
	{Glyph::Once | 6, {1,1,1,6,6,6}, {0,8.5,4,4,0,8.5}},	// H
	{Glyph::Once | 6, {2,6,4,4,2,6}, {0,0,0,8,8,8}},		// I
	{4, {1,1,6,6}, {6,8,8,-0.5}},							// J
	{Glyph::Once | 6, {1,1,6,1,1,6}, {-0.5,8.5,-0.5,4,4,8.5}},	// K
	{3, {1,1,6}, {-0.5,8,8}},								// L
	{5, {1,1,4,7,7}, {8.5,0,5,0,8.5}},						// M
	{4, {1,1,6,6}, {8.5,0,8,0}},							// N
	{5, {1,6,6,1,1}, {0,0,8,8,0}},							// O
	{5, {1,1,6,6,1}, {8.5,0,1,4,4}},						// P
	{6, {5,1,1,6,5,7}, {7,7,0,0,7,9}},						// Q
	{6, {1,1,6,6,1,6}, {8.5,0,1,4,4,8}},					// R
	{6, {6,1,1,6,6,1}, {0,0,3,5,8,8}},						// S
	{Glyph::Once | 4, {1,7,4,4}, {0,0,0,8.5}},				// T
	{4, {1,1,6,6}, {0,8,8,0}},								// U
	{3, {1,4,7}, {0,8,0}},									// V
	{5, {1,2,4,6,7}, {0,8,4,8,0}},							// W
	{Glyph::Once | 4, {1,6,6,1}, {0,8,0,8}},				// X
	{Glyph::Once | 6, {1,4,4,7,4,4}, {0,4,4,0,4,8.5}},		// Y
	{4, {1,6,1,6}, {0,0,8,8}},								// Z
	{4, {5,3,3,5}, {0,0,8,8}},								// [
	{Glyph::Once | 2, {2,6}, {0,8.5}},						/* \ */
	{4, {3,5,5,3}, {0,0,8,8}},								// ]
	{3, {1,4,7}, {4,0,4}},									// ^
	{Glyph::Once | 2, {1,7}, {8,8}},						// _
	{Glyph::Once | 2, {3,5}, {0,3}},						// `
	{6, {2,6,6,1,1,6}, {3,3,8,8,5,4}},						// a
	{5, {1,1,6,6,1}, {-0.5,8,8,4,3}},						// b
	{4, {6,1,1,6}, {3,4,8,8}},								// c
	{5, {6,6,1,1,6}, {-0.5,8,8,4,3}},						// d
	{6, {1,6,6,1,1,6}, {6,5,3,3,8,8}},						// e
	{Glyph::Once | 6, {7,3,3,3,5.5,1.5}, {0,0,0,8.5,3,3}},	// f
	{6, {6,1,1,6,6,2}, {8,7,3,3,10,10}},					// g
	{Glyph::Once | 6, {1,1,1,6,6,6}, {-0.5,8.5,3,4,4,8.5}},	// h
	{Glyph::Dot1 | 3, {3,3,4,4}, {0,3,3,8.5}},				// i
	{Glyph::Dot1 | 4, {4,4,5,5,1}, {0,3,3,10,10}},			// j
	{Glyph::Once | 6, {1,1,5,1,1,6}, {-0.5,8.5,3,5,5,8,5}},	// k
	{3, {3,3,5}, {-0.5,8.5,8.5}},							// l
	{7, {1,1,3,4,4,6,7}, {8.5,3,3,7,3,3,8.5}},				// m
	{4, {1,1,5,6}, {8.5,3,3,8.5}},							// n
	{5, {1,6,6,1,1}, {3,3,8,8,3}},							// o
	{5, {1,1,6,6,1}, {11,3,3,7,8}},							// p
	{5, {6,1,1,6,6}, {8,7,3,3,11}},							// q
	{4, {1,1,3,6}, {8.5,4,3,3}},							// r
	{6, {6.6,1,1,6,6,0.5}, {3,3,5,6,8,8}},					// s
	{Glyph::Once | 4, {1,6,3,3}, {3,3,-0.5,8.5}},			// t
	{4, {1,2,6,6}, {2.5,8,8,2.5}},							// u
	{3, {1,4,7}, {2.5,8,2.5}},								// v
	{5, {1,2,4,6,7}, {2.5,8,5,8,2.5}},						// w
	{Glyph::Once | 4, {1,6,1,6}, {8.5,2.5,2.5,8.5}},		// x
	{Glyph::Once | 4, {1,4,6,3}, {2.5,8,2.5,11}},			// y
	{4, {1,6,1,6.5}, {3,3,8,8}},							// z
	{7, {5,3,3,1,3,3,5}, {0,0,3,4,5,8,8}},					// {
	{Glyph::Once | 2, {4,4}, {0,8.5}},						// |
	{7, {3,5,5,7,5,5,3}, {0,0,3,4,5,8,8}},					// }
	{4, {1,3,5,7}, {2,0,2,0}}								// ~
};


bool character(int c, float dx, float dy){

	if(isgraph(c)){	// is graphical character?
		c -= 33;
		
		float * x = glyphs[c].x;
		float * y = glyphs[c].y;
		int n    = glyphs[c].size();
		int dots = glyphs[c].dots();

		if(dots){
			for(int j=0; j<dots; ++j){
				float l = x[j] + dx;	float t = y[j] + dy;
				float r = l + 1;		float b = t + 1;
				vertex(l, t); vertex(r, t); vertex(r, t); vertex(r, b);
				vertex(r, b); vertex(l, b);	vertex(l, b); vertex(l, t);
			}
			if(n == 0) return true;
			x += dots; y += dots;
		}

		n--;
		vertex(x[0] + dx, y[0] + dy);
		if(glyphs[c].once() == 0){	// line strip
			for(int i=1; i<n; ++i){
				vertex(x[i] + dx, y[i] + dy);
				vertex(x[i] + dx, y[i] + dy);
			}
		}
		else{		// normal lines
			for(int i=1; i<n; ++i) vertex(x[i] + dx, y[i] + dy);
		}
		vertex(x[n] + dx, y[n] + dy);
		return true;
	}
	return c == ' ';
}

void grid(float l, float t, float w, float h, float divx, float divy, bool incEnds){
	double inc, r=l+w, b=t+h;
	begin(Lines);
	if(divy > 0){
		inc = (double)h/(double)divy;
		double i = incEnds ? t-0.0001 : t-0.0001+inc, e = incEnds ? b : b-inc;
		for(; i<e; i+=inc){ vertex(l,i); vertex(r,i); }
	}
	if(divx > 0){
		inc = (double)w/(double)divx;
		double i = incEnds ? l-0.0001 : l-0.0001+inc, e = incEnds ? r : r-inc; 
		for(; i<e; i+=inc){ vertex(i,t); vertex(i,b); }
	}
	end();
}


void linesH(float l, float t, float w, float h, int n){
	double pos = 0;
	double inc = (double)h/(double)(n-1);
		
	push(); translate(l,t);
	begin(Lines);
	for(int i=0; i<n; ++i){
		vertex(0, pos); vertex(w, pos);
		pos+=inc;
	}
	end(); pop();
}


void linePattern(float l, float t, float w, float h, int n, const char * pat){
	const char * p = pat;
	double pos = 0;
	double inc = (double)w/(double)(n-1);
		
	push(); translate(l,t);
	begin(Lines);
	for(int i=0; i<n; ++i){
		if(!*p) p = pat;
		if(*p++ == '/'){ vertex(pos, 0); vertex(pos, h); }
		pos+=inc;
	}
	end(); pop();
}


void pgon(float l, float t, float w, float h, int n, float a){
	w *= 0.5; h *= 0.5;
	push(); translate(l + w, t + h); scale(w, h); rotate(0, 0, a * 360);
	begin(LineLoop);
	for(int i=0; i<n; ++i){
		float p = (double)i / (double)n * C_2PI;
		vertex(cos(p), sin(p));
	}
	end(); pop();
}


void spokes(float l, float t, float w, float h, int n, float a){
	w *= 0.5; h *= 0.5;
	push(); translate(l + w, t + h); scale(w, h); rotate(0, 0, a * 360);
	begin(Lines);
	for(int i=0; i<n; ++i){
		float p = (double)i / (double)n * C_2PI;
		vertex(0,0); vertex(cos(p), sin(p));
	}
	end(); pop();
}


void text(const char * s, float l, float t, float lineSpacing, unsigned int tabSpaces){
	float x=l, y=t, tabUnits = tabSpaces * Glyph::baseline();
	//x = (int)x + 0.5; y = (int)y + 0.5;
	begin(Lines);
	while(*s){
		switch(*s){
			case '\t':	x = ((int)(x/tabUnits) + 1) * tabUnits; break;
			case '\n':	x = l; y += Glyph::baseline() * 2.f * lineSpacing; break;
			default:	if(character(*s, x, y)) x += Glyph::baseline();
		}
		s++;
	}
	end();
}

} // draw::
} // glv::

#ifndef INC_GLV_COLOR_H
#define INC_GLV_COLOR_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_util.h"
#include <stdio.h>

namespace glv {

class HSV;

/// An RGBA color.
struct Color{

	float r;	///< Red component in [0, 1]
	float g;	///< Green component in [0, 1]
	float b;	///< Blue component in [0, 1]
	float a;	///< Alpha component in [0, 1]

	/// @param[in] r			red component
	/// @param[in] g			green component
	/// @param[in] b			blue component
	/// @param[in] a			alpha component
	Color(float r, float g, float b, float a = 1.f);
	
	/// @param[in] gray			red/green/blue components
	/// @param[in] a			alpha component
	Color(float gray=1.f, float a=1.f);
	
	Color(const HSV& hsv, float a=1.f);
	
	// Setters
	
	Color& operator= (const HSV& hsv);
	
	void set(float r, float g, float b, float a);	///< Set r, g, b, and a components
	void set(float r, float g, float b);			///< Set r, g, and b components
	void set(const Color& other, float rgbScale=1);	///< Copy another Color's colors
	void set(float gray);							///< Set as gray color
	void set(float gray, float alpha);				///< Set as gray color with alpha
	void invert();									///< Invert colors
	void scale(float amount);						///< Scale r, g, and b values
	
	/// Set color from HSV values in [0, 1]
	void setHSV(float h, float s, float v);

	/// Get HSV values in [0, 1] from color
	void getHSV(float &h, float &s, float &v) const;

	/// Set color from H value in [0, 6] and SV values in [0, 1].
	void setHSV6(float h, float s, float v);

	/// Get H value in [0, 6] and SV values in [0, 1] from color.
	void getHSV6(float &h, float &s, float &v) const;

	void print(const char * append="\n", FILE * fp=stdout) const;
};


struct HSV{

	float h, s, v;

	HSV(): h(0), s(1), v(1){}
	HSV(float h, float s, float v): h(h), s(s), v(v){}
	HSV(const Color& c){ *this = c; }
	
	HSV& operator=(const Color& c){
		c.getHSV(h, s, v); return *this;
	}
};



// Implementation ______________________________________________________________

inline Color::Color(float r, float g, float b, float a)
:	r(r), g(g), b(b), a(a)
{}

inline Color::Color(float v, float a)
:	r(v), g(v), b(v), a(a)
{}

inline Color::Color(const HSV& hsv, float a)
:	a(a)
{
	setHSV(hsv.h, hsv.s, hsv.v);
}


inline Color& Color::operator=(const HSV& hsv){
	setHSV(hsv.h, hsv.s, hsv.v); return *this;
}

inline void	Color::set(float re, float gr, float bl, float al){
	r = re; g = gr; b = bl; a = al;
}

inline void	Color::set(float re, float gr, float bl){ set(re, gr, bl, a); }

inline void	Color::set(const Color & c, float rgbScale){
	set(c.r, c.g, c.b, c.a);
	scale(rgbScale);
}
inline void	Color::set(float v){ set(v,v,v); }
inline void	Color::set(float v, float al){ set(v,v,v,al); }
inline void	Color::invert(){ set(1.f - r, 1.f - g, 1.f - b); }
inline void	Color::scale(float v){ set(r*v, g*v, b*v); }

inline void Color::setHSV(float h, float s, float v){
	setHSV6(h * 6.f, s, v);		
}

inline void Color::getHSV(float &h, float &s, float &v) const{
	getHSV6(h,s,v);
	h *= 0.166666667f;				// 0..1
}

} // glv::

#endif


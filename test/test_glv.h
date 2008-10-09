#ifndef INC_TEST_GLV_H
#define INC_TEST_GLV_H

#include "glv.h"
#include "glv_pimpl_binding.h"

namespace glv{


struct Group : public View{
	Group(const Rect& r=Rect(0))
	:	View(r)
	{
		disable(DrawBack | DrawBorder | CropChildren | FocusHighlight | HitTest);
	}
};


struct CharView : public View{
	CharView(const Rect& r=Rect(80, 110))
	:	View(r), input(0), thickness(2)
	{
		(*this)	(Event::MouseDrag, Behavior::mouseMove)
				(Event::MouseDrag, Behavior::mouseResize);

		disable(CropSelf);
	}
	
	int input;
	float thickness;

	virtual void onDraw(){
		using namespace glv::draw;
		scale(w/8, h/11);	// scale characters to extent of view
		
		color(colors().fore, 0.2);
		lineWidth(1);
		grid(0,0,8,11,8,11);
		
		color(colors().fore, 0.5);
		shape(Lines,0,8,8,8);	// draw base line
		
		lineWidth(thickness);
		color(colors().fore);
		begin(Lines);
			character(input);
		end();
	}
	
	virtual bool onEvent(Event::t e, GLV& glv){
		
		if(Event::KeyDown == e){
			int key = glv.keyboard.key();
			
			switch(key){
				case Key::Up:   thickness += 0.5; break;
				case Key::Down: thickness -= 0.5; if(thickness<1) thickness=1; break;
				default: input = key;
			}
		}
		
		return false;
	}
};


struct ColorView : public View{
	
	ColorView(const Rect& r=Rect(40)): View(r){}
	
	virtual void onDraw(){
		int divH = 32, divS = 16;
		float incH = 1./divH, incS = 1./divS;
		
		for(int i=0; i<divS; ++i){
			
			float sat = (float)i * incS;
			Color col;
			
			for(int j=0; j<divH; ++j){
				float hue = (float)j * incH;
				col.setHSV(hue, 1, sat*sat);
				draw::color(col);
				draw::rect(w * hue, h * sat, w * (hue + incH), h * (sat + incS));
			}
		}
	}
	
	virtual bool onEvent(Event::t e, GLV& glv){
		if(parent && glv.mouse.left()){
			float sat = glv.mouse.yRel()/h;
			parent->colors().back.setHSV(glv.mouse.xRel()/w, 1, sat*sat);
		}
		return false;
	}
};



/// View that mimics another View
class CopyCat : public View{
public:

	/// @param[in]	src				The View to mimic
	/// @param[in]	controllable	Whether to respond to input events
	CopyCat(View& src, bool controllable=true)
	:	View(src), mSource(&src)
	{	if(!controllable) disable(Controllable); }

	View& source() const { return *mSource; }
	CopyCat& source(View& v){ if(&v != this) mSource=&v; return *this; }

	virtual void onDraw(){
		space_t sw, sh; getDim(sw, sh); setDim(w, h);
		source().onDraw();
		setDim(sw, sh);
	}
	
	virtual bool onEvent(Event::t e, GLV& g){
		space_t sw, sh; getDim(sw, sh); setDim(w, h);
		bool r = source().onEvent(e, g);
		setDim(sw, sh);
		return r;
	}

protected:
	View * mSource;
	void getDim(space_t& sw, space_t& sh){ sw = source().w; sh = source().h; }
	void setDim(space_t  sw, space_t  sh){ source().w = sw; source().h = sh; }
};

} // glv::

#endif

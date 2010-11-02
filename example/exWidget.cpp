/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

/*
Example: 

*/

#include "example.h"


class Widget2 : public Widget{
public:
	Widget2(const Rect& r): Widget(r){}

	virtual bool onEvent(Event::t e, GLV& g){
		switch(e){			
			case Event::MouseDown:
				select(
					(g.mouse.xRel() / w) * sizeX(),
					(g.mouse.yRel() / h) * sizeY()
				);
				return false;

			case Event::MouseDrag:
				if(enabled(SelectOnDrag)){
					select(
						(g.mouse.xRel() / w) * sizeX(),
						(g.mouse.yRel() / h) * sizeY()
					);
				}
				return false;

			case Event::KeyDown:
				switch(g.keyboard.key()){
					case Key::Down:	select(selectedX()  , selectedY()+1); break;
					case Key::Up:	select(selectedX()  , selectedY()-1); break;
					case Key::Right:select(selectedX()+1, selectedY()  ); break;
					case Key::Left:	select(selectedX()-1, selectedY()  ); break;
					default: return true;
				}
				return false;

			default:;
		}
		return true;
	}
	
	virtual void onCellChange(int indexOld, int indexNew){
		//printf("%d -> %d\n", indexOld, indexNew);
	}
};


class Buttons2 : public Widget2{
public:
	Buttons2(const Rect& r): Widget2(r){}
	
	bool getValue(){ return getValue(selectedX(), selectedY()); }
	bool getValue(int ix){ return data().at<bool>(ix); }
	bool getValue(int ix, int iy){ return data().at<bool>(ix, iy); }
	
	virtual void onDraw(glv::GLV& g){
		Widget::onDraw(g);
		
		draw::color(colors().fore);
		const float p = padding();
		for(int j=0; j<sizeY(); ++j){
		for(int i=0; i<sizeX(); ++i){
			if(getValue(i,j)){
				draw::rectangle(i*dx()+p*0.5, j*dy()+p*0.5, (i+1)*dx()-p*0.5, (j+1)*dy()-p*0.5);
			}
		}}
	}
	
	virtual bool onEvent(Event::t e, GLV& g){
		Widget2::onEvent(e,g);
		switch(e){			

			case Event::MouseDown:
				setValue(!getValue());
				return false;

//			case Event::MouseDrag:
//				setValue(mPrevVal);
//				return false;

			default:;
		}
		return true;
	}
	
	virtual void onCellChange(int indexOld, int indexNew){
		
		//setValue();
	}
};


int main(){

	// Create the Views
	GLV glv;
	Buttons2 widget(Rect(400,400));

	widget.data().resize(Data::BOOL, 4,4);

	widget.disable(DrawGrid);
	widget.enable(DrawSelectionBox);
	widget.enable(SelectOnDrag);
	widget.pos(Place::CC).anchor(0.5,0.5);
	
	glv << widget;
	
	Window win(800,600, "Widget", &glv);

	Application::run();
}


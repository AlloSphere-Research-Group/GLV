/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_binding_fltk.h"
#include <FL/fl_draw.H>		// for cursor

namespace glv{


void AbstractWindow::hideCursor(bool v){}


//---- Application
void Application::quit(){
}

void Application::run(){
	AbstractApplication::run();
	Fl::run();
}


//---- Window
Window::Window(unsigned int width, unsigned int height, char * title, double framerate)
: AbstractWindow(width, height, title, framerate), Fl_Gl_Window(0, 0, width, height, title)
{	
	mFullscreenDeferred = false;
	Fl_Gl_Window::resizable(this);

	// Use timer to fix frame rate.
	Fl::add_timeout(1.0/framerate, Window::scheduleDraw, (void*)this);

	AbstractWindow::show(true);
	//fl_cursor(FL_CURSOR_NONE);
}

Window::~Window(){ }

void Window::scheduleDraw(void * userdata){
	Window * win = (Window *)userdata;
	win->redraw();
	Fl::repeat_timeout(1.0/win->fps(), scheduleDraw, userdata);
}

void Window::draw(){
	if (!glv) return;

	if(mFullscreenDeferred){
		mFullscreenDeferred = false;
		fltkFullscreen();
	}	
	
	if (mIsActive && mVisible){
		glv->drawGLV(AbstractWindow::w, AbstractWindow::h);

		// swap_buffers() is called automatically by FLTK after this method returns.
	}
}

int Window::handle(int event) {

	//---- Mouse Event
	if(FL_DRAG == event || FL_MOUSEWHEEL == event || FL_PUSH == event || FL_RELEASE == event){
		//printf("FLTK: mouse event x:%d y:%d dx:%3d dy:%3d bt:%d cl:%d\n", Fl::event_x(), Fl::event_y(), Fl::event_dx(), Fl::event_dy(), Fl::event_button(), Fl::event_clicks());
		space_t x = (space_t)Fl::event_x();
		space_t y = (space_t)Fl::event_y();
		space_t relx = x;
		space_t rely = y;
		
		int btn = Fl::event_button();
		switch(btn){
			case FL_LEFT_MOUSE:		btn = GLV_MOUSE_LEFT; break;
			case FL_MIDDLE_MOUSE:	btn = GLV_MOUSE_MIDDLE; break;
			case FL_RIGHT_MOUSE:	btn = GLV_MOUSE_RIGHT; break;
			default:				btn = GLV_MOUSE_EXTRA;		// unrecognized button
		}
		
		switch(event){
			case FL_DRAG:		glv->setMouseDrag(relx, rely, btn, Fl::event_clicks()); break;
			case FL_MOUSEWHEEL:	glv->setMouseWheel(Fl::event_dy()); break;
			case FL_PUSH:		glv->setMouseDown(relx, rely, btn, Fl::event_clicks()); break;
			case FL_RELEASE:	glv->setMouseUp(relx, rely, btn, Fl::event_clicks()); break;
		}

		glv->setMouseXY((int)x, (int)y, relx, rely);
		glv->propagateEvent();
		return 1;
	}

	//---- Keyboard Event
	//... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
	//... Return 1 if you understand/use the keyboard event, 0 otherwise...
	else if(FL_KEYDOWN == event || FL_KEYUP == event){
		
//		printf("FLTK: keyboard %d %d\n", Fl::event_key(), Fl::event_state());
		glv->keyboard.alt  (Fl::event_alt() != 0);
		glv->keyboard.caps ((Fl::event_state() & FL_CAPS_LOCK) != 0);
		glv->keyboard.ctrl (Fl::event_ctrl() != 0);
		glv->keyboard.meta ((Fl::event_state() & FL_META) != 0);
#if defined(__linux__)
		glv->keyboard.shift(Fl::event_shift() == 0);
#else
		glv->keyboard.shift(Fl::event_shift() != 0);
#endif

		switch(event){
			case FL_KEYDOWN:	glv->setKeyDown(Fl::event_key()); break;
			case FL_KEYUP:		glv->setKeyUp(Fl::event_key()); break;
		}

		glv->propagateEvent();
		return 1;
	}

	//---- Other Events
	else{
		switch(event) {
		case FL_FOCUS :
		case FL_UNFOCUS :	//... Return 1 if you want keyboard events, 0 otherwise
			return 1;
			
		case FL_SHORTCUT:	//... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
							//... Return 1 if you understand/use the shortcut event, 0 otherwise...
			return 1;
		
		default:			// pass other events to the base class...
			return Fl_Gl_Window::handle(event);
		}
	}
}

void Window::resize(int l, int t, int w, int h){
	AbstractWindow::resize(w, h);
}

void Window::platformFullscreen(){ mFullscreenDeferred = true; }

void Window::fltkFullscreen(){
	if(mFullscreen) Fl_Gl_Window::fullscreen();
	else			Fl_Gl_Window::fullscreen_off(0, 0, AbstractWindow::w, AbstractWindow::h);
}

void Window::platformResize(int width, int height){
	if(mFullscreen){
		int fx, fy, fw, fh;
		Fl::screen_xywh(fx, fy, fw, fh);
		Fl_Gl_Window::resize(fx, fy, fw, fh);
	}
	else Fl_Gl_Window::resize(20, 40, AbstractWindow::w, AbstractWindow::h);
}

void Window::platformShowHide(){
	mVisible ? Fl_Gl_Window::show() : Fl_Gl_Window::hide();
}

} // end namespace glv

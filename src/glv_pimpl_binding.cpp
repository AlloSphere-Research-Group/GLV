/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <stdio.h>
#include "glv_core.h"
#include "glv_pimpl_binding.h"

namespace glv{


void Application::run(){
	implRun();
}

void Application::quit(){
	
	// Send out Quit event to all GLV views
	for(unsigned i=0; i<windows().size(); ++i){
		Window& w = *windows()[i];
		if(w.glv) w.glv->broadcastEvent(Event::Quit);
	}
	
	implQuit();
}

std::vector<Window *>& Application::windows(){
	static std::vector<Window *> * ans = new std::vector<Window *>;
	return *ans;
}




Window::Window(unsigned int width, unsigned int height, char * title, GLV * glv_, double framerate, int mode)
:	glv(0), mFPS(framerate),
	mTitle(title), w(0), h(0),
	mDispMode(mode),
	mFullscreen(false), mVisible(true), mIsActive(false), mHideCursor(false)
{
	Application::windows().push_back(this);
	implCtor(width, height);

	mIsActive = true;	// assume that window has been successfully created
	if(glv_) setGLV(*glv_);
	setDims(width, height);
}

Window::~Window(){
	// remove self from application's list
	for(unsigned i=0; i<Application::windows().size(); ++i){
		if(Application::windows()[i] == this){
			Application::windows().erase(Application::windows().begin() + i);
			break;
		}
	}
	
	implDtor();
}

void Window::setDims(unsigned width, unsigned height){
	if(w!=width || h!=height){
		w=width; h=height;
		if(glv){
			glv->extent(w, h);
			if(active()) glv->broadcastEvent(Event::WindowResize);
		}
	}
}

void Window::setGLV(GLV& g){ 
	glv = &g;
	g.extent(w, h);
	onContextChange();
	if(active()) g.broadcastEvent(Event::WindowResize);
}

void Window::fullscreen(bool on){
	if(on && !mFullscreen){			// go into fullscreen
		wwin = w, hwin = h;		// store current window dimensions
		mFullscreen = true;
		implFullscreen();
		onContextChange();
	}
	else if(!on && mFullscreen){	// exit fullscreen
		//resize(wwin, hwin);
//		w = wwin; h = hwin;
//		if(glv) glv->extent(w, h);
		setDims(wwin, hwin);
		
		mFullscreen = false;
		implFullscreen();
		onContextChange();
	}
}

void Window::fullscreenToggle(){ fullscreen(!mFullscreen); }

void Window::hideCursor(bool hide){
	mHideCursor = hide;
	implHideCursor(hide);
}

void Window::onContextChange(){
	if(active()){
		GLV_PLATFORM_INIT_CONTEXT
		if(glv) glv->broadcastEvent(Event::ContextChange);
	}
}

void Window::resize(int width, int height){
	implResize(width, height);
	setDims(width, height);
//	if(w!=width || h!=height){
//		w = width; h = height;
//		if(glv){
//			glv->extent(w, h);
//			glv->broadcastEvent(Event::WindowResize);
//		}
//	}
}

bool Window::shouldDraw(){ return glv && active() && visible(); }

void Window::show(bool v){
	mVisible = v;
	implShowHide();
}

} // glv::

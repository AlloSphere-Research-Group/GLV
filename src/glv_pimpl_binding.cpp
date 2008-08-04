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
	implQuit();
}




void Window::setGLV(GLV & g){ 
	glv = &g;
	g.extent(w, h);
	onContextChange();
}

void Window::fullscreen(bool on){
	if(on && !mFullscreen){
		wwin = w, hwin = h;
		mFullscreen = true;
		implFullscreen();
		onContextChange();
	}
	else if(!on && mFullscreen){
		//resize(wwin, hwin);
		w = wwin; h = hwin;
		if(glv) glv->extent(w, h);
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
	if(mIsActive){
		GLV_PLATFORM_INIT_CONTEXT
		if(glv) glv->broadcastEvent(Event::ContextChange);
	}
}

void Window::resize(int width, int height){
	w = width; h = height;
	if(glv) glv->extent(w, h);
	implResize(w, h);
}

bool Window::shouldDraw(){ return glv && mIsActive && mVisible; }

void Window::show(bool v){
	mVisible = v;
	implShowHide();
}

} // glv::

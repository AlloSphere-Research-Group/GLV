#ifndef INC_GLV_PIMPL_BINDING_H
#define INC_GLV_PIMPL_BINDING_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <memory>

namespace glv{

class GLV;

/// Display mode bit masks
enum{
	SingleBuf	= 1<<0,
	DoubleBuf	= 1<<1,
	AccumBuf	= 1<<2,
	AlphaBuf	= 1<<3,
	DepthBuf	= 1<<4,
	StencilBuf	= 1<<5,
	Stereo		= 1<<6,
	DefaultBuf	= DoubleBuf|AlphaBuf|DepthBuf
};

class WindowImpl;

class Window{

public:
	Window(
		unsigned int width=800, unsigned int height=600, 
		char * title="GLV Window", GLV * glv=0, double framerate=40.0, int mode=DefaultBuf
	);
	~Window();

	int enabled(int dispMode) const { return mDispMode & dispMode; }
	void fullscreen(bool on);
	void fullscreenToggle();
	void resize(int w, int h);
	void show(bool v);
	void hideCursor(bool hide=true);

	void setGLV(GLV & g);

	double fps(){ return mFPS; }
	unsigned int height(){ return h; }
	unsigned int width(){ return w; }

	GLV * glv;
	
protected:	
	double mFPS;
	char * mLabel;
	unsigned int w, h, wwin, hwin;
	int mDispMode;		// display mode bit field
	bool mFullscreen;
	bool mVisible;		// window visible?
	bool mIsActive;		// window context ready?
	bool mHideCursor;	//
	
	bool shouldDraw();	// if the GLV views should be drawn
	
	void onContextChange();

	// These will be implemented in the the specific bindings
	void platformFullscreen();
	void platformHideCursor(bool hide);
	void platformResize(int width, int height);	// platform specific resize stuff
	void platformShowHide();						// platform specific show/hide stuff
    
    // pointer to the binding-specific implementation
    std::auto_ptr<WindowImpl> mImpl;
    // with the auto_ptr for the implementation, disallow assignment and copy
private:
    const Window& operator=(const Window&);
    Window(const Window&);
    friend class WindowImpl;
};



class Application{
public:

	static void			run();
	static void			quit();
//	static void			windowNotify(Notifier * sender, void * userdata);

protected:
	static Window *focusedWindow;
};

typedef Window AbstractWindow;
typedef Application AbstractApplication;

} // glv::

#endif


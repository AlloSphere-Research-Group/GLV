#ifndef INC_GLV_BINDING_H
#define INC_GLV_BINDING_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include <vector>

namespace glv{

class WindowImpl;	// binding-specific window implementation
class GLV;

/// Display mode bit masks
enum{
	SingleBuf	= 1<<0,		/**< Single-buffered */
	DoubleBuf	= 1<<1,		/**< Double-buffered */
	AccumBuf	= 1<<2,		/**< Use accumulation buffer */
	AlphaBuf	= 1<<3,		/**< Use alpha buffer */
	DepthBuf	= 1<<4,		/**< Use depth buffer */
	StencilBuf	= 1<<5,		/**< Use stencil buffer */
	Stereo		= 1<<6,		/**< Do left-right stereo buffering */
	Multisample = 1<<7,		/**< Multisampling support */
	DefaultBuf	= DoubleBuf|AlphaBuf|DepthBuf /**< Default display mode */
};



/// A window with an assignable GLV context
class Window{
public:

	struct Dimensions{ unsigned l,t,w,h; };

	/// Constructor
	Window(
		unsigned width=800, unsigned height=600, 
		const char * title="GLV Window", GLV * glv=0,
		double framerate=40.0, int mode=DefaultBuf
	);

	~Window();

	bool active() const { return mIsActive; }			///< Returns window active
	unsigned bottom() const { Dimensions d=dimensions(); return d.t+d.h; }	///< Returns bottom edge position
	Dimensions dimensions() const;						///< Get dimensions of window
	int enabled(int dispMode) const { return mDispMode & dispMode; } ///< Get a display mode status
	double fps() const { return mFPS; }					///< Returns requested frames/sec
	bool fullScreen() const { return mFullScreen; }		///< Returns full screen enabled
	bool gameMode() const { return mGameMode; }			///< Returns game mode enabled
	const GLV * glv() const { return mGLV; }			///< Get pointer to top-level GLV view
	unsigned height() const { return dimensions().h; }	///< Get window height
	bool hideCursor() const { return mHideCursor; }		///< Returns hide cursor enabled
	unsigned left() const { return dimensions().l; }	///< Returns left position
	bool visible() const { return mVisible; }			///< Returns visible enabled
	unsigned right() const { Dimensions d=dimensions(); return d.l+d.w; }	///< Returns right edge position
	unsigned top() const { return dimensions().t; }	///< Returns left position
	unsigned width() const { return dimensions().w; }	///< Get window width

	void dimensions(const Dimensions& d);				///< Set dimensions of window
	void fullScreen(bool on);							///< Set fullscreen mode
	void fullScreenToggle();							///< Toggle fullscreen
	void gameMode(bool on);								///< Set game mode
	void gameModeToggle();								///< Toggle game mode
	void hide();										///< Hide window
	void hideCursor(bool hide);							///< Show/hide cursor
	void iconify();										///< Iconify window
	void maximize(bool on);								///< Maximize window on screen
	void position(unsigned left, unsigned top);			///< Set position from left-top corner of screen
	void resize(unsigned w, unsigned h);				///< Resize window
	void setGLV(GLV& g);								///< Set top GLV View
	void show();										///< Show window
	
protected:
	friend class Application;

	GLV * mGLV;
	Dimensions mWinDims;	// backup for when going fullscreen
	double mFPS;
	const char * mTitle;
	int mDispMode;		// display mode bit field
	bool mFullScreen;
	bool mGameMode;
	bool mHideCursor;	// hide cursor?
	bool mIsActive;		// window context ready?
	bool mVisible;		// window visible? 
						// LJP: maybe not a good idea to manage this ourselves
						// since events from OS window manager can also change
						// window state.
	
	bool shouldDraw();	// if the GLV views should be drawn

	void onWindowCreate();
	void onWindowDestroy();

	// These are to be implemented by the specific binding
	void implCtor(unsigned w, unsigned h);		// this should manually create the mImpl object
	void implDtor();							// this should manually delete the mImpl object
	void implFinalize();						// call any cleanup/termination functions in windowing impl
	void implFullScreen();
	void implGameMode();
	void implHideCursor(bool hide);
	void implInitialize();						// initialize windowing impl
	void implPosition(unsigned l, unsigned t);
	void implResize(unsigned w, unsigned h);
	void implHide();
	void implShow();
	void implIconify();
	Dimensions implWinDims() const;				// get current position (left,top) and size of window from impl
	
    // pointer to the binding-specific implementation
	//std::auto_ptr<WindowImpl> mImpl;
	WindowImpl * mImpl;
    // with the auto_ptr for the implementation, disallow assignment and copy
private:
	void setGLVDims(unsigned w, unsigned h);
    const Window& operator=(const Window&);
    Window(const Window&);
    friend class WindowImpl;
};



/// Singleton GLV application
class Application{
public:

	/// Run main application event loop. This is a blocking call.
	static void	run();

	/// Quit main application.
	
	/// This can potentially cause the program to exit directly, so use 
	/// Event::Quit callbacks to handle any exiting code.
	static void	quit();
	
protected:
	friend class Window;
	static Window *focusedWindow;

	// Returns a vector of all created windows
	static std::vector<Window *>& windows();
	
	// These are to be implemented by the specific binding
	static void implRun();
	static void implQuit();
};

typedef Window AbstractWindow;
typedef Application AbstractApplication;

} // glv::

#endif


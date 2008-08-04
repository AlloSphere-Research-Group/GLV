/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_pimpl_binding.h"
#include "glv_core.h"

#if defined (__APPLE__) || defined (OSX)
	#include <GLUT/glut.h>
#endif
#if defined(__linux__)
	#include <GL/glut.h>
#endif
#ifdef WIN32
	#include <GL/glut.h>
#endif

#include <map>

namespace glv {

// GLUT window implementation
class WindowImpl
{
public:
    WindowImpl(Window *window, int window_id)
        : mWindow(window)
        , mGLUTWindowId(window_id)
        , mGLUTInFullScreen(false)
    {
        mWindows[mGLUTWindowId]=this;
    }
    ~WindowImpl()
    {
        mWindows.erase(mGLUTWindowId);
    }
	void draw();	// GLUT draw function
    void scheduleDraw()
    {
        scheduleDrawStatic(mGLUTInFullScreen?mGLUTFullscreenWindowId:mGLUTWindowId);
    }
    static WindowImpl *getWindowImpl()
    {
        return mWindows[glutGetWindow()];
    }
    static WindowImpl *getWindowImpl(int window_id)
    {
        return mWindows[window_id];
    }
    static Window *getWindow()
    {
        return getWindowImpl()->mWindow;
    }
private:
    static void scheduleDrawStatic(int window_id)
    {
        WindowImpl *impl = WindowImpl::getWindowImpl(window_id);
        if(impl)
        {
            int current = glutGetWindow();
            glutSetWindow(window_id);
            impl->draw();
            glutTimerFunc((unsigned int)(1000.0/WindowImpl::getWindow()->fps()), scheduleDrawStatic, window_id);
            if(current)
	            glutSetWindow(current);
        }
    }

    static std::map<int, WindowImpl *> mWindows;
    static bool mGLUTInitialized;

    Window *mWindow;
    int mGLUTWindowId;
    int mGLUTFullscreenWindowId;
    bool mGLUTInFullScreen;
    
    friend class Window;
};

bool WindowImpl::mGLUTInitialized = false;
std::map<int, WindowImpl *> WindowImpl::mWindows;



void Application::implQuit(){}

void Application::implRun(){
	glutMainLoop();	
}



static void glutDisplayCB(){
	// this is empty because we are using a periodic timer for drawing
}

// this must be called whenever a GLUT input event for a keyboard or mouse
// callback is generated.
static void modToGLV(){
    
	GLV * glv = WindowImpl::getWindow()->glv;
	int mod = glutGetModifiers();	
	glv->keyboard.alt  (mod & GLUT_ACTIVE_ALT);
	glv->keyboard.ctrl (mod & GLUT_ACTIVE_CTRL);
	glv->keyboard.shift(mod & GLUT_ACTIVE_SHIFT);
}

static void keyToGLV(unsigned int key, bool down, bool special){
	//printf("GLUT: keyboard event k:%d d:%d s:%d\n", key, down, special);
	GLV * glv = WindowImpl::getWindow()->glv;
	if(special){

		#define CS(glut, glv) case GLUT_KEY_##glut: key = Key::glv; break;
		switch(key){
			CS(LEFT, Left) CS(UP, Up) CS(RIGHT, Right) CS(DOWN, Down)
			CS(PAGE_UP, PageUp) CS(PAGE_DOWN, PageDown)
			CS(HOME, Home) CS(END, End) CS(INSERT, Insert)
		
			CS(F1, F1) CS(F2, F2) CS(F3, F3) CS(F4, F4)
			CS(F5, F5) CS(F6, F6) CS(F7, F7) CS(F8, F8)
			CS(F9, F9) CS(F10, F10)	CS(F11, F11) CS(F12, F12)
		}
		#undef CS
	}
	
	down ? glv->setKeyDown(key) : glv->setKeyUp(key);
	modToGLV();
	glv->propagateEvent();
}

static void glutKeyboardCB(unsigned char key, int x, int y){ keyToGLV(key, true, false); }
static void glutKeyboardUpCB(unsigned char key, int x, int y){ keyToGLV(key, false, false); }
static void glutSpecialCB(int key, int x, int y){ keyToGLV(key, true, true); }
static void glutSpecialUpCB(int key, int x, int y){ keyToGLV(key, false, true); }

static void glutMouseCB(int btn, int state, int ax, int ay){
	//printf("GLUT: mouse event x:%d y:%d bt:#%d,%d\n", ax,ay, btn, state==GLUT_DOWN);
	GLV * glv = WindowImpl::getWindow()->glv;
	space_t x = (space_t)ax;
	space_t y = (space_t)ay;
	space_t relx = x;
	space_t rely = y;

	switch(btn){
		case GLUT_LEFT_BUTTON:		btn = Mouse::Left; break;
		case GLUT_MIDDLE_BUTTON:	btn = Mouse::Middle; break;
		case GLUT_RIGHT_BUTTON:		btn = Mouse::Right; break;
		default:					btn = Mouse::Extra;		// unrecognized button
	}

	if(GLUT_DOWN == state)		glv->setMouseDown(relx, rely, btn, 0);
	else if(GLUT_UP == state)	glv->setMouseUp  (relx, rely, btn, 0);

	glv->setMousePos((int)x, (int)y, relx, rely);
	modToGLV();
	glv->propagateEvent();
}

static void glutMotionCB(int ax, int ay){
	//printf("GLUT: motion event x:%d y:%d\n", ax, ay);

	GLV * glv = WindowImpl::getWindow()->glv;
	space_t x = (space_t)ax;
	space_t y = (space_t)ay;
	space_t relx = x;
	space_t rely = y;
	
	//glv->setMouseDrag(relx, rely, glv->mouse.button(), 0);
	glv->setMouseDrag(relx, rely);
	glv->setMousePos((int)x, (int)y, relx, rely);
	//modToGLV();	// GLUT complains about calling glutGetModifiers()
	glv->propagateEvent();
}

static void glutReshapeCB(int w, int h){
	WindowImpl::getWindow()->resize(w, h);
}

static void registerCBs(){
	glutDisplayFunc(glutDisplayCB);
	glutKeyboardFunc(glutKeyboardCB);
	glutKeyboardUpFunc(glutKeyboardUpCB);
	glutMouseFunc(glutMouseCB);
	glutMotionFunc(glutMotionCB);
	glutReshapeFunc(glutReshapeCB);
	glutSpecialFunc(glutSpecialCB); 
	glutSpecialUpFunc(glutSpecialUpCB);
}

Window::Window(
	unsigned int w, unsigned int h, char * title, GLV * glv, double framerate, int mode
)
:	glv(0), mFPS(framerate),
	mLabel(title), 
	w(w), h(h),
	mDispMode(mode),
	mFullscreen(false), mVisible(true), mIsActive(true)
{
	if(glv) setGLV(*glv);
    
    if(!WindowImpl::mGLUTInitialized)
    {
        int argc = 0;
        char * argv[] = {0};

        glutInit(&argc,argv);
        WindowImpl::mGLUTInitialized = true;
    }
    glutInitWindowSize(w, h);
    //glutInitWindowPosition (100, 100);
		
    int bits = 
        (enabled(SingleBuf ) ? GLUT_SINGLE :0) |
        (enabled(DoubleBuf ) ? GLUT_DOUBLE :0) |
        (enabled(AccumBuf  ) ? GLUT_ACCUM  :0) |
        (enabled(AlphaBuf  ) ? GLUT_ALPHA  :0) |
        (enabled(DepthBuf  ) ? GLUT_DEPTH  :0) |
        (enabled(StencilBuf) ? GLUT_STENCIL:0) |
        (enabled(Stereo    ) ? GLUT_STEREO :0);
		
    glutInitDisplayMode(GLUT_RGBA | bits);
    int window_id = glutCreateWindow(title);
		
    glutIgnoreKeyRepeat(1);
		
    mImpl.reset(new WindowImpl(this, window_id));

	registerCBs();
    mImpl->scheduleDraw();
}

void Window::implFullscreen(){

// glutFullScreen() just maximizes the window.  We want use game mode to get rid
// of the OS toolbars.

	if(mFullscreen){

//		"width=1024 height=768 bpp=24 hertz=60"

//	=       Equal.
//	!=      Not equal.
//	<       Less  than  and  preferring larger difference (the
//			least is best).
//	>       Greater than  and  preferring  larger  differences
//			(the most is best).
//	<=      Less  than  or equal and preferring larger differ-
//			ence (the least is best).
//	>=      Greater than or equal and preferring more  instead
//			of less.  This comparator is useful for allocating
//			resources like color precsion or depth buffer pre-
//			cision  where  the  maximum  precison is generally
//			preferred.  Contrast with the tilde (~) comprator.
//	~       Greater  than or equal but preferring less instead
//			of more.  This compartor is useful for  allocating
//			resources  such as stencil bits or auxillary color
//			buffers where you would rather not over  allocate.

//		bpp     Bits per pixel for the frame buffer.
//		height  Height of the screen in pixels.
//		hertz   Video refresh rate of the screen in hertz.
//		num     Number  of  the  window  system depenedent display
//				mode configuration.
//		width   Width of the screen in pixels.
	
//		compact mode [ width "x" height ][ ":" bitsPerPixel ][ "@" videoRate ]

		// get current screen resolution
		int sw = glutGet(GLUT_SCREEN_WIDTH);
		int sh = glutGet(GLUT_SCREEN_HEIGHT);

		// use current resolution and refresh rate
		if(sw && sh){
			char buf[32];
			snprintf(buf, sizeof(buf), "%dx%d:24", sw, sh);
			glutGameModeString(buf);
			
			//int refresh = glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE);
			//printf("%d\n", refresh);
		}
		
		// otherwise, use sensible defaults
		else{
			glutGameModeString("1024x768:24");
		}
		
		mIsActive = false;
		mImpl->mGLUTFullscreenWindowId = glutEnterGameMode();
		WindowImpl::mWindows[mImpl->mGLUTFullscreenWindowId]=mImpl.get();
		mImpl->mGLUTInFullScreen = true;
		registerCBs();
		mImpl->scheduleDraw();
		mIsActive = true;
		
		hideCursor(mHideCursor);
	}
	else{
		mIsActive = false;
		WindowImpl::mWindows.erase(mImpl->mGLUTFullscreenWindowId);
		mImpl->mGLUTInFullScreen = false;
		glutLeaveGameMode();
		mIsActive = true;
	}
}

void Window::implHideCursor(bool v){
	glutSetCursor(v ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
}

void Window::implResize(int width, int height){
	glutReshapeWindow(width, height);
}

void Window::implShowHide(){ }

void WindowImpl::draw(){
	if(mWindow->shouldDraw()){
		mWindow->glv->drawGLV(mWindow->w, mWindow->h);
		glutSwapBuffers();
	}
}

Window::~Window()
{}


} // end namespace glv

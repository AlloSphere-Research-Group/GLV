#ifndef INC_GLV_CORE_H
#define INC_GLV_CORE_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_rect.h"
#include "glv_observer_pattern.h"
#include "glv_color.h"
#include "glv_draw.h"

#include <map>
#include <string>
#include <list>

namespace glv {

class GLV;
class View;

/// Type for graphical space values, i.e. dimensions, extents, positions, etc..
typedef float space_t;

/// Type for View rectangle geometry
typedef TRect<space_t> Rect;

/// Type for a drawing callback
typedef void (*drawCallback)(View * v);

/// Type for an event callback

/// The first parameter is the View receiving the event and the second is the
/// GLV context sending the event.  The function returns true if the event is
/// to be bubbled up to the receiver's parent View.
typedef bool (*eventCallback)(View * v, GLV& glv);

/// Type for a list of event callbacks
typedef std::list<eventCallback> eventCallbackList;



/// View property flags
enum{
	Visible			=1<< 0,	/**< Whether to draw myself */
	DrawBack		=1<< 1,	/**< Whether to draw back rect */
	DrawBorder		=1<< 2,	/**< Whether to draw border */
	CropChildren	=1<< 3,	/**< Whether to crop children when drawing */
	CropSelf		=1<< 4,	/**< Whether to crop own drawing routine(s) */
	Focused			=1<< 5,	/**< Whether View is focused */
	FocusHighlight	=1<< 6,	/**< Whether to highlight border when focused */
	HitTest			=1<< 7,	/**< Whether View can be clicked */
	Controllable	=1<< 8,	/**< Whether View can be controlled through events */
	
	DrawGrid		=1<<28,	/**< Whether to draw grid lines between widget elements */
	MutualExc		=1<<29,	/**< Whether only one element of a widget can be non-zero */
	SelectOnDrag	=1<<30,	/**< Whether a new element of a widget is selected while dragging */
	Toggleable		=1<<31	/**< Whether widget element toggles when clicked */
};



/// Direction types
struct Direction{

	/// Direction enumeration
	enum{
		N = 1<<0,	/**< North */
		E = 1<<1,	/**< East */
		S = 1<<2,	/**< South */
		W = 1<<3	/**< West */
	};

	Direction(): val(0){}
	Direction(int v): val(v){}
	operator int(){ return val; }
	int val;
};



/// Recognized event types.
namespace Event{

	/// Event type enum
	enum t{
		Null = 0,	/**< No event */
		
		// core events
		Quit,		/**< Application quit */
		ContextChange,	/**< New graphics context */
		
		// view events
		GetFocus,	/**< View got focus */
		LoseFocus,	/**< View lost focus */
		
		// mouse events
		MouseDown,	/**< Mouse button pressed */
		MouseUp,	/**< Mouse button released */
		MouseMove,	/**< Mouse has moved */
		MouseDrag,	/**< Mouse has moved with a button pressed */
		MouseWheel,	/**< Mouse wheel moved */
		
		// keyboard events
		KeyDown,	/**< Keyboard key pressed*/
		KeyUp,		/**< Keyboard key released */
		KeyRepeat,	/**< Keyboard key auto-repeated */
		
		// window events
	//	WindowActivated,	
	//	WindowDeactivated,
		Unused,		/**< Add to this for runtime event types */
		NumTypes	/**< Number of event types */
	};

	/// Returns a string of event type.
	const char * string(const Event::t e);
}



/// Constants of keyboard keys.
namespace Key{
	enum t{
		
		// Standard ASCII non-printable characters
		Enter		=3,		/**< */
		BackSpace	=8,		/**< */
		Tab			=9,		/**< */
		Return		=13,	/**< */
		Escape		=27,	/**< */
		Delete		=127,	/**< */
			
		// Non-standard, but common keys.
		F1=256, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
		Insert, Left, Up, Right, Down, PageDown, PageUp, End, Home,
	};
}



/// Placement types
namespace Place{
	enum t{
		TC=1<<0,	/**< Top-center */
		TR=1<<1,	/**< Top-right */
		CR=1<<2,	/**< Center-right */
		BR=1<<3,	/**< Bottom-right */
		BC=1<<4,	/**< Bottom-center */
		BL=1<<5,	/**< Bottom-left */
		CL=1<<6,	/**< Center-left */
		TL=1<<7,	/**< Top-left */
		CC=1<<8		/**< Center-center */
	};
}



/// Holds keyboard state.
class Keyboard{
	friend class GLV;
public:

	/// Constructor.
	Keyboard();
	~Keyboard(){}
	
	int key() const;		///< Returns key code (non-shifted character) of last key event.
	int keyAsNumber() const;///< Returns decimal number correlating to key code
	bool alt() const;		///< Whether an alt key is down.
	bool caps() const;		///< Whether capslock is down.
	bool ctrl() const;		///< Whether a ctrl key is down.
	bool meta() const;		///< Whether a meta (e.g. windows, apple) key is down.
	bool shift() const;		///< Whether a shift key is down.
	bool isDown() const;	///< Whether last event was button down.
	bool isNumber() const;	///< Whether key is a number key
	bool key(int k) const;	///< Whether the last key was 'k'.

	void alt  (bool state);	///< Set alt key state.
	void caps (bool state);	///< Set alt key state.
	void ctrl (bool state);	///< Set ctrl key state.
	void meta (bool state);	///< Set meta key state.
	void shift(bool state);	///< Set shift key state.

	void print();			///< Print keyboard state to stdout.

protected:
	int	mKeycode;			// last key event key number
	bool mIsDown;			// last key event state (pressed or released)
	bool mModifiers[5];		// Modifier key state array (shift, alt, ctrl, caps, meta)
};



/// Holds mouse state.
class Mouse{
friend class GLV;
public:

	/// Mouse button enumeration
	enum{ Left = 0, Middle, Right, Extra = (GLV_MAX_MOUSE_BUTTONS - 1) };

	/// Constructor.
	Mouse();
	~Mouse(){};

	// Accessors
	space_t x() const;				///< Current x position.
	space_t y() const;				///< Current y position.
	space_t x(int button) const;	///< Get button's last down x position.
	space_t y(int button) const;	///< Get button's last down y position.
	space_t w() const;				///< Current wheel position.
	
	space_t dx() const;				///< Current x velocity.
	space_t dy() const;				///< Current y velocity.
	space_t dw() const;				///< Current wheel velocity.
	space_t ddx() const;			///< Current x acceleration.
	space_t ddy() const;			///< Current y acceleration.
	space_t ddw() const;			///< Current wheel acceleration.

	space_t xRel() const;			///< Current x position relative to current listener.
	space_t yRel() const;			///< Current y position relative to current listener.
	
	int button() const;				///< Last event button number.
	int clicks() const;				///< Number of sequential clicks of buttons.
	bool isDown() const;			///< Whether last event was button down.
	bool isDown(int button) const;	///< Whether button is currently down.
	bool left() const;				///< Whether left button is currently down.
	bool middle() const;			///< Whether middle button is currently down.
	bool right() const;				///< Whether right button is currently down.
	

	void setContext(View * v);	///< Translate relative pos into sub-View.
	void unsetContext(View * v);///< Translate relative pos out of sub-View.

	void print();				///< Print out information about Mouse to stdout

protected:

	// Current (0) and previous (1,2) absolute mouse positions
	space_t mX[3], mY[3], mW[3];
	
	// Mouse coords in the current listener's relative coordinate space.
	space_t mXRel, mYRel;
	
	bool b[GLV_MAX_MOUSE_BUTTONS];		// button-down states
	space_t bx[GLV_MAX_MOUSE_BUTTONS];	// button-down absolute coordinates
	space_t by[GLV_MAX_MOUSE_BUTTONS];	
	
	int	mButton;
	int mClicks;
	bool mIsDown;
	
	void bufferPos(space_t newPos, space_t * prev3Pos);
	void pos(int x1, int y1);
	void posRel(space_t relx, space_t rely);
	void updateButton(int btn, bool pressed, int clicks);
};



/// Color style for View appearance.
class StyleColor{
public:
	StyleColor();
	
	enum{ BlackOnWhite, Gray, WhiteOnBlack };

	Color back;		///< Background
	Color border;	///< Border outline
	Color fore;		///< Foreground
	Color text;		///< Text

	void set(int preset);
	void hsv(float h, float s=1, float v=1);
};



/// Overall appearance scheme.
class Style : public SmartPointer{
public:
	Style(bool deletable): SmartPointer(deletable){}
	
	StyleColor color;	///< Color style

	/// Get reference to standard style
	static Style& standard(){
	
		// Note: This uses a Construct On First Use Idiom to avoid unpredictable
		// static initialization order.  The memory allocated will get freed from 
		// the heap when the program exits.
		static Style* ans = new Style(false);
		return *ans;
	}
};



/// The base class of all GUI elements.
class View : public Rect, public Notifier {

friend class GLV;

public:

	/// @param[in] left		Initial left edge position
	/// @param[in] top		Initial top edge position
	/// @param[in] width	Initial width
	/// @param[in] height	Initial height
	/// @param[in] cb		Drawing callback
	View(space_t left, space_t top, space_t width, space_t height, drawCallback cb=0);

	
	/// @param[in] rect		Rect geometry of View
	/// @param[in] anchor	Anchor place
	/// @param[in] cb		Drawing callback
	View(const Rect& rect=Rect(200, 200), Place::t anchor=Place::TL, drawCallback cb=0);

	virtual ~View();
	
	// Doubly linked tree list of views
	// TODO: move this stuff to a Node sub-class
	View * parent;				///< My parent view
	View * child;				///< My first child (next to be drawn)
	View * sibling;				///< My next sibling view (drawn after all my children)	
	void add(View & child);		///< Add a child view to myself, and update linked lists
	void makeLastSibling();
	void remove();				///< Remove myself from the parent view, and update linked lists

	/// Add a child view to myself
	View& operator << (View& child){ add( child); return *this; }
	View& operator << (View* child){ add(*child); return *this; }

	
	/// Map of event callback sequences.
	std::map<Event::t, eventCallbackList> callbackLists;
	
	drawCallback draw;		///< Drawing callback
	
	
	bool absToRel(View * target, space_t& x, space_t& y) const;
	StyleColor& colors() const;					///< Returns my style colors
	int enabled(int prop) const;				///< Returns whether a property is set
	int numEventCallbacks(Event::t e) const;	///< Returns number of registered callbacks
	void printDescendents() const;				///< Print tree of descendent Views to stdout
	void printFlags() const;					
	Style& style() const { return *mStyle; }	///< Get style object
	int visible() const;						///< Returns whether View is visible

		
	View& anchor(space_t mx, space_t my);		///< Set anchor translation factors	
	View& anchor(Place::t parentPlace);			///< Set anchor place on parent

	/// Append callback to a specific event type callback sequence.
	void appendCallback(Event::t type, eventCallback cb);
	View& operator()(Event::t e, eventCallback cb){ appendCallback(e, cb); return *this; }
	
	void cloneStyle();							///< Creates own copy of current style
	void constrainWithinParent();				///< Force to remain in parent	
	View& disable(int prop);					///< Disable property flag(s)
	View& enable(int prop);						///< Enable property flag(s)

	/// Returns View under these absolute coordinates or 0 if none.
	
	/// The coordinates are modified to be relative to the returned View's.
	///
	View * findTarget(space_t& x, space_t& y);

	void focused(bool b);						///< Set whether I'm focused
	void move(space_t x, space_t y);			///< Translate constraining within parent.
	void on(Event::t e, eventCallback cb=0);	///< Set first callback for a specific event type.

	virtual void onDraw();						///< Main drawing callback
	virtual bool onEvent(Event::t e, GLV& glv);	///< Main event callback
	virtual void onResize(space_t dx, space_t dy);	///< Resize callback

	View& pos(Place::t p, space_t x=0, space_t y=0);///< Set position according to a specific place on rect
	View& property(int prop, bool v);			///< Set property flag(s) to a specfic value	
	View& stretch(space_t mx, space_t my);		///< Set parent resize stretch factors
	void style(Style * style);					///< Set pointer to style	
	View& toggle(int prop);						///< Toggle property flag(s)
	
protected:
	int mFlags;						// Property flags
	Style * mStyle;					// Visual appearance
	space_t mAnchorX, mAnchorY;		// Position anchoring factors when parent is resized
	space_t mStretchX, mStretchY;	// Stretch factors when parent is resized

	void drawBack() const;			// Draw the back rect
	void drawBorder() const;		// Draw the border
	void reanchor(space_t dx, space_t dy);	// Reanchor when parent resizes
};


/// The top-level View.

/// 
///
class GLV : public View{
public:

	/// @param[in] cb		My drawing callback
	/// @param[in] width	Initial width
	/// @param[in] height	Initial height
	GLV(drawCallback cb = 0, space_t width = 800, space_t height = 600);

	Mouse mouse;		///< Current mouse state
	Keyboard keyboard;	///< Current keyboard state


	/// Send this event to everyone in tree
	void broadcastEvent(Event::t e);

	
	/// GLV MAIN RENDER LOOP: draw all Views in the GLV
	/// The assumption is that we are inside an OpenGL context of size [w, h]
	virtual void drawGLV(unsigned int w, unsigned int h);
	
	/// Draws all acive widgest in the GLV
	void drawWidgets(unsigned int w, unsigned int h);
	
	void eventType(Event::t e){ mEventType = e; }
	Event::t eventType() const { return mEventType; }
	
	/// Clears color and depth buffers.  Prepares OpenGL context for draw loop
	void preamble(unsigned int w, unsigned int h);

	/// Update input event state; called by external event handlers.
	/// be sure to set the eventType first!
	void propagateEvent();

	/// set current event target:
	void setFocus(View * v); 
	
	void setKeyDown(int keycode);		// Sets keyboard and GLV event state
	void setKeyUp(int keycode);			// Sets keyboard and GLV event state
	
	/// this function will modify the input coordinates to be relative to the target view's origin
	void setMouseDown(space_t& x, space_t& y, int button, int clicks);

	/// this function will modify the input coordinates to be relative to the target view's origin
	void setMouseDrag(space_t& x, space_t& y);
	
	// LJP: Deprecated. This doesn't work right when multiple buttons are held down.
	void setMouseDrag(space_t& x, space_t& y, int button, int clicks);

	void setMousePos(int x, int y, space_t relx, space_t rely);

	/// this function will modify the input coordinates to be relative to the target view's origin
	void setMouseUp(space_t& x, space_t& y, int button, int clicks);
	
	void setMouseMove(space_t& x, space_t& y);

	void setMouseWheel(int wheelDelta);	// Sets mouse and GLV event state

	View * focusedView(){ return mFocusedView; }

protected:
	View * mFocusedView;	// current focused widget
	Event::t mEventType;	// current event type
	
	// Returns whether the event should be bubbled to parent
	bool doEventCallbacks(View& target, glv::Event::t e);
	
	void doFocusCallback(bool get); // Call get or lose focus callback of focused view
};





// Implementation ______________________________________________________________

// View
inline int View::enabled(int prop) const { return mFlags & prop; }
inline int View::visible() const { return enabled(Visible); }

inline View& View::disable	(int prop){ mFlags &=~prop; return *this; }
inline View& View::enable	(int prop){ mFlags |= prop; return *this; }
inline View& View::property(int prop, bool v){ v ? enable(prop) : disable(prop); return *this; }
inline View& View::toggle	(int prop){ mFlags ^= prop; return *this; }

// Keyboard
inline int Keyboard::key() const { return mKeycode; }
inline int Keyboard::keyAsNumber() const { return key() - 48; }
inline bool Keyboard::isDown() const { return mIsDown; }
inline bool Keyboard::isNumber() const { return (key() >= '0') && (key() <= '9'); }
inline bool Keyboard::alt()   const { return mModifiers[1]; }
inline bool Keyboard::caps()  const { return mModifiers[3]; }
inline bool Keyboard::ctrl()  const { return mModifiers[2]; }
inline bool Keyboard::meta()  const { return mModifiers[4]; }
inline bool Keyboard::shift() const { return mModifiers[0]; }
inline bool Keyboard::key(int k) const { return mKeycode == k; }
inline void Keyboard::alt  (bool state){mModifiers[1] = state;}
inline void Keyboard::caps (bool state){mModifiers[3] = state;}
inline void Keyboard::ctrl (bool state){mModifiers[2] = state;}
inline void Keyboard::meta (bool state){mModifiers[4] = state;}
inline void Keyboard::shift(bool state){mModifiers[0] = state;}

// Mouse
inline space_t Mouse::x() const { return mX[0]; }
inline space_t Mouse::y() const { return mY[0]; }
inline space_t Mouse::x(int button) const { return bx[button]; }
inline space_t Mouse::y(int button) const { return by[button]; }
inline space_t Mouse::w() const { return mW[0]; }
inline space_t Mouse::dx() const { return mX[0] - mX[1]; }
inline space_t Mouse::dy() const { return mY[0] - mY[1]; }
inline space_t Mouse::dw() const { return mW[0] - mW[1]; }
inline space_t Mouse::ddx() const { return mX[0] - 2 * mX[1] + mX[2]; }
inline space_t Mouse::ddy() const { return mY[0] - 2 * mY[1] + mY[2];; }
inline space_t Mouse::ddw() const { return mW[0] - 2 * mW[1] + mW[2];; }
inline space_t Mouse::xRel() const { return mXRel; }
inline space_t Mouse::yRel() const { return mYRel; }
inline int Mouse::button() const { return mButton; }
inline int Mouse::clicks() const { return mClicks; }
inline bool Mouse::isDown() const { return mIsDown; }
inline bool Mouse::isDown(int button) const { return b[button]; }
inline bool Mouse::left() const { return b[Left]; }
inline bool Mouse::middle() const { return b[Middle]; }
inline bool Mouse::right() const { return b[Right]; }

inline void Mouse::  setContext(View * v){mXRel -= v->l; mYRel -= v->t;}
inline void Mouse::unsetContext(View * v){mXRel += v->l; mYRel += v->t;}
inline void	Mouse::pos(int x, int y) {
	bufferPos((space_t)x, mX);
	bufferPos((space_t)y, mY);
}
inline void Mouse::bufferPos(space_t newPos, space_t * pos){
	pos[2] = pos[1];
	pos[1] = pos[0];
	pos[0] = newPos;
}
inline void	Mouse::posRel(space_t rx, space_t ry){ mXRel=rx; mYRel=ry;}


} // glv::

#endif


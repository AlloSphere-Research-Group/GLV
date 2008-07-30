/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_binding_carbon.h"

//#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
	#define debug(format, args...) fprintf (stderr, format , ## args)
#else
	#define debug(format, args...)
#endif

namespace glv{

EventHandlerRef *	Application :: carbonApplicationHandlerRef;

void Application :: quit()
{
	QuitApplicationEventLoop();
	AbstractApplication::quit();
}

void Application :: run()
{
	OSStatus err;
	
	// the order of these calls is important:
	
	// INSTALL APPLICATION EVENT HANDLER //
	
	// create a typelist for events to respond to:
	const EventTypeSpec eventTypes[] =
	{	
	// keyboard
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyRepeat },
		//{ kEventClassKeyboard, kEventRawKeyModifiersChanged },
		
	// application
		{ kEventClassApplication, kEventAppActivated },
		{ kEventClassApplication, kEventAppDeactivated },
		
	// command
		{ kEventClassCommand, kEventCommandProcess }			
	};
	
	// register the multi-event handler
	err = InstallEventHandler (
			GetApplicationEventTarget(),		// EventTargetRef target, 
			NewEventHandlerUPP(carbonApplicationHandler),		// EventHandlerUPP handlerProc, 
			GetEventTypeCount(eventTypes),		// UInt32 numTypes, 
			eventTypes,							// const EventTypeSpec* typeList, 
			NULL,								// void * userData, (passed back to the handler)
			carbonApplicationHandlerRef			// EventHandlerRef* handlerRef (reference to the handler)
	); 
	
	if (err) printf("Error in registerApplicationHandler(): %i\n", err); 
	
	// INSTALL APPLICAITON MENU BAR //
	
	const char * name = "MenuBar";
	const char * resource = "main";
	
	IBNibRef nibRef;
	err = CreateNibReference(C2CFSTR(resource), &nibRef);
	
	if (err != 0) printf("unable to create Nib reference\n");
	if (err == 0) err = SetMenuBarFromNib(nibRef, C2CFSTR(name));
	if (err != 0) printf("unable to create menu bar\n");
	DisposeNibReference(nibRef);

	// CALL GLV APPLICATION RUN() //
	
	AbstractApplication::run();
	
	// START CARBON EVENT MODEL //
	
	RunApplicationEventLoop(); 
}

// a function to handle application level events:
OSStatus Application :: carbonApplicationHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void * udata) 
{	
	OSStatus result = noErr;
	GLV& glv = *((GLV *)udata);
	
	// Generic properties of all events:
	UInt32 eventClass = GetEventClass(theEvent); // e.g. to use in a switch statement
	UInt32 eventKind = GetEventKind(theEvent);
		
	// respond to event by type
	switch (eventClass) {
	
		case kEventClassKeyboard:
			// get keyboard keycode and modifiers:
			UInt32 keycode, modifiers;	
			char keychar;
			result = GetEventParameter (
				theEvent,				// EventRef inEvent,
				kEventParamKeyCode,		// EventParamName inName,
				typeUInt32,				// EventParamType inDesiredType,
				NULL,					// EventParamType * outActualType, /* can be NULL */
				sizeof(keycode),		// UInt32 inBufferSize,
				NULL,					// UInt32 * outActualSize, /* can be NULL */
				&keycode				// void * outData /* can be NULL */
			);			
			result = GetEventParameter (
				theEvent,				// EventRef inEvent,
				kEventParamKeyMacCharCodes,		// EventParamName inName,
				typeUTF8Text,				// EventParamType inDesiredType,
				NULL,					// EventParamType * outActualType, /* can be NULL */
				sizeof(keychar),		// UInt32 inBufferSize,
				NULL,					// UInt32 * outActualSize, /* can be NULL */
				&keychar				// void * outData /* can be NULL */
			);
			result = GetEventParameter (
				theEvent,				// EventRef inEvent,
				kEventParamKeyModifiers,	// EventParamName inName,
				typeUInt32,				// EventParamType inDesiredType,
				NULL,					// EventParamType * outActualType, /* can be NULL */
				sizeof(modifiers),		// UInt32 inBufferSize,
				NULL,					// UInt32 * outActualSize, /* can be NULL */
				&modifiers				// void * outData /* can be NULL */
			);
								
			// respond to event kind:
			switch (eventKind) {
				case kEventRawKeyUp:
					glv.eventType(Event::KeyUp);
					break;
				case kEventRawKeyDown:
					glv.eventType(Event::KeyDown);
					break;
				case kEventRawKeyRepeat:
					glv.eventType(Event::KeyRepeat);
					break;
//				case kEventRawKeyModifiersChanged:
//					glv.KeyEvent(0, keycode, keychar, modifiers);
//					debug("key mod %i %i %i\n", keycode, keychar, modifiers);
//					break;
				default:
					glv.eventType(Event::Null);
			}
						
			glv.propagateEvent();
				
			return result;		// stop the App from processing this event	
			break;	
			
		case kEventClassApplication:
		
			switch (eventKind) {
				case kEventAppActivated:
					debug("application activated\n");
					//self->mIsActive = true;
					break;
				case kEventAppDeactivated:
					debug("application deactivated\n");
					break;
				default:
					glv.eventType(Event::Null);
			}
					
			break;
					
		case kEventClassCommand:
		
			HICommand cmd;
			result = GetEventParameter(theEvent,
                            kEventParamDirectObject,
                            typeHICommand,
                            NULL,
                            sizeof(HICommand),
                            NULL,
                            &cmd);
							
			switch (cmd.commandID) 
			{
				case kHICommandOK: 
					debug("Command: ok \n");
					break; 
				case kHICommandCancel: 
					debug("Command: cancel \n");
					break;
				case kHICommandCopy: 
					debug("Command: copy \n");
					break;
				case kHICommandCut: 
					debug("Command: cut \n");
					break; 
				case kHICommandPaste: 
					debug("Command: paste \n");
					break;
				case kHICommandClear: 
					debug("Command: delete \n");
					break;
				case kHICommandSelectAll: 
					debug("Command: select all \n");
					break; 
				case kHICommandAbout: 
					debug("Command: about \n");
					break;
				case kHICommandAppHelp: 
					debug("Command: help \n");
					break;
				case kHICommandUndo: 
					debug("Command: undo \n");
					break; 
				case kHICommandRedo: 
					debug("Command: redo \n");
					break; 
				case kHICommandPreferences: 
					debug("Command: preferences \n");
					break; 
				case kHICommandNew:
					debug("Command: new \n");
					break; 
				case kHICommandOpen: 
					debug("Command: open \n");
					break; 
				case kHICommandClose: 
					debug("Command: close \n");
					break; 
				case kHICommandSave: 
					debug("Command: save \n");
					break; 
				case kHICommandSaveAs: 
					debug("Command: save as \n");
					break; 
				case kHICommandRevert: 
					debug("Command: revert \n");
					break; 
				case kHICommandPrint:
					debug("Command: print \n");
					break; 
				case kHICommandPageSetup: 
					debug("Command: page setup \n");
					break; 
				case kHICommandQuit: 
					debug("Command: quit \n"); 
					Application::quit();
					break; 
				
				default:
					glv.eventType(Event::Null);
			}

			break;
	}
	
	CallNextEventHandler( nextHandler, theEvent ); // propagate event

	return result; // Report failure 
}




Window :: Window(unsigned int width, unsigned int height, char * title, GLV * glv, double framerate)
: AbstractWindow(width, height, title, glv, framerate)
{
	OSStatus err;
	
	// CREATE A WINDOW FROM THE .NIB RESOURCE
	
	char * resource = "main";		// .nib reference
	char * resourceItem = "Window";
	
	/// create a nib reference from the resource name
	IBNibRef			nibRef;
	err = CreateNibReference(C2CFSTR(resource), &nibRef);
	if (err != 0) printf("unable to create Nib reference\n");
	
	err = CreateWindowFromNib(nibRef, C2CFSTR(resourceItem), &windowRef);
	if (err != 0) printf("unable to create window from Nib\n");
	DisposeNibReference(nibRef);
//
//	// create a window without using a .nib:
////	WindowAttributes  windowAttrs;               // Window attribute flags
////	CarbonRect              contentRect;               // Boundary of content region
////	EventTypeSpec     eventType;                 // Specifier for event type
////	EventHandlerUPP   handlerUPP;                // Pointer to event handler routine
//// 
////	windowAttrs = kWindowStandardDocumentAttributes      // Standard document window
////					| kWindowStandardHandlerAttribute;  // Use standard event handler
////	SetRect (&rectport, 100.0,  100.0,     // Set content rectangle
////						580.0, 460.0);
////	CreateNewWindow (kDocumentWindowClass, windowAttrs,  // Create the window
////                       &rectport, &windowRef);
 
	show(false);
	resize(width, height);
	
	CFStringRef CFTitle = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
    SetWindowTitleWithCFString(windowRef, CFTitle);
    CFRelease(CFTitle);
	
	// INITIALIZE AN OPENGL CONTEXT //
	
	//CGMainDisplayID returns the display ID of the main display 
	CGDirectDisplayID mainDisplay = CGMainDisplayID(); 
	
	/* THIS FOR FULLSCREEN 
	//  Captures display for exclusive use 
	CGDisplayCapture (mainDisplay); 
	//  Finds the closest display mode to the desired specifications 
	CFDictionaryRef refDisplayMode = CGDisplayBestModeForParameters (mainDisplay, bpp, width, height, NULL); 
	//  Switches display to the mode obtained in previous function 
	CGDisplaySwitchToMode (mainDisplay, refDisplayMode); 
	
	/* ---This here is Display Manager, from Carbon--- */ 
	//  Obtains a handle for the video device with the specified display ID 
	DMGetGDeviceByDisplayID ((DisplayIDType)mainDisplay, &graphicsdevice, false); 
	
	GLint			val = 0;
	
	short i = 0;
	
	attrib [i++] = AGL_ALL_RENDERERS;
	attrib [i++] = AGL_RGBA; // red green blue and alpha
	attrib [i++] = AGL_DOUBLEBUFFER; // double buffered
	attrib [i++] = AGL_ACCELERATED; // HWA pixel format only
	attrib [i++] = AGL_PIXEL_SIZE; // screen deoth we are looking for
	attrib [i++] = 32;
	attrib [i++] = AGL_DEPTH_SIZE; // screen deoth we are looking for
	attrib [i++] = 24;
	attrib [i++] = AGL_NONE;
	
	// from:http://tuvix.apple.com/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/index.html#//apple_ref/doc/uid/TP40001987
	if ((char *) kUnresolvedCFragSymbolAddress == (char *) aglChoosePixelFormat)  {// check for existence of OpenGL
		printf ("OpenGL not installed\n");
	}
	pixelformat = aglChoosePixelFormat(&graphicsdevice, 1, attrib); // this may fail if looking for acclerated across multiple monitors
	
	if (NULL == pixelformat) {
		printf("Could not find valid pixel format: %s\n", (char *)aglErrorString(aglGetError()));
	}

	aglDescribePixelFormat (pixelformat, AGL_PIXEL_SIZE, &val);

	aglcontext = aglCreateContext (pixelformat, NULL); // Create an AGL context
	if (NULL == aglcontext) {
		printf ("Could not create context: %s\n", (char *)aglErrorString(aglGetError()));
	}
	
	if (!aglSetDrawable(aglcontext, GetWindowPort(windowRef)))
	{
		printf ("Could not set drawable on window: %s\n", (char *)aglErrorString(aglGetError()));
	}

	if (!aglSetCurrentContext (aglcontext)) { // make the context the current context
		printf ("SetCurrentContext failed\n");
		aglSetDrawable (aglcontext, NULL); // turn off full screen
		debug(("%s\n", (char *)aglErrorString(aglGetError())));
	}
	
	long swapInterval = 1;
	aglSetInteger (aglcontext, AGL_SWAP_INTERVAL, &swapInterval);
	
	GetWindowBounds(windowRef, kWindowContentRgn, &rectport);
	
	//SetRect(&rectport, 0, 0, 600, 400); // l, t, r, b
	//sdebug (gcstrMode, "Stereo: %d x %d x %d", 480, 360, (**(**ghTargetDevice).gdPMap).pixelSize);
	

	
	
	
	// INITIALIZE THE WINDOW EVENT HANDLER //
	
	// get a target reference, using one of GetApplicationEventTarget, GetWindowEventTarget, GetMenuEventTarget, or GetControlEventTarget.
	EventTargetRef target = GetWindowEventTarget(windowRef);
	
	// create a handler
	EventHandlerUPP handlerProc; 
	handlerProc = NewEventHandlerUPP(Window::carbonWindowHandler); 
	
	// create a typelist for events to respond to:
	const EventTypeSpec eventTypes[] =
	{
	
	// keyboard
		{ kEventClassKeyboard, kEventRawKeyDown },
		{ kEventClassKeyboard, kEventRawKeyUp },
		{ kEventClassKeyboard, kEventRawKeyRepeat },
		{ kEventClassKeyboard, kEventRawKeyModifiersChanged },

	// mouse
		{ kEventClassMouse, kEventMouseDown },
		{ kEventClassMouse, kEventMouseUp },
		{ kEventClassMouse, kEventMouseMoved },
		{ kEventClassMouse, kEventMouseDragged },
		{ kEventClassMouse, kEventMouseEntered },
		{ kEventClassMouse, kEventMouseExited },
		{ kEventClassMouse, kEventMouseWheelMoved },

	// window
		{ kEventClassWindow, kEventWindowDrawContent },
		{ kEventClassWindow, kEventWindowContextualMenuSelect },
		{ kEventClassWindow, kEventWindowActivated },
		{ kEventClassWindow, kEventWindowDeactivated },
		{ kEventClassWindow, kEventWindowFocusAcquired },
		{ kEventClassWindow, kEventWindowFocusRelinquish },
		{ kEventClassWindow, kEventWindowClose },
		{ kEventClassWindow, kEventWindowClosed },
		{ kEventClassWindow, kEventWindowDispose },
		{ kEventClassWindow, kEventWindowResizeStarted },
		{ kEventClassWindow, kEventWindowResizeCompleted },
		{ kEventClassWindow, kEventWindowBoundsChanged },
		{ kEventClassWindow, kEventWindowGetIdealSize },
		{ kEventClassWindow, kEventWindowGetMinimumSize },
		{ kEventClassWindow, kEventWindowGetMaximumSize },
		
	// command
		{ kEventClassCommand, kEventCommandProcess }
	};
	
	// register the multi-event handler
	err = InstallEventHandler (		target,				// EventTargetRef target, 
									handlerProc,		// EventHandlerUPP handlerProc, 
									GetEventTypeCount(eventTypes),					// UInt32 numTypes, 
									eventTypes,			// const EventTypeSpec* typeList, 
									(void *)this,		// void* userData, (passed back to the handler)
									NULL				// EventHandlerRef* handlerRef (reference to the handler)
	); 
	
	timerUPP = NewEventLoopTimerUPP(carbonWindowDrawHandler);
	InstallEventLoopTimer(
		GetCurrentEventLoop(),					// event loop
		1000.0 * kEventDurationMillisecond/fps(),	// first delay
		1000.0 * kEventDurationMillisecond/fps(),	// repeat
		timerUPP,								// callback
		(void *)this,							// userdata
		0										// EventLoopTimerRef
	);
	
	eventClassDescription[kEventClassKeyboard] = "Keyboard Event";
	eventClassDescription[kEventClassMouse] = "Mouse Event";
	eventClassDescription[kEventClassWindow] = "Window Event";
	
	eventKindDescription[kEventRawKeyDown] = "Keyboard: RawKeyDown";
	eventKindDescription[kEventRawKeyUp] = "Keyboard: RawKeyUp";
	eventKindDescription[kEventRawKeyRepeat] = "Keyboard: RawKeyRepeat";
	eventKindDescription[kEventRawKeyModifiersChanged] = "Keyboard: RawKeyModifiersChanged";
	
	eventKindDescription[kEventMouseDown] = "Mouse: MouseDown";
	eventKindDescription[kEventMouseUp] = "Mouse: MouseUp";
	eventKindDescription[kEventMouseMoved] = "Mouse: MouseMoved";
	eventKindDescription[kEventMouseDragged] = "Mouse: MouseDragged";
	eventKindDescription[kEventMouseEntered] = "Mouse: MouseEntered";
	eventKindDescription[kEventMouseExited] = "Mouse: MouseExited";
	eventKindDescription[kEventMouseWheelMoved] = "Mouse: MouseWheelMoved";

	eventKindDescription[kEventWindowDrawContent] = "Window: DrawContent";
	eventKindDescription[kEventWindowContextualMenuSelect] = "Window: ContextualMenuSelect";
	eventKindDescription[kEventWindowActivated] = "Window: Activated";
	eventKindDescription[kEventWindowDeactivated] = "Window: Deactivated";
	eventKindDescription[kEventWindowFocusAcquired] = "Window: FocusAcquired";
	eventKindDescription[kEventWindowFocusRelinquish] = "Window: FocusRelinquish";
	eventKindDescription[kEventWindowClose] = "Window: Close";
	eventKindDescription[kEventWindowClosed] = "Window: Closed";
	eventKindDescription[kEventWindowDispose] = "Window: Dispose";
	eventKindDescription[kEventWindowResizeStarted] = "Window: ResizeStarted";
	eventKindDescription[kEventWindowResizeCompleted] = "Window: ResizeCompleted";
	eventKindDescription[kEventWindowBoundsChanged] = "Window: BoundsChanged";
	eventKindDescription[kEventWindowGetIdealSize] = "Window: GetIdealSize";
	eventKindDescription[kEventWindowGetMinimumSize] = "Window: GetMinimumSize";
	eventKindDescription[kEventWindowGetMaximumSize] = "Window: GetMaximumSize";

	// FOCUS AND SHOW THE WINDOW //
	
	show(true);
	ActivateWindow(windowRef, true);
}

Window :: ~Window()
{

}

void Window :: platformResize(int width, int height){
	SizeWindow(windowRef, width, height, true);
}

void Window :: platformShowHide(){
	ShowHide(windowRef, mVisible);
}

pascal void Window :: carbonWindowDrawHandler(EventLoopTimerRef inTimer, void* udata)
{
	Window * self = (Window *)udata;

	if (self->shouldDraw() && self->aglcontext)
	{
		aglSetCurrentContext(self->aglcontext);	// ensure our context is current prior to drawing
		aglUpdateContext(self->aglcontext);		// test not normally needed
		self->glv->drawGLV(self->w, self->h);
		aglSwapBuffers(self->aglcontext);		// send swap command
	}
}


// convert carbon mouse button to GLV
static int mouseButtonToGLV(EventMouseButton b){
	switch(b){
		case kEventMouseButtonPrimary: return Mouse::Left;
		case kEventMouseButtonSecondary: return Mouse::Right;
		case kEventMouseButtonTertiary: return Mouse::Middle;
		default: return Mouse::Extra;
	}
}

// a function to handle keyboard events:
OSStatus Window :: carbonWindowHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void * udata) 
{
	Window * self = (Window *)udata;
	if(!self->glv) return -1;
	GLV& glv = *self->glv;
	
	OSStatus result = noErr;
	
	// Generic properties of all events:
	UInt32 eventClass = GetEventClass(theEvent); // e.g. to use in a switch statement
	UInt32 eventKind = GetEventKind(theEvent); 
	
//	debug("Event: %s\t%s\n",	self->eventClassDescription[eventClass].data(),
//								self->eventKindDescription[eventKind].data());

//	extern OSStatus 
//	GetEventParameter(
//	  EventRef          inEvent,
//	  EventParamName    inName,
//	  EventParamType    inDesiredType,
//	  EventParamType *  outActualType,       /* can be NULL */
//	  UInt32            inBufferSize,
//	  UInt32 *          outActualSize,       /* can be NULL */
//	  void *            outData)             /* can be NULL */ 
	
	// respond to event by type
	switch (eventClass) {
	
		case kEventClassMouse:

			EventMouseButton button;
			UInt32 clicks;
		
			// get mouse position
			int x, y;
			space_t relx, rely;
			HIPoint pt;
			GetEventParameter(theEvent, kEventParamWindowMouseLocation, typeHIPoint, 0, sizeof(pt), 0, &pt);
			x = (int)pt.x - 1;
			y = (int)pt.y - CARBON_TITLEBARHEIGHT;
			relx = (space_t)x;
			rely = (space_t)y;
												
			// respond to event kind:
			switch(eventKind){

				case kEventMouseDown:
					GetEventParameter(theEvent, kEventParamMouseButton, typeMouseButton, 0, sizeof(button), 0, &button);
					GetEventParameter(theEvent, kEventParamClickCount, typeUInt32, 0, sizeof(clicks), 0, &clicks);
					glv.setMouseDown(relx, rely, mouseButtonToGLV(button), clicks);
					break;

				case kEventMouseDragged:
					glv.setMouseDrag(relx, rely);
					break;
				
				case kEventMouseMoved:
					glv.setMouseMove(relx, rely);
					break;
				
				case kEventMouseUp:
					GetEventParameter(theEvent, kEventParamMouseButton, typeMouseButton, 0, sizeof(button), 0, &button);
					glv.setMouseUp(relx, rely, mouseButtonToGLV(button), 1);
					break;

				case kEventMouseWheelMoved:
					SInt32 wheelDelta;	// EventMouseWheelAxis is an int for the axis 0 or 1
					GetEventParameter(theEvent,	kEventParamMouseWheelDelta, typeSInt32, 0, sizeof(wheelDelta), 0, &wheelDelta);
					//EventMouseWheelAxis wheelAxis;	// EventMouseWheelAxis is an int for the axis 0 or 1
					//GetEventParameter(theEvent,	kEventParamMouseWheelAxis, typeMouseWheelAxis, 0, sizeof(wheelAxis), 0, &wheelAxis);
					glv.setMouseWheel(wheelDelta);
					break;

				default:
					glv.eventType(Event::Null);
			}
			
			glv.setMousePos(x, y, relx, rely);
			glv.propagateEvent();
			break;
	
		case kEventClassKeyboard:
		
			// get keyboard keycode and modifiers:
			UInt32 keycode, modifiers;	
			char keychar;			
			GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, 0, sizeof(keycode), 0, &keycode);			
			GetEventParameter(theEvent, kEventParamKeyMacCharCodes, typeUTF8Text, 0, sizeof(keychar), 0, &keychar);
			GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, 0, sizeof(modifiers), 0, &modifiers);

			// respond to event kind:
			switch(eventKind){
				case kEventRawKeyUp:	glv.setKeyUp((int)keychar); break;
				case kEventRawKeyDown:	glv.setKeyDown((int)keychar); break;
				case kEventRawKeyRepeat:glv.eventType(Event::KeyRepeat); break;
//				case kEventRawKeyModifiersChanged:
//					glv.KeyEvent(0, keycode, keychar, modifiers);
//					debug("key mod %i %i %i\n", keycode, keychar, modifiers);
//					break;
				default:
					glv.eventType(Event::Null);
			}

			//debug("%d\n", modifiers);
			//for(int i=16; i>=0; i--){ debug("%d", (modifiers >> i) & 1 ); } debug("\n");

			glv.keyboard.alt  (modifiers & optionKey);
			glv.keyboard.caps (modifiers & alphaLock);
			glv.keyboard.ctrl (modifiers & controlKey);
			glv.keyboard.shift(modifiers & shiftKey);
			glv.propagateEvent();
				
			return result;		// stop the App from processing this event	
			break;		
		
		case kEventClassCommand:
		
			HICommand cmd;
			result = GetEventParameter(theEvent,
                            kEventParamDirectObject,
                            typeHICommand,
                            NULL,
                            sizeof(HICommand),
                            NULL,
                            &cmd);
							
			switch (cmd.commandID) 
			{
				case kHICommandOK: 
					debug("Command: ok \n");
					break; 
				case kHICommandCancel: 
					debug("Command: cancel \n");
					break;
				case kHICommandCopy: 
					debug("Command: copy \n");
					break;
				case kHICommandCut: 
					debug("Command: cut \n");
					break; 
				case kHICommandPaste: 
					debug("Command: paste \n");
					break;
				case kHICommandClear: 
					debug("Command: delete \n");
					break;
				case kHICommandSelectAll: 
					debug("Command: select all \n");
					break; 
				case kHICommandAbout: 
					debug("Command: about \n");
					break;
				case kHICommandAppHelp: 
					debug("Command: help \n");
					break;
				case kHICommandUndo: 
					debug("Command: undo \n");
					break; 
				case kHICommandRedo: 
					debug("Command: redo \n");
					break; 
				case kHICommandPreferences: 
					debug("Command: preferences \n");
					break; 
				case kHICommandNew:
					debug("Command: new \n");
					break; 
				case kHICommandOpen: 
					debug("Command: open \n");
					break; 
				case kHICommandClose: 
					debug("Command: close \n");
					break; 
				case kHICommandSave: 
					debug("Command: save \n");
					break; 
				case kHICommandSaveAs: 
					debug("Command: save as \n");
					break; 
				case kHICommandRevert: 
					debug("Command: revert \n");
					break; 
				case kHICommandPrint:
					debug("Command: print \n");
					break; 
				case kHICommandPageSetup: 
					debug("Command: page setup \n");
					break; 
				case kHICommandQuit: 
					debug("Command: quit \n"); 
					Application::quit();
					break; 
				default:
					glv.eventType(Event::Null);
			}
			
			return result;		// stop the App from processing this event
			break;
	
		case kEventClassWindow:
		
			switch (eventKind) 
			{
				case kEventWindowDrawContent: 
					debug("Window draw \n");
					break; 
				case kEventWindowContextualMenuSelect: 
					debug("Window right-click context menu \n");
					break;
				case kEventWindowActivated: { 
					debug("Window activated \n");
					debug("Event::GetFocus: %d  glv.eventtype: %d", Event::GetFocus, glv.eventType());
					self->mIsActive = true;
					break; 
				}
				case kEventWindowDeactivated: { 
					debug("Window deactivated \n");
					//ActivateWindow(self->windowRef, false);
					//self->mIsActive = false;
					break; 
				}
				case kEventWindowFocusAcquired: 
					debug("Event::GetFocus: %d  glv.eventtype: %d", Event::GetFocus, glv.eventType());
					debug("Window got focus \n");
					ActivateWindow(self->windowRef, true);
					break;
				case kEventWindowFocusRelinquish: 
					debug("Window lost focus \n");
					break;
				case kEventWindowClose: 
					debug("Window close \n"); 
					break; 
				case kEventWindowClosed: { 
					debug("Window closed \n"); 
					self->mIsActive = false;
					break; 
				}
				case kEventWindowDispose: { 
					//debug("Window dispose \n"); 
					DisposeEventLoopTimerUPP(self->timerUPP);
					break; 
				}
				case kEventWindowResizeStarted: { 
					//debug("Window resize start \n"); 
					self->mIsActive = false;
					break; 
				}
				case kEventWindowBoundsChanged: { 
					//debug("Window bounds changed \n"); 
					CarbonRect newbounds;
					unsigned int x, y;
					result = GetEventParameter (
						theEvent,				// EventRef inEvent,
						kEventParamCurrentBounds,	// EventParamName inName,
						typeQDRectangle,				// EventParamType inDesiredType,
						NULL,					// EventParamType * outActualType, /* can be NULL */
						sizeof(newbounds),		// UInt32 inBufferSize,
						NULL,					// UInt32 * outActualSize, /* can be NULL */
						&newbounds				// void * outData /* can be NULL */
					);
					
					x = newbounds.right-newbounds.left;
					y = newbounds.bottom-newbounds.top;
					
					debug("resize %i %i\n", x, y);
					
					if (x != self->w & y != self->h) {					
						self->resize(x,y);
					}
					break;
				}
				case kEventWindowResizeCompleted: { 
					debug("Window resize complete \n"); 
//					// this is necessary to restart drawing
					self->mIsActive = true;
					ActivateWindow(self->windowRef, true);
					break;
				}
				case kEventWindowGetIdealSize: { 
					debug("Window get ideal size \n"); 
					break; 
				}
				case kEventWindowGetMinimumSize: { 
					debug("Window get min size \n"); 
					break; 
				}	
				case kEventWindowGetMaximumSize: { 
					debug("Window get max size \n"); 
					break; 
				}	
				default:
					glv.eventType(Event::Null);
			}
			
			break;

	}
	
	CallNextEventHandler(nextHandler, theEvent); // propagate event

	return result; // Report any failure 
}

} // end namespace glv

#undef debug

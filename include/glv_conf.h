#ifndef INC_GLV_CONF_H
#define INC_GLV_CONF_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#define	GLV_VERSION "0.79"
#define	GLV_MAX_MOUSE_BUTTONS 8


// Macros for binding GLV to a specific platform's windowing system.
// Type "#include GLV_BINDING_H" in the source files that need it.
#ifndef GLV_BINDING_H
	#if defined (__APPLE__) || defined (OSX)
		#define GLV_BINDING_H "glv_binding_carbon.h"
	#endif
	#if defined(__linux__)
		#define GLV_BINDING_H "glv_binding_glut.h"
	#endif
	#ifdef WIN32
		#define GLV_BINDING_H ""
	#endif
#endif


// OpenGL platform-dependent includes
#if defined (__APPLE__) || defined (OSX)
	
	#define GLV_PLATFORM		"OSX"

	//#include <Carbon/Carbon.h>
	//#include <AGL/agl.h>
	//#include <AGL/aglRenderers.h>
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
	
	#define C2CFSTR(cstr) CFStringCreateWithCString(kCFAllocatorDefault, cstr, kCFStringEncodingMacRoman)	

	#define GLV_PLATFORM_INIT\
		GLint MacHackVBL = 1;\
		CGLSetParameter(CGLGetCurrentContext(),  kCGLCPSwapInterval, &MacHackVBL);

#endif

#if defined(__linux__)
	
	#define GLV_PLATFORM		"UNIX"
	
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <time.h>

	#define GLV_PLATFORM_INIT

#endif

#ifdef WIN32
	
	#define GLV_PLATFORM		"WIN32"

	#include <windows.h>

	#include <gl/gl.h>
	#include <gl/glu.h>

	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )
	
	#define GLV_PLATFORM_INIT

#endif



#endif

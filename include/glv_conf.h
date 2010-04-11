#ifndef INC_GLV_CONF_H
#define INC_GLV_CONF_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#define	GLV_VERSION "0.94"
#define	GLV_MAX_MOUSE_BUTTONS 8

//#define GLV_USE_OPENGL_ES 1


// OpenGL platform-dependent includes
#if defined (__APPLE__) || defined (OSX)
	
	#define GLV_PLATFORM		"OSX"
	#define GLV_PLATFORM_OSX

	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>

	#define GLV_PLATFORM_INIT_CONTEXT\
		{	/* prevents tearing */\
			GLint MacHackVBL = 1;\
			CGLContextObj ctx = CGLGetCurrentContext();\
			CGLSetParameter(ctx,  kCGLCPSwapInterval, &MacHackVBL); }

#endif

#if defined(__linux__)

	#define GLV_PLATFORM		"UNIX"
	#define GLV_PLATFORM_UNIX

	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <time.h>

	#define GLV_PLATFORM_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if (GLEW_OK != err){\
  				/* Problem: glewInit failed, something is seriously wrong. */\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}
#endif

#ifdef WIN32
	
	#define GLV_PLATFORM		"WIN32"
	#define GLV_PLATFORM_WIN

	#include <windows.h>

	#include <gl/gl.h>
	#include <gl/glu.h>

	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
	#pragma comment( lib, "glu32.lib" )
	
	#define GLV_PLATFORM_INIT_CONTEXT

#endif



#endif

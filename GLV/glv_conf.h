#ifndef INC_GLV_CONF_H
#define INC_GLV_CONF_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#define	GLV_VERSION "0.965"
#define	GLV_MAX_MOUSE_BUTTONS 8

/* Fixed pipeline (non-shader)
	compatible with OpenGL ES 1.1 and OpenGL 1.5 */
#define GLV_FIX_PIPE

/* Programmable pipeline (shader only)
	compatible with OpenGL ES 2.0 and OpenGL 2.0 */	
//#define GLV_PRG_PIPE

/* Run-time switchable fixed or programmable pipeline
	compatible with anything (!!!) */
#if defined(GLV_FIX_PIPE) && defined(GLV_PRG_PIPE) && !defined(GLV_DUO_PIPE)
	#define GLV_DUO_PIPE
#endif

// OpenGL platform-dependent includes
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
//#if defined(__IPHONE_3_0)

	#define GLV_PLATFORM		"IPHONE"
	#define GLV_PLATFORM_IPHONE
	#define GLV_PLATFORM_INIT_CONTEXT

	#ifdef GLV_PRG_PIPE
		#define GLV_OPENGL_ES2
		#import <OpenGLES/ES2/gl.h>
		#import <OpenGLES/ES2/glext.h>
	#endif

	#ifdef GLV_FIX_PIPE
		#define GLV_OPENGL_ES1
		#import <OpenGLES/ES1/gl.h>
		#import <OpenGLES/ES1/glext.h>

		// FIXME: shouldn't this only belong in windowing impl?
//		#define glGenFramebuffers glGenFramebuffersOES
//		#define glBindFramebuffer glBindFramebufferOES
//		#define glGenRenderbuffers glGenRenderbuffersOES
//		#define glBindRenderbuffer glBindRenderbufferOES
//		#define GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
//		#define GL_RENDERBUFFER GL_RENDERBUFFER_OES
//		#define GL_RENDERBUFFER_WIDTH GL_RENDERBUFFER_WIDTH_OES
//		#define GL_RENDERBUFFER_HEIGHT GL_RENDERBUFFER_HEIGHT_OES
//		#define glGetRenderbufferParameteriv glGetRenderbufferParameterivOES
//		#define glFramebufferRenderbuffer glFramebufferRenderbufferOES
//		#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
//		#define glCheckFramebufferStatus glCheckFramebufferStatusOES
//		#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES
//		#define glDeleteFramebuffers glDeleteFramebuffersOES
//		#define glDeleteRenderbuffers glDeleteRenderbuffersOES

		// FIXME: this will probably conflict with ES2
		#define glBlendEquation glBlendEquationOES
		#define GL_FUNC_ADD GL_FUNC_ADD_OES
		#define GL_FUNC_REVERSE_SUBTRACT GL_FUNC_REVERSE_SUBTRACT_OES
	#endif
	
#elif defined (__APPLE__) || defined (OSX)
	
	#define GLV_PLATFORM		"OSX"
	#define GLV_PLATFORM_OSX
	#define GLV_OPENGL

	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
//	#include <OpenGL/glu.h>

	#define GLV_PLATFORM_INIT_CONTEXT\
		{	/* prevents tearing */\
			GLint MacHackVBL = 1;\
			CGLContextObj ctx = CGLGetCurrentContext();\
			CGLSetParameter(ctx,  kCGLCPSwapInterval, &MacHackVBL); }


#elif defined(__linux__)

	#define GLV_PLATFORM		"UNIX"
	#define GLV_PLATFORM_UNIX
	#define GLV_OPENGL

	#include <time.h>
	#include <GL/glew.h>
	#include <GL/gl.h>
	#include <GL/glext.h>
//	#include <GL/glu.h>

	#define GLV_PLATFORM_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if(GLEW_OK != err){\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}


#elif defined(WIN32)
	
	#define GLV_PLATFORM		"WIN32"
	#define GLV_PLATFORM_WIN
	#define GLV_OPENGL

	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <windows.h>
	
	#ifdef min
	#undef min
	#endif
	#ifdef max
	#undef max
	#endif
	#ifdef far
	#undef far
	#endif
	#ifdef near
	#undef near
	#endif

	// note: return value of snprintf() and _snprintf() differ!
	#define snprintf _snprintf

	#include <GL/glew.h>
	#include <gl/gl.h>
//	#include <gl/glu.h>

	#pragma comment( lib, "glew32.lib")
	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
//	#pragma comment( lib, "glu32.lib" )
	
	#define GLV_PLATFORM_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if(GLEW_OK != err){\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}

#endif

#endif /* include guard */


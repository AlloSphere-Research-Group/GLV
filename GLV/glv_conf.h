#ifndef INC_GLV_CONF_H
#define INC_GLV_CONF_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#define	GLV_VERSION "0.9.7"
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

	#define GLV_PLATFORM_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if(GLEW_OK != err){\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}


#elif defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__MSYS__)
	
	#define GLV_PLATFORM		"WIN32"
	#define GLV_PLATFORM_WIN
	#define GLV_OPENGL

	#ifndef __MINGW32__
		#define GLEW_NO_GLU // GLU not used and throws errors with Mingw-w64
	#endif
	#include <GL/glew.h>
	#include <GL/gl.h>

	#pragma comment( lib, "glew32.lib")
	#pragma comment( lib, "winmm.lib")
	#pragma comment( lib, "opengl32.lib" )
	
	#define GLV_PLATFORM_INIT_CONTEXT\
		{	GLenum err = glewInit();\
			if(GLEW_OK != err){\
  				fprintf(stderr, "GLEW Init Error: %s\n", glewGetErrorString(err));\
			}\
		}

#endif

#define GLV_SNPRINTF snprintf

#ifdef __MINGW32__
	#undef GLV_SNPRINTF
	#define GLV_SNPRINTF _snprintf
#endif

#endif /* include guard */


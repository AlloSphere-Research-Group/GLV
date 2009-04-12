#ifndef INC_GLV_TEXTURE_H
#define INC_GLV_TEXTURE_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_conf.h"

namespace glv{

/// 2-D texture
class Texture2{
public:

	/// This constructor will allocate an internal pixel buffer
	Texture2(
		GLsizei width, GLsizei height, 
		GLenum format=GL_RGB, GLenum type=GL_UNSIGNED_BYTE);

	/// Constructor
	Texture2(
		GLsizei width, GLsizei height, GLvoid * pixels=0,
		GLenum format=GL_RGB, GLenum type=GL_UNSIGNED_BYTE,
		bool doesLoad=false);

	virtual ~Texture2();

	GLvoid * buffer() const { return mBuffer; }
	GLsizei width() const { return w; }		///< Get width
	GLsizei height() const { return h; }	///< Get height

	Texture2& bind();										///< Bind self to current context.
	Texture2& draw(											///< Draw texture to rectangular quad
		float ql, float qt, float qr, float qb,
		float tl=0, float tt=1, float tr=1, float tb=0
	);
	Texture2& load(GLsizei w, GLsizei h, GLvoid * pixels = 0);	///< Resizes texture on graphics card.
	Texture2& reload();										///< Reload texture onto GPU
	Texture2& send();										///< Send pointed to pixels to GPU

	Texture2& format(GLenum v);								///< Set the color format
	Texture2& type(GLenum v);								///< Set the color data type
	
private:
	GLuint mID;
	GLsizei w, h;
	GLvoid * mPixels;	// pointer to the client-side pixel data (0 if none)
	GLvoid * mBuffer;	// internally allocated pixel buffer
	GLenum mFormat;		// format of the pixel data:
						//   GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, 
						//   GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA.
					
	GLenum mType;		// type of the pixel data:
						//   GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT
						
	void freeMem();
	void allocMem();
};


} //glv::

#endif

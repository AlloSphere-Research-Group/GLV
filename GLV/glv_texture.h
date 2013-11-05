#ifndef INC_GLV_TEXTURE_H
#define INC_GLV_TEXTURE_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_conf.h"

namespace glv{

/// 2-D texture
class Texture2{
public:

	/// Construct a texture that allocates an internal pixel buffer
	
	/// @param[in] width	width, in texels
	/// @param[in] height	height, in texels
	/// @param[in] format	color format
	/// @param[in] type		color data type
	Texture2(
		GLsizei width, GLsizei height, 
		GLenum format=GL_RGB, GLenum type=GL_UNSIGNED_BYTE);

	/// Construct a texture that references an external pixel buffer

	/// @param[in] width	width, in texels, of external buffer
	/// @param[in] height	height, in texels, of external buffer
	/// @param[in] pixels	external texel buffer
	/// @param[in] format	color format of external buffer
	/// @param[in] type		color data type of external buffer
	/// @param[in] doesLoad	whether or not to load texture onto GPU
	Texture2(
		GLsizei width, GLsizei height, GLvoid * pixels,
		GLenum format=GL_RGB, GLenum type=GL_UNSIGNED_BYTE,
		bool doesLoad=false);

	virtual ~Texture2();

	/// Get pointer to local texture memory
	template <class T>
	T * buffer() const { return static_cast<T*>(mBuffer); }

	GLuint  id() const { return mID; }			///< Get unique identifier
	GLsizei width() const { return w; }			///< Get width
	GLsizei height() const { return h; }		///< Get height

	void begin() const;							///< Bind self to current context
	void end() const;							///< Binds default texture

	/// Allocate local texture memory. 
	
	/// Returns total number of bytes allocated.
	///
	int alloc(int w, int h);
	
	/// Free local texture memory
	void dealloc();

	Texture2& bind();							///< Bind self to current context
	
	/// Draw texture to rectangular quad
	Texture2& draw(
		float ql, float qt, float qr, float qb,
		float tl=0, float tt=1, float tr=1, float tb=0
	);
	
	void destroy();
	
	Texture2& create(GLsizei w, GLsizei h, GLvoid * pixels = 0);	///< Create new texture on graphics card
	Texture2& create();							///< Reload texture onto GPU

	Texture2& recreate();						///< Recreates texture on GPU using current settings

	Texture2& send();							///< Send pointed to pixels to GPU

	Texture2& format(GLenum v);					///< Set the color format
	Texture2& type(GLenum v);					///< Set the color data type
	Texture2& magFilter(GLenum v);				///< Set mag filter
	Texture2& wrapMode(GLenum v);				///< Set wrap mode

private:
	GLuint mID;
	GLsizei w, h;
	GLvoid * mPixels;	// pointer to the client-side pixel data (0 if none)
	GLvoid * mBuffer;	// internally allocated pixel buffer
	GLenum mFormat;		// format of the pixel data:
						//   GL_COLOR_INDEX, GL_ALPHA, 
						//   GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA.
					
	GLenum mType;		// type of the pixel data:
						//   GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_FLOAT

	GLenum mMagFilter;	// GL_LINEAR, GL_NEAREST
	GLenum mWrapMode;	// GL_CLAMP_TO_EDGE, GL_REPEAT

	void sendParams();
};


} //glv::

#endif

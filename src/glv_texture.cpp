/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_texture.h"
#include <stdlib.h>

namespace glv{

Texture2::Texture2(GLsizei w, GLsizei h, GLenum format, GLenum type)
	: mID(0), w(w), h(h), mPixels(0), mBuffer(0), mFormat(format), mType(type)
{
	allocMem();
}

Texture2::Texture2(GLsizei w, GLsizei h, GLvoid * pixs, GLenum format, GLenum type, bool doesLoad)
	: mID(0), w(w), h(h), mPixels(pixs), mBuffer(0), mFormat(format), mType(type)
{
	if(doesLoad) load(w, h, mPixels);
}

Texture2::~Texture2(){
	glDeleteTextures(1, &mID);
	freeMem();
}


void Texture2::freeMem(){
	if(mBuffer) free(mBuffer); mBuffer=0;
}

void Texture2::allocMem(){
	freeMem();
	
	int n=1;
	switch(mFormat){
		case GL_RGB:				n=3; break;
		case GL_RGBA:				n=4; break;
		case GL_LUMINANCE:	
		case GL_RED:
		case GL_GREEN:
		case GL_BLUE:
		case GL_ALPHA:				n=1; break;
		case GL_LUMINANCE_ALPHA:	n=2; break;
	};
	
	n *= w*h;

	#define CS(a,b) case a: mBuffer = malloc(n * sizeof(b)); break;
	switch(mType){
		CS(GL_BYTE, char)
		CS(GL_UNSIGNED_BYTE, unsigned char)
		CS(GL_SHORT, short)
		CS(GL_UNSIGNED_SHORT, unsigned short)
		CS(GL_INT, int)
		CS(GL_UNSIGNED_INT, unsigned int)
		CS(GL_FLOAT, float)
		CS(GL_DOUBLE, double)
	};
	#undef CS

	mPixels = mBuffer;
}

void Texture2::begin() const { glBindTexture(GL_TEXTURE_2D, id()); }
void Texture2::end() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture2& Texture2::bind(){ begin(); return *this; }


Texture2& Texture2::draw(
	float ql, float qt, float qr, float qb,
	float tl, float tt, float tr, float tb
){
#ifndef GLV_USE_OPENGL_ES
	glBegin(GL_QUADS);       
		glTexCoord2f(tl,tt); glVertex2f(ql,qt);
		glTexCoord2f(tl,tb); glVertex2f(ql,qb);
		glTexCoord2f(tr,tb); glVertex2f(qr,qb);
		glTexCoord2f(tr,tt); glVertex2f(qr,qt);
	glEnd();
#endif
	return *this;
}

Texture2& Texture2::format(GLenum v){ mFormat=v; return *this; }

Texture2& Texture2::load(GLsizei width, GLsizei height, GLvoid * pixs){
	w = width;
	h = height;
	mPixels = pixs;

	if(mID) glDeleteTextures(1, &mID);
	glGenTextures(1, &mID); //printf("%i\n", mID);
	glBindTexture(GL_TEXTURE_2D, mID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, mFormat, mType, mPixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return *this;
}

Texture2& Texture2::reload(){ load(w,h, mPixels); return *this; }

Texture2& Texture2::send(){

/*	void glTexSubImage2D(	GLenum target,
							GLint level,
							GLint xoffset, GLint yoffset,
							GLsizei width, GLsizei height,
							GLenum format, GLenum type,
							const GLvoid *pixels ) */

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, mFormat, mType, mPixels);
	return *this;
}

Texture2& Texture2::type(GLenum v){ mType=v; return *this; }

} // glv::

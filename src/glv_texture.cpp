/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_texture.h"

namespace glv{

Texture2::Texture2(GLsizei w, GLsizei h, GLvoid * pixs, GLenum format, GLenum type, bool doesLoad)
	: mID(0), w(w), h(h), mPixels(pixs), mFormat(format), mType(type)
{
	if(doesLoad) load(w, h, mPixels);
}

Texture2::~Texture2(){
	glDeleteTextures(1, &mID);
}

void Texture2::bind(){ glBindTexture(GL_TEXTURE_2D, mID); }


void Texture2::draw(
	float ql, float qt, float qr, float qb,
	float tl, float tt, float tr, float tb
){	
	glBegin(GL_QUADS);       
		glTexCoord2f(tl,tt); glVertex2f(ql,qt);
		glTexCoord2f(tl,tb); glVertex2f(ql,qb);
		glTexCoord2f(tr,tb); glVertex2f(qr,qb);
		glTexCoord2f(tr,tt); glVertex2f(qr,qt);
	glEnd();
}

Texture2& Texture2::format(GLenum v){ mFormat=v; return *this; }

void Texture2::load(GLsizei width, GLsizei height, GLvoid * pixs){
	w = width;
	h = height;
	mPixels = pixs;

	if(mID) glDeleteTextures(1, &mID);
	glGenTextures(1, &mID); //printf("%i\n", mID);
	glBindTexture(GL_TEXTURE_2D, mID);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, mFormat, mType, mPixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2::reload(){ load(w,h, mPixels); }

void Texture2::send(){

/*	void glTexSubImage2D(	GLenum target,
							GLint level,
							GLint xoffset, GLint yoffset,
							GLsizei width, GLsizei height,
							GLenum format, GLenum type,
							const GLvoid *pixels ) */

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, mFormat, mType, mPixels);
}

Texture2& Texture2::type(GLenum v){ mType=v; return *this; }

} // glv::

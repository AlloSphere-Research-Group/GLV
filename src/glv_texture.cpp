/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_texture.h"
#include <stdlib.h>

namespace glv{

Texture2::Texture2(GLsizei w_, GLsizei h_, GLenum format, GLenum type)
:	mID(0), w(0), h(0), mPixels(0), mBuffer(0),
	mFormat(format), mType(type), mMagFilter(GL_LINEAR), mWrapMode(GL_CLAMP_TO_EDGE)
{
	mUpdateRegion[0] = mUpdateRegion[1] =  0;
	mUpdateRegion[2] = mUpdateRegion[3] = -1;
	alloc(w_, h_);
}

Texture2::Texture2(GLsizei w, GLsizei h, GLvoid * pixs, GLenum format, GLenum type, bool doesLoad)
:	mID(0), w(w), h(h), mPixels(pixs), mBuffer(0),
	mFormat(format), mType(type), mMagFilter(GL_LINEAR), mWrapMode(GL_CLAMP_TO_EDGE)
{
	mUpdateRegion[0] = mUpdateRegion[1] =  0;
	mUpdateRegion[2] = mUpdateRegion[3] = -1;
	if(doesLoad) create(w, h, mPixels);
}

Texture2::~Texture2(){
	destroy();
	dealloc();
}

int Texture2::alloc(int w_, int h_){
	int Nnew = w_*h_;
	int Nold = w *h;
	int Nbytes = 0;

	if(Nnew != Nold){
		dealloc();

		int N=1;
		switch(mFormat){
			case GL_RGB:				N=3; break;
			case GL_RGBA:				N=4; break;
			case GL_LUMINANCE:	
        /* not ES compatible
			case GL_RED:
			case GL_GREEN:
			case GL_BLUE:
         */
			case GL_ALPHA:				N=1; break;
			case GL_LUMINANCE_ALPHA:	N=2; break;
		};
		
		N *= Nnew;

		#define CS(a,b) case a: Nbytes=N*sizeof(b); mBuffer = malloc(Nbytes); break;
		switch(mType){
			CS(GL_BYTE, char)
			CS(GL_UNSIGNED_BYTE, unsigned char)
			CS(GL_SHORT, short)
			CS(GL_UNSIGNED_SHORT, unsigned short)
        /* not ES compatible
			CS(GL_INT, int)
			CS(GL_UNSIGNED_INT, unsigned int)
			CS(GL_DOUBLE, double)
         */
			CS(GL_FLOAT, float)
			default:;
		};
		#undef CS

		if(mBuffer){
			mPixels = mBuffer;
			w = w_;
			h = h_;
		}
		else{
			w=h=0;
		}
	}
	
	w = w_;
	h = h_;
	
	return Nbytes;
}

void Texture2::dealloc(){
	if(mBuffer){
		free(mBuffer); mBuffer=0;
		w=h=0;
	}
}
  
void Texture2::begin() const { glBindTexture(GL_TEXTURE_2D, (GLuint)id()); }
void Texture2::end() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture2& Texture2::bind(){ begin(); return *this; }

Texture2& Texture2::create(GLsizei width, GLsizei height, GLvoid * pixs){
	if(!id() || w!=width || h!=height || pixs != mPixels){
		w = width;
		h = height;
		mPixels = pixs;
		recreate();
	}
	return *this;
}

Texture2& Texture2::create(){ create(w,h, mPixels); return *this; }

Texture2& Texture2::recreate(){
	destroy();
	glGenTextures(1, &mID); //printf("%i\n", mID);
	bind();
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, mFormat, mType, mPixels);
	glTexImage2D(GL_TEXTURE_2D, 0, mFormat, w, h, 0, mFormat, mType, mPixels);
	sendParams();
	return *this;
}

void Texture2::destroy(){
	if(id()){
		glDeleteTextures(1, &mID);
		mID = 0;
	}
}


Texture2& Texture2::draw(
	float ql, float qt, float qr, float qb,
	float tl, float tt, float tr, float tb
){
	int Nv=4;
	float verts[] = { ql,qt, ql,qb, qr,qt, qr,qb };
	float texcs[] = { tl,tt, tl,tb, tr,tt, tr,tb };

	// note: vertex arrays enabled at start of GLV draw loop
//	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, texcs);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, Nv);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_VERTEX_ARRAY);
	return *this;

}

Texture2& Texture2::format(GLenum v){ mFormat=v; return *this; }
Texture2& Texture2::magFilter(GLenum v){ mMagFilter=v; return *this; } 
Texture2& Texture2::wrapMode(GLenum v){ mWrapMode=v; return *this; } 

Texture2& Texture2::send(){

/*	void glTexSubImage2D(	GLenum target,
							GLint level,
							GLint xoffset, GLint yoffset,
							GLsizei width, GLsizei height,
							GLenum format, GLenum type,
							const GLvoid *pixels ) */
	sendParams();

	int tw = mUpdateRegion[2];
	if(tw < 0) tw = w+1+tw;
	int th = mUpdateRegion[3];
	if(th < 0) th = h+1+th;
	
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		mUpdateRegion[0], mUpdateRegion[1],
		tw, th,
		mFormat, mType, mPixels
	);
	return *this;
}

void Texture2::sendParams(){
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapMode);
}

Texture2& Texture2::type(GLenum v){ mType=v; return *this; }

Texture2& Texture2::updateRegion(int ux, int uy, int uw, int uh){
	mUpdateRegion[0]=ux;
	mUpdateRegion[1]=uy;
	mUpdateRegion[2]=uw;
	mUpdateRegion[3]=uh;
	return *this;
}

} // glv::

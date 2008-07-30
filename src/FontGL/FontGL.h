/***************************************************************************/
/*                                                                         */
/*  FontGL.h                                                               */
/*                                                                         */
/*  FontGL: An OpenGL font-rendering library.                              */
/*                                                                         */
/*  Copyright 2006 by Eric Newman.                                         */
/*  Contact: e.newman@mat.ucsb.edu                                         */
/*                                                                         */
/*  FontGL may be freely used and modified for non-commercial purposes by  */
/*  any individual, organization, or entity. However, please be kind and   */
/*  cite the author's work where appropriate.                              */
/*                                                                         */
/***************************************************************************/

#ifndef FONT_GL_H
#define FONT_GL_H

#if defined (__APPLE__) || defined (OSX)

	#include <OpenGL/OpenGL.h>
	#include <OpenGL/glu.h>
	#include <ft2build.h>
	#include FT_FREETYPE_H

#elif defined(__linux__)

	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <ft2build.h>
	#include <freetype/freetype.h>

#elif defined WIN32

	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include <ft2build.h>
	#include FT_FREETYPE_H

#endif


// must be called before instanciating any fonts.
void FontGL_InitializeLibrary();

// must be called afer all font instances are deleted.
void FontGL_CloseLibrary();

#define ASCII_SIZE 255 // number of characters to use

typedef struct
{
	float r, g, b, a;
}
FontGLColor;

class FontGL
{
public:

	FontGL(char *fontName, int fontSize, bool antiAliased, bool cacheGlyphs = false);
	~FontGL();

	// renders a text string at the specified 3D position
	void render(float x, float y, float z, const char *text);

	// returns the width of a text string in pixels
	float advance(const char *text);

	// returns the "above-line" height of the font in pixels
	float ascender() { return face->size->metrics.ascender/64.f; }
	
	// returns the "below-line" height of the font in pixels
	float descender() { return face->size->metrics.descender/64.f; }

	// returns the total height of the font in pixels
	int size() { return fontsize; }

	// sets the color of the font
	void setColor(float R, float G, float B, float A)
		{ color->r = R; color->g = G; color->b = B; color->a = A; }

	// these functions are necessary if your application
	// creates/destroys multiple windows while running
	void initTextures();
	void uninitTextures();

protected:

	FT_Face face;
	FontGLColor *color;
	GLuint *textures;
	int *widths, *vOffsets, fontsize, textureSize;
	bool antialiased, cacheglyphs;
	unsigned char *glyphImages[ASCII_SIZE];

	void loadTexture(int textureNum, unsigned char *glyphImage);
	void loadGlyph(int glyphNum, unsigned char *glyphImage);
};

#endif // FONT_GL_H

/***************************************************************************/
/*                                                                         */
/*  FontGL.cpp                                                             */
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

#include "FontGL.h"

FT_Library *FontGL_Library = 0;

void FontGL_InitializeLibrary()
{
	if (!FontGL_Library) 
	{
		FontGL_Library = new FT_Library;
		FT_Init_FreeType(FontGL_Library);
	}
}

void FontGL_CloseLibrary()
{
	FT_Done_FreeType(*FontGL_Library);
	delete FontGL_Library;
	FontGL_Library = 0;
}

FontGL::FontGL(char *fontName, int fontSize, bool antiAliased, bool cacheGlyphs)
{
	FT_New_Face(*FontGL_Library, fontName, 0, &face);
	
	fontsize = fontSize;
	antialiased = antiAliased;
	cacheglyphs = cacheGlyphs;

	color = new FontGLColor;
	setColor(1.f, 1.f, 1.f, 1.f);

	widths = new int[ASCII_SIZE];
	vOffsets = new int[ASCII_SIZE];
	textures = new GLuint[ASCII_SIZE];

	textureSize = 1;
	while (textureSize < fontsize) textureSize *= 2;

	if (cacheglyphs)
	{
		for (int i=0; i<ASCII_SIZE; i++)
		{
			glyphImages[i] = new unsigned char[textureSize * textureSize * 2];
			loadGlyph(i, glyphImages[i]);
		}
	}

	initTextures();
}

FontGL::~FontGL()
{
	uninitTextures();

	if (cacheglyphs)
	{
		for (int i=0; i<ASCII_SIZE; i++)
			delete glyphImages[i];
	}

	delete textures;
	delete widths;
	delete vOffsets;

	delete color;

	FT_Done_Face(face);
}

void FontGL::initTextures()
{
	glGenTextures(255, textures);

	if (cacheglyphs)
	{
		for (int i=0; i<ASCII_SIZE; i++)
			loadTexture(i, glyphImages[i]);
	}
	else
	{
		unsigned char *glyphImage = new unsigned char[textureSize * textureSize * 2];

		for (int i=0; i<ASCII_SIZE; i++)
		{
			loadGlyph(i, glyphImage);
			loadTexture(i, glyphImage);
		}

		delete glyphImage;
	}
}

void FontGL::loadGlyph(int glyphNum, unsigned char *glyphImage)
{
	int j, k, l;
	int glyph_index;

	glyph_index = FT_Get_Char_Index(face, glyphNum);

	FT_Set_Char_Size(face, 0, fontsize * 64, 0, 0);

	if (antialiased)
	{
		FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	}
	else
	{
		FT_Load_Glyph(face, glyph_index, FT_LOAD_MONOCHROME);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
	}

	widths[glyphNum] = face->glyph->advance.x/64;
	vOffsets[glyphNum] = face->glyph->bitmap_top;

	memset(glyphImage, 0, textureSize * textureSize * 2);

	FT_Bitmap *bitmap = &face->glyph->bitmap;

	if (antialiased)
	{
		for (j=0; j<bitmap->rows; j++)
		{
			for (k=0; k<bitmap->width; k++)
			{
				glyphImage[(j * textureSize * 2) + (k * 2)] = 255;
				glyphImage[(j * textureSize * 2) + (k * 2) + 1] = bitmap->buffer[j * bitmap->width + k];
			}
		}
	}
	else
	{
		unsigned char temp;

		for (j=0; j<bitmap->rows; j++)
		{
			for (k=0; k<bitmap->pitch; k++)
			{
				for (l=7; l>=0; l--)
				{
					temp = (bitmap->buffer[j * bitmap->pitch + k] >> l) & 1;

					if (temp)
					{
						glyphImage[(j * textureSize * 2) + ((k * 8 + (7 - l)) * 2)] = 255;
						glyphImage[(j * textureSize * 2) + ((k * 8 + (7 - l)) * 2) + 1] = 255;
					}
				}
			}
		}
	}
}

void FontGL::loadTexture(int textureNum, unsigned char *glyphImage)
{
	glBindTexture(GL_TEXTURE_2D, textures[textureNum]);
	glTexImage2D(GL_TEXTURE_2D, 0, 2, textureSize, textureSize, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, glyphImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void FontGL::uninitTextures()
{
	glDeleteTextures(255, textures);
}

float FontGL::advance(const char *text)
{
	float total = 0.f;

	for (int i=0; i<(int) strlen(text); i++)
		total += widths[text[i]];

	return total;
}

void FontGL::render(float x, float y, float z, const char *text)
{
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	GLint blendDest, blendSource;
	glGetIntegerv(GL_BLEND_DST, &blendDest);
	glGetIntegerv(GL_BLEND_SRC, &blendSource);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();

	glTranslatef(x, y, z);
	glColor3f(color->r, color->g, color->b);
	
	int width, vOffset;

	for (unsigned i=0; i<strlen(text); i++)
	{
		width = widths[text[i]];
		vOffset = vOffsets[text[i]];

		glBindTexture(GL_TEXTURE_2D, textures[text[i]]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.f,					(float) (-vOffset),					0.f);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.f,					(float) (-vOffset + textureSize),	0.f);
		glTexCoord2f(1.0, 1.0); glVertex3f((float) textureSize,	(float) (-vOffset + textureSize),	0.f);
		glTexCoord2f(1.0, 0.0); glVertex3f((float) textureSize,	(float) (-vOffset),					0.f);
		glEnd();

		glTranslatef((float) width, 0.f, 0.f);
	}

	glPopMatrix();

	glPopAttrib();

	glBlendFunc(blendSource, blendDest);
}


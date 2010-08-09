#ifndef INC_GLV_FONT_H
#define INC_GLV_FONT_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

namespace glv{

// font rendering styles
//enum
//{
//	GLV_FONT_BITMAP = 0,
//	GLV_FONT_PIXMAP,
//	GLV_FONT_TEXTURE,
//	GLV_FONT_POLYGON,
//	GLV_FONT_OUTLINE,
//	GLV_FONT_EXTRUDE
//};

namespace FontStyle{
	enum t{
		Regular,
		Bold,
		Italic
	};
}


/*
A font is a particular style of a typeface (roman, boldface, italic)
A typeface is a set of character glyphs with consistent visual appearance

Metrics
Font-wide:
	cap height
	x-height
	ascender height
	descender depth
	bounding box

Glyph:
	bounding box
	advance width (total space for glyph)
	sidebearings (padding on either side)
*/

/// Font
class Font{
public:
	Font(unsigned size=8);
	
	~Font();
	
	//void render(char c, float x=0, float=0, float z=0);
	void render(const char * text, float x=0, float=0, float z=0);

	/// Set spacing, in ems, between the left and right edges of successive letters
	Font& letterSpacing(float v);

	/// Set spacing, in ems, between lines
	Font& lineSpacing(float v);

	/// Set the size in pixels
	Font& size(unsigned size);
	
	/// Set number of spaces per tab
	Font& tabSpaces(unsigned spaces);

	/// Returns advance width of character
	float advance(char c) const;

	/// Returns total advance width of text string
	float advance(const char *text) const;
	
	float cap() const;
	float median() const;
	float baseline() const;
	float descent() const;

	float letterSpacing() const { return mLetterSpacing; }
	float lineSpacing() const { return mLineSpacing; }
	float scaleX() const { return mScaleX; }
	unsigned size() const { return mSize; }
	unsigned tabSpaces() const { return mTabSpaces; }

private:
	unsigned mSize;
	float mScaleX, mScaleY;
	float mLetterSpacing;
	float mLineSpacing;
	unsigned mTabSpaces;
};


/*
// From an older version of GLV...
class Font{
public:
	Font(char * typeface, Color * color = 0, unsigned size = 0, int fontStyle = GLV_FONT_BITMAP);
	Font(Color * color, unsigned size = 0, int fontStyle = GLV_FONT_BITMAP);
	Font(unsigned size, int fontStyle);
	Font(int fontStyle);

	Font(unsigned size=8);
	
	~Font(){}
	
	void render(float x, float y, float z, const char * text);
	
	float advance(const char *text);
	float ascender();
	float descender();

	void size(unsigned size);
	unsigned int size() const;

private:

	// function pointer to a void(float, float, char *) method of class Font
	typedef void (Font::* fpFontDraw)(float, float, float, const char *);

	void init(char * fontFace = 0, Color * color = 0, unsigned newSize = 0, int fontStyle = GLV_FONT_BITMAP);

	FTFont * face;
 	Color *	mColor;
	fpFontDraw draw;

	void drawRaster(float x, float y, float z, const char * text);
	void drawVector(float x, float y, float z, const char * text);

	static map<char *, FTGLBitmapFont *>	bitmapfonts;		// Vector of available Fonts.  Made on demand.
	static map<char *, FTGLTextureFont *>	texturefonts;		// Vector of available Fonts.  Made on demand.
	static map<char *, FTGLOutlineFont *>	outlinefonts;		// Vector of available Fonts.  Made on demand.
	static map<char *, FTGLPixmapFont *>	pixmapfonts;		// Vector of available Fonts.  Made on demand.
	static map<char *, FTGLPolygonFont *>	polygonfonts;		// Vector of available Fonts.  Made on demand.
};
*/

} // glv::

#endif

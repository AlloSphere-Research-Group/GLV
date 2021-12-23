#ifndef INC_GLV_FONT_H
#define INC_GLV_FONT_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_draw.h"

namespace glv{

/// Font
class Font{
public:
	Font(float size=8);
	
	virtual ~Font();

	/// Get advance amount (width) of character
	virtual float advance(const char c) const;

	/// Returns total advance width of text string
	virtual float advance(const char *text) const;

	/// Get bounding box of text string
	virtual void getBounds(float& w, float& h, const char * text) const;

	/// Render text string
	virtual void render(const char * text, float x=0, float y=0, float z=0) const;

	virtual void render(GraphicsData& g, const char * text, float x=0, float y=0, float z=0) const;

	/// Set spacing, in ems, between the left and right edges of successive letters
	Font& letterSpacing(float v);

	/// Set spacing, in ems, between lines
	Font& lineSpacing(float v);

	/// Set the font size in pixels
	Font& size(float size);
	
	/// Set number of spaces per tab
	Font& tabSpaces(unsigned spaces);

	float baseline() const;		///< Get absolute position on which glyphs rest
	float cap() const;			///< Get distance from baseline to top of uppercase glyphs
	float xheight() const;		///< Get distance from baseline to top of lowercase glyphs
	float descent() const;		///< Get distance from baseline to lowest position of glyphs

	/// Get letter spacing, in ems
	float letterSpacing() const { return mLetterSpacing; }
	
	/// Get line spacing, in ems
	float lineSpacing() const { return mLineSpacing; }
	
	/// Get scaling factor in x direction
	float scaleX() const { return mScaleX; }
	
	/// Get font size, in pixels
	float size() const { return mSize; }
	
	/// Get number of spaces per tab
	unsigned tabSpaces() const { return mTabSpaces; }

private:
	float mSize;
	float mScaleX, mScaleY;
	float mLetterSpacing;
	float mLineSpacing;
	unsigned mTabSpaces;
};

} // glv::

#endif

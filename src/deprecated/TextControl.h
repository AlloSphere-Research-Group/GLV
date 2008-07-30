#ifndef GLV_TEXTCONTROL_H_INC
#define GLV_TEXTCONTROL_H_INC

#include "glv-core.h"

namespace glv {

typedef pair <string, string> glvTokenPair;

class TextDisplayBox : public View
{
public:

	TextDisplayBox(int left, int top, int width, int height);

	Padding *linePadding;

	void setText(string aText);
	string getText() { return tokenFilterForOutput(text); }

	virtual void onDrag(Mouse * m);
	virtual void onMouseDown(Mouse * m);
	virtual void onMouseUp(Mouse * m);
	virtual void onKeyDown(Keyboard * k);
	virtual void onGetFocus(InputDevice * i) {};
	virtual void onLoseFocus(InputDevice * i);

	virtual void onDraw();
	virtual void onResize() { wrapWords(); };

	int numLines() { return lines.size(); }
	void deleteFirstLine();

private:

	string text;
	vector<string> words;		// the 'text' string sorted into words
	vector<float> wordWidths;	// the pixel width of each word in the 'text' string
	float spaceWidth;			// the pixel width of the space character (' ')
	vector<string> lines;		// the 'text' string sorted into wrapped lines based on the box width
	vector<float> lineWidths;	// the pixel width of each line
	vector<int> letterOffsets;	// the first character of each line as index to the 'text' string

	void wrapWords();

	vector <glvTokenPair> tokenPairs;	// this should be a global variable accessible to all text widgets
	string tokenFilterForOutput(string input);

	void calculateSelectionBox();
	vector <float *> selectionBoxes;
	float selectBeginY, selectBeginX, selectEndX, selectEndY;	// pixel coordinates of the selection box
	int startLine, startChar, endLine, endChar;					// letter indices of the selection box
	bool selectionActive;

};

class SingleLineTextEntryBox : public View, public SignalString
{
public:

	SingleLineTextEntryBox(int left, int top, int width, int height) : View(left, top, width, height) {
		cursorPixelPosition = 0.0f; cursorLetterPosition = 0; letterOffset = 0; active = false; }
		
	void setText(string aText);
	string getText() { return text; }
	
	virtual void onDraw();
	virtual void onMouseDown(Mouse * m);
	virtual void onGetFocus(InputDevice * i) {};
	virtual void onLoseFocus(InputDevice * i);
	virtual void onKeyDown(Keyboard * k);
	virtual void onDrag(Mouse * m) {};
	virtual void onChar(int ascii, bool state);

private:

	void processText();

	string text;				// the text
	vector<float> letterWidths;	// the pixel width of each character in the 'text' string

	float cursorPixelPosition;	// the x-position of the cursor in pixels, relative to left side of box
	int cursorLetterPosition;	// the position of the cursor as an index to the 'text' string (i.e., text[cursorLetterPosition])
	int letterOffset;			// the first letter displayed on the left side of the box, also as index to 'text' string
	bool active;				// whether or not the box has focus
};

class Console : public View, public SignalString
{
public:

	Console(int left, int top, int width, int height);
	
	virtual void		onDraw();

	virtual void		onKeyDown(Keyboard * k);
	virtual void		onGetFocus() { APP->keyboard->characterInputListener = commandEntry; }
	
	virtual void		onDrag(Mouse * m);
	virtual void		onMouseDown(Mouse * m);
	virtual void		onMouseUp(Mouse * m) { consume(m); }
	virtual void		onGetFocus(InputDevice * i) {};
	virtual void		onLoseFocus(InputDevice * i);

	string				getCommand() { return commandEntry->getText(); }

	void				addLine(string message);

private:

	SingleLineTextEntryBox *	commandEntry;
	TextDisplayBox *			commandHistory;
	string						command;

	int							numDisplayLines;
};


}

#endif

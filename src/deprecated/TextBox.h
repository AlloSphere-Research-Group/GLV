#ifndef GLV_TEXTBOX_H_INC
#define GLV_TEXTBOX_H_INC

#include "glv-core.h"

namespace glv {

class Button;
class ScrollBar;

class TextBox : public View
{
public:

	TextBox(int left = 0, int top = 0, int width = 0, int height = 0);
	~TextBox();

	void setEditable(bool b) { editable = b; }
	void setWordWrapping(bool b) { wordWrapping = b; }
	void showCursor(bool b) { cursor = b; }
	void showScrollBars(bool b);
	void setText(string s);
	void scrollToEnd();

	string getText() { return text; }

	Padding *innerPadding;

	virtual void onDraw();
	virtual void onChar(int ascii, bool state);
	virtual void onKeyDown(Keyboard *k);
	virtual void onMouseDown(Mouse *m);
	virtual void onMouseUp(Mouse *m);
	virtual void onDrag(Mouse *m);
	virtual void onLoseFocus(InputDevice * i);
	virtual void onGetFocus(InputDevice * i);
	virtual void onResize();

private:

	void wrapWords();

	void updateCursorPixelPosition();
	void updateScrollBarSizes();
	void updateScrollBarPositions();
	void calculateCurrentLine();
	void calculateDisplayLines();

	bool editable, active, wordWrapping, scrollBars, cursor;

	float cursorHeight, baseline, lineHeight, spaceWidth;

	RelayFloatRanged *hOffset, *vOffset;

	Button *buttonMisc;
	ScrollBar *scrollVertical, *scrollHorizontal;
	View *textArea;

	string text;

	int startLine, startChar, currentChar, numDisplayLines, currentLine, prevKey;
	float cursorPixelPositionX, cursorPixelPositionY, maxLineWidth;

	vector <float> letterWidths;
	vector <string> lines;
	vector <int> letterOffsets;
	vector <string> linesOffset;
	vector <int> linesOffsetIndex;
	vector <float> lineStartX;
};



class Console2 : public View, public SignalString
{
public:

	Console2(int left = 0, int top = 0, int width = 0, int height = 0);
	~Console2();

	void addLine(string s);
	string getHistory();
	void setHistory(string s);

	virtual void onDraw();
	virtual void onKeyDown(Keyboard *k);
	virtual void onMouseDown(Mouse *m);
	virtual void onMouseUp(Mouse *m);
	virtual void onDrag(Mouse *m);
	virtual void onResize();
	virtual void onGetFocus(InputDevice * i);
	virtual void onLoseFocus(InputDevice * i);

private:

	TextBox *entry;
	TextBox *history;

	vector <string> previousCommands;
	int currentPreviousCommand;
};

}

#endif
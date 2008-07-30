#include <math.h>

#include "Icon.h"
#include "RangedControl.h"

#include "TextBox.h"

using namespace glv;

TextBox::TextBox(int left, int top, int width, int height) : View(left, top, width, height)
{
	innerPadding = new Padding(floor(font->advance("W")/2.f), 0.0f, floor(font->advance("W")/2.f), 2.0f);
	
	buttonMisc = new Button(0, 0, 0, 0);
	buttonMisc->icon(Icon::check);
	buttonMisc->onValue(1.0f);

	hOffset = new RelayFloatRanged(0.f, 1.f, "horizontal scrollbar position", "");
	vOffset = new RelayFloatRanged(0.f, 1.f, "vertical scrollbar position", "");

	scrollHorizontal = new ScrollBar();
	scrollVertical = new ScrollBar();

	scrollHorizontal->connectFloatRanged(hOffset);
	scrollVertical->connectFloatRanged(vOffset);

	hOffset->update();
	vOffset->update();

	// this view is for measurement purposes only
	textArea = new View();
	
	editable = true;
	cursor = true;
	scrollBars = true;
	wordWrapping = true;
	active = false;

	// pre-calculate font metrics
	baseline = font->descender();
	cursorHeight = font->ascender();
	lineHeight = (float) font->size();
	spaceWidth = font->advance(" ");

	prevKey = 0;

	setText("");

	onResize();
}

TextBox::~TextBox()
{
	delete buttonMisc;

	delete scrollHorizontal;
	delete scrollVertical;

	delete hOffset;
	delete vOffset;

	delete textArea;

	delete innerPadding;
}

void TextBox::onResize()
{
	buttonMisc->setRect(w-10, h-10, 10, 10);
	
	scrollHorizontal->setRect(0, h-10, w-10-buttonMisc->padding->l, 10);
	scrollHorizontal->onResize();
	
	scrollVertical->setRect(w-10, 0, 10, h-10-buttonMisc->padding->t);
	scrollVertical->onResize();

	showScrollBars(scrollBars);
}

void TextBox::showScrollBars(bool b)
{
	scrollBars = b;

	if (!scrollBars)
	{
		textArea->setRect(innerPadding->l, innerPadding->t, w-innerPadding->l-innerPadding->r, h-innerPadding->t-innerPadding->b);
	}
	else
	{
		textArea->setRect(innerPadding->l, innerPadding->t, w-10-scrollVertical->padding->l-innerPadding->l-innerPadding->r,
			h-10-scrollHorizontal->padding->t-innerPadding->t-innerPadding->b);
	}

	numDisplayLines = (int) (textArea->h/lineHeight);

	wrapWords();
}

void TextBox::setText(string s)
{
	text = s;

	letterWidths.clear();

	// calculate pixel width of each character
	for (unsigned i=0; i<text.size(); i++)
	{
		if (text.substr(i, 1) == "\n")
			letterWidths.push_back(0.0f);
		else
			letterWidths.push_back(font->advance(text.substr(i, 1).c_str()));
	}

	currentChar = 0;

	wrapWords();
}

void TextBox::wrapWords()
{
	int pos, pos1 = -1, pos2 = -1;
	unsigned i = 0;
	float currentLineWidth = 0.0f, nextWordWidth;
	string currentLetter;

	maxLineWidth = 0.0f;

	letterOffsets.clear();
	letterOffsets.push_back(0);

	lines.clear();
	lines.push_back("");

	while (i < text.size())
	{
		if (wordWrapping)
			pos1 = (int) text.find_first_of(' ', i);
		
		pos2 = (int) text.find_first_of('\n', i);

		if (pos1 == -1)
			pos1 = (int) text.size();
		if (pos2 == -1)
			pos2 = (int) text.size();

		pos = (pos1 < pos2) ? pos1 : pos2;
		if (pos == i)
			pos++;

		nextWordWidth = font->advance(text.substr(i, pos-i).c_str());

		currentLetter = text.substr(i, 1);

		if ((wordWrapping && currentLineWidth + nextWordWidth >= textArea->w) || currentLetter == "\n")
		{
			if (currentLetter == "\n")
				i++;
			else if (currentLineWidth == 0.0f)
			{
				while (currentLineWidth + letterWidths[i] < textArea->w)
				{
					lines.back().append(text.substr(i, 1));
					currentLineWidth += letterWidths[i];
					i++;
				}
			}

			currentLineWidth = 0.0f;
			lines.push_back("");

			letterOffsets.push_back(i);
		}
		else
		{
			lines.back().append(currentLetter);
			currentLineWidth += letterWidths[i];
			i++;

			if (currentLineWidth > maxLineWidth)
				maxLineWidth = currentLineWidth;
		}
	}

	updateScrollBarSizes();
	calculateCurrentLine();
	updateScrollBarPositions();
	calculateDisplayLines();
	updateCursorPixelPosition();
}

void TextBox::onChar(int ascii, bool state)
{
	if (state && ascii >= 32 && ascii < 127)
	{
		string temp(1, ascii);
		text.insert(currentChar, temp);

		letterWidths.insert(letterWidths.begin() + currentChar, font->advance(temp.c_str()));

		currentChar++;

		wrapWords();
	}
}

void TextBox::onKeyDown(Keyboard *k)
{
	static float startX = -1.0f;

	if (k->key(GLV_KEY_LEFT))
	{
		consume(k);

		if (currentChar > 0)
		{
			currentChar--;

			prevKey = GLV_KEY_LEFT;

			calculateCurrentLine();
			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_RIGHT))
	{
		consume(k);

		if (currentChar < (int) text.size())
		{
			currentChar++;

			prevKey = GLV_KEY_RIGHT;

			calculateCurrentLine();
			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_UP))
	{
		consume(k);

		if (currentLine > 0)
		{
			int i = 0, prevOffset = letterOffsets[currentLine-1];
			float advance = innerPadding->l - 0.5f;

			if (prevKey != GLV_KEY_UP && prevKey != GLV_KEY_DOWN)
				startX = cursorPixelPositionX - hOffset->value();

			while (i < (int) lines[currentLine-1].size() && advance <= startX - (letterWidths[i+prevOffset]*0.5f))
			{
				advance += letterWidths[i+prevOffset];
				i++;
			}

			currentChar = prevOffset + i;
			currentLine--;

			prevKey = GLV_KEY_UP;

			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_DOWN))
	{
		consume(k);

		if (currentLine < (int) lines.size() - 1)
		{
			int i = 0, nextOffset = letterOffsets[currentLine+1];
			float advance = innerPadding->l - 0.5f;

			if (prevKey != GLV_KEY_DOWN && prevKey != GLV_KEY_UP)
				startX = cursorPixelPositionX - hOffset->value();

			while (i < (int) lines[currentLine+1].size() && advance <= startX - (letterWidths[i+nextOffset]*0.5f))
			{
				advance += letterWidths[i+nextOffset];
				i++;
			}

			currentChar = nextOffset + i;
			currentLine++;

			prevKey = GLV_KEY_DOWN;

			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_END) && k->ctrl())
	{
		consume(k);

		int temp = (int) text.size();

		if (currentChar < temp)
		{
			scrollToEnd();

			prevKey = 0; // this should be something unique to Ctrl+End
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_END))
	{
		consume(k);

		int temp = letterOffsets[currentLine] + (int) lines[currentLine].size();

		if (currentChar < temp)
		{
			currentChar = temp;

			prevKey = GLV_KEY_END;

			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_HOME) && k->ctrl())
	{
		consume(k);

		if (currentChar != 0)
		{
			currentChar = 0;

			prevKey = 0; // this should be something unique to Ctrl+Home

			calculateCurrentLine();
			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_HOME))
	{
		consume(k);

		if (currentChar > letterOffsets[currentLine])
		{
			currentChar = letterOffsets[currentLine];

			prevKey = GLV_KEY_HOME;

			calculateCurrentLine();
			updateScrollBarPositions();
			calculateDisplayLines();
			updateCursorPixelPosition();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_ENTER) || k->key(GLV_KEY_RETURN))
	{
		consume(k);

		text.insert(currentChar, "\n");
		letterWidths.insert(letterWidths.begin() + currentChar, 0.0f);

		currentChar++;

		prevKey = GLV_KEY_ENTER;

		wrapWords();
	}
	else if (k->key(GLV_KEY_DELETE))
	{
		consume(k);

		if (currentChar < (int) text.size())
		{
			text.erase(currentChar, 1); // remove 1 character to the right of cursor
			letterWidths.erase(letterWidths.begin()+currentChar);

			prevKey = GLV_KEY_DELETE;

			wrapWords();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_BACKSPACE))
	{
		consume(k);

		if (currentChar > 0)
		{
			text.erase(currentChar-1, 1); // remove 1 character to the left of cursor
			letterWidths.erase(letterWidths.begin()+currentChar-1);

			currentChar--;

			prevKey = GLV_KEY_BACKSPACE;

			wrapWords();
		}
		else
			return;
	}
	//else if (k->key('V') && k->ctrl())
	//{
	//	string temp = APP->clipboard();
	//	text.insert(currentChar, temp);
	//		
	//	char c[2];
	//	c[1] = '\0';

	//	for (int i=0; i<temp.size(); i++)
	//	{
	//		c[0] = temp.c_str()[i];
	//		letterWidths.insert(letterWidths.begin() + currentChar + i, font->advance(c));
	//	}
	//	
	//	currentChar += temp.size();
	//	processText();
	//}
	else if (k->key('K') && k->ctrl())
	{
		consume(k);

		setText("");

		prevKey = 'K';
	}
	else
	{
		prevKey = 0;
	}
}

void TextBox::scrollToEnd()
{
	if (editable)
		currentChar = (int) text.size();
	else
		currentChar = letterOffsets[lines.size() - 1];

	calculateCurrentLine();
	updateScrollBarPositions();
	calculateDisplayLines();
	updateCursorPixelPosition();
}

void TextBox::updateCursorPixelPosition()
{
	int i = letterOffsets[currentLine];

	cursorPixelPositionX = innerPadding->l - 0.5f + hOffset->value();

	while (i < currentChar)
	{
		cursorPixelPositionX += letterWidths[i];
		i++;
	}

	cursorPixelPositionY = (currentLine - vOffset->value() + 1.0f) * lineHeight;
}

void TextBox::updateScrollBarSizes()
{
	float temp;

	float hTemp = (textArea->w - maxLineWidth > -1.0f) ? -1.0f : textArea->w - maxLineWidth;
	float vTemp = ((int) lines.size() <= numDisplayLines) ? 1.0f : (int) lines.size() - numDisplayLines;
	
	hOffset->scalar(hTemp);
	vOffset->scalar(vTemp);

	temp = numDisplayLines/(float) lines.size();
	scrollVertical->setKnobScale(temp > 1.0f ? 1.0f : temp);
	scrollVertical->setArrowButtonIncrement(1.f/vTemp);

	temp = (textArea->w + innerPadding->l + innerPadding->r)/maxLineWidth;
	scrollHorizontal->setKnobScale(temp > 1.0f ? 1.0f : temp);
	scrollHorizontal->setArrowButtonIncrement(spaceWidth/maxLineWidth);
}

void TextBox::updateScrollBarPositions()
{
	int i;
	float currentCharPos;
	
	if (currentLine < (int) vOffset->value())
		vOffset->value((float) currentLine);
	else if (currentLine >= (int) vOffset->value() + numDisplayLines)
		vOffset->value((float) (currentLine - numDisplayLines + 1));
	
	if ((int) lines.size() <= numDisplayLines)
		vOffset->value(0.0f);
	else if ((int) lines.size() - currentLine + 1 <= numDisplayLines)
		vOffset->value((float) lines.size() - (float) numDisplayLines);
 
	i = letterOffsets[currentLine];
	currentCharPos = hOffset->value() + innerPadding->l;

	while (i < currentChar)
	{
		currentCharPos += letterWidths[i];
		i++;
	}

	if (currentCharPos < innerPadding->l)
		hOffset->value(hOffset->value() - currentCharPos + innerPadding->l);
	else if (currentCharPos >= textArea->right())
		hOffset->value(hOffset->value() - currentCharPos + textArea->right());
}

void TextBox::calculateCurrentLine()
{
	int i = (int) lines.size() - 1;

	while (letterOffsets[i] > currentChar)
		i--;

	currentLine = i;
}

void TextBox::calculateDisplayLines()
{
	int i, j, k;
	float currentLineWidth;
	float currentLineStart;
	//float x = absX();

	lineStartX.clear();
	linesOffset.clear();
	linesOffsetIndex.clear();

	//if (x < 0.0f)
	//	x = 0.0f;

	for (i = 0; i < (int) lines.size(); i++)
	{
		currentLineStart = innerPadding->l + hOffset->value();
		j = 0;

		while (j < (int) lines[i].size() && currentLineStart /*+ letterWidths[j + letterOffsets[i]]*/ < 0.0f)
		{
			currentLineStart += letterWidths[j + letterOffsets[i]];
			j++;
		}
		
		currentLineWidth = currentLineStart;
		k = 0;

		while (k < (int) lines[i].size() && currentLineWidth < textArea->right() + innerPadding->r)
		{
			currentLineWidth += letterWidths[j + k + letterOffsets[i]];
			k++;
		}

		linesOffset.push_back(lines[i].substr(j, k));
		linesOffsetIndex.push_back(j + letterOffsets[i]);
		lineStartX.push_back(currentLineStart);
	}
}

void TextBox::onDrag(Mouse * m)
{
	if (scrollVertical->isDragging())
	{
		scrollVertical->onDrag(m);
		vOffset->value(floor(vOffset->value())); // round value to nearest line

		if (active)
			updateCursorPixelPosition();
	}
	else if (scrollHorizontal->isDragging())
	{
		scrollHorizontal->onDrag(m);
		hOffset->value(floor(hOffset->value()));

		calculateDisplayLines();

		if (active)
			updateCursorPixelPosition();
	}
}

void TextBox::onMouseDown(Mouse * m)
{
	if (scrollBars && m->x() > scrollVertical->l - scrollVertical->padding->l)
	{
		if (m->y() <= scrollVertical->bottom())
		{
			m->setContext(scrollVertical);
			scrollVertical->onMouseDown(m);
			m->unsetContext(scrollVertical);

			if (active)
				updateCursorPixelPosition();
		}
		else if (m->y() > buttonMisc->t)
		{
			buttonMisc->onMouseDown(m);
			wordWrapping = !wordWrapping;

			if (wordWrapping)
				hOffset->value(0.0f);

			wrapWords();
		}
	}
	else if (scrollBars && m->y() > scrollHorizontal->t - scrollHorizontal->padding->t)
	{

		m->setContext(scrollHorizontal);
			scrollHorizontal->onMouseDown(m);
			hOffset->value(floor(hOffset->value()));
		m->unsetContext(scrollHorizontal);

		calculateDisplayLines();

		if (active)
			updateCursorPixelPosition();
	}
	else
	{
		int i;
		float y = m->y();

		if (y < 0.0f) y = 0.0f;

		startLine = (int) ((y+baseline)/lineHeight + vOffset->value());

		if (startLine >= (int) lines.size())
			startLine = (int) lines.size() - 1;

		float advance = lineStartX[startLine];
		i = linesOffsetIndex[startLine];

		while (i < linesOffsetIndex[startLine] + (int) linesOffset[startLine].size() && advance <= m->x() - (letterWidths[i]*0.5f - 1.0f))
		{
			advance += letterWidths[i];
			i++;
		}
		
		active = true;

		if (editable)
		{
			APP->keyboard->characterInputListener = this;
		}

		currentLine = startLine;
		startChar = i;
		currentChar = startChar;
		cursorPixelPositionX = advance - 0.5f;
		cursorPixelPositionY = (startLine-vOffset->value()+1.0f) * lineHeight;

		prevKey = 0;
	}

	consume(m);
/*
	selectBeginX = advance + CURSOR_X_OFFSET;
	selectBeginY = startLine * lineHeight + CURSOR_Y_OFFSET;
	selectEndX = selectBeginX;
	selectEndY = selectBeginY;
	endLine = startLine;

	selectionActive = true;

	//printf("%d: %s\n", letterOffsets[startLine], text.substr(letterOffsets[startLine], 1).c_str());

	calculateSelectionBox();*/
}

void TextBox::onMouseUp(Mouse *m)
{
	if (scrollBars)
	{
		buttonMisc->onMouseUp(m);
		scrollHorizontal->onMouseUp(m);
		scrollVertical->onMouseUp(m);
	}
}

void TextBox::onGetFocus(InputDevice *i)
{
	//active = true;

	//if (editable)
	//{
	//	APP->keyboard->characterInputListener = this;
	//}
}

void TextBox::onLoseFocus(InputDevice *i)
{
	active = false;

	if (editable)
	{
		APP->keyboard->characterInputListener = 0;
	}
}

void TextBox::onDraw()
{
	float yPos = lineHeight + innerPadding->t;

	int i;
/*
	// draw the selection if it's active
	if (selectionActive)
	{
		glColor3f(GLV_BLUE);

		for (i=0; i<selectionBoxes.size(); i++)
			glRectf(selectionBoxes[i][0], selectionBoxes[i][1], selectionBoxes[i][2], selectionBoxes[i][3]);
	}
*/
	i = (int) vOffset->value();

	//glDisable(GL_SCISSOR_TEST); // for debugging
	glPushAttrib(GL_SCISSOR_BIT);
	glScissor((int) (l+innerPadding->l), (int) (APP->window->h - (t+innerPadding->t+textArea->h)),
		(int) (textArea->w+innerPadding->r), (int) textArea->h);

	// draw the text
	while (yPos < textArea->bottom() && i < (int) lines.size())
	{
		font->render(lineStartX[i], yPos, 0.0f, linesOffset[i].c_str());
		
		yPos += lineHeight;
		i++;
	}

	glPopAttrib();

	if (cursor && active && cursorPixelPositionY > textArea->t && cursorPixelPositionY <= textArea->bottom() &&
		cursorPixelPositionX >= textArea->l - 0.5f && cursorPixelPositionX <= textArea->right() - 0.5f) // draw the cursor
	{
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glVertex3f(cursorPixelPositionX, cursorPixelPositionY - baseline - cursorHeight, 0.0f);
		glVertex3f(cursorPixelPositionX, cursorPixelPositionY - baseline, 0.0f);
		glEnd();
	}


	// draw widget components

	if (scrollBars)
	{
		glTranslatef(buttonMisc->l, buttonMisc->t, 0.0f);
		buttonMisc->drawBackground();
		buttonMisc->drawBackground();
		buttonMisc->onDraw();
		glTranslatef(-buttonMisc->l, -buttonMisc->t, 0.0f);

		glTranslatef(scrollHorizontal->l, scrollHorizontal->t, 0.0f);
		scrollHorizontal->drawBackground();
		scrollHorizontal->onDraw();
		glTranslatef(-scrollHorizontal->l, -scrollHorizontal->t, 0.0f);

		glTranslatef(scrollVertical->l, scrollVertical->t, 0.0f);
		scrollVertical->drawBackground();
		scrollVertical->onDraw();
		glTranslatef(-scrollVertical->l, -scrollVertical->t, 0.0f);
	}
}



Console2::Console2(int left, int top, int width, int height) : View(left, top, width, height)
{
	entry = new TextBox();
	entry->bgColor = new Color(0.f, 0.f);
	history = new TextBox();

	entry->showScrollBars(false);
	entry->setWordWrapping(false);
	history->showCursor(false);
	history->setEditable(false);

	previousCommands.push_back("");
	currentPreviousCommand = 0;

	onResize();
}

Console2::~Console2()
{
	delete entry->bgColor;
	delete entry;
	delete history;
}

void Console2::onResize()
{
	float totalPadding = entry->padding->t+entry->padding->b;

	//entry->setRect(0, h-(font->size()+totalPadding), w, font->size()+totalPadding);
	//history->setRect(0, 0, w, h-(font->size()+totalPadding));
	history->setRect(0, font->size()+totalPadding+history->padding->t, w, h-font->size()-totalPadding-history->padding->t);
	entry->setRect(0, 0, w, font->size()+totalPadding);

	entry->onResize();
	history->onResize();

	history->scrollToEnd();
}

string Console2::getHistory()
{
	return history->getText();
}

void Console2::setHistory(string s)
{
	history->setText(s);
	history->scrollToEnd();
}

void Console2::onDraw()
{
	glTranslatef(entry->l, entry->t, 0.0f);
	entry->t += this->t;
	entry->l += this->l;
	entry->drawBackground();
	entry->onDraw();
	entry->t -= this->t;
	entry->l -= this->l;
	glTranslatef(-entry->l, -entry->t, 0.0f);

	glTranslatef(history->l, history->t, 0.0f);
	history->t += this->t;
	history->l += this->l;
	history->onDraw();
	history->t -= this->t;
	history->l -= this->l;
	glTranslatef(-history->l, -history->t, 0.0f);
}

void Console2::onKeyDown(Keyboard *k)
{
	if (k->key(GLV_KEY_ENTER) || k->key(GLV_KEY_RETURN))
	{
		string oldText = history->getText();
		string newText = entry->getText();

		if (oldText == "")
			history->setText(newText);
		else
			history->setText(oldText + "\n" + newText);

		if (newText != "")
		{
			previousCommands[0] = "";
			previousCommands.insert(previousCommands.begin() + 1, newText);
			currentPreviousCommand = 0;
		}

		entry->setText("");

		history->scrollToEnd();

		action(newText);
	}
	else if (k->key(GLV_KEY_UP))
	{
		if (currentPreviousCommand < (int) previousCommands.size() - 1)
		{
			if (currentPreviousCommand == 0)
				previousCommands[0] = entry->getText();
			currentPreviousCommand++;
			entry->setText(previousCommands[currentPreviousCommand]);
			entry->scrollToEnd();
		}
	}
	else if (k->key(GLV_KEY_DOWN))
	{
		if (currentPreviousCommand > 0)
		{
			currentPreviousCommand--;
			entry->setText(previousCommands[currentPreviousCommand]);
			entry->scrollToEnd();
		}
	}
	else
	{
		entry->onKeyDown(k);
	}
}

void Console2::addLine(string s)
{
	history->setText(history->getText() + "\n" + s);
	history->scrollToEnd();
}

void Console2::onMouseDown(Mouse *m)
{
	//if (m->y >= entry->t)
	if (m->y() <= entry->bottom())
	{
		m->setContext(entry);
		entry->onMouseDown(m);
		m->unsetContext(entry);
	}
	else
	{
		m->setContext(history);
		history->onMouseDown(m);
		m->unsetContext(history);
	}
}

void Console2::onMouseUp(Mouse *m)
{
	entry->onMouseUp(m);
	history->onMouseUp(m);
}

void Console2::onDrag(Mouse *m)
{
	//if (m->y >= entry->t)
	if (m->y() <= entry->bottom())
	{
		entry->onDrag(m);
	}
	else
	{
		history->onDrag(m);
	}
}

void Console2::onGetFocus(InputDevice *i)
{
	entry->onGetFocus(i);
//	history->onLoseFocus(i);
}

void Console2::onLoseFocus(InputDevice *i)
{
	entry->onLoseFocus(i);
//	history->onLoseFocus(i);
}

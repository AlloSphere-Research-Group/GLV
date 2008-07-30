#include "TextControl.h"

using namespace glv;

// to do: these should be moved to Style or somewhere global

#define LEFT_PADDING		4.0f // x-distance from the left side of the box to the beginning of text
#define RIGHT_PADDING		2.0f // x-distance from the right side of the box to the end of text
#define TOP_PADDING			0.0f // y-distance from the top of the box to the first line of text
#define BOTTOM_PADDING		4.0f // y-distance from the bottom of the box to the last line of text

#define CURSOR_Y_OFFSET		4.0f // y-distance from the baseline of the font to the bottom of the cursor
#define CURSOR_X_OFFSET		-0.5f // x-distance from the cursor to the beginning of the next letter
#define CURSOR_HEIGHT		(font->size()-2.0f) // y-size of the cursor


TextDisplayBox::TextDisplayBox(int left, int top, int width, int height) : View(left, top, width, height)
{	
	// this code should probably go somewhere where it only has to be executed once per application
	// it allows us to not have to search per-character when we wrap words on a resize
	tokenPairs.push_back(glvTokenPair("\n", " \\n "));	// GLV newline
	// etc.
	
	selectionActive = false;
	
	linePadding = new Padding(4.0f, 0.0f, 2.0f, 4.0f);
	
	setText(" ");	// to prevent crashing in onMouseDown
}

void TextDisplayBox::setText(string aText)
{
	text = aText;

	// empty the vectors
	words.clear();
	wordWidths.clear();
	
	string temp;
	char ch;
	int pos, i = 0, j;

	// replace standard c token-characters with those GLV has implemented
	for (j=0; j<tokenPairs.size(); j++)
	{
		i = 0;

		while (i < text.size())
		{
			pos = text.find_first_of(tokenPairs[j].first, i);
			if (pos == -1)
				break;
			text.erase(pos, 1);
			text.insert(pos, tokenPairs[j].second);
			i = pos+tokenPairs[j].second.size();
		}
	}

	i = 0;

	// filter the string to delete remaining non-standard ascii values
	// we don't necessarily have to do this
	while (i < text.size())
	{
		ch = text.substr(i, 1).c_str()[0];

		if (ch < 127 && ch >= 32)
			i++;
		else
			text.erase(i, 1);
	}

	i = 0;

	// sort the text into words and calculate the pixel width of each word
	while (i < text.size())
	{
		pos = text.find_first_of(' ', i);	// assume that words are separated by one space
		if (pos == -1)						// if there are no more spaces we are at the end of the string
			pos = text.size();
		temp = text.substr(i, pos-i);
		words.push_back(temp);
		wordWidths.push_back(font->advance(temp.c_str()));

		i = pos + 1;
	}

	spaceWidth = font->advance(" ");		// so that we don't have to recalculate it each time

	//printf("%s\n", text.c_str());

	wrapWords();
}

void TextDisplayBox::wrapWords()
{
	// empty the vectors
	lines.clear();
	lineWidths.clear();
	letterOffsets.clear();

	int currentWord = 0;
	int currentLetter = 0;
	float currentLineWidth;

	while (currentWord < words.size())
	{
		if (words[currentWord] == "\\n")
		{
			currentLetter += words[currentWord].size() + 1;
			currentWord++;
		}
		else
		{
			letterOffsets.push_back(currentLetter);
			currentLetter += words[currentWord].size() + 1;

			// add the first word to the line no matter what
			currentLineWidth = wordWidths[currentWord];// + spaceWidth;
			lines.push_back(words[currentWord]);
			//lines.back() += " ";

			currentWord++;

			// add more words to the line until they fall outside the box width
			while (currentLineWidth + spaceWidth + wordWidths[currentWord] < w - linePadding->l - linePadding->r && currentWord < words.size())
			{
				currentLetter += words[currentWord].size() + 1;

				if (words[currentWord] == "\\n")
				{
					currentWord++;
					if (lines.back() == "")
						currentLineWidth += spaceWidth/2.0f;
					break;
				}

				currentLineWidth += wordWidths[currentWord] + spaceWidth;
				lines.back() += " " + words[currentWord];// + " ";
				currentWord++;
			}

			lineWidths.push_back(currentLineWidth);
		}
	}
}

void TextDisplayBox::onKeyDown(Keyboard *k)
{
	if (selectionActive && k->key('C') && k->ctrl())
	{
		int startPos = 0, length = 0;

		if (startLine < endLine || (startLine == endLine && startChar < endChar))
		{
			startPos = letterOffsets[startLine] + startChar;
			length = letterOffsets[endLine] + endChar - startPos;
		}
		else if (endLine < startLine || (startLine == endLine && endChar < startChar))
		{
			startPos = letterOffsets[endLine] + endChar;
			length = letterOffsets[startLine] + startChar - startPos;
		}

		string temp = tokenFilterForOutput(text.substr(startPos, length));

		APP->clipboard(temp); // send the string to the clipboard
		printf("%s\n", temp.c_str());
	}
}

string TextDisplayBox::tokenFilterForOutput(string input)
{
	string output = input;
	int i, j, pos;

	for (j=0; j<tokenPairs.size(); j++)
	{
		i = 0;

		while (i < output.size())
		{
			pos = output.find(tokenPairs[j].second, i);
			if (pos == -1)
				break;
			output.erase(pos, tokenPairs[j].second.size());
			output.insert(pos, tokenPairs[j].first);
			i = pos+tokenPairs[j].first.size();
		}
	}

	return output;
}

void TextDisplayBox::onDraw()
{
	float textHeight = font->size();
	float yPos = textHeight;
	
	int i;

	// draw the selection if it's active
	if (selectionActive)
	{
		glColor3f(GLV_BLUE);

		for (i=0; i<selectionBoxes.size(); i++)
			glRectf(selectionBoxes[i][0], selectionBoxes[i][1], selectionBoxes[i][2], selectionBoxes[i][3]);
	}

	i = 0;

	// draw the text
	while (yPos < h && i < lines.size())
	{
		font->render(linePadding->l, yPos, 0.0f, lines[i].c_str());

		yPos += textHeight;
		i++;
	}
}

void TextDisplayBox::onMouseDown(Mouse * m)
{
	// calculate start of selection

	int i;
	float y = m->y();

	if (y < 0.0f) y = 0.0f;

	startLine = (int)((y-CURSOR_Y_OFFSET)/(float)font->size());
	
	if (startLine >= lines.size())
		startLine = lines.size() - 1;

	float advance = linePadding->l;
	i = 0;

	while (i < lines[startLine].size() && advance <= m->x() - (font->advance(lines[startLine].substr(i, 1).c_str())*0.5f - 1.0f))
	{
		advance += font->advance(lines[startLine].substr(i, 1).c_str());
		i++;
	}

	startChar = i;

	selectBeginX = advance + CURSOR_X_OFFSET;
	selectBeginY = startLine * font->size() + CURSOR_Y_OFFSET;
	selectEndX = selectBeginX;
	selectEndY = selectBeginY;
	endLine = startLine;

	selectionActive = true;

	//printf("%d: %s\n", letterOffsets[startLine], text.substr(letterOffsets[startLine], 1).c_str());

	calculateSelectionBox();
}

void TextDisplayBox::onDrag(Mouse * m)
{
	// calculate end of selection
	int i;
	float y = m->y();

	if (y < 0.0f) y = 0.0f;

	endLine = (int)((y-CURSOR_Y_OFFSET)/(float)font->size());

	if (endLine >= lines.size())
		endLine = lines.size() - 1;

	float advance = linePadding->l;
	i = 0;

	while (i < lines[endLine].size() && advance <= m->x() - (font->advance(lines[endLine].substr(i, 1).c_str())*0.5f - 1.0f))
	{
		advance += font->advance(lines[endLine].substr(i, 1).c_str());
		i++;
	}

	endChar = i;

	selectEndX = advance + CURSOR_X_OFFSET;
	selectEndY = endLine * font->size() + CURSOR_Y_OFFSET;

	//printf("%f %f\n", m->x, m->y);

	calculateSelectionBox();
}

void TextDisplayBox::calculateSelectionBox()
{
	int i, j;

	for (i=0; i<selectionBoxes.size(); i++)
		delete selectionBoxes[i];

	selectionBoxes.clear();

	float *box;

	if (startLine == endLine)
	{
		box = new float[4];
		box[0] = selectBeginX; box[1] = selectBeginY;
		box[2] = selectEndX; box[3] = selectEndY+CURSOR_HEIGHT;

		selectionBoxes.push_back(box);
	}
	else if (startLine < endLine)
	{
		box = new float[4];
		box[0] = selectBeginX; box[1] = selectBeginY;
		box[2] = linePadding->l+lineWidths[startLine]+CURSOR_X_OFFSET;
		box[3] = selectBeginY+CURSOR_HEIGHT;
		selectionBoxes.push_back(box);

		box = new float[4];
		box[0] = linePadding->l+CURSOR_X_OFFSET; box[1] = selectEndY;
		box[2] = selectEndX; box[3] = selectEndY+CURSOR_HEIGHT;
		selectionBoxes.push_back(box);

		if (endLine - startLine > 1)
			for (j=startLine+1; j<endLine; j++)
			{
				box = new float[4];
				box[0] = linePadding->l+CURSOR_X_OFFSET;
				box[1] = selectBeginY+font->size()*(j-startLine);
				box[2] = linePadding->l+lineWidths[j]+CURSOR_X_OFFSET;
				box[3] = selectBeginY+font->size()*(j-startLine)+CURSOR_HEIGHT;
				selectionBoxes.push_back(box);
			}
	}
	else
	{
		box = new float[4];
		box[0] = selectEndX; box[1] = selectEndY;
		box[2] = linePadding->l+lineWidths[endLine]+CURSOR_X_OFFSET;
		box[3] = selectEndY+CURSOR_HEIGHT;
		selectionBoxes.push_back(box);

		box = new float[4];
		box[0] = linePadding->l+CURSOR_X_OFFSET; box[1] = selectBeginY;
		box[2] = selectBeginX; box[3] = selectBeginY+CURSOR_HEIGHT;
		selectionBoxes.push_back(box);

		if (startLine - endLine > 1)
			for (j=endLine+1; j<startLine; j++)
			{
				box = new float[4];
				box[0] = linePadding->l+CURSOR_X_OFFSET;
				box[1] = selectEndY+font->size()*(j-endLine);
				box[2] = linePadding->l+lineWidths[j]+CURSOR_X_OFFSET;
				box[3] = selectEndY+font->size()*(j-endLine)+CURSOR_HEIGHT;
				selectionBoxes.push_back(box);
			}
	}
}

void TextDisplayBox::onMouseUp(Mouse * m)
{
	// don't really need to do anything
}

void TextDisplayBox::onLoseFocus(InputDevice * i)
{
	// deactivate the selection box?
	selectionActive = false;
}

void TextDisplayBox::deleteFirstLine()
{
	text.erase(0, text.find(" \\n ") + 4);
}

void SingleLineTextEntryBox::setText(string aText)
{
	text = aText;

	letterOffset = 0;
	cursorLetterPosition = 0;
	cursorPixelPosition = LEFT_PADDING + CURSOR_X_OFFSET;

	letterWidths.clear();	// empty the vector

	// get the width of each letter in the text string
	for (int i=0; i<text.size(); i++)
	{
		float advance = font->advance(text.substr(i, 1).c_str());
		
		letterWidths.push_back(advance);		
		cursorPixelPosition += advance;
		cursorLetterPosition++;
	}
}

void SingleLineTextEntryBox::onDraw()
{
	font->render(LEFT_PADDING, font->size(), 0.0f, text.substr(letterOffset, text.size()).c_str());

	if (active) // draw the cursor
	{
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		glVertex3f(cursorPixelPosition, CURSOR_HEIGHT+CURSOR_Y_OFFSET, 0.0f);
		glVertex3f(cursorPixelPosition, CURSOR_Y_OFFSET, 0.0f);
		glEnd();
	}
}

void SingleLineTextEntryBox::onMouseDown(Mouse *m)
{
	float advance = LEFT_PADDING;
	int i = letterOffset;

	// align the cursor to the letter nearest the mouse click
	while (i < letterWidths.size() && advance <= m->x() - (letterWidths[i]*0.5f - 1.0f))
	{
		advance += letterWidths[i];
		i++;
	}

	cursorLetterPosition = i;
	cursorPixelPosition = advance + CURSOR_X_OFFSET;
	active = true;

	APP->keyboard->characterInputListener = this;

	consume(m);
}

void SingleLineTextEntryBox::onLoseFocus(InputDevice *i)
{
	active = false;
	APP->keyboard->characterInputListener = 0;
//	letterOffset = 0;
}

void SingleLineTextEntryBox::onChar(int ascii, bool state)
{
	if (ascii >= 32 && ascii < 127 && state == true)
	{
		string temp(1, ascii);
		text.insert(cursorLetterPosition, temp);

		letterWidths.insert(letterWidths.begin() + cursorLetterPosition, font->advance(temp.c_str()));
		
		cursorLetterPosition++;
		processText();
	}
}

void SingleLineTextEntryBox::onKeyDown(Keyboard *k)
{
	if (k->key(GLV_KEY_LEFT))
	{
		consume(k);

		if (cursorLetterPosition > 0)
		{
			cursorLetterPosition--;
			processText();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_RIGHT))
	{
		consume(k);

		if (cursorLetterPosition < text.size())
		{
			cursorLetterPosition++;
			processText();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_END))
	{
		consume(k);

		int temp = text.size();

		if (cursorLetterPosition < temp)
		{
			cursorLetterPosition = temp;
			processText();
		}
		else
			return;
	}
	else if (k->key(GLV_KEY_HOME))
	{
		consume(k);

		letterOffset = 0;
		cursorLetterPosition = 0;
		cursorPixelPosition = LEFT_PADDING + CURSOR_X_OFFSET;
		return;
	}
	else if (k->key(GLV_KEY_DELETE))
	{
		consume(k);

		if (cursorLetterPosition < text.size())
		{
			text.erase(cursorLetterPosition, 1); // remove 1 character to the right of cursor
			letterWidths.erase(letterWidths.begin()+cursorLetterPosition);
		}
		return;
	}
	else if (k->key(GLV_KEY_BACKSPACE))
	{
		consume(k);

		if (cursorLetterPosition > 0)
		{
			text.erase(cursorLetterPosition-1, 1); // remove 1 character to the left of cursor
			letterWidths.erase(letterWidths.begin()+cursorLetterPosition-1);
			cursorLetterPosition--;
			processText();
		}
		else
			return;
	}
	else if (k->key('V') && k->ctrl())
	{
		string temp = APP->clipboard();
		text.insert(cursorLetterPosition, temp);
			
		char c[2];
		c[1] = '\0';

		for (int i=0; i<temp.size(); i++)
		{
			c[0] = temp.c_str()[i];
			letterWidths.insert(letterWidths.begin() + cursorLetterPosition + i, font->advance(c));
		}
		
		cursorLetterPosition += temp.size();
		processText();
	}
	else if (k->key('K') && k->ctrl())
	{
		setText("");
	}
}

void SingleLineTextEntryBox :: processText()
{
	// this function determines the first (leftmost) letter that should be visible in the box (letterOffset),
	// and where the cursor should be drawn (cursorPixelPosition).

	float advance; // temporary variable used to hold running count of width

	if (cursorLetterPosition < letterOffset) // scroll the text one character to the left
	{
		advance = LEFT_PADDING; // set the cursor position to the leftmost side of the box
		letterOffset--; // index one character to the left in the 'text' string
	}
/*	else if (cursorLetterPosition == text.size()) // scroll to the end of the text
	{
		// count backwards from right side

		int i = cursorLetterPosition - 1;

		advance = RIGHT_PADDING;

		while (advance < w-LEFT_PADDING && i >= 0)
		{
			advance += letterWidths[i];
			i--;
		}

		letterOffset = i+2;
	}*/
	else // cursor is somewhere in the middle or at the right edge, so...
	{
		int i;

		do
		{
			i = letterOffset;
			advance = LEFT_PADDING;

			while (i < cursorLetterPosition)
			{
				advance += letterWidths[i];

				if (advance + RIGHT_PADDING > w)// && i+1 < text.size())
				{
					letterOffset++;
					break;
				}
				else
					i++;
			}
		}
		while (i != cursorLetterPosition);
	}

	cursorPixelPosition = advance + CURSOR_X_OFFSET;
}

Console::Console(int left, int top, int width, int height) : View(left, top, width, height)
{
	int commandHeight = font->size() + 4;

	commandEntry = new SingleLineTextEntryBox(0, int(this->h-commandHeight), width, commandHeight);
	commandHistory = new TextDisplayBox(0, 0, (int)this->w, int(this->h-commandHeight-padding->b));

	commandHistory->bgColor = new Color(GLV_BLACK, 0.0f);
	commandEntry->bgColor = commandHistory->bgColor;
		
	numDisplayLines = (int) ((commandHistory->h-BOTTOM_PADDING)/font->size());
	
	string initialText = "";
	int i = 0;

	// fill the text display box with newlines so that the next line added appears at the bottom
	while (i < numDisplayLines-1)
	{
		initialText += '\n';
		i++;
	}

	commandHistory->setText(initialText);
	commandEntry->setText("");
	
	//commandEntry->padding = Style::noborder->padding;
	//add(commandEntry);
}

void Console::onDraw()
{
	commandHistory->drawBackground();
	commandHistory->onDraw();
	
	float spacing = commandHistory->padding->b;// + Style::standard->padding->b;
	
	glTranslatef(0.f,commandHistory->h + spacing,0.f);
		//commandEntry->drawBackground();
		commandEntry->onDraw();
	glTranslatef(0.f,-commandHistory->h-spacing,0.f);
}

void Console::onKeyDown(Keyboard *k)
{
	if (k->key(GLV_KEY_ENTER) || k->key(GLV_KEY_RETURN))
	{
		command = commandEntry->getText();
		
		action(command);

		commandEntry->setText("");
		consume(k);
	} else if (k->key(GLV_KEY_UP)) {
		commandEntry->setText(command);
		consume(k);
	} else if (k->key('C') && k->ctrl()) {
		commandHistory->onKeyDown(k);
	} else {
		commandEntry->onKeyDown(k);
	}
}

void Console::addLine(string message)
{
	commandHistory->deleteFirstLine();
	commandHistory->setText(commandHistory->getText() + '\n' + message);
/*
	while (commandHistory->numLines() < numDisplayLines-1)
		commandHistory->setText(commandHistory->getText() + '\n');
*/
	while (commandHistory->numLines() > numDisplayLines)
	{
		commandHistory->deleteFirstLine();
		commandHistory->setText(commandHistory->getText());
	}
}

void Console::onMouseDown(Mouse * m)
{
	if (m->y() > commandEntry->t)
		commandEntry->onMouseDown(m);
	else
		commandHistory->onMouseDown(m);

	consume(m);
}

void Console::onDrag(Mouse * m)
{
	if (m->y() > commandEntry->t)
		commandEntry->onDrag(m);
	else
		commandHistory->onDrag(m);

	consume(m);
}

void Console::onLoseFocus(InputDevice * i)
{
	commandEntry->onLoseFocus(i);
	commandHistory->onLoseFocus(i);
}

#include "../RangedValue.h"
#include "Icon.h"
#include "RangedControl.h"

using namespace glv;

RangedControl :: RangedControl(int l, int t, int w, int h, int numParameters)
	: View(l,t,w,h)
{
	mRangedValues.resize(numParameters);
}

bool RangedControl :: connect(RangedValue * model, int index)
{
	if(index < mRangedValues.size()){
		mRangedValues[index] = model;
		return true;
	}
	// What to do for more than one RangedValue?
//	if(model){	
//		label = model->name;	// assign model's name to mine
//	}
	return false;
}


// Can't be inlined because of forward declaration of RangedValue
float RangedControl :: value(int index){
	return mRangedValues[index]->value();
}


Button :: Button(int l, int t, int w, int h)
	: RangedControl(l, t, w, h, 1)
{
	borderSize = 1.f;
	activeColor = Style::standard->indicatorColor;
	iconColor = Style::standard->iconColor;
	isToggleable = true;
	
	iconInactive(Icon::none);
	iconActive(Icon::rect);
	
	iconScale = 0.7f;
	
	mValue = 0.f;
}

Button :: ~Button()
{
	activeColor->smartDelete();
	iconColor->smartDelete();
}


void Button::icon(void (* iconFunction)(float l, float t, float w, float h))
{
	mIconInactive = iconFunction;
	mIconActive = iconFunction;
}

void Button::iconInactive(void (* iconFunction)(float l, float t, float w, float h))
{
	mIconInactive = iconFunction;
}

void Button::iconActive(void (* iconFunction)(float l, float t, float w, float h))
{
	mIconActive = iconFunction;
}
	
void Button :: onDraw()
{
	RangedValue * rangedValue = mRangedValues[0];

	if(rangedValue){
		float normal = rangedValue->normal();
		
		float amt1 = iconScale;
		float amt2 = (1.f - amt1) * 0.5f;
		
		if(normal > 0.f){
			glColor4f(glvColor4(activeColor));
			mIconActive(w * amt2, h * amt2, w * amt1, h * amt1);
		}
		else{
			glColor4f(glvColor4(iconColor));
			mIconInactive(w * amt2, h * amt2, w * amt1, h * amt1);		
		}
	} else {
		float amt1 = iconScale;
		float amt2 = (1.f - amt1) * 0.5f;
		
		if(mValue > 0.f){
			glColor4f(glvColor4(activeColor));
			mIconActive(w * amt2, h * amt2, w * amt1, h * amt1);
		}
		else{
			glColor4f(glvColor4(iconColor));
			mIconInactive(w * amt2, h * amt2, w * amt1, h * amt1);		
		}
	}

}

void Button :: onMouseDown(Mouse * m)
{
	RangedValue * rangedValue = mRangedValues[0];
	
	if(rangedValue){
		float normal = rangedValue->normal();
		rangedValue->normal(1.f - normal);
	} else {
		mValue = isToggleable ? mValue == 0.f : 1.f; 
		action(mValue);
	}
}

void Button :: onMouseUp(Mouse * m)
{

	RangedValue * rangedValue = mRangedValues[0];

	if(!isToggleable && rangedValue){
		float normal = rangedValue->normal();
		rangedValue->normal(1.f - normal);		
	} else {
		if (!isToggleable) {
			mValue = 0.f; action(mValue);
		}
	}
}


Slider :: Slider(int l, int t, int w, int h, bool setOnMouseDown)
	: RangedControl(l, t, w, h, 1)
{
	this->setOnMouseDown = setOnMouseDown;
	borderSize = 1.f;
	sliderColor = Style::standard->indicatorColor;
	dragSensitivity = 1.f;
}


Slider :: ~Slider()
{
	sliderColor->smartDelete();
}


void Slider :: onDraw()
{
	glColor4f(glvColor4(sliderColor)); 
	
	//	// convert value to string for printing
//	char labelValue[7];
//	sprintf(labelValue, "% -6.2f", value);

	// draw horizontal slider
	if(w >= h){
		glRectf(w * mValue, h, 0.f, 0.f);
		drawLabel(4.f);
		
		//bitmapString(l + borderSize + 2.f, t + h * 0.5f + 4.f, GLUT_BITMAP_HELVETICA_12, string);
		//bitmapString(r, t + h * 0.5f + 4.f, GLUT_BITMAP_HELVETICA_12, labelValue);
	}
	
	// draw vertical slider
	else {
		glRectf(w, h, 0.f, h * (1.f - mValue));
		
		//bitmapString(l, t - 2.f, GLUT_BITMAP_HELVETICA_12, string);
		//bitmapString(l, b + 12.f, GLUT_BITMAP_HELVETICA_12, labelValue);	
	}
}


void Slider :: onDrag(Mouse * m)
{
	RangedValue * rangedValue = mRangedValues[0];
	float offsetScalar = APP->keyboard->shift() ? 0.25f : 1.f;
	float normOffset;
	
	if(rangedValue){

		if (w >= h)		normOffset = m->dx() / w;		// horizontal case
		else			normOffset = -m->dy() / h;	// vertical case

		rangedValue->offsetNormal(normOffset * offsetScalar);
	} else {
		
		if (w >= h)		normOffset = (m->x() - m->x(0))/w; 
		else			normOffset = (m->y(0) - m->y())/h; 
		
		mValue = clickValue + normOffset * offsetScalar * dragSensitivity; //mValue += normOffset * offsetScalar;
		mValue = mValue > 1.f ? 1.f : mValue < 0.f ? 0.f : mValue;
		action(mValue);
	}
}


void Slider :: onMouseDown(Mouse * m)
{
	if (setOnMouseDown) {
		if (w >= h)		mValue = m->x()/w; 
		else			mValue = (h - m->y())/h;
	}
	clickValue = mValue;	// cache my current value for onDrag
	action(mValue);
}

void Slider :: onKeyDown(Keyboard * k)
{
	if (k->shift()) {
		clickValue += (mValue - clickValue) * 0.75f;
	}
}
void Slider :: onKeyUp(Keyboard * k)
{
	if (k->shift()) {
		clickValue += (clickValue - mValue) * 3.f;
	}
}



void Slider :: onMouseUp(Mouse * m) {} 

Slider2d :: Slider2d(int l, int t, int w, int h)
	: RangedControl(l, t, w, h, 2)
{
	borderSize = 1.f;
	sliderColor = Style::standard->indicatorColor;
}


Slider2d :: ~Slider2d()
{
	sliderColor->smartDelete();
}


void Slider2d :: onDraw()
{
	RangedValue * rangedValueX = mRangedValues[0];
	RangedValue * rangedValueY = mRangedValues[1];

	float valueNormX;
	if(rangedValueX)	valueNormX = rangedValueX->normal();
	else				valueNormX = 0.5f;
	
	float posX = (w-1.f) * valueNormX;
	
	//drawLabel(4.f);

	float valueNormY;
	if(rangedValueY)	valueNormY = rangedValueY->normal();
	else				valueNormY = 0.5f;

	float posY = (h-1.f) * (1.f - valueNormY);
	
	if(rangedValueX || rangedValueY)	glColor4f(glvColor4(sliderColor));
	else								glColor4f(glvColor3(sliderColor), 0.5f);
	
	//float hh = h * 0.5f;
	//float hw = w * 0.5f;
	
	// Triangles
//	glBegin(GL_TRIANGLE_FAN);
//		glVertex2f(0.f, h);
//		glVertex2f(hw, h);
//		glVertex2f(posX, posY);
//		glVertex2f(0.f, hh);
//	glEnd();

	// Rectangle
//	glRectf(posX, posY, 0.f, h);

	// Web
//	glBegin(GL_LINES);
//		glVertex2f(posX, posY); glVertex2f(hw, h);
//		glVertex2f(posX, posY); glVertex2f(w, hh);
//		glVertex2f(posX, posY); glVertex2f(hw, 0.f);
//		glVertex2f(posX, posY); glVertex2f(0.f, hh);
//
//		glVertex2f(posX, posY); glVertex2f(0.f, h);
//		glVertex2f(posX, posY); glVertex2f(w, h);
//		glVertex2f(posX, posY); glVertex2f(w, 0.f);
//		glVertex2f(posX, posY); glVertex2f(0.f, 0.f);
//
////		glVertex2f(0.f, posY); glVertex2f(w, posY);
////		glVertex2f(posX, 0.f); glVertex2f(posX, h);
//	glEnd();
	
	// Quad
	glBegin(GL_QUADS);
		glVertex2f(0.f, posY);
		glVertex2f(posX, h);
		glVertex2f(w, posY);
		glVertex2f(posX, 0.f);
	glEnd();
	
	glColor4f(glvColor4(bgColor));
	glRectf(posX, h, posX+1, 0.f);
	glRectf(0.f, posY, w, posY+1);

//	if((posX > 6.f) && (posY < (h - 6.f))) return;

	// fade color in
//	glColor4f(COLOR3(sliderColor), 1.f - ((posX < h - posY) ? posX : h - posY) * 0.16667f);

	// double diagonal indicators
//	glBegin(GL_LINES);
//		glVertex2f(posX, posY); glVertex2f(w, hh);
//		glVertex2f(posX, posY); glVertex2f(hw, 0.f);
//	glEnd();

	// clock hand
//	glBegin(GL_LINES);
//		glVertex2f(posX, posY); glVertex2f(hw, hh);
//	glEnd();
	
	// x
//	glBegin(GL_LINES);
//		float d = 6.f;
//		glVertex2f(posX - d, posY - d);
//		glVertex2f(posX + d, posY + d);
//		glVertex2f(posX, posY);
//		glVertex2f(posX + d, posY - d);
//	glEnd();
	
	// overline
//	glBegin(GL_LINES);
//		float d = 6.f;
//		glVertex2f(0.f, h - d);
//		glVertex2f(posX, h - d);
//		glVertex2f(d, h);
//		glVertex2f(d, posY);
//	glEnd();

}


void Slider2d :: onDrag(Mouse * m)
{
	RangedValue * rangedValueX = mRangedValues[0];
	RangedValue * rangedValueY = mRangedValues[1];
	
	float offsetScalar = 0.5f;

	if(APP->keyboard->shift()){
		offsetScalar = 0.125f;
	}
	
	if(rangedValueX){
		float normOffset = m->dx() / w;		
		rangedValueX->offsetNormal(normOffset * offsetScalar);	
	}

	if(rangedValueY){
		float normOffset = -m->dy() / h;		
		rangedValueY->offsetNormal(normOffset * offsetScalar);	
	}	
	
}



ScrollBar :: ScrollBar(int l, int t, int w, int h)
	: RangedControl(l, t, w, h, 1)
{
	if (w > h)
	{
		buttonUp = new Button(0, 0, h, h);
		buttonDown = new Button(w - h, 0, h, h);
		buttonUp->icon(Icon::triangleL);
		buttonDown->icon(Icon::triangleR);

		trackOffset = h + buttonUp->padding->l;
		trackSize = w - 2.f * trackOffset;
	}
	else
	{
		buttonUp = new Button(0, 0, w, w);
		buttonDown = new Button(0, h - w, w, w);
		buttonUp->icon(Icon::triangleU);
		buttonDown->icon(Icon::triangleD);

		trackOffset = w + buttonUp->padding->t;
		trackSize = h - 2.f * trackOffset;
	}

	buttonUp->isToggleable = false;
	buttonDown->isToggleable = false;

	setKnobScale(1.f);
	setArrowButtonIncrement(1.f/trackSize);

	dragging = false;
}

ScrollBar :: ~ScrollBar()
{
	delete buttonUp;
	delete buttonDown;
}

void ScrollBar :: setKnobScale(float scale)
{
	 if (w > h)
	 {
		 kW = trackSize * scale;
		 kH = h;
	 }
	 else
	 {
		 kW = w;
		 kH = trackSize * scale;
	 }

	 knobSize = scale;
}

void ScrollBar :: onDraw()
{
	glColor4f(glvColor4(Style::standard->indicatorColor));

	if (w > h)
	{
		kL = trackOffset + mValue * (trackSize - kW);
		kT = 0.f;
	}
	else
	{
		kL = 0.f;
		kT = trackOffset + mValue * (trackSize - kH);
	}

	glRectf(kL, kT, kL+kW, kT+kH);

	glTranslatef(buttonUp->l, buttonUp->t, 0.0f);
	buttonUp->drawBackground();
	buttonUp->onDraw();
	glTranslatef(-buttonUp->l, -buttonUp->t, 0.0f);

	glTranslatef(buttonDown->l, buttonDown->t, 0.0f);
	buttonDown->drawBackground();
	buttonDown->onDraw();
	glTranslatef(-buttonDown->l, -buttonDown->t, 0.0f);
}

void ScrollBar :: onDrag(Mouse * m)
{
	if (dragging)
	{
		if (w > h && kW < trackSize)
			mValue = clickValue + (m->x() - m->x(0))/(trackSize - kW);
		else if (h > w && kH < trackSize)
			mValue = clickValue + (m->y() - m->y(0))/(trackSize - kH);

		if (mValue < 0.0f)
			mValue = 0.0f;
		else if (mValue > 1.0f)
			mValue = 1.0f;

		action(mValue);
	}
}

void ScrollBar :: onMouseDown(Mouse * m)
{
	if (m->x() >= kL && m->x() <= kL+kW && m->y() >= kT && m->y() <= kT+kH)
	{
		dragging = true;
		clickValue = mValue;
	}
	else if ((w > h && m->x() < trackOffset) || (w <= h && m->y() < trackOffset))
	{
		buttonUp->onMouseDown(m);
		if (knobSize != 1.0f)
			mValue -= arrowButtonIncrement;
		if (mValue < 0.0f)
			mValue = 0.0f;
		action(mValue);
	}
	else if ((w > h && m->x() > w - trackOffset) || (w <= h && m->y() > h - trackOffset))
	{
		buttonDown->onMouseDown(m);
		if (knobSize != 1.0f)
			mValue += arrowButtonIncrement;
		if (mValue > 1.0f)
			mValue = 1.0f;
		action(mValue);
	}
}

void ScrollBar :: onMouseUp(Mouse * m)
{
	 dragging = false;

	 buttonUp->onMouseUp(m);
	 buttonDown->onMouseUp(m);
}
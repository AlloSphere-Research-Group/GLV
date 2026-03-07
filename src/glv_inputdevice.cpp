/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

Keyboard::Keyboard()
:	mKeycode(-1), mIsDown(false)
{
	for(int i=0; i<5; ++i) mModifiers[i] = false;
}

int Keyboard::key() const { return mKeycode; }
int Keyboard::keyAsNumber() const { return key() - 48; }
bool Keyboard::isDown() const { return mIsDown; }
bool Keyboard::isNumber() const {return (key() >= '0') && (key() <= '9'); }
bool Keyboard::isPrint() const { return (key() >= ' ') && (key() <= '~'); }
bool Keyboard::alt()   const { return mModifiers[1]; }
bool Keyboard::caps()  const { return mModifiers[3]; }
bool Keyboard::ctrl()  const { return mModifiers[2]; }
bool Keyboard::meta()  const { return mModifiers[4]; }
bool Keyboard::shift() const { return mModifiers[0]; }
bool Keyboard::key(int k) const { return mKeycode == k; }
void Keyboard::alt  (bool state){mModifiers[1] = state;}
void Keyboard::caps (bool state){mModifiers[3] = state;}
void Keyboard::ctrl (bool state){mModifiers[2] = state;}
void Keyboard::meta (bool state){mModifiers[4] = state;}
void Keyboard::shift(bool state){mModifiers[0] = state;}

void Keyboard::print() const{
	printf("code:%3d (%c) down:%d a:%d C:%d c:%d m:%d s:%d\n", 
		key(), key(), isDown(), alt(), caps(), ctrl(), meta(), shift());
}



Mouse::Mouse()
:	mXRel(0), mYRel(0), mButton(0), mClicks(0), mIsDown(false)
{
	mX[0] = mX[1] = mX[2] = mY[0] = mY[1] = mY[2] = mW[0] = mW[1] = mW[2] = 0;
	
	for (int i=0; i<GLV_MAX_MOUSE_BUTTONS; i++){
		b[i] = false;
		bx[i] = by[i] = bxRel[i] = byRel[i] = 0;
	}
}

space_t Mouse::x() const { return mX[0]; }
space_t Mouse::y() const { return mY[0]; }
space_t Mouse::x(int button) const { return bx[button]; }
space_t Mouse::y(int button) const { return by[button]; }
space_t Mouse::w() const { return mW[0]; }
space_t Mouse::dx() const { return mX[0] - mX[1]; }
space_t Mouse::dy() const { return mY[0] - mY[1]; }
space_t Mouse::dw() const { return mW[0] - mW[1]; }
space_t Mouse::ddx() const { return mX[0] - 2 * mX[1] + mX[2]; }
space_t Mouse::ddy() const { return mY[0] - 2 * mY[1] + mY[2];; }
space_t Mouse::ddw() const { return mW[0] - 2 * mW[1] + mW[2];; }
space_t Mouse::xRel() const { return mXRel; }
space_t Mouse::yRel() const { return mYRel; }
space_t Mouse::xRel(int button) const { return bxRel[button]; }
space_t Mouse::yRel(int button) const { return byRel[button]; }
int Mouse::button() const { return mButton; }
int Mouse::clicks() const { return mClicks; }
bool Mouse::isDown() const { return mIsDown; }
bool Mouse::isDown(int button) const { return b[button]; }
bool Mouse::left() const { return b[Left]; }
bool Mouse::middle() const { return b[Middle]; }
bool Mouse::right() const { return b[Right]; }

void Mouse::  setContext(View * v){mXRel -= v->l; mYRel -= v->t;}
void Mouse::unsetContext(View * v){mXRel += v->l; mYRel += v->t;}
void Mouse::pos(int x, int y) {
	bufferPos((space_t)x, mX);
	bufferPos((space_t)y, mY);
}
void Mouse::bufferPos(space_t newPos, space_t * pos){
	pos[2] = pos[1];
	pos[1] = pos[0];
	pos[0] = newPos;
}
void Mouse::posRel(space_t rx, space_t ry){ mXRel=rx; mYRel=ry;}

bool Mouse::isDownAny() const {
	for(unsigned i=0; i<GLV_MAX_MOUSE_BUTTONS; ++i){
		if(b[i]) return true;
	}
	return false;
}

void Mouse::print() const {
	printf("abspos: [%i, %i]\n", (int)x(), (int)y());
	printf("relpos: [%i, %i]\n", (int)xRel(), (int)yRel());
	printf("dpos:   [%i, %i]\n", (int)dx(), (int)dy());
	printf("ddpos:  [%i, %i]\n", (int)ddx(), (int)ddy());
	printf("button: ");
	for(int i=0; i<GLV_MAX_MOUSE_BUTTONS; i++){
		printf(b[i] ? "|" : ".");
	}
	for(int i=0; i<GLV_MAX_MOUSE_BUTTONS; i++){
		printf("\nbpos%i: [%i, %i]", i, (int)bx[i], (int)by[i]);
	}
	printf("\n\n");
}

void Mouse::updateButton(int btn, bool pressed, int clicks){
	if(btn >= 0 && btn < GLV_MAX_MOUSE_BUTTONS) {
		mButton = btn;
		mClicks = clicks;
		mIsDown = b[mButton] = pressed;
		bx[mButton] = x();
		by[mButton] = y();
		bxRel[mButton] = xRel();
		byRel[mButton] = yRel();
	}
}

} // end namespace glv

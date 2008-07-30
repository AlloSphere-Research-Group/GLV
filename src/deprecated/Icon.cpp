#include "../../core/glv-macros.h"
#include "Icon.h"

using namespace glv;

float Icon::thickness = 2.f;

void Icon::rect(float l, float t, float w, float h){
	glBegin(GL_QUADS);
		glVertex2f(l, t);
		glVertex2f(l, t + h);
		glVertex2f(l + w, t + h);
		glVertex2f(l + w, t);
	glEnd();
}

void Icon::triangleR(float l, float t, float w, float h){
	glBegin(GL_TRIANGLES);
		glVertex2f(l + w, t + h * 0.5f);
		glVertex2f(l, t);
		glVertex2f(l, t + h);
	glEnd();
}

void Icon::triangleL(float l, float t, float w, float h){
	glBegin(GL_TRIANGLES);
		glVertex2f(l, t + h * 0.5f);
		glVertex2f(l + w, t + h);
		glVertex2f(l + w, t);
	glEnd();
}

void Icon::triangleU(float l, float t, float w, float h){
	glBegin(GL_TRIANGLES);
		glVertex2f(l + w * 0.5f, t);
		glVertex2f(l, t + h);
		glVertex2f(l + w, t + h);
	glEnd();
}

void Icon::triangleD(float l, float t, float w, float h){
	glBegin(GL_TRIANGLES);
		glVertex2f(l + w * 0.5f, t + h);
		glVertex2f(l + w, t);
		glVertex2f(l, t);
	glEnd();
}

void Icon::x(float l, float t, float w, float h){
	glLineWidth(thickness);
	glBegin(GL_LINES);
		glVertex2f(l, t);
		glVertex2f(l + w, t + h);
		glVertex2f(l, t + h);
		glVertex2f(l + w, t);
	glEnd();
}

void Icon::plus(float l, float t, float w, float h){
	glLineWidth(thickness);
	float mx = l + w * 0.5f;
	float my = t + h * 0.5f;
	glBegin(GL_LINES);
		glVertex2f(mx, t);
		glVertex2f(mx, t + h);
		glVertex2f(l, my);
		glVertex2f(l + w, my);
	glEnd();
}

void Icon::minus(float l, float t, float w, float h){
	float my = t + h * 0.5f;
	glLineWidth(thickness);
	glBegin(GL_LINES);
		glVertex2f(l, my);
		glVertex2f(l + w, my);
	glEnd();
}

void Icon::check(float l, float t, float w, float h){
	glLineWidth(thickness);
	glBegin(GL_LINE_STRIP);
		glVertex2f(l, t + h * 0.5f);
		glVertex2f(l + w * 0.3f, t + h);
		glVertex2f(l + w, t);
	glEnd();
}

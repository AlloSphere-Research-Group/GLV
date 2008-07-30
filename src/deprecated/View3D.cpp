#include "View3D.h"

using namespace glv;

View3D::View3D(int left, int top, int width, int height) : View(left, top, width, height)
{

}

void View3D::onDraw()
{	
	//create viewport just at widget location
	glViewport((int) l, (int) (APP->window->h - this->bottom()), (int) this->w, (int) this->h);
	
	//save openGL state
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluPerspective( 45.0f, (GLfloat)w/(GLfloat)h, 1.0f, 100.0f );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Do all opel GL drawing
	drawGL();

	//return openGL state
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPopAttrib();
	
	//return viewport to being entire APP window
	glViewport(0, 0, (int) APP->window->w, (int) APP->window->h);
}

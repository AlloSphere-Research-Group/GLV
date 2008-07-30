#ifndef GLV_VIEW3D_H_INC
#define GLV_VIEW3D_H_INC

#include "glv-core.h"

namespace glv {

class View3D : public View
{
public:

	View3D(int left, int top, int width, int height);
	
	virtual void onDraw();
	virtual void drawGL() {};

private:

	string title;
};

} //end glv namespace

#endif // GLV_VIEW3D_H_INC
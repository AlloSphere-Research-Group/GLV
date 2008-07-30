#ifndef INC_GLV_BEHAVIOR_H
#define INC_GLV_BEHAVIOR_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

namespace Behavior{

	static bool mouseMove(View * v, GLV & glv){
		if(glv.mouse.right()){
			v->move(glv.mouse.dx(), glv.mouse.dy());
		}
		return false;
	}

	static bool mouseResize(View * v, GLV & glv){
		if(glv.mouse.middle()){
		
			float dx = glv.mouse.dx();
			float dy = glv.mouse.dy();
			float mx = glv.mouse.xRel() / v->w;
			float my = glv.mouse.yRel() / v->h;
		
			if(mx < 0.5){
				v->resizeLeftTo(v->l + dx);
				my < 0.5 ? v->resizeTopTo(v->t + dy) : v->resizeBottomTo(v->bottom() + dy);
			}
			else{
				v->resizeRightTo(v->right() + dx);
				my < 0.5 ? v->resizeTopTo(v->t + dy) : v->resizeBottomTo(v->bottom() + dy);					
			}
			v->fixNegativeExtent();
		}
		return false;
	}

} // Behavior::
} // glv::

#endif

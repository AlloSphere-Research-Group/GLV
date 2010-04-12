#ifndef INC_GLV_BEHAVIOR_H
#define INC_GLV_BEHAVIOR_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

namespace Behavior{

	static bool mouseMove(View * v, GLV & glv){
		if(glv.mouse.left()){
			v->move(glv.mouse.dx(), glv.mouse.dy());
			return false;
		}
		return true;
	}
	
	static bool mouseResizeCorner(View * v, GLV & glv){
		if(glv.mouse.left()){

			float mx = glv.mouse.xRel();
			float my = glv.mouse.yRel();

			if(mx > (v->w-16) && my > (v->h-16)){

				float dx = glv.mouse.dx();
				float dy = glv.mouse.dy();

				v->resizeRightTo(v->right() + dx);
				v->resizeBottomTo(v->bottom() + dy);
				v->fixNegativeExtent();		
				
				return false;
			}			
		}
		return true;
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
			return false;
		}
		return true;
	}

} // Behavior::
} // glv::

#endif

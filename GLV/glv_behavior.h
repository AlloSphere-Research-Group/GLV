#ifndef INC_GLV_BEHAVIOR_H
#define INC_GLV_BEHAVIOR_H

/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "glv_core.h"

namespace glv{

namespace Behavior{

	struct MouseMove : public EventHandler {
		bool onEvent(View& v, GLV& g){
			if(g.mouse().left()){
				v.move(g.mouse().dx(), g.mouse().dy());
				return false;
			}
			return true;
		}
	};

	struct MouseResizeCorner : public EventHandler {
		bool onEvent(View& v, GLV& g){
			if(g.mouse().left()){

				float dx = g.mouse().dx();
				float dy = g.mouse().dy();
				float mx = g.mouse().xRel() - dx;	// subtract diff because position already updated
				float my = g.mouse().yRel() - dy;

				if(mx < v.w && mx > (v.w-16) && my < v.h && my > (v.h-16)){
					v.resizeRightTo(v.right() + dx);
					v.resizeBottomTo(v.bottom() + dy);
					v.rectifyGeometry();
					return false;
				}
			}
			return true;
		}
	};

	struct MouseResize : public EventHandler {
		bool onEvent(View& v, GLV& g){
			if(g.mouse().middle()){
			
				float dx = g.mouse().dx();
				float dy = g.mouse().dy();
				float mx = g.mouse().xRel() / v.w;
				float my = g.mouse().yRel() / v.h;
			
				if(mx < 0.5){
					v.resizeLeftTo(v.l + dx);
					my < 0.5 ? v.resizeTopTo(v.t + dy) : v.resizeBottomTo(v.bottom() + dy);
				}
				else{
					v.resizeRightTo(v.right() + dx);
					my < 0.5 ? v.resizeTopTo(v.t + dy) : v.resizeBottomTo(v.bottom() + dy);					
				}
				v.rectifyGeometry();
				return false;
			}
			return true;
		}
	};

	static MouseMove mouseMove;	
	static MouseResizeCorner mouseResizeCorner;
	static MouseResize mouseResize;

} // Behavior::
} // glv::

#endif

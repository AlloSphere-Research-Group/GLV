/*
 *  Lua_Udata_Slider.h
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/26/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_UDATA_SLIDER_H
#define LUA_UDATA_SLIDER_H

#include "Lua_Udata.h"
#include "glv_core.h"
#include "glv_sliders.h"

namespace glv {

class Lua_Udata_Slider : public Slider, public Lua_Udata<Lua_Udata_Slider>
{
	public:
		Lua_Udata_Slider(int l=0, int t=0, int w=32, int h=32);
		~Lua_Udata_Slider();
		
		//override
		static void luaDraw(View *v);
		static bool luaOnGeneric(View * target, GLV * glv);
		
		//default wrappers
		static int defaultDraw(lua_State *L);
		static int defaultMouseDown(lua_State *L);
		static int defaultMouseDrag(lua_State *L);
		
		static int create(lua_State * L);

		static int getValue(lua_State * L);
		static int setValue(lua_State * L);
		
		static int getClickValue(lua_State * L);
		static int setClickValue(lua_State * L);
		
		static int getDragSensitivity(lua_State * L);
		static int setDragSensitivity(lua_State * L);
		
		static int getSliderColor(lua_State *L);
		static int setSliderColor(lua_State *L);
		
		static const luaL_reg * getMetaLib() {return lua_lib_meta;}
		static const char ** getSuperclassTable() {return superclass_table;}
	
		static const char *name;
		
	protected:
		static const luaL_reg lua_lib_meta[];
		static const char *superclass_table[];
};

}	//end namespace glv

#endif	//LUA_UDATA_SLIDER_H
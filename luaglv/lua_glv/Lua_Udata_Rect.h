/*
 *  Lua_Udata_Rect.h
 *  luaglv
 *
 *  Created by Wesley Smith on 3/9/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_UDATA_RECT_H
#define LUA_UDATA_RECT_H

#include "Lua_Udata.h"
#include "glv_core.h"

namespace glv {

class Lua_Udata_Rect : public Rect<space_t>, public Lua_Udata<Lua_Udata_Rect>
{
	public:
		Lua_Udata_Rect(int l=0, int t=0, int w=32, int h=32);
		~Lua_Udata_Rect();
		
		//lua exposure
		static int create(lua_State * L);
		
		static int getRect(lua_State * L);
		static int setRect(lua_State * L);
		
		static int getOrigin(lua_State * L);
		static int setOrigin(lua_State * L);

		static int translate(lua_State * L);
		
		static int getExtent(lua_State * L);
		static int setExtent(lua_State * L);
		
		static int getLeft(lua_State * L);
		static int setLeft(lua_State * L);
		
		static int getTop(lua_State * L);
		static int setTop(lua_State * L);
		
		static int getWidth(lua_State * L);
		static int setWidth(lua_State * L);
		
		static int getHeight(lua_State * L);
		static int setHeight(lua_State * L);
		
		static int getBottom(lua_State * L);
		static int setBottom(lua_State * L);
		
		static int getRight(lua_State * L);
		static int setRight(lua_State * L);
		
		static int resizeLeftTo(lua_State * L);
		static int resizeTopTo(lua_State * L);
		static int resizeRightTo(lua_State * L);
		static int resizeBottomTo(lua_State * L);
		static int resizeEdgesBy(lua_State * L);
		
		static int fixNegativeExtent(lua_State * L);
		static int transpose(lua_State * L);
		
		static int area(lua_State * L);
		static int centerX(lua_State * L);
		static int centerY(lua_State * L);

		//static member variables
		static const luaL_reg * getMetaLib() {return lua_lib_meta;}
		static const char ** getSuperclassTable() {return superclass_table;}
		static const char *name;
		
	protected:
		static const luaL_reg lua_lib_meta[];
		static const char *superclass_table[];
};

}	//end namespace glv

#endif	//LUA_UDATA_RECT_H
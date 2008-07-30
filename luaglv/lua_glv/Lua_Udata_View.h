/*
 *  Lua_Udata_View.h
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/26/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_UDATA_VIEW_H
#define LUA_UDATA_VIEW_H

#include "Lua_Udata.h"
#include "glv_core.h"

namespace glv {

class Lua_Udata_View : public View, public Lua_Udata<Lua_Udata_View>
{
	public:
		Lua_Udata_View(int l=0, int t=0, int w=32, int h=32);
		~Lua_Udata_View();

		//override
		static void luaDraw(View *v);
		
		//lua exposure
		static int create(lua_State * L);
		static int add(lua_State * L);
		static int remove(lua_State * L);
		static int view(lua_State * L);
		static int notify(lua_State * L);
		static int attachHandler(lua_State * L);
		
		static int setBackColor(lua_State *L);
		static int getBackColor(lua_State *L);
		
		static int setBorderColor(lua_State *L);
		static int getBorderColor(lua_State *L);
		
		static int move(lua_State *L);
		static int scroll(lua_State *L);
		/*static int getFocus(lua_State *L);*/
		
		//callbacks
		static bool luaOnGeneric(View * target, GLV * glv);

		//static member variables
		static const luaL_reg * getMetaLib() {return lua_lib_meta;}
		static const char ** getSuperclassTable() {return superclass_table;}
		static const char *name;
		
	protected:
		static const luaL_reg lua_lib_meta[];
		static const char *superclass_table[];
};

}	//end namespace glv

#endif	//LUA_UDATA_VIEW_H
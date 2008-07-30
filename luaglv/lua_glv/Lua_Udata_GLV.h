/*
 *  Lua_Udata_GLV.h
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/26/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_UDATA_GLV_H
#define LUA_UDATA_GLV_H

#include "Lua_Udata.h"
#include "glv_core.h"

namespace glv {

class Lua_Udata_GLV : public GLV, public Lua_Udata<Lua_Udata_GLV>
{
	public:
		Lua_Udata_GLV();
		~Lua_Udata_GLV();
		
		static int create(lua_State * L);
		static int mousePos(lua_State * L);
		static int mousePosRel(lua_State * L);
		static int mouseDelta(lua_State * L);
		static int drawGLV(lua_State * L);
		static int setMouseEvent(lua_State * L);
		
		static int shift(lua_State * L);
		
		static const luaL_reg * getMetaLib() {return lua_lib_meta;}
		static const char ** getSuperclassTable() {return superclass_table;}
		static const char *name;
		void drawGLV(unsigned int w, unsigned int h);
	protected:
		static const luaL_reg lua_lib_meta[];
		static const char *superclass_table[];
};

}	//end namespace glv

#endif	//LUA_UDATA_GLV_H
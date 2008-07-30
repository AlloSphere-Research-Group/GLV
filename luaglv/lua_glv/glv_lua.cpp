/*
 *  glv_lua.cpp
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/28/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
 
#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "glv_lua.h"

using namespace glv;


typedef struct _glv_str_value {
  const char *str;
  GLenum value;
} t_glv_str_valie;


static const t_glv_str_valie glv_str[] = {
	{"Null",			Event::Null},
	
	// core events
	{"Quit",			Event::Quit},
	
	// view events
	{"GetFocus",		Event::GetFocus},
	{"LoseFocus",		Event::LoseFocus},
	
	// mouse events
	{"MouseDown",		Event::MouseDown},
	{"MouseUp",			Event::MouseUp},
	{"MouseMove",		Event::MouseMove},
	{"MouseDrag",		Event::MouseDrag},
	{"MouseWheel",		Event::MouseWheel},
	
	// keyboard events
	{"KeyDown",			Event::KeyDown},
	{"KeyUp",			Event::KeyUp},
	{"KeyRepeat",		Event::KeyRepeat},
	
	// case else
	{"Unused",			Event::Unused},
	{NULL, NULL}
};

void opentable(lua_State *L)
{
	Lua_GLVData :: initData();	//init luaglv data!
	
	Lua_Udata_Rect::init_lua_udata(L);
	Lua_Udata_View::init_lua_udata(L);
	Lua_Udata_GLV::init_lua_udata(L);
	Lua_Udata_Slider::init_lua_udata(L);
	Lua_Udata_Notify::init_lua_udata(L);
}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_glv (lua_State *L)
{
	lua_newtable(L);
	lua_setglobal(L, "glv");
	opentable(L);
	
	lua_getglobal(L, "glv");
	
	luaL_newmetatable(L, GLV_LIB_META);
	
	//init instance counting field used in __gc for tracking 
	//multiple instances of the same variable
	lua_pushstring(L, GLV_INSTANCES_METAFIELD);
	lua_newtable(L);
	
	if(luaL_newmetatable(L, GLV_INSTANCES_META))
	{
		//make the instances table a weak table
		//(should make instances table its own meta instead)
		lua_pushstring(L, "__mode");
		lua_pushstring(L, "v");
		lua_settable(L, -3);
	}
	
	lua_setmetatable(L, -2);
	lua_settable(L, -3);
	lua_setmetatable(L, -2);
	
	//put string->enum table in the glv
	lua_pushstring(L, "enum");		//maybe put in registry??
	lua_newtable(L);
	
	int i=0;
	while(glv_str[i].str) {
		lua_pushstring(L, glv_str[i].str);
		lua_pushinteger(L, glv_str[i].value);
		lua_settable(L, -3);
		i++;
	}
	lua_settable(L, -3);

	lua_pushstring(L, GLV_NOTIFICATION_TABLE);
	Lua_Udata_Notify::create(L);
	lua_settable(L, LUA_REGISTRYINDEX);
	
	return 1;
}

#ifdef __cplusplus
}
#endif
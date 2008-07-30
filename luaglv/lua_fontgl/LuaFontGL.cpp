/*
 *  LuaFontGL.cpp
 *  MiniGLV
 *
 *  Created by Wesley Smith on 3/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "LuaFontGL.h"

#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }
#define LUA_FONTGL_METATABLE			"FontGL_Meta"

const luaL_reg LuaFontGL::luaFontGL_lib[] = 
{
	TABLE_ENTRY(luaFontGL, new),
	TABLE_ENTRY(luaFontGL, initialize),
	TABLE_ENTRY(luaFontGL, close),
	{NULL, NULL}
};

const luaL_reg LuaFontGL::luaFontGL_meta[] = 
{
	TABLE_ENTRY(luaFontGL, render),
	TABLE_ENTRY(luaFontGL, advance),
	TABLE_ENTRY(luaFontGL, ascender),
	TABLE_ENTRY(luaFontGL, descender),
	TABLE_ENTRY(luaFontGL, size),
	TABLE_ENTRY(luaFontGL, name),
	TABLE_ENTRY(luaFontGL, setColor),
	TABLE_ENTRY(luaFontGL, getColor),
	TABLE_ENTRY(luaFontGL, __index),
	TABLE_ENTRY(luaFontGL, __gc),
	TABLE_ENTRY(luaFontGL, __tostring),
	{NULL, NULL}
};

LuaFontGL :: LuaFontGL(char *fontName, int fontSize, bool antiAliased, bool cacheGlyphs) :
			FontGL(fontName, fontSize, antiAliased, cacheGlyphs)
{
	strcpy(mFontName, fontName);
}

LuaFontGL :: ~LuaFontGL()
{
	//nothing for now
}

LuaFontGL * LuaFontGL :: toFontGL(lua_State *L, int index)
{
	LuaFontGL **p_obj = (LuaFontGL **)luaL_checkudata(L, index, LUA_FONTGL_METATABLE);
	return *p_obj;
}

int LuaFontGL :: luaFontGL_new(lua_State *L)
{
	if(lua_type(L, 1) != LUA_TSTRING)
		luaL_error(L, " argument 1 of fontgl.new must be a string");
	
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 of fontgl.new must be a number");
	
	if(!lua_isboolean(L, 3))
		luaL_error(L, " argument 3 of fontgl.new must be a boolean");
	
	LuaFontGL **p_obj = (LuaFontGL **)lua_newuserdata(L, sizeof(LuaFontGL*));
	
	const char *constFontName = lua_tostring(L, 1);
	char fontName[256];
	strcpy(fontName, constFontName);
	
	if(lua_isboolean(L, 4))
		*p_obj = new LuaFontGL(fontName, lua_tonumber(L, 2), lua_toboolean(L, 3), lua_toboolean(L, 4));
	else
		*p_obj = new LuaFontGL(fontName, lua_tonumber(L, 2), lua_toboolean(L, 3));

	luaL_getmetatable(L, LUA_FONTGL_METATABLE);
	lua_setmetatable(L, -2);
	
	return 1;
}

int LuaFontGL :: luaFontGL_render(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	
	for(int i=2; i < 5; i++) {
		if(!lua_isnumber(L, i)) {
			luaL_error(L, " argument %d for fontgl.render must be a number", i);
		}
	}
	
	if(lua_type(L, 5) != LUA_TSTRING)
		luaL_error(L, " argument 5 for fontgl.render must be a string");
	
	font->render(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tostring(L, 5));
	
	return 0;
}

int LuaFontGL :: luaFontGL_advance(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	
	if(lua_type(L, 2) != LUA_TSTRING)
		luaL_error(L, " argument 2 for fontgl.advance must be a string");

	lua_pushnumber(L, font->advance(lua_tostring(L, 2)));
	
	return 1;
}

int LuaFontGL :: luaFontGL_ascender(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	lua_pushnumber(L, font->ascender());
	
	return 1;
}

int LuaFontGL :: luaFontGL_descender(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	lua_pushnumber(L, font->descender());
	
	return 1;
}

int LuaFontGL :: luaFontGL_size(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	lua_pushnumber(L, font->size());
	
	return 1;
}

int LuaFontGL :: luaFontGL_name(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	lua_pushstring(L, font->getFontName());
	
	return 1;
}

int LuaFontGL :: luaFontGL_setColor(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);

	for(int i=2; i < 6; i++) {
		if(!lua_isnumber(L, i)) {
			luaL_error(L, " argument %d for fontgl.setColor must be a number", i);
		}
	}

	font->setColor(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	
	return 0;
}

int LuaFontGL :: luaFontGL_getColor(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	const FontGLColor *color = font->getColor();
	
	lua_pushnumber(L, color->r);
	lua_pushnumber(L, color->g);
	lua_pushnumber(L, color->b);
	lua_pushnumber(L, color->a);
	
	return 4;
}

int LuaFontGL :: luaFontGL_initialize(lua_State *L)
{
	FontGL_InitializeLibrary();
	return 0;
}

int LuaFontGL :: luaFontGL_close(lua_State *L)
{
	FontGL_CloseLibrary();
	return 0;
}

int LuaFontGL :: luaFontGL___index(lua_State *L)
{
	lua_getmetatable(L, 1);		//get userdata metatable
	lua_pushvalue(L, 2);	
	lua_gettable(L, -2);		//check metatable for field

	return 1;
}

int LuaFontGL :: luaFontGL___gc(lua_State *L)
{
	LuaFontGL **p_obj = (LuaFontGL **)lua_touserdata(L, 1);
	if(p_obj && *p_obj)
		delete *p_obj;

	return 0;
}

int LuaFontGL :: luaFontGL___tostring(lua_State *L)
{
	LuaFontGL *font = toFontGL(L, 1);
	lua_pushfstring(L, "%s %d\n", font->getFontName(), font->size());
	
	return 1;
}

#ifdef __cplusplus
extern "C" {
#endif

extern int luaopen_fontgl(lua_State *L)
{
	luaL_openlib(L, "fontgl", LuaFontGL::luaFontGL_lib, 0);
	luaL_newmetatable(L, LUA_FONTGL_METATABLE);
	
	int i=0;
	while(LuaFontGL::luaFontGL_meta[i].name) {
		lua_pushstring(L, LuaFontGL::luaFontGL_meta[i].name);
		lua_pushcfunction(L, LuaFontGL::luaFontGL_meta[i].func);
		lua_settable(L, -3);
		i++;
	}
	
	lua_pop(L, 1);	//pop metatable from stack
	
	return 1;
}

#ifdef __cplusplus
}
#endif
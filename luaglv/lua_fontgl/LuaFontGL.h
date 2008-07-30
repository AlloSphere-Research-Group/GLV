/*
 *  LuaFontGL.h
 *  MiniGLV
 *
 *  Created by Wesley Smith on 3/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_FONT_GL_H
#define LUA_FONT_GL_H

#include "FontGL.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

extern int luaopen_fontgl(lua_State *L);

#ifdef __cplusplus
}
#endif


class LuaFontGL : public FontGL
{
	public:
			LuaFontGL(char *fontName, int fontSize, bool antiAliased, bool cacheGlyphs = false);
			~LuaFontGL();
			
			static int luaFontGL___index(lua_State *L);
			static int luaFontGL___gc(lua_State *L);
			static int luaFontGL___tostring(lua_State *L);
			
			static int luaFontGL_new(lua_State *L);
			static int luaFontGL_render(lua_State *L);
			static int luaFontGL_advance(lua_State *L);
			static int luaFontGL_ascender(lua_State *L);
			static int luaFontGL_descender(lua_State *L);
			static int luaFontGL_size(lua_State *L);
			static int luaFontGL_name(lua_State *L);
			static int luaFontGL_setColor(lua_State *L);
			static int luaFontGL_getColor(lua_State *L);
			
			static int luaFontGL_initialize(lua_State *L);
			static int luaFontGL_close(lua_State *L);
			
			static LuaFontGL * toFontGL(lua_State *L, int index);
			
			static const luaL_reg luaFontGL_lib[];
			static const luaL_reg luaFontGL_meta[];
			
			const char * getFontName() {return mFontName;}
			const FontGLColor * getColor() {return color;}
			
	private:
			char mFontName[256];
			
};


#endif //LUA_FONT_GL_H
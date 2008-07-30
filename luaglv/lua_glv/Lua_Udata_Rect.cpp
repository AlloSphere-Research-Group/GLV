/*
 *  Lua_Udata_Rect.cpp
 *  luaglv
 *
 *  Created by Wesley Smith on 3/9/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Lua_Udata_Rect.h"

using namespace glv;

const char * Lua_Udata_Rect :: name = "view";

const luaL_reg Lua_Udata_Rect :: lua_lib_meta[] = 
{
	{"new", create},
	
	{"getRect", getRect},		{"setRect", setRect},		
	{"getOrigin", getOrigin},	{"setOrigin", setOrigin},
	{"translate", translate},
	{"getExtent", getExtent},	{"setExtent", setExtent},
	{"getLeft", getLeft},		{"setLeft", setLeft},
	{"getTop", getTop},			{"setTop", setTop},
	{"getWidth", getWidth},		{"setWidth", setWidth},
	{"getHeight", getHeight},	{"setHeight", setHeight},
	{"getBottom", getBottom},	{"setBottom", setBottom},
	{"getRight", getRight},		{"setRight", setRight},
	{"resizeLeftTo", resizeLeftTo},
	{"resizeTopTo", resizeTopTo},
	{"resizeRightTo", resizeRightTo},
	{"resizeBottomTo", resizeBottomTo},
	{"resizeEdgesBy", resizeEdgesBy},
	{"fixNegativeExtent", fixNegativeExtent},
	{"transpose", transpose},
	{"area", area},
	{"centerX", centerX},
	{"centerY", centerY},
	
	{NULL, NULL}
};

const char * Lua_Udata_Rect::superclass_table[] = {NULL};

Lua_Udata_Rect :: Lua_Udata_Rect(int l, int t, int w, int h) :
				Rect<space_t>(l, t, w, h)
{
	//nothing for now
}

Lua_Udata_Rect :: ~Lua_Udata_Rect()
{
	if(mL)
		null_udata(mL, this);
}

int Lua_Udata_Rect :: create(lua_State * L)
{
	int l, t, w, h;
	l = lua_isnumber(L, 1) ? lua_tointeger(L, 1) : 0;
	t = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : 0;
	w = lua_isnumber(L, 3) ? lua_tointeger(L, 3) : 32;
	h = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 32;
	
	udata_push(L, new Lua_Udata_Rect(l, t, w, h));
		
	return 1;					// return the userdata
}

int Lua_Udata_Rect :: getRect(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->left());
	lua_pushnumber(L, self->top());
	lua_pushnumber(L, self->width());
	lua_pushnumber(L, self->height());
	return 4;
}

int Lua_Udata_Rect :: setRect(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setRect must be a number");
		
	if(!lua_isnumber(L, 3))
		luaL_error(L, " argument 3 for rect.setRect must be a number");

	if(!lua_isnumber(L, 4))
		luaL_error(L, " argument 4 for rect.setRect must be a number");
		
	if(!lua_isnumber(L, 5))
		luaL_error(L, " argument 5 for rect.setRect must be a number");

	self->setRect(lua_tonumber(L, 2), lua_tonumber(L, 3),
				lua_tonumber(L, 4), lua_tonumber(L, 5));
	return 0;
}

int Lua_Udata_Rect :: getOrigin(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->left());
	lua_pushnumber(L, self->top());
	return 2;
}

int Lua_Udata_Rect :: setOrigin(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setOrigin must be a number");
		
	if(!lua_isnumber(L, 3))
		luaL_error(L, " argument 3 for rect.setOrigin must be a number");

	self->origin(lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

int Lua_Udata_Rect :: translate(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.translate must be a number");
		
	if(!lua_isnumber(L, 3))
		luaL_error(L, " argument 3 for rect.translate must be a number");

	self->translate(lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

int Lua_Udata_Rect :: getExtent(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->width());
	lua_pushnumber(L, self->height());
	
	return 2;
}

int Lua_Udata_Rect :: setExtent(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setExtent must be a number");
		
	if(!lua_isnumber(L, 3))
		luaL_error(L, " argument 3 for rect.setExtent must be a number");

	self->extent(lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

int Lua_Udata_Rect :: getLeft(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->left());
		
	return 1;
}

int Lua_Udata_Rect :: setLeft(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setLeft must be a number");
		
	self->left(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: getTop(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->top());
		
	return 1;
}

int Lua_Udata_Rect :: setTop(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setTop must be a number");
		
	self->top(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: getWidth(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->width());

	return 1;
}

int Lua_Udata_Rect :: setWidth(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setWidth must be a number");
		
	self->width(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: getHeight(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->height());

	return 1;
}

int Lua_Udata_Rect :: setHeight(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setHeight must be a number");
		
	self->height(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: getBottom(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->bottom());
	return 1;
}

int Lua_Udata_Rect :: setBottom(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setBottom must be a number");
		
	self->bottom(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: getRight(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->right());
	return 1;
}

int Lua_Udata_Rect :: setRight(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.setRight must be a number");
		
	self->right(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: resizeLeftTo(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.resizeLeftTo must be a number");
		
	self->resizeLeftTo(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: resizeTopTo(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.resizeTopTo must be a number");
		
	self->resizeTopTo(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: resizeRightTo(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.resizeRightTo must be a number");
		
	self->resizeRightTo(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: resizeBottomTo(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.resizeBottomTo must be a number");
		
	self->resizeBottomTo(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: resizeEdgesBy(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	if(!lua_isnumber(L, 2))
		luaL_error(L, " argument 2 for rect.resizeEdgesBy must be a number");
		
	self->resizeEdgesBy(lua_tonumber(L, 2));
	return 0;
}

int Lua_Udata_Rect :: fixNegativeExtent(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	self->fixNegativeExtent();
	return 0;
}

int Lua_Udata_Rect :: transpose(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	self->transpose();
	return 0;
}

int Lua_Udata_Rect :: area(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->area());
	return 1;
}

int Lua_Udata_Rect :: centerX(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->centerX());
	return 1;
}

int Lua_Udata_Rect :: centerY(lua_State * L)
{
	Rect<space_t> * self = to_udata(L, 1);
	lua_pushnumber(L, self->centerY());
	return 1;
}
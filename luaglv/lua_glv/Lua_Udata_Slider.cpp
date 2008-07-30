/*
 *  Lua_Udata_Slider.cpp
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/26/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Lua_Udata_Slider.h"
#include "Lua_Udata_Rect.h"
#include "Lua_Udata_View.h"
#include "Lua_Udata_GLV.h"

using namespace glv;

const char * Lua_Udata_Slider :: name = "slider";

const luaL_reg Lua_Udata_Slider :: lua_lib_meta[] = 
{
	{"new", create},
	
	{"getValue", getValue},				{"setValue", setValue},
	{"getClickValue", getClickValue},	{"setClickValue", setClickValue},
	{"getDragSensitivity", getDragSensitivity}, {"setDragSensitivity", setDragSensitivity},
	
	{"defaultDraw", defaultDraw},
	{"defaultMouseDown", defaultMouseDown},
	{"defaultMouseDrag", defaultMouseDrag},
	{"setSliderColor", setSliderColor},
	{"getSliderColor", getSliderColor},
	{NULL, NULL}
};

template <>
const char * Lua_Udata_Slider::superclass_table[] = {Lua_Udata_Rect::name,
													Lua_Udata_View::name,
													NULL};

Lua_Udata_Slider :: Lua_Udata_Slider(int l, int t, int w, int h) :
				Slider(l, t, w, h)
{
	draw = luaDraw;

	std::map<std::string, Event::type>::iterator iter;
	for( iter = Lua_GLVData::eventNameValues.begin(); iter != Lua_GLVData::eventNameValues.end(); iter++ )
	{
		on(iter->second, luaOnGeneric);
	}
}

Lua_Udata_Slider :: ~Lua_Udata_Slider()
{
	if(mL)
		null_udata(mL, this);
}

int Lua_Udata_Slider :: create(lua_State * L)
{
	int l, t, w, h;
	l = lua_isnumber(L, 1) ? lua_tointeger(L, 1) : 0;
	t = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : 0;
	w = lua_isnumber(L, 3) ? lua_tointeger(L, 3) : 32;
	h = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 32;
	
	udata_push(L, new Lua_Udata_Slider(l, t, w, h));

	lua_getfenv(L, -1);
	lua_pushstring(L, Lua_GLVData::getEventName(Event::MouseDown));
	lua_pushcfunction(L, defaultMouseDown);
	lua_settable(L, -3);
	
	lua_pushstring(L, Lua_GLVData::getEventName(Event::MouseDrag));
	lua_pushcfunction(L, defaultMouseDrag);
	lua_settable(L, -3);
	lua_pop(L, 1);
	
	return 1;					// return the userdata
}

int Lua_Udata_Slider :: setValue(lua_State * L)
{
	if(!lua_isnumber(L, 2))
		luaL_error(L, "argument 2 for slider.setValue must be a number");
	
	Slider *s = to_udata(L, 1);
	
	if(s)
		s->setValue(lua_tonumber(L, 2));
		
	return 0;
}

int Lua_Udata_Slider :: getValue(lua_State * L)
{
	Slider *s = to_udata(L, 1);
	
	if(s)
		lua_pushnumber(L, s->getValue());
	else
		lua_pushnil(L);
		
	return 1;
}

int Lua_Udata_Slider :: setClickValue(lua_State * L)
{
	if(!lua_isnumber(L, 2))
		luaL_error(L, "argument 2 for slider.setClickValue must be a number");
	
	Slider *s = to_udata(L, 1);
	
	if(s)
		s->setClickValue(lua_tonumber(L, 2));
		
	return 0;
}

int Lua_Udata_Slider :: getClickValue(lua_State * L)
{
	Slider *s = to_udata(L, 1);
	
	if(s)
		lua_pushnumber(L, s->getClickValue());
	else
		lua_pushnil(L);
		
	return 1;
}

int Lua_Udata_Slider :: setDragSensitivity(lua_State * L)
{
	if(!lua_isnumber(L, 2))
		luaL_error(L, "argument 2 for slider.setClickValue must be a number");
	
	Slider *s = to_udata(L, 1);
	
	if(s)
		s->setDragSensitivity(lua_tonumber(L, 2));
		
	return 0;
}

int Lua_Udata_Slider :: getDragSensitivity(lua_State * L)
{
	Slider *s = to_udata(L, 1);
	
	if(s)
		lua_pushnumber(L, s->getDragSensitivity());
	else
		lua_pushnil(L);
		
	return 1;
}

void Lua_Udata_Slider :: luaDraw(View *v)
{
	Lua_Udata_Slider *lua_v = (Lua_Udata_Slider *)v;
	lua_State *L = lua_v->getState();
	instance_to_userdata(L, lua_v);
	lua_getfenv(L, -1);
	lua_pushstring(L, "Draw");
	lua_gettable(L, -2);
	
	if(lua_isnil(L, -1)) {
		lua_pop(L, 3);
		Slider::onDraw(v);
	}
	else {
		lua_pushvalue(L, -3);
		lua_call(L, 1, 0);
		lua_pop(L, 2);
	}
}

bool Lua_Udata_Slider :: luaOnGeneric(View * target, GLV * glv)
{
	Lua_Udata_Slider *luaTarget = (Lua_Udata_Slider *)target;
	lua_State *L = luaTarget->getState();
	instance_to_userdata(L, luaTarget);

	//do shit
	lua_getfenv(L, -1);
	lua_pushstring(L, Lua_GLVData::getEventName(glv->eventtype));
	lua_gettable(L, -2);
	
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);
	}
	else {
		lua_pushvalue(L, -3);
		Lua_Udata<Lua_Udata_GLV>::instance_to_userdata(L, (Lua_Udata_GLV *)glv);
		lua_call(L, 2, 0);
	}
	
	lua_pop(L, 2);	//pop userdata, env table
}

int Lua_Udata_Slider :: defaultDraw(lua_State *L)
{
	View *v = Lua_Udata<Lua_Udata_View>::to_udata(L, 1);
	Slider::onDraw(v);
	
	return 0;
}

int Lua_Udata_Slider :: defaultMouseDown(lua_State *L)
{
	View *v = Lua_Udata<Lua_Udata_View>::to_udata(L, 1);
	GLV *glv = Lua_Udata<Lua_Udata_GLV>::to_udata(L, 2);
	Slider::sliderMouseDown(v, glv);
	
	return 0;
}

int Lua_Udata_Slider :: defaultMouseDrag(lua_State *L)
{
	View *v = Lua_Udata<Lua_Udata_View>::to_udata(L, 1);
	GLV *glv = Lua_Udata<Lua_Udata_GLV>::to_udata(L, 2);
	Slider::sliderMouseDrag(v, glv);
	
	return 0;
}

int Lua_Udata_Slider :: setSliderColor(lua_State *L)
{
	Slider * self = to_udata(L, 1);
	int num_args = lua_gettop(L);
	
	switch(num_args)
	{
		case 5:
				self->sliderColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3),
								lua_tonumber(L, 4), lua_tonumber(L, 5));
				break;
		
		case 4:
				self->sliderColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3),
								lua_tonumber(L, 4));
				break;
				
		case 3:
				self->sliderColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3));
				break;
		
		case 2:
				self->sliderColor->set(lua_tonumber(L, 2));
				break;
		
		case 1:
				luaL_error(L, " view.backgColor requires 2 or more arguments");
				break;
	}	
	
	return 0;
}

//may change to a table!
int Lua_Udata_Slider :: getSliderColor(lua_State *L)
{
	Slider * self = to_udata(L, 1);
	lua_pushnumber(L, self->sliderColor->r);
	lua_pushnumber(L, self->sliderColor->g);
	lua_pushnumber(L, self->sliderColor->b);
	lua_pushnumber(L, self->sliderColor->a);
	
	return 4;
}
/*
 *  Lua_Udata_View.cpp
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/26/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Lua_Udata_View.h"
#include "Lua_Udata_Rect.h"
#include "Lua_Udata_GLV.h"
#include "Lua_Udata_Notify.h"

using namespace glv;

const char * Lua_Udata_View :: name = "view";

const luaL_reg Lua_Udata_View :: lua_lib_meta[] = 
{
	{"new", create},
	{"add", add},
	{"remove", remove},
	{"attachHandler", attachHandler},
	{"notify", Lua_Udata_View::notify},

	{"setBackColor", setBackColor},
	{"getBackColor", getBackColor},
	
	{"setBorderColor", setBorderColor},	
	{"getBorderColor", getBorderColor},
	
	{"move", move},
	{"scroll", scroll},
	/*{"getFocus", getFocus},*/
	{NULL, NULL}
};

const char * Lua_Udata_View::superclass_table[] =  {Lua_Udata_Rect::name,
													NULL};

Lua_Udata_View :: Lua_Udata_View(int l, int t, int w, int h) :
				View(l, t, w, h)
{
	draw = luaDraw;
	
	std::map<std::string, Event::type>::iterator iter;
	for( iter = Lua_GLVData::eventNameValues.begin(); iter != Lua_GLVData::eventNameValues.end(); iter++ )
	{
		on(iter->second, luaOnGeneric);
	}
}

Lua_Udata_View :: ~Lua_Udata_View()
{
	if(mL)
		null_udata(mL, this);
}

void Lua_Udata_View :: luaDraw(View *v)
{
	Lua_Udata_View *lua_v = (Lua_Udata_View *)v;
	lua_State *L = lua_v->getState();
	instance_to_userdata(L, lua_v);
	lua_getfenv(L, -1);
	lua_pushstring(L, "Draw");
	lua_gettable(L, -2);
	
	if(lua_isnil(L, -1)) {
		lua_pop(L, 3);
		View::defaultDraw(v);
	}
	else {
		lua_pushvalue(L, -3);
		lua_call(L, 1, 0);
		lua_pop(L, 2);
	}
}

int Lua_Udata_View :: create(lua_State * L)
{
	int l, t, w, h;
	l = lua_isnumber(L, 1) ? lua_tointeger(L, 1) : 0;
	t = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : 0;
	w = lua_isnumber(L, 3) ? lua_tointeger(L, 3) : 32;
	h = lua_isnumber(L, 4) ? lua_tointeger(L, 4) : 32;
	
	udata_push(L, new Lua_Udata_View(l, t, w, h));
		
	return 1;					// return the userdata
}

int Lua_Udata_View :: add(lua_State * L)
{
	View * self = to_udata(L, 1);
	View * target = to_udata(L, 2);
	if (self && target) {
		self->add(target);
	}
	
	return 0;
}

int Lua_Udata_View :: remove(lua_State * L)
{
	View * self = to_udata(L, 1);
	if (self) {
		self->remove();
	}
	
	return 0;
}

int Lua_Udata_View :: attachHandler(lua_State * L)
{
	View * self = to_udata(L, 1);

	if (self) {
		if(lua_isfunction(L, 2)) {
			lua_pushstring(L, GLV_NOTIFICATION_TABLE);
			lua_gettable(L, LUA_REGISTRYINDEX);
			
			Lua_Udata_Notify *notifier = Lua_Udata_Notify::to_udata(L, -1);
			self->attachHandler(Lua_Udata_Notify::notify, notifier);
			notifier->setAsHandler(self, 3, 2);
			
			lua_pop(L, 1);		//pop notify userdata
		}
		else {
			luaL_error(L, " argument 2 to view.attachHandler must be a function");
		}
	}
	
	return 0;
}

int Lua_Udata_View :: notify(lua_State * L)
{
	View * self = to_udata(L, 1);
	
	if (self) {
		self->notify();
	}
	
	return 0;
}

bool Lua_Udata_View :: luaOnGeneric(View * target, GLV * glv)
{
	Lua_Udata_View *luaTarget = (Lua_Udata_View *)target;
	lua_State *L = luaTarget->getState();
	instance_to_userdata(L, luaTarget);

	//do shit
	lua_getfenv(L, -1);
	lua_pushstring(L,  Lua_GLVData::getEventName(glv->eventtype));
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

int Lua_Udata_View :: setBackColor(lua_State *L)
{
	View * self = to_udata(L, 1);
	int num_args = lua_gettop(L);
	switch(num_args)
	{
		case 5:
				self->backColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3),
								lua_tonumber(L, 4), lua_tonumber(L, 5));
				break;
		
		case 4:
				self->backColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3),
								lua_tonumber(L, 4));
				break;
				
		case 3:
				self->backColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3));
				break;
		
		case 2:
				self->backColor->set(lua_tonumber(L, 2));
				break;
		
		case 1:
				luaL_error(L, " view.backColor requires 2 or more arguments");
				break;
	}
	
	return 0;
}

//may change to a table!
int Lua_Udata_View :: getBackColor(lua_State *L)
{
	View * self = to_udata(L, 1);
	lua_pushnumber(L, self->backColor->r);
	lua_pushnumber(L, self->backColor->g);
	lua_pushnumber(L, self->backColor->b);
	lua_pushnumber(L, self->backColor->a);
	
	return 4;
}

int Lua_Udata_View :: setBorderColor(lua_State *L)
{
	View * self = to_udata(L, 1);
	int num_args = lua_gettop(L);
	
	switch(num_args)
	{
		case 5:
				self->borderColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3),
								lua_tonumber(L, 4), lua_tonumber(L, 5));
				break;
		
		case 4:
				self->borderColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3),
								lua_tonumber(L, 4));
				break;
				
		case 3:
				self->borderColor->set(lua_tonumber(L, 2), lua_tonumber(L, 3));
				break;
		
		case 2:
				self->borderColor->set(lua_tonumber(L, 2));
				break;
		
		case 1:
				luaL_error(L, " view.backgColor requires 2 or more arguments");
				break;
	}	
	
	return 0;
}

int Lua_Udata_View :: getBorderColor(lua_State *L)
{
	View * self = to_udata(L, 1);
	lua_pushnumber(L, self->borderColor->r);
	lua_pushnumber(L, self->borderColor->g);
	lua_pushnumber(L, self->borderColor->b);
	lua_pushnumber(L, self->borderColor->a);
	
	return 4;
}

int Lua_Udata_View :: move(lua_State *L)
{
	View * self = to_udata(L, 1);
	self->move(lua_tonumber(L, 2), lua_tonumber(L, 3));
	
	return 0;
}

int Lua_Udata_View :: scroll(lua_State *L)
{
	View * self = to_udata(L, 1);
	self->scroll(lua_tonumber(L, 2), lua_tonumber(L, 3));
	
	return 0;
}

/*
int Lua_Udata_View :: getFocus(lua_State *L)
{
	View * self = to_udata(L, 1);
	
	lua_pushboolean(L, self->hasFocus);
	return 1;
}*/
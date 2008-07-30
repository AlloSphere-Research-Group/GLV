/*
 *  Lua_Udata_GLV.cpp
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/26/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */


#include "Lua_Udata_GLV.h"
#include "Lua_Udata_Rect.h"
#include "Lua_Udata_View.h"
#include "Lua_Udata_Slider.h"

using namespace glv;

const char * Lua_Udata_GLV :: name = "glv";

const luaL_reg Lua_Udata_GLV :: lua_lib_meta[] = 
{
	{"new", create},
	{"mousePos", mousePos},
	{"mousePosRel", mousePosRel},
	{"mouseDelta", mouseDelta},
	{"drawGLV", drawGLV},
	{"setMouseEvent", setMouseEvent},
	
	{"shift", shift},
	{NULL, NULL}
};

const char * Lua_Udata_GLV::superclass_table[] = {	Lua_Udata_Rect::name,
													Lua_Udata_View::name,
													NULL};

Lua_Udata_GLV :: Lua_Udata_GLV() : GLV()
{
	backColor = new Color(0., 0., 0., 0.);
	clearColor = &Color::black;
}

Lua_Udata_GLV :: ~Lua_Udata_GLV()
{
	if(mL)
		null_udata(mL, this);
}

int Lua_Udata_GLV :: create(lua_State * L)
{	
	udata_push(L, new Lua_Udata_GLV());
	return 1;					// return the userdata
}

int Lua_Udata_GLV :: drawGLV(lua_State * L)
{
	GLV * self = to_udata(L, 1);
	self->drawGLV(lua_tonumber(L, 2), lua_tonumber(L, 3));
	
	return 0;
}

void Lua_Udata_GLV :: drawGLV(unsigned int w, unsigned int h)
{
	preamble(w, h);
	
	Lua_Udata_GLV *lua_glv = (Lua_Udata_GLV *)this;
	lua_State *L = lua_glv->getState();
	instance_to_userdata(L, lua_glv);
	lua_getfenv(L, -1);
	lua_pushstring(L, "Draw3D");
	lua_gettable(L, -2);
	
	if(lua_isnil(L, -1)) {
		lua_pop(L, 3);
	}
	else {
		lua_pushvalue(L, -3);
		lua_pushinteger(L, w);
		lua_pushinteger(L, h);
		
		//setup typical starting point for drawing a 3D Scene
		glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(	0., 0., 2.,
						0., 0., 0.,
						0., 1., 0.);
			
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45., 1.33, 0.1, 100.);
			glMatrixMode(GL_MODELVIEW);

				lua_call(L, 3, 0);
		glPopMatrix();

		lua_pop(L, 2);
	}
	
	drawWidgets(w, h);
}

//need to store clicks w/ timestamp for DBL click
int Lua_Udata_GLV :: setMouseEvent(lua_State * L)
{
	GLV * self = to_udata(L, 1);
	
	const char *key = lua_tostring(L, 2);
	lua_getglobal(L, "glv");
	lua_getfield(L, -1, "enum");
	lua_getfield(L, -1, key);
	lua_remove(L, -3);
	lua_remove(L, -2);
	
	Event::type eventType = (Event::type)lua_tointeger(L, -1);
	int x = lua_tointeger(L, 3);
	int y = lua_tointeger(L, 4);
	space_t relx = (space_t)x;
	space_t rely = (space_t)y;
	
	switch(eventType)
	{
		case Event::MouseMove:
		case Event::MouseUp:
				self->setEventType(eventType);
				break;
		
		case Event::MouseDrag:
				self->setMouseDrag(relx, rely, lua_tointeger(L, 5), 1);
				break;
		
		case Event::MouseDown:
				self->setMouseDown(relx, rely, lua_tointeger(L, 5), 1);
				break;
	}
	
//	printf("%s %d %d %d\n", key, eventType, x, y);
	self->setMouseXY(x, y, relx, rely);
	self->propagateEvent();
	
	return 0;
}

int Lua_Udata_GLV :: mousePos(lua_State * L)
{
	GLV * self = to_udata(L, 1);
	
	lua_pushinteger(L, self->mouse->x());
	lua_pushinteger(L, self->mouse->y());
	
	return 2;
}

int Lua_Udata_GLV :: mousePosRel(lua_State * L)
{
	GLV * self = to_udata(L, 1);
	
	lua_pushinteger(L, self->mouse->xRel());
	lua_pushinteger(L, self->mouse->yRel());
	
	return 2;
}

int Lua_Udata_GLV :: mouseDelta(lua_State * L)
{
	GLV * self = to_udata(L, 1);
	
	lua_pushinteger(L, self->mouse->dx());
	lua_pushinteger(L, self->mouse->dy());
	
	return 2;
}

int Lua_Udata_GLV :: shift(lua_State * L)
{
	GLV * self = to_udata(L, 1);
	
	lua_pushboolean(L, self->keyboard->shift());
	
	return 1;
}
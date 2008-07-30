/*
 *  Lua_Udata_Notify.cpp
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/27/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Lua_Udata_Notify.h"

using namespace glv;

const char * Lua_Udata_Notify :: name = "notify";

const luaL_reg Lua_Udata_Notify :: lua_lib_meta[] = 
{
	{NULL, NULL}
};

const char * Lua_Udata_Notify::superclass_table[] = {NULL};

Lua_Udata_Notify :: Lua_Udata_Notify()
{
	//nothing
}

Lua_Udata_Notify :: ~Lua_Udata_Notify()
{
	if(mL)
		null_udata(mL, this);
}

int Lua_Udata_Notify :: create(lua_State * L)
{
	udata_push(L, new Lua_Udata_Notify());

	//need to make this table a weak "k" table
	//so that userdata will not be prevented from being
	//garbage collected due to it being reference in this table
	lua_getfenv(L, -1);		// get userdata environment
	lua_pushstring(L, "__notify_function");
	lua_newtable(L);
	lua_settable(L, -3);
	lua_pop(L, 1);
			
	return 1;				// return the userdata
}

//positive indices only!!
void Lua_Udata_Notify :: setAsHandler(View *sender, int notify_index, int func_ndex)
{
	lua_getfenv(mL, notify_index);		// get userdata environment
	lua_pushstring(mL, "__notify_function");
	lua_gettable(mL, -2);
	
	lua_pushinteger(mL, (int)sender);
	lua_pushvalue(mL, func_ndex);
	
	lua_settable(mL, -3);
	
	lua_pop(mL, 2);			//pop __notify_function table and environment table
}

void Lua_Udata_Notify :: notify(Notifier * sender, void * userdata)
{
	View *view = (View *)sender;
	Lua_Udata_Notify *notify = (Lua_Udata_Notify *)userdata;
	lua_State *L = notify->getState();
	instance_to_userdata(L, notify);

	if(!lua_isnil(L, -1))
	{
		lua_getfenv(L, -1);		//get userdata environment
		lua_pushstring(L, "__notify_function");
		lua_gettable(L, -2);
		
		lua_pushinteger(L, (int)view);
		lua_gettable(L, -2);
		
		lua_call(L, 0, 0);
		lua_pop(L, 3);
	}
	else
	{
		printf("void Lua_Udata_Notify :: notify ERROR\n");
	}
}
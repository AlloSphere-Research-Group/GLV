/*
 *  Lua_Udata_Notify.h
 *  MiniGLV
 *
 *  Created by Wesley Smith on 2/27/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_UDATA_NOTIFY_H
#define LUA_UDATA_NOTIFY_H

#include "Lua_Udata.h"
#include "glv_core.h"
#include <vector>

namespace glv {

typedef Notifier * Notifier_Udata;

class Lua_Udata_Notify : public Lua_Udata<Lua_Udata_Notify>
{
	public:
		Lua_Udata_Notify();
		~Lua_Udata_Notify();
		
		void setAsHandler(View *sender, int notify_index, int func_ndex);
		
		static int create(lua_State * L);
		static void notify(Notifier * sender, void * userdata);
		
		static const luaL_reg * getMetaLib() {return lua_lib_meta;}
		static const char ** getSuperclassTable() {return superclass_table;}
	
		static const char *name;
		
	protected:
		static const luaL_reg lua_lib_meta[];
		static const char *superclass_table[];
		
		//stuff needed to setup notification mechanism
		//Lua based stuff is stored in the environment table
		
		//so that we can remove ourselves later (userdata)
		//can check if it has been deleted already or not!
		std::vector<Notifier_Udata *>	mNotifierUdatas;	
		
		//Lua function to call is in the environment table under:
		//__notify_function
		
};

}	//end namespace glv

#endif	//LUA_UDATA_NOTIFY_H
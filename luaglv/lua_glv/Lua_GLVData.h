/*
 *  Lua_GLVData.h
 *  luaglv
 *
 *  Created by Wesley Smith on 3/9/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LUA_GLVDATA_G
#define LUA_GLVDATA_G

#include <map>
#include <string>
#include "glv_core.h"

namespace glv {

class Lua_GLVData
{
	public:
		static void initData();
		static Event::type getEventValue(const char *name);
		static const char * getEventName(Event::type value);
	
		static std::map<std::string, Event::type> eventNameValues;
};

}

#endif	//LUA_GLVDATA_G
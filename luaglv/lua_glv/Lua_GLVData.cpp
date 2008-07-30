/*
 *  Lua_GLVData.cpp
 *  luaglv
 *
 *  Created by Wesley Smith on 3/9/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Lua_GLVData.h"

using namespace glv;

std::map<std::string, Event::type> Lua_GLVData :: eventNameValues;

void Lua_GLVData :: initData()
{
	eventNameValues["Null"] = Event::Null;
	eventNameValues["Quit"] = Event::Quit;
	eventNameValues["GetFocus"] = Event::GetFocus;
	eventNameValues["LoseFocus"] = Event::LoseFocus;
	eventNameValues["MouseDown"] = Event::MouseDown;
	eventNameValues["MouseUp"] = Event::MouseUp;
	eventNameValues["MouseMove"] = Event::MouseMove;
	eventNameValues["MouseDrag"] = Event::MouseDrag;
	eventNameValues["MouseWheel"] = Event::MouseWheel;
	eventNameValues["KeyDown"] = Event::KeyDown;
	eventNameValues["KeyUp"] = Event::KeyUp;
	eventNameValues["KeyRepeat"] = Event::KeyRepeat;
	eventNameValues["Unused"] = Event::Unused;
}

Event::type Lua_GLVData :: getEventValue(const char *name)
{
	return eventNameValues[name];
}

const char * Lua_GLVData :: getEventName(Event::type value)
{
	return Event::getName(value);
}
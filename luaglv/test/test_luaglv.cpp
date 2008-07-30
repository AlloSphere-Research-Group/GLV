#include "glv_core.h"
#include GLV_BINDING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "LuaGL.h"
#include "LuaGlu.h"

#ifdef __cplusplus
}
#endif

#include "glv_lua.h"
#include "LuaFontGL.h"

using namespace glv;

bool moveView(View * v, GLV * glv){
	v->move(glv->mouse->dx(), glv->mouse->dy());
}

Window *win = NULL;

int create_window(lua_State *L)
{
	win = new Window();	
	return 0;
}

int addglv_towindow(lua_State *L)
{
	GLV * glv = Lua_Udata<Lua_Udata_GLV>::to_udata(L, 1);
	win->setGLV(glv);

	return 0;
}

int main (int argc, char ** argv)
{
	lua_State *L = lua_open();
	luaL_openlibs(L);
	
	luaopen_opengl(L);
	luaopen_glu(L);
	luaopen_fontgl(L);
	luaopen_glv(L);

	lua_pushcfunction(L, create_window);
	lua_setglobal(L, "create_window");
	lua_pushcfunction(L, addglv_towindow);
	lua_setglobal(L, "addglv_towindow");

	
	int err;
	err = luaL_dofile(L, "../../../test/test_luaglv.lua");
	if(err) {
		printf("lua pcall error %s\n", lua_tostring(L, -1));
		lua_pop(L, -1);
	}
	
	Application::run();		// Application is also an AL binding, but unlike Window, it is a singleton
	
	lua_close(L);
	delete win;
}
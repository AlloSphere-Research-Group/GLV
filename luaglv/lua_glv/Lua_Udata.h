#ifndef LUA_UDATA_H
#define LUA_UDATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#include "Lua_GLVData.h"



#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }

#define GLV_LIB_META			"GLV_lib_meta"
#define GLV_INSTANCES_META		"GLV_instances_meta"
#define GLV_NOTIFICATION_TABLE	"GLV_notification_singleton"
#define GLV_INSTANCES_METAFIELD	"__instances"



namespace glv {

template <class T>
class Lua_Udata
{
	typedef T *TID;
	
	public:	
		static void * check_udata(lua_State *L, int ud);
		static int check_equality(lua_State *L);
		static TID to_udata(lua_State *L, int index);
		static TID * udata_push(lua_State *L, T *obj);
		static int init_lua_udata(lua_State *L);
		static int setup_functions(lua_State *L);
		static int inherit(lua_State *L);
		
		static int lua_udata___index(lua_State *L);
		static int lua_udata___newindex(lua_State *L);
		static int lua_udata___gc(lua_State *L);
		static int lua_udata___tostring(lua_State *L);
		static int stackDump(lua_State *L);
		
		static int null_udata(lua_State *L, T *o);
		static int instance_to_userdata(lua_State *L, T *o);
		
		virtual void setState(lua_State *L) {mL = L;}
		virtual lua_State * getState() {return mL;}
		
	protected:
		static const luaL_reg lua_udata_lib_meta[];
		
		lua_State *mL;
		
};

template <class T>
void * Lua_Udata<T> :: check_udata(lua_State *L, int ud)
{
	void *p = lua_touserdata(L, ud);
	if (p != NULL && lua_getmetatable(L, ud)) {
		lua_pushlightuserdata(L, (void *)T::name);
		lua_rawget(L, -2);
		
		if(!lua_isnil(L, -1)) {
			lua_pop(L, 2);
			return p;
		}
		
		lua_pop(L, 2);
	}

	return NULL;
}

// -1 is the constant
// -2 changes based on __super
template <class T>
int Lua_Udata<T> :: check_equality(lua_State *L)
{
	//quick check of equality
	int equal = 0;
	
	lua_pushlightuserdata(L, (void *)T::name);
	lua_rawget(L, -2);
	
	if(!lua_isnil(L, -1)) {
		equal = 1;
	}
	lua_pop(L, 1);
	
	return equal;
}

template <class T>
typename Lua_Udata<T>::TID Lua_Udata<T> :: to_udata(lua_State *L, int index)
{
	TID *p_obj = (TID *)check_udata(L, index);
	if(p_obj)
		return *p_obj;
	else
		return NULL;
}

template <class T>
typename Lua_Udata<T>::TID * Lua_Udata<T> :: udata_push(lua_State *L, T *obj)
{
	if(obj)
	{
		obj->setState(L);

		luaL_getmetatable(L, GLV_LIB_META);
		
		//check instances field in metatable
		lua_pushstring(L, GLV_INSTANCES_METAFIELD);
		lua_rawget(L, -2);					//get instances table
		lua_pushinteger(L, (int)obj);
		lua_rawget(L, -2);					//get this instance's field
		
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);		//pop nil from stack			
			lua_pushinteger(L, (int)obj);
			
			T **p_obj = (T **)lua_newuserdata(L, sizeof(TID));
			*p_obj = obj;

			lua_pushvalue(L, -1);
			lua_insert(L, -4);			
			lua_rawset(L, -3);
			
			lua_pop(L, 1);					//pop instances from stack
			lua_remove(L, -2);
			
			luaL_getmetatable(L, T::name);
			lua_setmetatable(L, -2);
			
			lua_newtable(L);
			lua_setfenv(L, -2);
			
			return p_obj;
		}
		else
		{
			lua_insert(L, -3);
			lua_pop(L, 2);
			
			return (T **)luaL_checkudata(L, -1, T::name);
		}
	}
	else
	{
		lua_pushnil(L);
		return 0;
	}
}

template <class T>
int Lua_Udata<T> :: lua_udata___index(lua_State *L)
{
	lua_getfenv(L, 1);				//get userdata environment
	lua_pushvalue(L, 2);
	lua_gettable(L, -2);			//check environment table for field
	
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_getmetatable(L, 1);		//get userdata metatable
		lua_pushvalue(L, 2);	
		lua_gettable(L, -2);		//check metatable for field
	}

	return 1;
}

template <class T>
int Lua_Udata<T> :: lua_udata___newindex(lua_State * L)
{
	if (lua_isnil(L, 2)) {
		luaL_typerror(L, 2, " can't set a table with a nil key");
	}
	
	//check if enough args to set table key-value pair
	if (lua_gettop(L) == 2)
		lua_pushnil(L);
		
	lua_getfenv(L, 1);		//get userdata environment
	
	lua_pushvalue(L, 2);
	lua_pushvalue(L, 3);
	lua_settable(L, -3);
	
	return 0;
}

template <class T>
int Lua_Udata<T> :: lua_udata___gc(lua_State *L)
{
	TID o = to_udata(L, 1);
	if(o) {
		o->setState(0);
		delete o;
	}

	return 0;
}

template <class T>
int Lua_Udata<T> :: lua_udata___tostring(lua_State *L)
{
	TID o = to_udata(L, 1);
	lua_pushfstring(L, "%p %s", o, T::name);
	
	return 1;
}

template <class T>
int Lua_Udata<T> :: null_udata(lua_State *L, T *o)
{
	luaL_getmetatable(L, GLV_LIB_META);
	lua_pushstring(L, GLV_INSTANCES_METAFIELD);
	lua_rawget(L, -2);					//get instances table
	lua_pushinteger(L, (int)o);
	lua_rawget(L, -2);					//get this instance's field

	if(!lua_isnil(L, -1))
	{
		//have to null out the userdata value
		TID *p_obj = (TID *)luaL_checkudata(L, -1, T::name);
		*p_obj = NULL;
	}
	
	return 0;
}

//might be better ro rearchitect instances to be GLV_wide
//clearing stack properly?
template <class T>
int Lua_Udata<T> :: instance_to_userdata(lua_State *L, T *o)
{
	luaL_getmetatable(L, GLV_LIB_META);
	lua_pushstring(L, GLV_INSTANCES_METAFIELD);
	lua_rawget(L, -2);					//get instances table
	lua_pushinteger(L, (int)o);
	lua_rawget(L, -2);					//get this instance's field

	lua_remove(L, -2);
	lua_remove(L, -2);
	
	return 1;
}

template <class T>
const luaL_reg Lua_Udata<T>::lua_udata_lib_meta[] = 
{
	{"__index", Lua_Udata<T>::lua_udata___index},
	{"__newindex", Lua_Udata<T>::lua_udata___newindex},
	{"__gc", Lua_Udata<T>::lua_udata___gc},
	{"__tostring", Lua_Udata<T>::lua_udata___tostring},
	{NULL, NULL}
};

//copy superclass metafunctions into own metatable to avoid lookup in __index
template <class T>
int Lua_Udata<T> :: init_lua_udata(lua_State *L)
{
	lua_getglobal(L, "glv");
	luaL_newmetatable(L, T::name);
	
	//setup introspection
	lua_pushstring(L, "__name");
	lua_pushstring(L, T::name);
	lua_settable(L, -3);
	
	setup_functions(L);		//udata generic functions and class functions
	inherit(L);				//superclass functions
	
	//set as field in "glv" table
	lua_pushstring(L, T::name);
	lua_insert(L, -2);	
	lua_settable(L, -3);
	lua_pop(L, 1);
	
	return 0;
}

template <class T>
int Lua_Udata<T> :: setup_functions(lua_State *L)
{
	//fill udata metatable
	{
		int i=0;
		
		while(Lua_Udata<T>::lua_udata_lib_meta[i].name)
		{
			lua_pushstring(L, Lua_Udata<T>::lua_udata_lib_meta[i].name);
			lua_pushcfunction(L, (Lua_Udata<T>::lua_udata_lib_meta[i].func));
			lua_settable(L, -3);
			i++;
		}
		
		const luaL_reg *class_lib = T::getMetaLib();
		i=0;
	
		while(class_lib[i].name)
		{
			lua_pushstring(L, class_lib[i].name);
			lua_pushcfunction(L, class_lib[i].func);
			lua_settable(L, -3);
			i++;
		}
	}
	
	return 0;
}

template <class T>
int Lua_Udata<T> :: inherit(lua_State *L)
{
	lua_pushlightuserdata(L, (void *)T::name);
	lua_pushboolean(L, true);		
	lua_rawset(L, -3);
	
	int k=0;
	const char **superTable = T::getSuperclassTable();
	while(superTable[k])
	{
		lua_pushlightuserdata(L, (void *)superTable[k]);
		lua_pushboolean(L, true);		
		lua_rawset(L, -3);
		
		//put methods of metatable in as well
		luaL_getmetatable(L, superTable[k]);
		if(!lua_isnil(L, -1))
		{
			// table is in the stack at index 't'
			lua_pushnil(L);  // first key
			while (lua_next(L, -2) != 0) {
				// uses 'key' (at index -2) and 'value' (at index -1) 
				if(lua_isfunction(L, -1)) {		//want to transfer superclass tables
					if(lua_isstring(L, -2)) {	//get string value of key
						const char *name = lua_tostring(L, -2);
						if(name[0] != '_' && name[1] != '_')  {		//not a reserved __meta field
							luaL_getmetatable(L, T::name);
							lua_pushvalue(L, -3);
							lua_rawget(L, -2);
							
							if(lua_isnil(L, -1)) {	//doesn't already exist in table
								lua_pop(L, 1);		//pop nil
								lua_pushvalue(L, -3);
								lua_pushvalue(L, -3);
								lua_rawset(L, -3);
								lua_pop(L, 1);
							}
							else {
								lua_pop(L, 2);
							}
						}
					}
				}
				
				// removes 'value'; keeps 'key' for next iteration
				lua_pop(L, 1);
			}
			
		}
		lua_pop(L, 1);
		
		k++;
	}
	
	return 0;
}

template <class T>
int Lua_Udata<T> :: stackDump(lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	
	printf("stack dump %d:\n", top);
	for(i=top; i >= 1; i--) {
		int t = lua_type(L, i);
		
		switch(t)
		{
			case LUA_TSTRING:
					printf("%d \t'%s' string\n", i, lua_tostring(L, i));
					break;
					
			case LUA_TBOOLEAN:
					printf(lua_toboolean(L, i) ? "\ttrue\n" : "\tfalse\n");
					break;
					
			case LUA_TNUMBER:
					printf("%d \t'%f' number\n", i, lua_tonumber(L, i));
					break;
			
			case LUA_TLIGHTUSERDATA:
					printf("%d \t'%x'\n", i, lua_touserdata(L, i));
					break;
					
			case LUA_TUSERDATA:
					printf("%d ", i);
					lua_getglobal(L, "print");
					lua_pushvalue(L, i);
					lua_call(L, 1, 0);
					//printf("%d \t'%x %s'\n", i, lua_touserdata(L, i), T::name);
					break;
					
			case LUA_TFUNCTION:
					printf("%d \t'func: %x'\n", i, lua_tocfunction(L, i));
					break;
			
			case LUA_TTABLE:
					printf("%d ", i);
					lua_getglobal(L, "print");
					lua_pushvalue(L, i);
					lua_call(L, 1, 0);
					break;
			
			default:
					printf("%d \t'%s'\n", i, lua_typename(L, t));
					break;
		}
	}
	
	return 0;
}

}	//end namespace glv

#endif	//LUA_UDATA_H
#include <string.h>
//#include <malloc.h>

#include <lauxlib.h>
#include "LuaGlu.h"

static GLenum get_glu_enum(lua_State *L, int index)
{
	const char *key = lua_tostring(L, index);
	
	lua_getglobal(L, "glu");				//put table on stack
	lua_getfield(L, -1, "enums");			//get enums table and put it on the stack
	lua_getfield(L, -1, key);				//get enum value from enum table and put it on the stack
	lua_remove(L, -3);						//remove glu table from stack
	lua_remove(L, -2);						//remove enum table from stack
	
	return (GLenum)lua_tointeger(L, -1);	//return enum value
}

static int get_arrayf(lua_State *L, int index, GLfloat **array)
{
	int i;
	int n = luaL_getn(L, index);

	*array = (GLfloat *)malloc(n * sizeof(GLfloat));

	for(i = 0; i < n; i++)
	{
	  lua_rawgeti(L, index, i + 1);
	  (*array)[i] = (GLfloat)lua_tonumber(L, -1);
	}

	return n; /* return the number of valid elements found.*/
}

//replace with better functions for memory access
static int get_arrayf2d(lua_State *L, int index, GLfloat **array)
{
	int w, h, v;

	//get number of rows in table
	w = luaL_getn(L, index);
	
	//get number of columns in table
	lua_pushinteger(L, 1);
	lua_gettable(L, index);
	h = luaL_getn(L, -1);
	
	//get number of values per cell
	lua_pushinteger(L, 1);
	lua_gettable(L, -2);
	v = luaL_getn(L, -1);
	//stackDump(L);

	return 1; /* return the number of valid elements found.*/
}

void string_from_array(int length, GLfloat *array, char *string)
{
	int i;
	char temp[64];
	
	string[0] = '\0';
	
	for(i=0; i < length; i++) {
		sprintf(temp, "%f ", array[i]);
		strcat(string, temp);
	}
}

/* glu.BeginSurface(nurbs) -> none */
static int glu_BeginSurface(lua_State *L)
{
	/* Type check arguments */
	if( !lua_isuserdata(L, 1) )
	{
		luaL_error(L, "incorrect string argument to function 'glu.BeginSurface'");
		return 0;
	}
	
	gluBeginSurface(lua_touserdata(L, 1));
	
	return 0;
}

/* glu.EndSurface(nurbs) -> none */
static int glu_EndSurface(lua_State *L)
{
	/* Type check arguments */
	if( !lua_isuserdata(L, 1) )
	{
		luaL_error(L, "incorrect string argument to function 'glu.EndSurface'");
		return 0;
	}
	
	gluEndSurface(lua_touserdata(L, 1));
	
	return 0;
}

/* glu.LookAt(Ex, Ey, Ez, Lx, Ly, Lz, Ux, Uy, Uz) -> none */
static int glu_LookAt(lua_State *L)
{
	if (!( lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)
		&& lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6)
		&& lua_isnumber(L, 7) && lua_isnumber(L, 8) && lua_isnumber(L, 9)))
	{
		luaL_error(L, "incorrect string argument to function 'glu.LookAt'");
		return 0;
	}
	gluLookAt(
			(GLdouble)lua_tonumber(L, 1), (GLdouble)lua_tonumber(L, 2), (GLdouble)lua_tonumber(L, 3),
			(GLdouble)lua_tonumber(L, 4), (GLdouble)lua_tonumber(L, 5), (GLdouble)lua_tonumber(L, 6),
			(GLdouble)lua_tonumber(L, 7), (GLdouble)lua_tonumber(L, 8), (GLdouble)lua_tonumber(L, 9));
			
	return 0;
}

/* glu.NewNurbsRenderer() -> nurbs */
static int glu_NewNurbsRenderer(lua_State *L)
{
	lua_pushlightuserdata(L, gluNewNurbsRenderer());
//	post("glu_NewNurbsRenderer");
		
	return 1;
}

/* glu.NurbsNewRenderer(nurbs, propery, value) -> none */
static int glu_NurbsProperty(lua_State *L)
{
	/* Type check arguments */
	if(! (lua_isuserdata(L, 1) && lua_isstring(L, 2)) )
	{
		luaL_error(L, "incorrect arguments to function 'glu.NurbsProperty'");
		return 0;
	}
	
	switch(lua_type(L, 3))
	{
		case LUA_TNUMBER:
				gluNurbsProperty(lua_touserdata(L, 1), get_glu_enum(L, 2), lua_tonumber(L, 3));
				break;
				
		case LUA_TSTRING:
				gluNurbsProperty(lua_touserdata(L, 1), get_glu_enum(L, 2), get_glu_enum(L, 3));
				break;
				
		default:
				luaL_error(L, "incorrect argument 3 to function 'glu.NurbsProperty'");
				return 0;
				break;
	}
	
	return 0;
}

/* glu.NurbsSurface(nurbs, sknot_count, sknots, tknot_count, tknots,
					ctlarray, sorder, torder, type) -> none */
static int glu_NurbsSurface(lua_State *L)
{
	GLfloat *sknots;
	GLfloat *tknots;
	GLfloat *ctl_points;
	char string[256];

	/* Type check arguments */
	if(! (lua_isuserdata(L, 1) && lua_istable(L, 2) && lua_istable(L, 3) && 
			lua_istable(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6) && 
			lua_isstring(L, 7)) )
	{
		luaL_error(L, "incorrect arguments to function 'glu.NurbsSurface'");
		return 0;
	}
	
	get_arrayf(L, 2, &sknots);
	get_arrayf(L, 3, &tknots);
	get_arrayf2d(L, 4, &ctl_points);
	
	/*post("1 nurbs: %x", lua_touserdata(L, 1));
	post("2 sknot_count: %d", luaL_getn(L, 2));
	string_from_array(luaL_getn(L, 2), sknots, string);
	post("2 sknots: %s", string);
	post("3 tknot_count: %d", luaL_getn(L, 3));
	string_from_array(luaL_getn(L, 3), tknots, string);
	post("3 tknots: %s", string);
	post("5 sorder: %d", lua_tointeger(L, 5));
	post("6 torder: %d", lua_tointeger(L, 6));
	post("7 type: %s", lua_tointeger(L, 7));
	*/
	free(sknots);
	free(tknots);
	
	return 0;
}

static int glu_Ortho2D(lua_State *L)
{
	if (!( lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)))
	{
		luaL_error(L, "incorrect string argument to function 'glu.Ortho2D'");
		return 0;
	}
	
	gluOrtho2D(	(GLdouble)lua_tonumber(L, 1), (GLdouble)lua_tonumber(L, 2), 
				(GLdouble)lua_tonumber(L, 3), (GLdouble)lua_tonumber(L, 4));
	
	return 0;
}

static int glu_Perspective(lua_State *L)
{
	if (!( lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)))
	{
		luaL_error(L, "incorrect string argument to function 'glu.Perspective'");
		return 0;
	}
	gluPerspective((GLdouble)lua_tonumber(L, 1), (GLdouble)lua_tonumber(L, 2), (GLdouble)lua_tonumber(L, 3), (GLdouble)lua_tonumber(L, 4));
	return 0;
}

static luaL_reg const glulib[] = {
	TABLE_ENTRY(glu, BeginSurface),
	TABLE_ENTRY(glu, EndSurface),
	TABLE_ENTRY(glu, LookAt),
	TABLE_ENTRY(glu, NewNurbsRenderer),
	TABLE_ENTRY(glu, NurbsProperty),
	TABLE_ENTRY(glu, NurbsSurface),
	TABLE_ENTRY(glu, Ortho2D),
	TABLE_ENTRY(glu, Perspective),
	{NULL, NULL}
};

LGLU_API int luaopen_glu(lua_State *L)
{
	luaL_register(L, "glu", glulib);				//register table and put on stack
	lua_pushstring(L, "enums");						//put key on stack
	
	//make enum table
	lua_newtable(L);								//put value on stack

	//fill new table
	{
		int i=0;
		
		while(glu_str[i].str)
		{
			lua_pushstring(L, glu_str[i].str);		//strings as enum keys
			lua_pushinteger(L, glu_str[i].value);	//enum value as values
			lua_settable(L, -3);					//set table with (key, value) pair
			i++;
		}
	}

	lua_settable(L, -3);							//set table with (key, value) pair

	return 1;
}
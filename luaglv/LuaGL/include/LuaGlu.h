#ifndef __LGLU_H__
#define __LGLU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>

#define LGLU_API extern

//# quotes a define argument
//## concatentates strings
#define ENUM_ERROR (GLenum)-2
#define TABLE_ENTRY(PREFIX, NAME)		{ #NAME, PREFIX##_##NAME }

typedef struct glu_str_value {
  const char *str;
  GLenum value;
} glu_str_value;


static const glu_str_value glu_str[] =
{
	TABLE_ENTRY(GLU, AUTO_LOAD_MATRIX),
	TABLE_ENTRY(GLU, CULLING),
	TABLE_ENTRY(GLU, DISPLAY_MODE),
	TABLE_ENTRY(GLU, ERROR),
	TABLE_ENTRY(GLU, FILL),
	TABLE_ENTRY(GLU, OUTLINE_PATCH),
	TABLE_ENTRY(GLU, OUTLINE_POLYGON),
	TABLE_ENTRY(GLU, PATH_LENGTH),
	TABLE_ENTRY(GLU, SAMPLING_TOLERANCE),
	TABLE_ENTRY(GL, FALSE),
	TABLE_ENTRY(GL, MAP2_COLOR_4),
	TABLE_ENTRY(GL, MAP2_NORMAL),
	TABLE_ENTRY(GL, MAP2_TEXTURE_COORD_2),
	TABLE_ENTRY(GL, MAP2_VERTEX_3),
	TABLE_ENTRY(GL, MAP2_VERTEX_4),
	TABLE_ENTRY(GL, TRUE),
	{0, 0}
};

LGLU_API int luaopen_glu(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif

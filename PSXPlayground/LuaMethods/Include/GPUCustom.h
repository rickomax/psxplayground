#pragma once
#include "lua.h"

void nextPrimitive();
PSX_INT_TYPE getOTIndex(long z);
int lua_setClearColor(lua_State *L);
int lua_drawTile(lua_State* L);
int lua_drawGeom(lua_State* L);
int lua_drawText(lua_State* L);
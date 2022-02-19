#pragma once
#include "lua.h"

int lua_SetColorMatrix(lua_State *L);
int lua_SetLightMatrix(lua_State *L);
int lua_ApplyMatrixLV(lua_State *L);
int lua_ApplyMatrixSV(lua_State* L);
int lua_SetTransMatrix(lua_State *L);
int lua_SetRotMatrix(lua_State *L);
int lua_MulMatrix0(lua_State *L);
int lua_CompMatrix(lua_State *L);
int lua_PopMatrix(lua_State *L);
int lua_PushMatrix(lua_State *L);
int lua_TransMatrix(lua_State *L);
int lua_RotMatrix(lua_State *L);
int lua_popMatrix(lua_State *L);
int lua_pushMatrix(lua_State *L);
int lua_setFogNearFar(lua_State *L);
int lua_setFogNear(lua_State *L);
int lua_setFarColor(lua_State *L);
int lua_setBackColor(lua_State *L);
int lua_setGeomScreen(lua_State *L);
int lua_setGeomOffset(lua_State *L);
int lua_initGeom(lua_State *L);
int lua_InvertMatrix(lua_State *L);

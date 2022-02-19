#pragma once
#include "lua.h"
#include "libgte.h"

void writeLuaMatrix(lua_State *L,MATRIX matrix);
MATRIX readLuaMatrix(lua_State *L,int idx);
void writeLuaVector(lua_State *L,const VECTOR vector);
void writeLuaSVector(lua_State* L, const SVECTOR vector);
VECTOR readLuaVector(lua_State *L,int idx);
SVECTOR readLuaSVector(lua_State *L,int idx);
int lua_ToInteger(lua_State *L);
void lua_push_number_to_integer(lua_State *L,const PSX_INT_TYPE n);
PSX_INT_TYPE lua_number_to_integer(lua_State *L,const PSX_INT_TYPE i);

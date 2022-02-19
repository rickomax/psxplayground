#pragma once
#include "lua.h"

int lua_getPadRightStickXY(lua_State *L);
int lua_getPadLeftStickXY(lua_State *L);
int lua_padDown(lua_State *L);
int lua_padPressed(lua_State *L);

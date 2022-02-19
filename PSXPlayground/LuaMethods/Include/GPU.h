#pragma once
#include "lua.h"

void getStringRes(const char* str, int n, int* w, int* h);
int lua_FntOpen(lua_State* L);
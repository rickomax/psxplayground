#pragma once
#include "libgte.h"
#include "libgpu.h"
#include "lua.h"
#include "types.h"

void loadBMPInternal(const char* filename, int magicPink, int transparentBlack, int timDepth, int abr, u_short* tPage, int* w, int* h, int* x, int* y);
int lua_loadBMP(lua_State *L);
int lua_drawBMP(lua_State* L);

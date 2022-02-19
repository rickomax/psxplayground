#pragma once
#include "lua.h"
#include "../../Include/PSXPlayground.h"

typedef struct
{
	VECTOR Data;
	VECTOR Pos;
} LIGHT;

int loadOBJ(const char* filename, const int modelScale, const int abr, const int magicPink, const int transparentBlack, const int timDepth, int* objId, LIGHT* light);
void drawOBJ(MATRIX* matrix, MATRIX* finalMatrix, const int enableCulling, const int wireframe, VECTOR objRGB, OBJECT_ELEMENT* elements);
int lua_drawOBJ(lua_State* L);
int lua_loadOBJ(lua_State* L);
int lua_getOBJTriangleInfo(lua_State* L);
int lua_getOBJBounds(lua_State* L);

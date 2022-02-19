#include "Include/GPUCustom.h"
#include "Include/Common.h"
#include "libgte.h"
#include "libgpu.h"
#include "../Include/PSXPlayground.h"

PSX_INT_TYPE getOTIndex(long z)
{
	const float fraction = (float)z / MAX_DEPTH; //todo: float
	const PSX_INT_TYPE result = fraction * OT_LEN;
	return result;
}

//todo: use primitive len
void nextPrimitive()
{
	const int code = getcode(primitive);
	switch (code)
	{
	case 0x20:
		primitive += sizeof(POLY_F3);
		break;
	case 0x24:
		primitive += sizeof(POLY_FT3);
		break;
	case 0x30:
		primitive += sizeof(POLY_G3);
		break;
	case 0x34:
		primitive += sizeof(POLY_GT3);
		break;
	case 0x28:
		primitive += sizeof(POLY_F4);
		break;
	case 0x2c:
		primitive += sizeof(POLY_FT4);
		break;
	case 0x38:
		primitive += sizeof(POLY_G4);
		break;
	case 0x3c:
		primitive += sizeof(POLY_GT4);
		break;
	case 0x58:
		primitive += sizeof(LINE_G3);
		break;
	case 0x5c:
		primitive += sizeof(LINE_G4);
		break;
	case 0x60:
		primitive += sizeof(TILE);
		break;
	}
}
int lua_setClearColor(lua_State* L)
{
	PSX_INT_TYPE r = lua_number_to_integer(L, 1);
	PSX_INT_TYPE g = lua_number_to_integer(L, 2);
	PSX_INT_TYPE b = lua_number_to_integer(L, 3);
	setRGB0(&draw[0], r, g, b);
	return 0;
}

int lua_drawTile(lua_State* L)
{
	TILE tile;
	const PSX_INT_TYPE x = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE y = lua_number_to_integer(L, 2);
	const PSX_INT_TYPE w = lua_number_to_integer(L, 3);
	const PSX_INT_TYPE h = lua_number_to_integer(L, 4);
	const VECTOR color = readLuaVector(L, 5);
	setTile(&tile);
	setXY0(&tile, _HF(x), _HF(y));
	setWH(&tile, _HF(w), _HF(h));
	if (color.pad < 128)
	{
		setSemiTrans(&tile, 1);
	}
	setRGB0(&tile, color.vx, color.vy, color.vz);
	DrawPrim(&tile);
	DrawSync(0);
	return 0;
}

int lua_drawGeom(lua_State* L)
{
	DrawOTag((ulong*)(ot[0] + OT_LEN - 1));
	DrawSync(0);
	return 0;
}

int lua_drawText(lua_State* L)
{
	FntFlush();
	return 0;
}

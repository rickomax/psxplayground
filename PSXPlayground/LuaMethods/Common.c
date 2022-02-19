#include <stdio.h>

#include "Include/Common.h"
#include "lauxlib.h"

PSX_INT_TYPE lua_number_to_integer(lua_State* L, const PSX_INT_TYPE i) {
	const lua_Number v = lua_tonumber(L, i);
	return v / ONE;
}

void lua_push_number_to_integer(lua_State* L, const PSX_INT_TYPE n)
{
	lua_pushinteger(L, n * ONE);
}

int lua_ToInteger(lua_State* L)
{
	const PSX_INT_TYPE f = lua_number_to_integer(L, 1);
	lua_pushinteger(L, f);
	return 1;
}

SVECTOR readLuaSVector(lua_State* L, int idx)
{
	SVECTOR vector = { 0,0,0,0 };
	if (lua_rawlen(L, idx) != 4)
	{
		return vector;
	}

	lua_pushinteger(L, 1);
	lua_gettable(L, idx);
	vector.vx = lua_tonumber(L, -1);
	lua_remove(L, -1);

	lua_pushinteger(L, 2);
	lua_gettable(L, idx);
	vector.vy = lua_tonumber(L, -1);
	lua_remove(L, -1);

	lua_pushinteger(L, 3);
	lua_gettable(L, idx);
	vector.vz = lua_tonumber(L, -1);
	lua_remove(L, -1);

	lua_pushinteger(L, 4);
	lua_gettable(L, idx);
	vector.pad = lua_tonumber(L, -1);
	lua_remove(L, -1);

	return vector;
}

VECTOR readLuaVector(lua_State* L, int idx)
{
	VECTOR vector = { 0,0,0,0 };
	if (lua_rawlen(L, idx) != 4)
	{
		return vector;
	}

	lua_pushinteger(L, 1);
	lua_gettable(L, idx);
	vector.vx = lua_number_to_integer(L, -1);
	lua_remove(L, -1);

	lua_pushinteger(L, 2);
	lua_gettable(L, idx);
	vector.vy = lua_number_to_integer(L, -1);
	lua_remove(L, -1);

	lua_pushinteger(L, 3);
	lua_gettable(L, idx);
	vector.vz = lua_number_to_integer(L, -1);
	lua_remove(L, -1);

	lua_pushinteger(L, 4);
	lua_gettable(L, idx);
	vector.pad = lua_number_to_integer(L, -1);
	lua_remove(L, -1);

	return vector;
}

void writeLuaVector(lua_State* L, const VECTOR vector)
{
	lua_createtable(L, 4, 0);

	lua_pushinteger(L, 1);
	lua_push_number_to_integer(L, vector.vx);
	lua_settable(L, -3);

	lua_pushinteger(L, 2);
	lua_push_number_to_integer(L, vector.vy);
	lua_settable(L, -3);

	lua_pushinteger(L, 3);
	lua_push_number_to_integer(L, vector.vz);
	lua_settable(L, -3);

	lua_pushinteger(L, 4);
	lua_push_number_to_integer(L, vector.pad);
	lua_settable(L, -3);
}

void writeLuaSVector(lua_State* L, const SVECTOR vector)
{
	lua_createtable(L, 4, 0);

	lua_pushinteger(L, 1);
	lua_pushinteger(L, vector.vx);
	lua_settable(L, -3);

	lua_pushinteger(L, 2);
	lua_pushinteger(L, vector.vy);
	lua_settable(L, -3);

	lua_pushinteger(L, 3);
	lua_pushinteger(L, vector.vz);
	lua_settable(L, -3);

	lua_pushinteger(L, 4);
	lua_pushinteger(L, vector.pad);
	lua_settable(L, -3);
}

MATRIX readLuaMatrix(lua_State* L, int idx)
{
	MATRIX matrix = { {{0,0,0}, {0,0,0}, {0,0,0}},{0,0,0} };
	if (lua_rawlen(L, idx) != 12)
	{
		return matrix;
	}
	for (size_t i = 0; i < 12; i++)
	{
		lua_pushinteger(L, i + 1);
		lua_gettable(L, idx);
		const lua_Number value = lua_tonumber(L, -1);
		if (i < 9)
		{
			matrix.m[i / 3][i % 3] = value;
		}
		else
		{
			matrix.t[i - 9] = value;
		}
		lua_remove(L, -1);
	}
	return matrix;
}

void writeLuaMatrix(lua_State* L, MATRIX matrix)
{
	lua_createtable(L, 12, 0);
	for (size_t i = 0; i < 12; i++)
	{
		lua_pushinteger(L, i + 1);
		if (i < 9)
		{
			lua_pushinteger(L, matrix.m[i / 3][i % 3]);
		}
		else
		{
			lua_pushinteger(L, matrix.t[i - 9]);
		}
		lua_settable(L, -3);
	}
}
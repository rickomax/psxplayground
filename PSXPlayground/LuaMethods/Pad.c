#include "Include/Pad.h"
#include "Include/Common.h"
#include "libpad.h"
#include "../Include/PSXPlayground.h"

int lua_padPressed(lua_State* L)
{
	const PSX_INT_TYPE padIndex = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE button = lua_number_to_integer(L, 2);
	if (padIndex < 1 || padIndex > 2)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	const LPPADRAW pad = (LPPADRAW)padbuff[padIndex - 1];
	int result = 0;
	if (pad->status == 0)
	{
		const ushort test = *(u_short*)pad->buttons;
		result = !(test & button);
	}
	lua_pushboolean(L, result);
	return 1;
}

int lua_padDown(lua_State* L)
{
	const PSX_INT_TYPE padIndex = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE button = lua_number_to_integer(L, 2);
	if (padIndex < 1 || padIndex > 2)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	const LPPADRAW prevPad = (LPPADRAW)prevPadbuff[padIndex - 1];
	const LPPADRAW pad = (LPPADRAW)padbuff[padIndex - 1];
	int result = 0;
	if (pad->status == 0)
	{
		const ushort prevTest = *(u_short*)prevPad->buttons;
		const ushort test = *(u_short*)pad->buttons;
		result = (prevTest & button) && !(test & button);
	}
	lua_pushboolean(L, result);
	return 1;
}

int lua_getPadLeftStickXY(lua_State* L)
{
	const PSX_INT_TYPE padIndex = lua_number_to_integer(L, 1);
	if (padIndex < 1 || padIndex > 2)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	const LPPADRAW pad = (LPPADRAW)padbuff[padIndex-1];
	lua_pushinteger(L, pad->analog[2 + 0]);
	lua_pushinteger(L, pad->analog[2 + 1]);
	return 2;
}

int lua_getPadRightStickXY(lua_State* L)
{
	const PSX_INT_TYPE padIndex = lua_number_to_integer(L, 1);
	if (padIndex < 1 || padIndex > 2)
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	const LPPADRAW pad = (LPPADRAW)padbuff[padIndex-1];
	lua_pushinteger(L, pad->analog[0 + 0]);
	lua_pushinteger(L, pad->analog[0 + 1]);
	return 2;
}
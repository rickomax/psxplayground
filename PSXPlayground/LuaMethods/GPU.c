#include "Include/GPU.h"

#include <string.h>

#include "libgte.h"
#include "libgpu.h"
#include "Include/Common.h"

void getStringRes(const char* str, int n, int* w, int* h)
{
	int maxX = -1;
	int curX = 0;
	*h = 8;
	for (int i = 0; i < n; i++)
	{
		if (str[i] == '\n')
		{
			*h += 8;
			curX = 0;
		}
		else
		{
			curX++;
		}
		if (curX > maxX) {
			maxX = curX;
		}
	}
	*w = maxX * 8;
}

int lua_FntOpen(lua_State* L)
{
	const PSX_INT_TYPE x = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE y = lua_number_to_integer(L, 2);
	const int isbg = lua_toboolean(L, 3);
	const char* text = lua_tostring(L, 4);
	const int n = strlen(text);
	int w;
	int h;
	getStringRes(text, n, &w, &h);
	const PSX_INT_TYPE r = FntOpen(x, y, w, h, isbg, n + 1);
	FntPrint(text);
	lua_push_number_to_integer(L, r);
	return 1;
}
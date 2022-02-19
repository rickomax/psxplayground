#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Include/BMP.h"
#include "Include/Common.h"
#include "MaxRectsBinPackC.h"
#include "../Include/PSXPlayground.h"
#include "../Include/BMP.h"

//todo: output x and y to pad the texture from vram
//todo: handle clut

void loadBMPInternal(const char* filename, int magicPink, int transparentBlack, int timDepth, int abr, u_short* tPage, int* w, int* h, int* x, int* y)
{
	for (size_t i = 0; i < textureCount; i++)
	{
		if (strcmp(filename, textures[i].textureName) == 0)
		{
			*x = textures[i].textureX;
			*y = textures[i].textureY;
			*w = textures[i].textureWidth;
			*h = textures[i].textureHeight;
			*tPage = textures[i].textureTPage;
			return;
		}
	}
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		printf("Could not find file %s\n", filename);
	}
	else
	{
		unsigned char* data = loadBMP(file, 0, magicPink, transparentBlack, timDepth, w, h);
		BinPackRect r = MaxRectsBinPackInsert(mrbp, *w, *h, 0);
		*x = r.x + screenWidth;
		*y = r.y;
		if (data != 0) {
			int tp;
			enum
			{
				TP_4BIT,
				TP_8BIT,
				TP_16BIT

			};
			switch (timDepth)
			{
			case 4:
			{
				tp = TP_4BIT;
				break;
			}
			case 8:
			{
				tp = TP_8BIT;
				break;
			}
			case 16:
			{
				tp = TP_16BIT;
				break;
			}
			default:
			{
				exit(1);
			}
			}
			*tPage = LoadTPage((ulong*)data, tp, abr, *x, *y, *w, *h);

			strcpy(&textures[textureCount].textureName, filename);
			textures[textureCount].textureTPage = *tPage;
			textures[textureCount].textureX = *x;
			textures[textureCount].textureY = *y;
			textures[textureCount].textureWidth = *w;
			textures[textureCount].textureHeight = *h;
			textureCount++;

			DrawSync(0);
			free(data);
			/*if (image->mode & 0x8) {

				LoadImage(image->cRECT16, (u_long*)image->caddr);
				DrawSync(0);
			}*/
		}
	}
}

//todo: return x,y,w,h, so user can adjust the uvs
int lua_loadBMP(lua_State* L)
{
	const char* filename = lua_tostring(L, 1);
	const PSX_INT_TYPE abr = lua_number_to_integer(L, 2);
	const PSX_INT_TYPE magicPink = lua_toboolean(L, 3);
	const PSX_INT_TYPE transparentBlack = lua_toboolean(L, 4);
	const PSX_INT_TYPE timDepth = lua_number_to_integer(L, 5);
	u_short tPage;
	int w;
	int h;
	int x;
	int y;
	loadBMPInternal(filename, magicPink, transparentBlack, timDepth, abr, &tPage, &w, &h, &x, &y);
	lua_push_number_to_integer(L, textureCount);
	return 1;
}

int lua_drawBMP(lua_State* L)
{
	const PSX_INT_TYPE textureId = lua_number_to_integer(L, 1);
	if (textureId < 1 || textureId > textureCount)
	{
		return 0;
	}
	const PSX_INT_TYPE x = lua_number_to_integer(L, 2);
	const PSX_INT_TYPE y = lua_number_to_integer(L, 3);
	const VECTOR color = readLuaVector(L, 4);
	SPRT sprt;
	DR_TPAGE tPage;
	setDrawTPage(&tPage, 1, 0, textures[textureId - 1].textureTPage);
	setSprt(&sprt);
	int uOffset = (textures[textureId - 1].textureX % 256) - 64;
	int vOffset = textures[textureId - 1].textureY % 256;
	setUV0(&sprt, uOffset, vOffset);
	setXY0(&sprt, _HF(x), _HF(y));
	setWH(&sprt, _HF(textures[textureId - 1].textureWidth), _HF(textures[textureId - 1].textureHeight));
	if (color.pad < 128)
	{
		setSemiTrans(&sprt, 1);
	}
	setRGB0(&sprt, color.vx, color.vy, color.vz);
	DrawPrim(&tPage);
	DrawPrim(&sprt);
	DrawSync(0);
	return 0;
}
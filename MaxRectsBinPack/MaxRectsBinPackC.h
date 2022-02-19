#pragma once
typedef struct
{
	int x;
	int y;
	int width;
	int height;
} BinPackRect;

#ifdef __cplusplus
extern "C" {
#endif
	void* CreateMaxRectsBinPack(int width, int height, int allowFlip);
	BinPackRect MaxRectsBinPackInsert(void* maxRectsBinPack, int width, int height, int method);
#ifdef __cplusplus
}
#endif
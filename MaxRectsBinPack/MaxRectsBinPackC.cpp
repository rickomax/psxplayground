#include "MaxRectsBinPack.h"
#include "MaxRectsBinPackC.h"
#include "Rect.h"

extern "C" {
	void* CreateMaxRectsBinPack(int width, int height, int allowFlip = 1)
	{
		void* ptr = new rbp::MaxRectsBinPack(width, height, allowFlip);
		return ptr;
	}

	BinPackRect MaxRectsBinPackInsert(void* maxRectsBinPack, int width, int height, int method)
	{
		BinPackRect r;
		const rbp::Rect result = ((rbp::MaxRectsBinPack*)maxRectsBinPack)->Insert(width, height, (rbp::MaxRectsBinPack::FreeRectChoiceHeuristic)method);
		r.x = result.x;
		r.y = result.y;
		r.width = result.width;
		r.height = result.height;
		return r;
	}
}
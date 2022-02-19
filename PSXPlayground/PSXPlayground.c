#include "Include/PSXPlayground.h"

#include <direct.h>
#include <stdlib.h>
#include <string.h>

#include "lauxlib.h"
#include "lualib.h"
#include "Include/LuaMethods.h"
#include "MaxRectsBinPackC.h"
#include "Include/FileUtils.h"
#include "PsyX/PsyX_public.h"

void init(lua_State* L, char* script)
{
	PadInitDirect((unsigned char*)padbuff[0], (unsigned char*)padbuff[1]);
	PadStartCom();

	SetDispMask(0);
	StopCallback();
	ResetCallback();
	ResetGraph(0);
	SetDispMask(0);
	SetGraphDebug(0);

	SpuInit();
	SpuCommonAttr commonAttr;
	commonAttr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);
	commonAttr.mvol.left = 0x3fff;
	commonAttr.mvol.right = 0x3fff;
	SpuSetCommonAttr(&commonAttr);

	//todo: mode by lua calls
	SetVideoMode(MODE_NTSC);
	screenWidth = 320;
	screenHeight = GetVideoMode() == MODE_NTSC == 0 ? 240 : 256;
	SetDefDispEnv(&disp[0], 0, screenHeight, screenWidth, screenHeight);
	SetDefDrawEnv(&draw[0], 0, 0, screenWidth, screenHeight);
	draw[0].isbg = 1;
	setRGB0(&draw[0], 0, 0, 0);
	
	mrbp = CreateMaxRectsBinPack(1024 - screenWidth, 512, 0);

	const BinPackRect fontRect = MaxRectsBinPackInsert(mrbp, 32, 64, 0);
	FntLoad(fontRect.x+screenWidth, fontRect.y);

	char scriptDirectory[2048];
	strcpy_s((void*)&scriptDirectory, sizeof(scriptDirectory), script);
	getFileDirectory(scriptDirectory);
	getFileName(script);
	if (_chdir(scriptDirectory))
	{
		printf("Could not change directory.\n");
		exit(-1);
	}
	
	luaL_openlibs(L);
	set_luaVars(L);
	registerLuaMethods(L);
	const int result = luaL_dofile(L, script);
	if (result != 0)
	{
		printf("LUA Error: %s\n", lua_tostring(L, -1));
	}
	lua_getglobal(L, "init");
	if (lua_pcall(L, 0, 0, 0) != 0)
	{
		printf("Missing LUA \"init\" function.\n");
		exit(-1);
	}

	PutDispEnv(&disp[0]);
	PutDrawEnv(&draw[0]);

	SetDispMask(1);
}

void render(lua_State* L)
{
	PsyX_BeginScene();
	PsyX_UpdateInput();

	ClearOTagR((ulong*)ot[0], OT_LEN);

	primitive = primitiveBuffer;

	lua_pushvalue(L, -1);
	lua_call(L, 0, 0);
	
	VSync(0);
	
	PutDispEnv(&disp[0]);
	PutDrawEnv(&draw[0]);

	memcpy(&prevPadbuff, padbuff, sizeof(padbuff));

	memset(&primitiveBuffer, 0, sizeof(primitiveBuffer));

	PsyX_EndScene();
}

int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		printf("PSXPlayground <lua script filename>  <screen width> <screen height> <fullscreen 0 or 1>.");
		return 1;
	}

	const int psyxScreenWidth = atoi(argv[2]);
	const int psyxScreenHeight = atoi(argv[3]);
	const int psyxFullscreen = atoi(argv[4]);

	PsyX_Initialise("PSXPlayground", psyxScreenWidth, psyxScreenHeight, psyxFullscreen);

	lua_State* L = luaL_newstate();

	init(L, argv[1]);

	//todo: check if function exists
	lua_getglobal(L, "render");
	while (1)
	{
		render(L);
	}
}

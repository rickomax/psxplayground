#include "Include/LuaMethods.h"
#include "Include/PSXPlayground.h"

#include "lauxlib.h"
#include "LuaMethods/Include/Common.h"
#include "LuaMethods/Include/Math.h"
#include "LuaMethods/Include/GPU.h"
#include "LuaMethods/Include/GPUCustom.h"
#include "LuaMethods/Include/GTE.h"
#include "LuaMethods/Include/Pad.h"
#include "LuaMethods/Include/BMP.h"
#include "LuaMethods/Include/OBJ.h"
#include "LuaMethods/Include/WAV.h"
#include "LuaMethods/Include/Physics.h"

void set_luaVars(lua_State* L)
{
	lua_push_number_to_integer(L, PAD_SELECT);
	lua_setglobal(L, "PAD_SELECT");

	lua_push_number_to_integer(L, PAD_L3);
	lua_setglobal(L, "PAD_L3");

	lua_push_number_to_integer(L, PAD_R3);
	lua_setglobal(L, "PAD_R3");

	lua_push_number_to_integer(L, PAD_START);
	lua_setglobal(L, "PAD_START");

	lua_push_number_to_integer(L, PAD_UP);
	lua_setglobal(L, "PAD_UP");

	lua_push_number_to_integer(L, PAD_RIGHT);
	lua_setglobal(L, "PAD_RIGHT");

	lua_push_number_to_integer(L, PAD_DOWN);
	lua_setglobal(L, "PAD_DOWN");

	lua_push_number_to_integer(L, PAD_LEFT);
	lua_setglobal(L, "PAD_LEFT");

	lua_push_number_to_integer(L, PAD_L2);
	lua_setglobal(L, "PAD_L2");

	lua_push_number_to_integer(L, PAD_R2);
	lua_setglobal(L, "PAD_R2");

	lua_push_number_to_integer(L, PAD_L1);
	lua_setglobal(L, "PAD_L1");

	lua_push_number_to_integer(L, PAD_R1);
	lua_setglobal(L, "PAD_R1");

	lua_push_number_to_integer(L, PAD_TRIANGLE);
	lua_setglobal(L, "PAD_TRIANGLE");

	lua_push_number_to_integer(L, PAD_CIRCLE);
	lua_setglobal(L, "PAD_CIRCLE");

	lua_push_number_to_integer(L, PAD_CROSS);
	lua_setglobal(L, "PAD_CROSS");

	lua_push_number_to_integer(L, PAD_SQUARE);
	lua_setglobal(L, "PAD_SQUARE");

	lua_push_number_to_integer(L, screenWidth);
	lua_setglobal(L, "SCREEN_XRES");

	lua_push_number_to_integer(L, screenHeight);
	lua_setglobal(L, "SCREEN_YRES");

	lua_pushinteger(L, 1);
	lua_setglobal(L, "X");

	lua_pushinteger(L, 2);
	lua_setglobal(L, "Y");

	lua_pushinteger(L, 3);
	lua_setglobal(L, "Z");
	
	lua_pushinteger(L, 4);
	lua_setglobal(L, "W");
}

void registerLuaMethods(lua_State* L)
{
	//GTE 1
	lua_register(L, "InitGeom", lua_initGeom);
	lua_register(L, "SetGeomOffset", lua_setGeomOffset);
	lua_register(L, "SetGeomScreen", lua_setGeomScreen);
	lua_register(L, "SetBackColor", lua_setBackColor);
	lua_register(L, "SetFarColor", lua_setFarColor);
	lua_register(L, "SetFogNear", lua_setFogNear);
	lua_register(L, "SetFogNearFar", lua_setFogNearFar);
	lua_register(L, "RotMatrix", lua_RotMatrix);
	lua_register(L, "TransMatrix", lua_TransMatrix);
	lua_register(L, "CompMatrix", lua_CompMatrix);
	lua_register(L, "MulMatrix0", lua_MulMatrix0);
	lua_register(L, "SetTransMatrix", lua_SetTransMatrix);
	lua_register(L, "SetRotMatrix", lua_SetRotMatrix);
	lua_register(L, "SetLightMatrix", lua_SetLightMatrix);
	lua_register(L, "SetColorMatrix", lua_SetColorMatrix);
	lua_register(L, "PopMatrix", lua_PopMatrix);
	lua_register(L, "PushMatrix", lua_PushMatrix);
	lua_register(L, "ApplyMatrixLV", lua_ApplyMatrixLV);
	lua_register(L, "ApplyMatrixSV", lua_ApplyMatrixSV);

	//PAD
	lua_register(L, "padPressed", lua_padPressed);
	lua_register(L, "padDown", lua_padDown);
	lua_register(L, "getPadLeftStickXY", lua_getPadLeftStickXY);
	lua_register(L, "getPadRightStickXY", lua_getPadRightStickXY);

	//MATH
	lua_register(L, "iclamp", lua_iclamp);
	lua_register(L, "icos", lua_icos);
	lua_register(L, "isin", lua_isin);
	lua_register(L, "iatan2", lua_iatan2);
	lua_register(L, "int", lua_ToInteger);
	lua_register(L, "invertMatrix", lua_InvertMatrix);
	lua_register(L, "lookAt", lua_lookat);
	lua_register(L, "idot", lua_idot);
	lua_register(L, "ilerp", lua_ilerp);
	lua_register(L, "ilerpAngle", lua_ilerpAngle);
	lua_register(L, "projectOnPlane", lua_projectOnPlane);
	lua_register(L, "vectorMulIntegerSV", lua_vectorMulIntegerSV);
	lua_register(L, "vectorMulIntegerLV", lua_vectorMulIntegerLV);
	lua_register(L, "vectorAddSV", lua_vectorAddSV);
	lua_register(L, "vectorAddLV", lua_vectorAddLV);
	lua_register(L, "vectorSubSV", lua_vectorSubSV);
	lua_register(L, "vectorSubLV", lua_vectorSubLV);
	lua_register(L, "vectorDivSV", lua_vectorDivSV);
	lua_register(L, "vectorDivLV", lua_vectorDivLV);
	lua_register(L, "vectorMulSV", lua_vectorMulSV);
	lua_register(L, "vectorMulLV", lua_vectorMulLV);
	lua_register(L, "normalizeVectorSV", lua_normalizeVectorSV);
	lua_register(L, "normalizeVectorLV", lua_normalizeVectorLV);
	lua_register(L, "vectorLengthSV", lua_vectorLengthSV);
	lua_register(L, "vectorLengthLV", lua_vectorLengthLV);
	lua_register(L, "vectorCopySV", lua_vectorCopySV);
	lua_register(L, "vectorCopyLV", lua_vectorCopyLV);
	lua_register(L, "vectorInvertSV", lua_vectorInvertSV);
	lua_register(L, "vectorInvertLV", lua_vectorInvertLV);

	//GPU CUSTOM
	lua_register(L, "setClearColor", lua_setClearColor);
	lua_register(L, "drawTile", lua_drawTile);
	lua_register(L, "drawGeom", lua_drawGeom);
	lua_register(L, "drawText", lua_drawText);
	
	//GPU
	lua_register(L, "FntOpen", lua_FntOpen);
	
	//DATA
	lua_register(L, "drawOBJ", lua_drawOBJ);
	lua_register(L, "loadOBJ", lua_loadOBJ);
	lua_register(L, "getOBJTriangleInfo", lua_getOBJTriangleInfo);
	lua_register(L, "getOBJBounds", lua_getOBJBounds);
	lua_register(L, "loadBMP", lua_loadBMP);
	lua_register(L, "drawBMP", lua_drawBMP);
	lua_register(L, "loadWAV", lua_loadWAV);
	lua_register(L, "playWAV", lua_playWAV);

	//PHYSICS
	lua_register(L, "raycast", lua_raycast);
	lua_register(L, "linecast", lua_linecast);
	lua_register(L, "sphereOBJIntersection", lua_sphereOBJIntersection);
}

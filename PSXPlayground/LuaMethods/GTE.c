#include "Include/GTE.h"

#include "libgte.h"
#include "Include/Common.h"
#include "Include/Math.h"

int lua_InvertMatrix(lua_State* L)
{
	MATRIX m = readLuaMatrix(L, 1);
	m = invertMatrix(&m);
	writeLuaMatrix(L, m);
	return 1;
}

int lua_initGeom(lua_State* L)
{
	InitGeom();
	return 0;
}

int lua_setGeomOffset(lua_State* L)
{
	const PSX_INT_TYPE x = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE y = lua_number_to_integer(L, 2);
	SetGeomOffset(x, y);
	return 0;
}

int lua_setGeomScreen(lua_State* L)
{
	const PSX_INT_TYPE x = lua_number_to_integer(L, 1);
	SetGeomScreen(x);
	return 0;
}

int lua_setBackColor(lua_State* L)
{
	const PSX_INT_TYPE r = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE g = lua_number_to_integer(L, 2);
	const PSX_INT_TYPE b = lua_number_to_integer(L, 3);
	SetBackColor(r, g, b);
	return 0;
}

int lua_setFarColor(lua_State* L)
{
	const PSX_INT_TYPE r = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE g = lua_number_to_integer(L, 2);
	const PSX_INT_TYPE b = lua_number_to_integer(L, 3);
	SetFarColor(r, g, b);
	return 0;
}

int lua_setFogNear(lua_State* L)
{
	const PSX_INT_TYPE n = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE h = lua_number_to_integer(L, 2);
	SetFogNear(n, h);
	return 0;
}

int lua_setFogNearFar(lua_State* L)
{
	const PSX_INT_TYPE n = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE f = lua_number_to_integer(L, 2);
	const PSX_INT_TYPE h = lua_number_to_integer(L, 3);
	SetFogNearFar(n, f, h);
	return 0;
}

int lua_pushMatrix(lua_State* L)
{
	PushMatrix();
	return 0;
}

int lua_popMatrix(lua_State* L)
{
	PopMatrix();
	return 0;
}

int lua_RotMatrix(lua_State* L)
{
	SVECTOR vector = readLuaSVector(L, 1);
	MATRIX matrix;
	RotMatrix(&vector, &matrix);
	writeLuaMatrix(L, matrix);
	return 1;
}

int lua_TransMatrix(lua_State* L)
{
	MATRIX matrix = readLuaMatrix(L, 1);
	VECTOR vector = readLuaVector(L, 2);
	TransMatrix(&matrix, &vector);
	writeLuaMatrix(L, matrix);
	return 1;
}

int lua_PushMatrix(lua_State* L)
{
	PushMatrix();
	return 0;
}

int lua_PopMatrix(lua_State* L)
{
	PopMatrix();
	return 0;
}

int lua_CompMatrix(lua_State* L)
{
	MATRIX a = readLuaMatrix(L, 1);
	MATRIX b = readLuaMatrix(L, 2);
	MATRIX c;
	CompMatrix(&a, &b, &c);
	writeLuaMatrix(L, c);
	return 1;
}

int lua_MulMatrix0(lua_State* L)
{
	MATRIX a = readLuaMatrix(L, 1);
	MATRIX b = readLuaMatrix(L, 2);
	MATRIX c;
	MulMatrix0(&a, &b, &c);
	writeLuaMatrix(L, c);
	return 1;
}

int lua_SetRotMatrix(lua_State* L)
{
	MATRIX matrix = readLuaMatrix(L, 1);
	SetRotMatrix(&matrix);
	return 0;
}

int lua_SetTransMatrix(lua_State* L)
{
	MATRIX matrix = readLuaMatrix(L, 1);
	SetTransMatrix(&matrix);
	return 0;
}

int lua_ApplyMatrixLV(lua_State* L)
{
	MATRIX matrix = readLuaMatrix(L, 1);
	VECTOR v0 = readLuaVector(L, 2);
	VECTOR v1;
	ApplyMatrixLV(&matrix, &v0, &v1);
	writeLuaVector(L, v1);
	return 1;
}

int lua_ApplyMatrixSV(lua_State* L)
{
	MATRIX matrix = readLuaMatrix(L, 1);
	SVECTOR v0 = readLuaSVector(L, 2);
	SVECTOR v1;
	ApplyMatrixSV(&matrix, &v0, &v1);
	writeLuaSVector(L, v1);
	return 1;
}

//
//int lua_ApplyRotMatrixLV(lua_State* L)
//{
//	VECTOR v0 = readLuaVector(L, 1);
//	VECTOR v1;
//	ApplyRotMatrixLV(&v0, &v1);
//	writeLuaVector(L, v1);
//	return 1;
//}

int lua_SetLightMatrix(lua_State* L)
{
	MATRIX matrix = readLuaMatrix(L, 1);
	SetLightMatrix(&matrix);
	return 0;
}

int lua_SetColorMatrix(lua_State* L)
{
	//todo: has to be allocated?
	MATRIX matrix = readLuaMatrix(L, 1);
	SetColorMatrix(&matrix);
	return 0;
}

//int lua_gte_ldv0(lua_State* L)
//{
//	VECTOR a = readLuaVector(L, 1);
//	gte_ldv0(&a);
//	return 0;
//}
//
//int lua_gte_ldv3(lua_State* L)
//{
//	VECTOR a = readLuaVector(L, 1);
//	VECTOR b = readLuaVector(L, 2);
//	VECTOR c = readLuaVector(L, 3);
//	gte_ldv3(&a, &b, &c);
//	return 0;
//}
//
//int lua_gte_rtpt(lua_State* L)
//{
//	gte_rtpt();
//	return 0;
//}
//
//int lua_gte_rtps(lua_State* L)
//{
//	gte_rtps();
//	return 0;
//}
//
//int lua_gte_nclip(lua_State* L)
//{
//	gte_nclip();
//	return 0;
//}
//
//int lua_gte_stopz(lua_State* L)
//{
//	PSX_INT_TYPE value;
//	gte_stopz(&value);
//	lua_pushinteger(L, value);
//	return 1;
//}
//
//int lua_gte_avsz4(lua_State* L)
//{
//	gte_avsz4();
//	return 0;
//}
//
//int lua_gte_stotz(lua_State* L)
//{
//	PSX_INT_TYPE value;
//	gte_stotz(&value);
//	lua_pushinteger(L, value);
//	return 1;
//}
//
//int lua_gte_stsxy0(lua_State* L)
//{
//	VERTTYPE* ptr = 0;
//	const int code = getcode(primitive);
//	switch (code)
//	{
//	case 0x20:
//		ptr = &((POLY_F3*)primitive)->x0;
//		break;
//	case 0x24:
//		ptr = &((POLY_FT3*)primitive)->x0;
//		break;
//	case 0x30:
//		ptr = &((POLY_G3*)primitive)->x0;
//		break;
//	case 0x34:
//		ptr = &((POLY_GT3*)primitive)->x0;
//		break;
//	case 0x28:
//		ptr = &((POLY_F4*)primitive)->x0;
//		break;
//	case 0x2c:
//		ptr = &((POLY_FT4*)primitive)->x0;
//		break;
//	case 0x38:
//		ptr = &((POLY_G4*)primitive)->x0;
//		break;
//	case 0x3c:
//		ptr = &((POLY_GT4*)primitive)->x0;
//		break;
//	}
//	if (ptr != 0)
//	{
//		gte_stsxy0(ptr);
//	}
//	return 0;
//}
//
//int lua_gte_stsxy1(lua_State* L)
//{
//	VERTTYPE* ptr = 0;
//	const int code = getcode(primitive);
//	switch (code)
//	{
//	case 0x20:
//		ptr = &((POLY_F3*)primitive)->x1;
//		break;
//	case 0x24:
//		ptr = &((POLY_FT3*)primitive)->x1;
//		break;
//	case 0x30:
//		ptr = &((POLY_G3*)primitive)->x1;
//		break;
//	case 0x34:
//		ptr = &((POLY_GT3*)primitive)->x1;
//		break;
//	case 0x28:
//		ptr = &((POLY_F4*)primitive)->x1;
//		break;
//	case 0x2c:
//		ptr = &((POLY_FT4*)primitive)->x1;
//		break;
//	case 0x38:
//		ptr = &((POLY_G4*)primitive)->x1;
//		break;
//	case 0x3c:
//		ptr = &((POLY_GT4*)primitive)->x1;
//		break;
//	}
//	if (ptr != 0)
//	{
//		gte_stsxy1(ptr);
//	}
//	return 0;
//}
//
//int lua_gte_stsxy2(lua_State* L)
//{
//	VERTTYPE* ptr = 0;
//	const int code = getcode(primitive);
//	switch (code)
//	{
//	case 0x20:
//		ptr = &((POLY_F3*)primitive)->x2;
//		break;
//	case 0x24:
//		ptr = &((POLY_FT3*)primitive)->x2;
//		break;
//	case 0x30:
//		ptr = &((POLY_G3*)primitive)->x2;
//		break;
//	case 0x34:
//		ptr = &((POLY_GT3*)primitive)->x2;
//		break;
//	case 0x28:
//		ptr = &((POLY_F4*)primitive)->x2;
//		break;
//	case 0x2c:
//		ptr = &((POLY_FT4*)primitive)->x2;
//		break;
//	case 0x38:
//		ptr = &((POLY_G4*)primitive)->x2;
//		break;
//	case 0x3c:
//		ptr = &((POLY_GT4*)primitive)->x2;
//		break;
//	}
//	if (ptr != 0)
//	{
//		gte_stsxy2(ptr);
//	}
//	return 0;
//}
//
//int lua_gte_stsxy(lua_State* L)
//{
//	VERTTYPE* ptr = 0;
//	const PSX_INT_TYPE field = lua_number_to_integer(L, 1);
//	const int code = getcode(primitive);
//	if (field == 3) {
//		switch (code)
//		{
//		case 0x20:
//			//ptr = &((POLY_F3*)primitive)->x3;
//			break;
//		case 0x24:
//			//ptr = &((POLY_FT3*)primitive)->x3;
//			break;
//		case 0x30:
//			//ptr = &((POLY_G3*)primitive)->x3;
//			break;
//		case 0x34:
//			//ptr = &((POLY_GT3*)primitive)->x3;
//			break;
//		case 0x28:
//			ptr = &((POLY_F4*)primitive)->x3;
//			break;
//		case 0x2c:
//			ptr = &((POLY_FT4*)primitive)->x3;
//			break;
//		case 0x38:
//			ptr = &((POLY_G4*)primitive)->x3;
//			break;
//		case 0x3c:
//			ptr = &((POLY_GT4*)primitive)->x3;
//			break;
//		}
//	}
//	if (ptr != 0)
//	{
//		gte_stsxy(ptr);
//	}
//	return 0;
//}
//
//int lua_gte_ldrgb(lua_State* L)
//{
//	unsigned char* ptr = 0;
//	const PSX_INT_TYPE field = lua_number_to_integer(L, 1);
//	const int code = getcode(primitive);
//	if (field == 0) {
//		switch (code)
//		{
//		case 0x20:
//			ptr = &((POLY_F3*)primitive)->r0;
//			break;
//		case 0x24:
//			ptr = &((POLY_FT3*)primitive)->r0;
//			break;
//		case 0x30:
//			ptr = &((POLY_G3*)primitive)->r0;
//			break;
//		case 0x34:
//			ptr = &((POLY_GT3*)primitive)->r0;
//			break;
//		case 0x28:
//			ptr = &((POLY_F4*)primitive)->r0;
//			break;
//		case 0x2c:
//			ptr = &((POLY_FT4*)primitive)->r0;
//			break;
//		case 0x38:
//			ptr = &((POLY_G4*)primitive)->r0;
//			break;
//		case 0x3c:
//			ptr = &((POLY_GT4*)primitive)->r0;
//			break;
//		}
//	}
//	if (ptr != 0)
//	{
//		gte_ldrgb(ptr);
//	}
//	return 0;
//}
//
//int lua_gte_strgb(lua_State* L)
//{
//	unsigned char* ptr = 0;
//	const PSX_INT_TYPE field = lua_number_to_integer(L, 1);
//	const int code = getcode(primitive);
//	if (field == 0) {
//		switch (code)
//		{
//		case 0x20:
//			ptr = &((POLY_F3*)primitive)->r0;
//			break;
//		case 0x24:
//			ptr = &((POLY_FT3*)primitive)->r0;
//			break;
//		case 0x30:
//			ptr = &((POLY_G3*)primitive)->r0;
//			break;
//		case 0x34:
//			ptr = &((POLY_GT3*)primitive)->r0;
//			break;
//		case 0x28:
//			ptr = &((POLY_F4*)primitive)->r0;
//			break;
//		case 0x2c:
//			ptr = &((POLY_FT4*)primitive)->r0;
//			break;
//		case 0x38:
//			ptr = &((POLY_G4*)primitive)->r0;
//			break;
//		case 0x3c:
//			ptr = &((POLY_GT4*)primitive)->r0;
//			break;
//		}
//	}
//	if (ptr != 0)
//	{
//		gte_strgb(ptr);
//	}
//	return 0;
//}
//
//int lua_gte_ncs(lua_State* L)
//{
//	gte_ncs();
//	return 0;
//}
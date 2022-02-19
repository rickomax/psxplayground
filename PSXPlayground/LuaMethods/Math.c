#include "Include/Math.h"

#include "libgpu.h"
#include "Include/Common.h"

PSX_INT_TYPE iabs(PSX_INT_TYPE number)
{
	return number < 0 ? -number : number;
}

PSX_INT_TYPE clamp(PSX_INT_TYPE n, PSX_INT_TYPE min, PSX_INT_TYPE max)
{
	return n < min ? min : (n > max ? max : n);
}

PSX_INT_TYPE lerp(PSX_INT_TYPE a, PSX_INT_TYPE b, PSX_INT_TYPE f)
{
	return fixedMul(a, (ONE - f)) + fixedMul(b, f);
}

int lua_iclamp(lua_State* L) {
	PSX_INT_TYPE v = lua_number_to_integer(L, 1);
	PSX_INT_TYPE min = lua_number_to_integer(L, 2);
	PSX_INT_TYPE max = lua_number_to_integer(L, 3);
	PSX_INT_TYPE r = clamp(v, min, max);
	lua_push_number_to_integer(L, r);
	return 1;
}

void lookAt(VECTOR* eye, VECTOR* at, SVECTOR* up, SVECTOR* xaxis, SVECTOR* yaxis, SVECTOR* zaxis) {
	VECTOR taxis;
	setVector(&taxis, at->vx - eye->vx, at->vy - eye->vy, at->vz - eye->vz);
	VecNormalize(zaxis, &taxis);
	VecCross(&taxis, zaxis, up);
	VecNormalize(xaxis, &taxis);
	VecCross(&taxis, zaxis, xaxis);
	VecNormalize(yaxis, &taxis);
}

MATRIX invertMatrix(MATRIX* matrix)
{
	MATRIX newMatrix;
	newMatrix.m[0][0] = matrix->m[0][0];
	newMatrix.m[0][1] = matrix->m[1][0];
	newMatrix.m[0][2] = matrix->m[2][0];
	newMatrix.m[1][0] = matrix->m[0][1];
	newMatrix.m[1][1] = matrix->m[1][1];
	newMatrix.m[1][2] = matrix->m[2][1];
	newMatrix.m[2][0] = matrix->m[0][2];
	newMatrix.m[2][1] = matrix->m[1][2];
	newMatrix.m[2][2] = matrix->m[2][2];
	newMatrix.t[0] = -matrix->t[0];
	newMatrix.t[1] = -matrix->t[1];
	newMatrix.t[2] = -matrix->t[2];
	return newMatrix;
}

int lua_icos(lua_State* L)
{
	const PSX_INT_TYPE value = lua_tonumber(L, 1);
	const PSX_INT_TYPE r = rcos(value);
	lua_pushinteger(L, r);
	return 1;
}

int lua_isin(lua_State* L)
{
	const PSX_INT_TYPE value = lua_tonumber(L, 1);
	const PSX_INT_TYPE r = rsin(value);
	lua_pushinteger(L, r);
	return 1;
}

int lua_iatan2(lua_State* L)
{
	const PSX_INT_TYPE x = lua_tonumber(L, 1);
	const PSX_INT_TYPE y = lua_tonumber(L, 2);
	const PSX_INT_TYPE r = ratan2(x, y);
	lua_pushinteger(L, r);
	return 1;
}

//todo: return a matrix
int lua_lookat(lua_State* L)
{
	const VECTOR eye = readLuaVector(L, 1);
	const VECTOR at = readLuaVector(L, 2);
	const SVECTOR up = readLuaSVector(L, 3);
	SVECTOR xaxis;
	SVECTOR yaxis;
	SVECTOR zaxis;
	lookAt(&eye, &at, &up, &xaxis, &yaxis, &zaxis);
	writeLuaSVector(L, xaxis);
	writeLuaSVector(L, yaxis);
	writeLuaSVector(L, zaxis);
	return 3;
}

int lua_idot(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	const SVECTOR b = readLuaSVector(L, 2);
	PSX_INT_TYPE d; VecDot(d, &a, &b);
	lua_pushinteger(L, d);
	return 1;
}

int lua_ilerp(lua_State* L)
{
	const PSX_INT_TYPE a = lua_number_to_integer(L, 1);
	const PSX_INT_TYPE b = lua_number_to_integer(L, 2);
	const lua_Number t = lua_tonumber(L, 3);
	const PSX_INT_TYPE r = lerp(a, b, t);
	lua_push_number_to_integer(L, r);
	return 1;
}

int lua_ilerpAngle(lua_State* L)
{
	const lua_Number a = lua_tonumber(L, 1);
	const lua_Number b = lua_tonumber(L, 2);
	const lua_Number t = lua_tonumber(L, 3);
	PSX_INT_TYPE delta = (b - a) % ONE;
	if (delta > 2048) {
		delta -= ONE;
	}
	const PSX_INT_TYPE r = a + fixedMul(delta, clamp(t, 0, ONE));
	lua_pushinteger(L, r);
	return 1;
}

int lua_vectorAddSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	const SVECTOR b = readLuaSVector(L, 2);
	SVECTOR r; VecAdd(&r, &a, &b);
	writeLuaSVector(L, r);
	return 1;
}

int lua_vectorAddLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	const VECTOR b = readLuaVector(L, 2);
	VECTOR r; VecAdd(&r, &a, &b);
	writeLuaVector(L, r);
	return 1;
}

int lua_vectorSubSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	const SVECTOR b = readLuaSVector(L, 2);
	SVECTOR r; VecSub(&r, &a, &b);
	writeLuaSVector(L, r);
	return 1;
}

int lua_vectorSubLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	const VECTOR b = readLuaVector(L, 2);
	VECTOR r; VecSub(&r, &a, &b);
	writeLuaVector(L, r);
	return 1;
}

int lua_vectorMulIntegerSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	const PSX_INT_TYPE b = lua_tonumber(L, 2);
	SVECTOR r; VecMulInteger(&r, &a, &b);
	writeLuaSVector(L, r);
	return 1;
}

int lua_vectorMulIntegerLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	const PSX_INT_TYPE b = lua_tonumber(L, 2);
	VECTOR r; VecMulInteger(&r, &a, &b);
	writeLuaVector(L, r);
	return 1;
}

int lua_vectorDivSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	const SVECTOR b = readLuaSVector(L, 2);
	SVECTOR r; VecMulInteger(&r, &a, &b);
	writeLuaSVector(L, r);
	return 1;
}

int lua_vectorDivLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	const VECTOR b = readLuaVector(L, 2);
	VECTOR r; VecDiv(&r, &a, &b);
	writeLuaVector(L, r);
	return 1;
}

int lua_vectorMulSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	const SVECTOR b = readLuaSVector(L, 2);
	SVECTOR r; VecMul(&r, &a, &b);
	writeLuaSVector(L, r);
	return 1;
}

int lua_vectorMulLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	const VECTOR b = readLuaVector(L, 2);
	VECTOR r; VecMul(&r, &a, &b);
	writeLuaVector(L, r);
	return 1;
}

int lua_normalizeVectorLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	VECTOR n; VecNormalize(&n, &a);
	writeLuaVector(L, n);
	return 1;
}

int lua_normalizeVectorSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	SVECTOR n; VecNormalize(&n, &a);
	writeLuaSVector(L, n);
	return 1;
}

int lua_vectorLengthSV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	PSX_INT_TYPE l; VecLength(l, &a);
	lua_push_number_to_integer(L, l);
	return 1;
}

int lua_vectorLengthLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	PSX_INT_TYPE l; VecLength(l, &a);
	lua_push_number_to_integer(L, l);
	return 1;
}

int lua_vectorCopySV(lua_State* L)
{
	const SVECTOR a = readLuaSVector(L, 1);
	writeLuaSVector(L, a);
	return 1;
}

int lua_vectorCopyLV(lua_State* L)
{
	const VECTOR a = readLuaVector(L, 1);
	writeLuaVector(L, a);
	return 1;
}

int lua_vectorInvertSV(lua_State* L)
{
	SVECTOR a = readLuaSVector(L, 1);
	a.vx = -a.vx;
	a.vy = -a.vy;
	a.vz = -a.vz;
	a.pad = -a.pad;
	writeLuaSVector(L, a);
	return 1;
}

int lua_vectorInvertLV(lua_State* L)
{
	VECTOR a = readLuaVector(L, 1);
	a.vx = -a.vx;
	a.vy = -a.vy;
	a.vz = -a.vz;
	a.pad = -a.pad;
	writeLuaVector(L, a);
	return 1;
}

int lua_projectOnPlane(lua_State* L)
{
	const SVECTOR vector = readLuaSVector(L, 1);
	const SVECTOR plane = readLuaSVector(L, 2);
	PSX_INT_TYPE vl; VecLength(vl, &plane);
	PSX_INT_TYPE vDotP; VecDot(vDotP, &vector, &plane);
	const PSX_INT_TYPE vDotPdVl = fixedDiv(vDotP, vl);
	SVECTOR vScaled; VecMulInteger(&vScaled, &plane, vDotPdVl);
	SVECTOR rejected; VecSub(&rejected, &vector, &vScaled);
	writeLuaSVector(L, rejected);
	return 1;
}

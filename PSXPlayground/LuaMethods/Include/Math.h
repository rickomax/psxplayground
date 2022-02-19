#pragma once
#include "libgte.h"
#include "lua.h"

#define VecCopy(vecOut, vecLeft) \
{ \
	(vecOut)->vx =(vecLeft)->vx; \
	(vecOut)->vy =(vecLeft)->vy; \
	(vecOut)->vz =(vecLeft)->vz; \
}

#define VecMul(vecOut, vecLeft, vecRight) \
{ \
	(vecOut)->vx = fixedMul((vecLeft)->vx, (vecRight)->vx); \
	(vecOut)->vy = fixedMul((vecLeft)->vy, (vecRight)->vy); \
	(vecOut)->vz = fixedMul((vecLeft)->vz, (vecRight)->vz); \
}

#define VecMulInteger(vecOut, vecLeft, right) \
{ \
	(vecOut)->vx = fixedMul((vecLeft)->vx, right); \
	(vecOut)->vy = fixedMul((vecLeft)->vy, right); \
	(vecOut)->vz = fixedMul((vecLeft)->vz, right); \
}

#define VecDiv(vecOut, vecLeft, vecRight) \
{ \
	(vecOut)->vx = fixedDiv((vecLeft)->vx, (vecRight)->vx); \
	(vecOut)->vy = fixedDiv((vecLeft)->vy, (vecRight)->vy); \
	(vecOut)->vz = fixedDiv((vecLeft)->vz, (vecRight)->vz); \
}

#define VecAdd(vecOut, vecLeft, vecRight) \
{ \
	(vecOut)->vx = (vecLeft)->vx + (vecRight)->vx; \
	(vecOut)->vy = (vecLeft)->vy + (vecRight)->vy; \
	(vecOut)->vz = (vecLeft)->vz + (vecRight)->vz; \
}

#define VecSub(vecOut, vecLeft, vecRight) \
{ \
	(vecOut)->vx = (vecLeft)->vx - (vecRight)->vx; \
	(vecOut)->vy = (vecLeft)->vy - (vecRight)->vy; \
	(vecOut)->vz = (vecLeft)->vz - (vecRight)->vz; \
}

#define VecNegate(vecOut, vecLeft, vecRight) \
{ \
	(vecOut)->vx = -(vecRight)->vx; \
	(vecOut)->vy = -(vecRight)->vy; \
	(vecOut)->vz = -(vecRight)->vz; \
}

#define VecLength(out, vecLeft) (out) = SquareRoot0((vecLeft)->vx * (vecLeft)->vx + (vecLeft)->vy * (vecLeft)->vy + (vecLeft)->vz * (vecLeft)->vz);

#define VecCross(vecOut, vecLeft, vecRight) \
{ \
	(vecOut)->vx = (((vecLeft)->vy * (vecRight)->vz) - ((vecLeft)->vz * (vecRight)->vy)) / ONE; \
	(vecOut)->vy = (((vecLeft)->vz * (vecRight)->vx) - ((vecLeft)->vx * (vecRight)->vz)) / ONE; \
	(vecOut)->vz = (((vecLeft)->vx * (vecRight)->vy) - ((vecLeft)->vy * (vecRight)->vx)) / ONE; \
}

#define VecDot(out, vecLeft, vecRight) (out) = ((vecLeft)->vx* (vecRight)->vx + (vecLeft)->vy * (vecRight)->vy + (vecLeft)->vz * (vecRight)->vz) / ONE;

#define VecNormalize(out, vecLeft) \
{ \
	PSX_INT_TYPE nLen; VecLength(nLen, vecLeft); \
	PSX_INT_TYPE invNLen = fixedDiv(ONE, nLen);  \
	VecMulInteger(out, vecLeft, invNLen);  \
}

#define ColorMul(a,b) (((a)*(b)/255))
#define ColorSum(a,b) (((a)+(b)/255))

PSX_INT_TYPE iabs(PSX_INT_TYPE number);
PSX_INT_TYPE clamp(PSX_INT_TYPE n, PSX_INT_TYPE min, PSX_INT_TYPE max);
PSX_INT_TYPE lerp(PSX_INT_TYPE a, PSX_INT_TYPE b, PSX_INT_TYPE f);
void lookAt(VECTOR* eye, VECTOR* at, SVECTOR* up, SVECTOR* xaxis, SVECTOR* yaxis, SVECTOR* zaxis);
MATRIX invertMatrix(MATRIX* matrix);
int lua_iclamp(lua_State* L);
int lua_isin(lua_State *L);
int lua_icos(lua_State *L);
int lua_iatan2(lua_State* L);
int lua_lookat(lua_State* L);
int lua_idot(lua_State* L);
int lua_ilerp(lua_State* L);
int lua_ilerpAngle(lua_State* L);
int lua_vectorMulIntegerSV(lua_State* L);
int lua_vectorMulIntegerLV(lua_State* L);
int lua_vectorAddSV(lua_State* L);
int lua_vectorAddLV(lua_State* L);
int lua_vectorSubSV(lua_State* L);
int lua_vectorSubLV(lua_State* L);
int lua_vectorDivSV(lua_State* L);
int lua_vectorDivLV(lua_State* L);
int lua_vectorMulSV(lua_State* L);
int lua_vectorMulLV(lua_State* L);
int lua_normalizeVectorLV(lua_State* L);
int lua_normalizeVectorSV(lua_State* L);
int lua_vectorLengthSV(lua_State* L);
int lua_vectorLengthLV(lua_State* L);
int lua_vectorCopyLV(lua_State* L);
int lua_vectorCopySV(lua_State* L);
int lua_vectorInvertLV(lua_State* L);
int lua_vectorInvertSV(lua_State* L);
int lua_projectOnPlane(lua_State* L);

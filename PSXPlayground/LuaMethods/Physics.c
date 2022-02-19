#include <string.h>
#include <stdio.h>

#include "Include/Physics.h"
#include "Include/Common.h"
#include "Include/Math.h"
#include "../Include/PSXPlayground.h"

VECTOR closestPointOnTriangle(VECTOR* sv0, VECTOR* sv1, VECTOR* sv2, VECTOR* p) {

	VECTOR edge0; VecSub(&edge0, sv1, sv0);
	VECTOR edge1; VecSub(&edge1, sv2, sv0);
	VECTOR v0; VecSub(&v0, sv0, p);

	PSX_INT_TYPE a = VecDot(a, &edge0, &edge0);
	PSX_INT_TYPE b = VecDot(b, &edge0, &edge1);
	PSX_INT_TYPE c = VecDot(c, &edge1, &edge1);
	PSX_INT_TYPE d = VecDot(d, &edge0, &v0);
	PSX_INT_TYPE e = VecDot(e, &edge1, &v0);

	PSX_INT_TYPE det = fixedMul(a, c) - fixedMul(b, b);
	PSX_INT_TYPE s = fixedMul(b, e) - fixedMul(c, d);
	PSX_INT_TYPE t = fixedMul(b, d) - fixedMul(a, e);

	if (s + t < det)
	{
		if (s < 0)
		{
			if (t < 0)
			{
				if (d < 0)
				{
					s = clamp(fixedDiv(-d, a), 0, ONE);
					t = 0;
				}
				else
				{
					s = 0;
					t = clamp(fixedDiv(-e, c), 0, ONE);
				}
			}
			else
			{
				s = 0;
				t = clamp(fixedDiv(-e, c), 0, ONE);
			}
		}
		else if (t < 0)
		{
			s = clamp(fixedDiv(-d, a), 0, ONE);
			t = 0;
		}
		else
		{
			PSX_INT_TYPE invDet = fixedDiv(ONE, det);
			s = fixedMul(s, invDet);
			t = fixedMul(t, invDet);
		}
	}
	else
	{
		if (s < 0)
		{
			PSX_INT_TYPE tmp0 = b + d;
			PSX_INT_TYPE tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				PSX_INT_TYPE numer = tmp1 - tmp0;
				PSX_INT_TYPE denom = fixedMul(a - (ONE * 2), b + c);
				s = clamp(fixedDiv(numer, denom), 0, ONE);
				t = ONE - s;
			}
			else
			{
				t = clamp(fixedDiv(-e, c), 0, ONE);
				s = 0;
			}
		}
		else if (t < 0)
		{
			if (a + d > b + e)
			{
				PSX_INT_TYPE numer = c + e - b - d;
				PSX_INT_TYPE denom = fixedMul(a - (ONE * 2), b + c);
				s = clamp(fixedDiv(numer, denom), 0, ONE);
				t = ONE - s;
			}
			else
			{
				s = clamp(fixedDiv(-e, c), 0, ONE);
				t = 0;
			}
		}
		else
		{
			PSX_INT_TYPE numer = c + e - b - d;
			PSX_INT_TYPE denom = fixedMul(a - (ONE * 2), b + c);
			s = clamp(fixedDiv(numer, denom), 0, ONE);
			t = ONE - s;
		}
	}

	VECTOR es0; VecMulInteger(&es0, &edge0, s);
	VECTOR et0; VecMulInteger(&et0, &edge1, t);
	VECTOR result; VecAdd(&result, sv0, &es0);
	VecAdd(&result, &result, &et0);
	return result;
}

//todo: triangle is back facing
VECTOR sphereTriangleIntersection(OBJECT_ELEMENT* elements, MATRIX* matrix, VECTOR* p, PSX_INT_TYPE r)
{
	VECTOR up;
	setVector(&up, 0, ONE, 0, 0);

	VECTOR separation;
	setVector(&separation, 0, 0, 0, 0);
	
	while (1)
	{
		if (!elements->isSet)
		{
			break;
		}
		VECTOR a, b, c;
		ApplyMatrix(matrix, &elements->v[0], &a);
		ApplyMatrix(matrix, &elements->v[1], &b);
		ApplyMatrix(matrix, &elements->v[2], &c);

		VECTOR trans;
		trans.vx = matrix->t[0];
		trans.vy = matrix->t[1];
		trans.vz = matrix->t[2];
		
		VecAdd(&a, &a, &trans);
		VecAdd(&b, &b, &trans);
		VecAdd(&c, &c, &trans);

		VECTOR t = closestPointOnTriangle(&a, &b, &c, p);

		VECTOR relativeT; VecSub(&relativeT, p, &t);
		PSX_INT_TYPE relativeTLength; VecLength(relativeTLength, &relativeT);
		const PSX_INT_TYPE distance = iabs(relativeTLength);
		if (distance <= r)
		{
			VECTOR edge0; VecSub(&edge0, &b, &a);
			VECTOR edge1; VecSub(&edge1, &c, &a);
			VECTOR n; VecCross(&n, &edge0, &edge1);
			VecNormalize(&n, &n);
			const PSX_INT_TYPE distanceToBorder = r - distance;
			VECTOR displacement; VecMulInteger(&displacement, &n, distanceToBorder);
			VecAdd(&separation, &separation, &displacement);
		}
	nextElement:
		elements++;
	}
	return separation;
}

PSX_INT_TYPE raycast(OBJECT_ELEMENT* elements, MATRIX* matrix, VECTOR* rayOrigin, SVECTOR* rayVector, SVECTOR* hitNormal, PSX_INT_TYPE* elementIndex)
{
	VECTOR up;
	setVector(&up, 0, -ONE, 0, 0);

	const PSX_INT_TYPE epsilon = 8;
	PSX_INT_TYPE minT = PSX_INT_MAX;
	PSX_INT_TYPE curElementIndex = 0;
	while (1)
	{
		if (!elements->isSet)
		{
			break;
		}
		VECTOR v0, v1, v2;
		ApplyMatrix(matrix, &elements->v[2], &v0);
		ApplyMatrix(matrix, &elements->v[1], &v1);
		ApplyMatrix(matrix, &elements->v[0], &v2);

		VECTOR trans;
		trans.vx = matrix->t[0];
		trans.vy = matrix->t[1];
		trans.vz = matrix->t[2];

		VecAdd(&v0, &v0, &trans);
		VecAdd(&v1, &v1, &trans);
		VecAdd(&v2, &v2, &trans);

		VECTOR v0v1; VecSub(&v0v1, &v1, &v0);
		VECTOR v0v2; VecSub(&v0v2, &v2, &v0);
		VECTOR pvec; VecCross(&pvec, rayVector, &v0v2);
		PSX_INT_TYPE det; VecDot(det, &v0v1, &pvec);

		// if the determinant is negative the triangle is backfacing
		// if the determinant is close to 0, the ray misses the triangle
		if (det <= epsilon)
		{
			goto nextElement;
		}

		// ray and triangle are parallel if det is close to 0
		if (det >= -epsilon && det <= epsilon)
		{
			goto nextElement;
		}
		
		PSX_INT_TYPE invDet = fixedDiv(ONE, det);

		VECTOR tvec; VecSub(&tvec, rayOrigin, &v0);
		PSX_INT_TYPE tDotP; VecDot(tDotP , &tvec, &pvec);
		PSX_INT_TYPE u = fixedMul(tDotP, invDet);
		if (u < 0 || u > ONE)
		{
			goto nextElement;
		}

		VECTOR qvec; VecCross(&qvec, &tvec, &v0v1);
		PSX_INT_TYPE rDotQ; VecDot(rDotQ, rayVector, &qvec);
		PSX_INT_TYPE v = fixedMul(rDotQ, invDet);
		if (v < 0 || u + v > ONE)
		{
			goto nextElement;
		}

		PSX_INT_TYPE vDotQ; VecDot(vDotQ , &v0v2, &qvec);
		PSX_INT_TYPE t = fixedMul(vDotQ, invDet);
		
		if (t > epsilon && t <= minT) // ray intersection
		{
			SVECTOR n; VecCross(&n, &v0v1, &v0v2);
			VecNormalize(&n, &n);
			*hitNormal = n;
			*elementIndex = curElementIndex;
			minT = t;
		}
	nextElement:
		elements++;
		curElementIndex++;
	}
	return minT == PSX_INT_MAX ? -1 : minT;
}

int lua_sphereOBJIntersection(lua_State* L) {
	const PSX_INT_TYPE objId = lua_number_to_integer(L, 1);
	if (objId < 1 || objId > objectCount)
	{
		return 0;
	}
	OBJECT_ELEMENT* elements = &objects[objId - 1][0];
	MATRIX matrix = readLuaMatrix(L, 2);
	VECTOR position = readLuaVector(L, 3);
	const PSX_INT_TYPE radius = lua_number_to_integer(L, 4);
	const VECTOR separation = sphereTriangleIntersection(elements, &matrix, &position, radius);
	writeLuaVector(L, separation);
	return 1;
}

int lua_raycast(lua_State* L) {
	const int objId = lua_number_to_integer(L, 1);
	if (objId < 1 || objId > objectCount)
	{
		return 0;
	}
	OBJECT_ELEMENT* elements = &objects[objId - 1][0];
	MATRIX matrix = readLuaMatrix(L, 2);
	VECTOR rayOrigin = readLuaVector(L, 3);
	SVECTOR rayVector = readLuaSVector(L, 4);
	SVECTOR hitNormal;
	setVector(&hitNormal, 0, 0, 0, 0);
	PSX_INT_TYPE elementIndex = 0;
	const PSX_INT_TYPE result = raycast(elements, &matrix, &rayOrigin, &rayVector, &hitNormal, &elementIndex);
	lua_push_number_to_integer(L, result);
	VECTOR hitPoint;
	if (result < 0)
	{
		VecCopy(&hitPoint, &rayOrigin);
	}
	else {
		VECTOR vec;
		VecMulInteger(&vec, &rayVector, result);
		VecAdd(&hitPoint, &rayOrigin, &vec);
	}
	writeLuaVector(L, hitPoint);
	writeLuaSVector(L, hitNormal);
	lua_push_number_to_integer(L, elementIndex);
	return 4;
}

int lua_linecast(lua_State* L) {
	const int objId = lua_number_to_integer(L, 1);
	if (objId < 1 || objId > objectCount)
	{
		return 0;
	}
	OBJECT_ELEMENT* elements = &objects[objId - 1][0];
	MATRIX matrix = readLuaMatrix(L, 2);
	VECTOR rayOrigin = readLuaVector(L, 3);
	VECTOR rayEnd = readLuaVector(L, 4);
	VECTOR rayVector; VecSub(&rayVector, &rayEnd, &rayOrigin);
	SVECTOR nRayVector; VecNormalize(&nRayVector, &rayVector);
	PSX_INT_TYPE maxLength; VecLength(maxLength, &rayVector);
	SVECTOR hitNormal;
	setVector(&hitNormal, 0, 0, 0, 0);
	PSX_INT_TYPE elementIndex = 0;
	const PSX_INT_TYPE result = raycast(elements, &matrix, &rayOrigin, &nRayVector, &hitNormal, &elementIndex);
	lua_push_number_to_integer(L, result);
	VECTOR hitPoint;
	if (result < 0 || result > maxLength)
	{
		VecAdd(&hitPoint, &rayOrigin, &rayVector);
	}
	else {
		VECTOR vec;
		VecMulInteger(&vec, &nRayVector, result);
		VecAdd(&hitPoint, &rayOrigin, &vec);
	}
	writeLuaVector(L, hitPoint);
	writeLuaSVector(L, hitNormal);
	lua_push_number_to_integer(L, elementIndex);
	return 4;
}

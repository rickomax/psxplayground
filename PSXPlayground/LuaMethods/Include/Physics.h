#pragma once
#include "libgte.h"
#include "lua.h"
#include "../../Include/PSXPlayground.h"

PSX_INT_TYPE raycast(OBJECT_ELEMENT* elements, MATRIX* matrix, VECTOR* rayOrigin, SVECTOR* rayVector, SVECTOR* hitNormal, PSX_INT_TYPE* elementIndex);
VECTOR sphereTriangleIntersection(OBJECT_ELEMENT* elements, MATRIX* matrix, VECTOR* p, PSX_INT_TYPE r, PSX_INT_TYPE maxUpDot);
VECTOR closestPointOnTriangle(VECTOR* sv0, VECTOR* sv1, VECTOR* sv2, VECTOR* p);
int lua_raycast(lua_State* L);
int lua_linecast(lua_State* L);
int lua_sphereOBJIntersection(lua_State* L);

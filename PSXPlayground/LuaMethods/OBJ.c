#include "Include/OBJ.h"
#include "Include/Math.h"
#include "Include/BMP.h"
#include "Include/Common.h"
#include "Include/Math.h"
#include "Include/GPUCustom.h"
#include "lauxlib.h"
#include "../Include/PSXPlayground.h"
#include "../Include/OBJ.h"

int loadOBJ(const char* filename, const int modelScale, const int abr, const int magicPink, const int transparentBlack, const int timDepth, int* objId)
{
	tinyobj_attrib_t attrib;
	tinyobj_shape_t* shapes = NULL;
	size_t num_shapes;
	tinyobj_material_t* materials = NULL;
	size_t num_materials;
	const int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, filename, get_file_data, NULL, TINYOBJ_FLAG_TRIANGULATE);
	if (ret == TINYOBJ_SUCCESS) {
		OBJECT_ELEMENT* elements = &objects[objectCount++][0];
		size_t face_offset = 0;

		for (size_t i = 0; i < attrib.num_face_num_verts; i++) {
			for (size_t f = 0; f < (size_t)attrib.face_num_verts[i] / 3; f++) {
				float v[3][3];
				float c[3][3];
				float n[3][3];
				float t[3][2];

				memset(v, 0, sizeof(v));
				memset(c, 0, sizeof(c));
				memset(n, 0, sizeof(n));
				memset(t, 0, sizeof(t));

				tinyobj_vertex_index_t idx0 = attrib.faces[face_offset + 3 * f + 0];
				tinyobj_vertex_index_t idx1 = attrib.faces[face_offset + 3 * f + 1];
				tinyobj_vertex_index_t idx2 = attrib.faces[face_offset + 3 * f + 2];

				for (size_t k = 0; k < 3; k++) {
					int f0 = idx0.v_idx;
					int f1 = idx1.v_idx;
					int f2 = idx2.v_idx;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);
					v[0][k] = attrib.vertices[3 * (size_t)f0 + k];
					v[1][k] = attrib.vertices[3 * (size_t)f1 + k];
					v[2][k] = attrib.vertices[3 * (size_t)f2 + k];

					c[0][k] = attrib.colors[3 * (size_t)f0 + k];
					c[1][k] = attrib.colors[3 * (size_t)f1 + k];
					c[2][k] = attrib.colors[3 * (size_t)f2 + k];
				}

				if (attrib.num_normals > 0) {
					int f0 = idx0.vn_idx;
					int f1 = idx1.vn_idx;
					int f2 = idx2.vn_idx;
					if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
						assert(f0 < (int)attrib.num_normals);
						assert(f1 < (int)attrib.num_normals);
						assert(f2 < (int)attrib.num_normals);
						for (size_t k = 0; k < 3; k++) {
							n[0][k] = attrib.normals[3 * (size_t)f0 + k];
							n[1][k] = attrib.normals[3 * (size_t)f1 + k];
							n[2][k] = attrib.normals[3 * (size_t)f2 + k];
						}
					}
				}

				if (attrib.num_texcoords > 0) {
					int f0 = idx0.vt_idx;
					int f1 = idx1.vt_idx;
					int f2 = idx2.vt_idx;
					if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
						assert(f0 < (int)attrib.num_texcoords);
						assert(f1 < (int)attrib.num_texcoords);
						assert(f2 < (int)attrib.num_texcoords);
						for (size_t k = 0; k < 2; k++) {
							t[0][k] = attrib.texcoords[2 * (size_t)f0 + k];
							t[1][k] = attrib.texcoords[2 * (size_t)f1 + k];
							t[2][k] = attrib.texcoords[2 * (size_t)f2 + k];
						}
					}
				}

				for (size_t k = 0; k < 3; k++) {
					SVECTOR* vector = &elements->v[k];
					*((short*)vector + 0) = v[k][2] * modelScale;
					*((short*)vector + 1) = v[k][1] * modelScale;
					*((short*)vector + 2) = v[k][0] * modelScale;

					SVECTOR* normal = &elements->n[k];
					*((short*)normal + 0) = n[k][2] * 4095;
					*((short*)normal + 1) = n[k][1] * 4095;
					*((short*)normal + 2) = n[k][0] * 4095;

					TVECTOR* texcoord = &elements->t[k];
					*((u_char*)texcoord + 0) = t[k][0] * 255;
					*((u_char*)texcoord + 1) = (1.0f - t[k][1]) * 255;

					CVECTOR* color = &elements->rgb[k];
					*((u_char*)color + 0) = c[k][0] * 255;
					*((u_char*)color + 1) = c[k][1] * 255;
					*((u_char*)color + 2) = c[k][2] * 255;
					*((u_char*)color + 3) = 128;
				}
			}

			if (materials) {
				int materialId = attrib.material_ids[i];
				if (materialId >= 0) {
					tinyobj_material_t material = materials[materialId];
					if (material.diffuse_texname != 0)
					{
						u_short tPage = 0;
						int w;
						int h;
						int x;
						int y;
						loadBMPInternal(material.diffuse_texname, magicPink, transparentBlack, timDepth, abr, &tPage, &w, &h, &x, &y);

						//todo: 64 is based on a 320 screen width
						int uOffset = (x % 256) - 64;
						int vOffset = y % 256;

						for (size_t k = 0; k < 3; k++) {
							//todo: better way?
							elements->t[k].vx = (((float)elements->t[k].vx / 255.0f) * ((float)w / 255.0f) * 255) + uOffset;
							elements->t[k].vy = (((float)elements->t[k].vy / 255.0f) * ((float)h / 255.0f) * 255) + vOffset;
						}

						elements->tPage = tPage;
					}
					for (size_t k = 0; k < 3; k++) {
						elements->rgb[k].r *= material.diffuse[0];
						elements->rgb[k].g *= material.diffuse[1];
						elements->rgb[k].b *= material.diffuse[2];
						elements->rgb[k].cd = 128;
						elements->semiTrans = material.dissolve <= 0.5f;
					}
				}
			}

			elements->isSet = 1;
			elements++;

			face_offset += (size_t)attrib.face_num_verts[i];
		}
		*objId = objectCount;
		return 1;
	}
	return 0;
}

int lua_loadOBJ(lua_State* L)
{
	const char* filename = lua_tostring(L, 1);
	const int modelScale = lua_number_to_integer(L, 2);
	const int abr = lua_number_to_integer(L, 3);
	const int magicPink = lua_number_to_integer(L, 4);
	const int transparentBlack = lua_number_to_integer(L, 5);
	const int timDepth = lua_number_to_integer(L, 6);
	int objId;
	if (loadOBJ(filename, modelScale, abr, magicPink, transparentBlack, timDepth, &objId)) {
		lua_push_number_to_integer(L, objId);
		return 1;
	}
	lua_push_number_to_integer(L, 0);
	return 1;
}

void drawOBJ(MATRIX* matrix, MATRIX* finalMatrix, const int enableCulling, const int wireframe, VECTOR objRGB, OBJECT_ELEMENT* elements, LIGHT* light)
{
	SetRotMatrix(finalMatrix);
	SetTransMatrix(finalMatrix);
	
	CVECTOR white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	white.cd = 128;
	unsigned char rgb0[4];
	unsigned char rgb1[4];
	unsigned char rgb2[4];

	while (1)
	{
		if (!elements->isSet)
		{
			break;
		}

		gte_ldv3(&elements->v[0], &elements->v[1], &elements->v[2]);
		gte_rtpt();
		long p;
		//if (!wireframe) {
			gte_nclip();
			gte_stopz(&p);
		//}
		//else
		//{
		//	p = 1;
		//}
		if (enableCulling && p > 0 || !enableCulling)
		{
			gte_avsz3();
			gte_stotz(&p);
			if (p > 0)
			{
				const long otIndex = getOTIndex(p);
				if (otIndex < OT_LEN)
				{
					nextPrimitive();
					//if (wireframe) {
					//	LINE_G4* primitivePtr = (LINE_G4*)primitive;
					//	setLineG4(primitivePtr);
					//
					//	gte_stsxy0(&primitivePtr->x0);
					//	gte_stsxy1(&primitivePtr->x1);
					//	gte_stsxy1(&primitivePtr->x2);
					//	gte_stsxy2(&primitivePtr->x3);
					//
					//	primitivePtr->r0 = objRGB.vx;
					//	primitivePtr->r1 = objRGB.vx;
					//	primitivePtr->r2 = objRGB.vx;
					//
					//	primitivePtr->g0 = objRGB.vy;
					//	primitivePtr->g1 = objRGB.vy;
					//	primitivePtr->g2 = objRGB.vy;
					//
					//	primitivePtr->b0 = objRGB.vz;
					//	primitivePtr->b1 = objRGB.vz;
					//	primitivePtr->b2 = objRGB.vz;
					//
					//	if (elements->semiTrans)
					//	{
					//		SetSemiTrans(primitivePtr, 1);
					//	}
					//}
					//else
					//{
						POLY_GT3* primitivePtr = (POLY_GT3*)primitive;
						setPolyGT3(primitivePtr);

						gte_stsxy0(&primitivePtr->x0);
						gte_stsxy1(&primitivePtr->x1);
						gte_stsxy2(&primitivePtr->x2);

						gte_ldrgb(&white);
						gte_ldv3(&elements->n[0], &elements->n[1], &elements->n[2]);
						gte_ncct();

						gte_strgb3(&rgb0, &rgb1, &rgb2);
						primitivePtr->r0 = ColorMul(ColorMul(rgb0[0], elements->rgb[0].r), objRGB.vx);
						primitivePtr->r1 = ColorMul(ColorMul(rgb1[0], elements->rgb[1].r), objRGB.vx);
						primitivePtr->r2 = ColorMul(ColorMul(rgb2[0], elements->rgb[2].r), objRGB.vx);

						primitivePtr->g0 = ColorMul(ColorMul(rgb0[1], elements->rgb[0].g), objRGB.vy);
						primitivePtr->g1 = ColorMul(ColorMul(rgb1[1], elements->rgb[1].g), objRGB.vy);
						primitivePtr->g2 = ColorMul(ColorMul(rgb2[1], elements->rgb[2].g), objRGB.vy);

						primitivePtr->b0 = ColorMul(ColorMul(rgb0[2], elements->rgb[0].b), objRGB.vz);
						primitivePtr->b1 = ColorMul(ColorMul(rgb1[2], elements->rgb[1].b), objRGB.vz);
						primitivePtr->b2 = ColorMul(ColorMul(rgb2[2], elements->rgb[2].b), objRGB.vz);

						if (light != 0)
						{
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
						
							VECTOR toLight0; VecSub(&toLight0, &v0, &light->Pos);
							VECTOR toLight1; VecSub(&toLight1, &v1, &light->Pos);
							VECTOR toLight2; VecSub(&toLight2, &v2, &light->Pos);
						
							PSX_INT_TYPE length0; VecLength(length0, &toLight0);
							PSX_INT_TYPE length1; VecLength(length1, &toLight1);
							PSX_INT_TYPE length2; VecLength(length2, &toLight2);
						
							PSX_INT_TYPE factor0 = clamp(fixedDiv(light->Data.pad, length0), 0, ONE);
							PSX_INT_TYPE factor1 = clamp(fixedDiv(light->Data.pad, length1), 0, ONE);
							PSX_INT_TYPE factor2 = clamp(fixedDiv(light->Data.pad, length2), 0, ONE);
							
							VECTOR color0; VecMulInteger(&color0, &light->Data, factor0);
							VECTOR color1; VecMulInteger(&color1, &light->Data, factor1);
							VECTOR color2; VecMulInteger(&color2, &light->Data, factor2);
						
							primitivePtr->r0 = ColorMul(primitivePtr->r0, color2.vx);
							primitivePtr->r1 = ColorMul(primitivePtr->r1, color1.vx);
							primitivePtr->r2 = ColorMul(primitivePtr->r2, color0.vx);
													
							primitivePtr->g0 = ColorMul(primitivePtr->g0, color2.vy);
							primitivePtr->g1 = ColorMul(primitivePtr->g1, color1.vy);
							primitivePtr->g2 = ColorMul(primitivePtr->g2, color0.vy);
													
							primitivePtr->b0 = ColorMul(primitivePtr->b0, color2.vz);
							primitivePtr->b1 = ColorMul(primitivePtr->b1, color1.vz);
							primitivePtr->b2 = ColorMul(primitivePtr->b2, color0.vz);
						}
						//todo: hacky, have to reset these
						SetRotMatrix(finalMatrix);
						SetTransMatrix(finalMatrix);

						if (elements->tPage > 0)
						{
							primitivePtr->tpage = elements->tPage;

							primitivePtr->u0 = elements->t[0].vx;
							primitivePtr->u1 = elements->t[1].vx;
							primitivePtr->u2 = elements->t[2].vx;

							primitivePtr->v0 = elements->t[0].vy;
							primitivePtr->v1 = elements->t[1].vy;
							primitivePtr->v2 = elements->t[2].vy;
						}

						if (elements->semiTrans)
						{
							SetSemiTrans(primitivePtr, 1);
						}
					//}

					OTTYPE* otPtr = &ot[0][0];
					addPrim(otPtr + otIndex, primitive);
				}
			}
		}
		elements++;
	}
}

int lua_drawOBJ(lua_State* L)
{
	const PSX_INT_TYPE objId = lua_number_to_integer(L, 1);
	if (objId < 1 || objId > objectCount)
	{
		return 0;
	}
	const MATRIX matrix = readLuaMatrix(L, 2);
	const MATRIX invMatrix = readLuaMatrix(L, 3);
	const int enableCulling = lua_toboolean(L, 4);
	const int wireframe = lua_toboolean(L, 5);
	VECTOR objRGB = readLuaVector(L, 6);
	LIGHT light;
	light.Pos = readLuaVector(L, 7);
	light.Data = readLuaVector(L, 8);
	int hasLightData; VecLength(hasLightData, &light.Data);
	OBJECT_ELEMENT* elements = &objects[objId - 1][0];
	drawOBJ(&matrix, &invMatrix, enableCulling, wireframe, objRGB, elements, hasLightData ? &light : 0);
	return 0;
}

#define CHECK_BOUNDS(a) \
	if ((a).vx < minX) \
	{ \
		minX = (a).vx; \
	} \
	else if ((a).vx > maxX) \
	{ \
		maxX = (a).vx; \
	} \
	if ((a).vy < minY) \
	{ \
		minY = (a).vy; \
	} \
	else if ((a).vy > maxY) \
	{ \
		maxY = (a).vy; \
	} \
	if ((a).vz < minZ) \
	{ \
		minZ = (a).vz; \
	} \
	else if ((a).vz > maxZ) \
	{ \
		maxZ = (a).vz; \
	} \

int lua_getOBJBounds(lua_State* L)
{
	const PSX_INT_TYPE objId = lua_number_to_integer(L, 1);
	if (objId < 1 || objId > objectCount)
	{
		return 0;
	}
	PSX_INT_TYPE minX = PSX_INT_MAX;
	PSX_INT_TYPE minY = PSX_INT_MAX;
	PSX_INT_TYPE minZ = PSX_INT_MAX;
	PSX_INT_TYPE maxX = -PSX_INT_MAX;
	PSX_INT_TYPE maxY = -PSX_INT_MAX;
	PSX_INT_TYPE maxZ = -PSX_INT_MAX;
	OBJECT_ELEMENT* elements = &objects[objId - 1][0];
	MATRIX matrix = readLuaMatrix(L, 2);
	while (1)
	{
		if (!elements->isSet)
		{
			break;
		}
		VECTOR v0, v1, v2;
		ApplyMatrix(&matrix, &elements->v[2], &v0);
		ApplyMatrix(&matrix, &elements->v[1], &v1);
		ApplyMatrix(&matrix, &elements->v[0], &v2);

		VECTOR trans;
		trans.vx = matrix.t[0];
		trans.vy = matrix.t[1];
		trans.vz = matrix.t[2];

		VecAdd(&v0, &v0, &trans);
		VecAdd(&v1, &v1, &trans);
		VecAdd(&v2, &v2, &trans);

		CHECK_BOUNDS(v0);
		CHECK_BOUNDS(v1);
		CHECK_BOUNDS(v2);

		elements++;
	}
	VECTOR min;
	setVector(&min, minX, minY, minZ);
	VECTOR max;
	setVector(&max, maxX, maxY, maxZ);
	VECTOR extents;
	VecSub(&extents, &max, &min);
	extents.vx = extents.vx / 2;
	extents.vy = extents.vy / 2;
	extents.vz = extents.vz / 2;
	VECTOR center;
	center.vx = lerp(min.vx, max.vx, 2048);
	center.vy = lerp(min.vy, max.vy, 2048);
	center.vz = lerp(min.vz, max.vz, 2048);
	writeLuaVector(L, center);
	writeLuaVector(L, extents);
	return 2;
}


int lua_getOBJTriangleInfo(lua_State* L)
{
	SVECTOR color;
	setVector(&color, 255, 255, 255, 255);
	const PSX_INT_TYPE objId = lua_number_to_integer(L, 1);
	if (objId < 1 || objId > objectCount)
	{
		return 0;
	}
	const PSX_INT_TYPE triangleIndex = lua_number_to_integer(L, 2);
	OBJECT_ELEMENT* elements = &objects[objId - 1][0];
	PSX_INT_TYPE elementIndex = 0;
	while (1)
	{
		if (!elements->isSet)
		{
			break;
		}
		if (elementIndex == triangleIndex)
		{
			color.vx = (elements->rgb[0].r + elements->rgb[1].r + elements->rgb[2].r) / 3;
			color.vy = (elements->rgb[0].g + elements->rgb[1].g + elements->rgb[2].g) / 3;
			color.vz = (elements->rgb[0].b + elements->rgb[1].b + elements->rgb[2].b) / 3;
			color.pad = 255;
			break;
		}
		elements++;
		elementIndex++;
	}
	writeLuaSVector(L, color);
	return 1;
}

int lua_debugBounds(lua_State* L)
{
	VECTOR center = readLuaVector(L, 1);
	VECTOR extents = readLuaVector(L, 2);
	VECTOR min; VecSub(&min, &center, &extents);
	VECTOR max; VecAdd(&max, &center, &extents);

	return 0;
}
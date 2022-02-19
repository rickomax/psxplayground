#pragma once
#include "libgte.h"
#include "libgpu.h"
#include "libetc.h"
#include "inline_c.h"
#include "libpad.h"
#include "libspu.h"
#include "lua.h"

//----------------------------------------------------------------------------
// OT
//----------------------------------------------------------------------------
#if defined(PSX)
typedef unsigned long OTTYPE
#elif defined(_M_X64) || defined(__amd64__)
typedef OT_TAG OTTYPE;
#else
typedef unsigned long long OTTYPE;
#endif

//----------------------------------------------------------------------------
// SCREEN/PRIMITIVE BUFFERS
//----------------------------------------------------------------------------
DISPENV disp[1];
DRAWENV draw[1];

//todo: load these from LUA call?
#define MAX_DEPTH 65534
#define MAX_PRIM 4096
#define OT_LEN MAX_DEPTH

#define PRIM_LEN sizeof(POLY_GT4)*MAX_PRIM

OTTYPE ot[1][OT_LEN];

char primitiveBuffer[PRIM_LEN];
char* primitive;

int screenWidth;
int screenHeight;

//----------------------------------------------------------------------------
// OBJ HANDLING
//----------------------------------------------------------------------------
//todo load from LUA call?
#define MAX_OBJS 256

typedef struct {		/* 2D char vector */
	u_char vx, vy;
} TVECTOR;

typedef struct
{
	char isSet;
	SVECTOR v[3];
	SVECTOR n[3];
	TVECTOR t[3];
	CVECTOR rgb[3];
	u_short tPage;
	char clut;
	char semiTrans;
} OBJECT_ELEMENT;


OBJECT_ELEMENT objects[MAX_OBJS][MAX_PRIM];

int objectCount;

//----------------------------------------------------------------------------
// PAD
//----------------------------------------------------------------------------
#define PAD_SELECT      1
#define PAD_L3          2
#define PAD_R3          4
#define PAD_START       8
#define PAD_UP          16
#define PAD_RIGHT       32
#define PAD_DOWN        64
#define PAD_LEFT        128
#define PAD_L2          256
#define PAD_R2          512
#define PAD_L1          1024
#define PAD_R1          2048
#define PAD_TRIANGLE    4096
#define PAD_CIRCLE      8192
#define PAD_CROSS       16384
#define PAD_SQUARE      32768

char padbuff[2][34];
char prevPadbuff[2][34]; //used in delta check

//----------------------------------------------------------------------------
// TEXTURES
//----------------------------------------------------------------------------
//todo: load these from LUA call?
#define MAX_TEXTURES 256
#define MAX_CHARS 1024

void* mrbp; //maxrectbinpack instance

typedef struct
{
	char textureName[MAX_CHARS];
	u_short textureTPage;
	int textureX;
	int textureY;
	int textureWidth;
	int textureHeight;
} TEXTURE;

TEXTURE textures[MAX_TEXTURES];

int textureCount;

//----------------------------------------------------------------------------
// AUDIO
//----------------------------------------------------------------------------
//todo: load these from LUA call?
#define MAX_AUDIO 256

typedef struct
{
	int audioAddress;
	int audioPitch;
} AUDIO;

AUDIO audios[MAX_AUDIO];

int audioCount;

//----------------------------------------------------------------------------
// PSXPLAYGROUND.C
//----------------------------------------------------------------------------
void init(lua_State* L, char* script);
void render(lua_State* L);
#pragma once

#define ONE				4096
#define ONE_BITS		12

#define PH_ONE			256
#define PH_ONE_BITS		8

// math constants
#define FixHalfRound(x, bits)		(((x) + (1 << (bits-1))) >> bits)
#define FixFloorSigned(x, bits)		((x) / (1 << bits)) // in disassembly: ((int(x) < 0 ? int(x) + (1 << bits)-1 : int(x)) >> bits)

#define	FIXEDH(a)		FixHalfRound(a, ONE_BITS)		// Fixed Half Round number
#define	FIXED(a)		((a) >> ONE_BITS)				// Fixed number (unsigned)

#define RSIN(a)			(rcossin_tbl[((a) & 4095) * 2])
#define RCOS(a)			(rcossin_tbl[((a) & 4095) * 2 + 1])

#define PSX_INT_TYPE int
#define PSX_INT_MAX INT_MAX

PSX_INT_TYPE fixed(const float f);

PSX_INT_TYPE integer(const PSX_INT_TYPE f);

PSX_INT_TYPE strToFixed(const char* s, char** endPtr);

PSX_INT_TYPE fixedDiv(const PSX_INT_TYPE a, const PSX_INT_TYPE b);

PSX_INT_TYPE fixedMul(const PSX_INT_TYPE a, const PSX_INT_TYPE b);

PSX_INT_TYPE fixedMod(const PSX_INT_TYPE a, const PSX_INT_TYPE b);
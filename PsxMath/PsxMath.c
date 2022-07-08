#include "PsxMath.h"

#include <math.h>
#include <stdlib.h>

//PSX_INT_TYPE is the integer type used to represent the float numbers (long in the current implementation)
//fixed converts a float number to a fixed one
//integer converts a fixed number to the rounded integer representation
//strToFixed converts a float string representation to a fixed number
//fixedDiv is used to divide a fixed number
//fixedMul is used to multiply a fixed number
//fixedMod is used to modulate a fixed number

PSX_INT_TYPE fixed(const float f)
{
	return f * ONE;
}

PSX_INT_TYPE integer(const PSX_INT_TYPE f)
{
	return f / ONE;
}

PSX_INT_TYPE strToFixed(const char* s, char** endPtr)
{
	const float f = strtof(s, endPtr);
	return fixed(f);
}

PSX_INT_TYPE fixedDiv(const PSX_INT_TYPE a, const PSX_INT_TYPE b)
{
	if (a == 0 || b == 0)
	{
		return 0;
	}
	return ((long long)a * ONE) / b;
}

PSX_INT_TYPE fixedMul(const PSX_INT_TYPE a, const PSX_INT_TYPE b)
{
	return ((long long)a * (long long)b) / ONE;
}

PSX_INT_TYPE fixedMod(const PSX_INT_TYPE a, const PSX_INT_TYPE b)
{
	if (a == 0 || b == 0)
	{
		return 0;
	}
	return (integer(a) % integer(b)) * ONE;
}

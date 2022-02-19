#include "PsxMath.h"

#include <math.h>
#include <stdlib.h>

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
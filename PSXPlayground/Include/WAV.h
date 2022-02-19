#pragma once
#include "PsxMath.h"

unsigned char* loadWAV(const char* filename, PSX_INT_TYPE enable_looping, int* vag_size, PSX_INT_TYPE* pitch);

static double f[5][2] = { { 0.0, 0.0 },
                            {  -60.0 / 64.0, 0.0 },
                            { -115.0 / 64.0, 52.0 / 64.0 },
                            {  -98.0 / 64.0, 55.0 / 64.0 },
                            { -122.0 / 64.0, 60.0 / 64.0 } };
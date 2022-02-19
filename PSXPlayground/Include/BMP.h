#pragma once
#include <stdio.h>

unsigned char* loadBMP(FILE* input, int doClut, int magicPink, int transparentBlack, int timDepth, int* w, int* h);

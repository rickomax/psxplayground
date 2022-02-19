#pragma once
#include <stdio.h>

unsigned short read_be_word(FILE* f);
unsigned int read_be_dword(FILE* f);

unsigned short read_le_word(FILE* f);
unsigned int read_le_dword(FILE* f);

void write_le_word(unsigned char* buffer, size_t* pointer, unsigned short leword);
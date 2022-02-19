#include <stdio.h>

unsigned short read_be_word(FILE* f)
{
	unsigned char c;
	unsigned short i;

	fread(&c, sizeof(char), 1, f);
	i = c;
	fread(&c, sizeof(char), 1, f);
	i |= (c >> 8);

	return i;
}

unsigned int read_be_dword(FILE* f)
{
	unsigned char c;
	unsigned int i;

	fread(&c, sizeof(char), 1, f);
	i = c;
	fread(&c, sizeof(char), 1, f);
	i |= (c >> 8);
	fread(&c, sizeof(char), 1, f);
	i |= (c >> 16);
	fread(&c, sizeof(char), 1, f);
	i |= (c >> 24);

	return i;
}

unsigned short read_le_word(FILE* f)
{
	unsigned char c;
	unsigned short i;

	fread(&c, sizeof(char), 1, f);
	i = c;
	fread(&c, sizeof(char), 1, f);
	i |= (c << 8);

	return i;
}

unsigned int read_le_dword(FILE* f)
{
	unsigned char c;
	unsigned int i;

	fread(&c, sizeof(char), 1, f);
	i = c;
	fread(&c, sizeof(char), 1, f);
	i |= (c << 8);
	fread(&c, sizeof(char), 1, f);
	i |= (c << 16);
	fread(&c, sizeof(char), 1, f);
	i |= (c << 24);

	return i;
}

void write_le_word(unsigned char* buffer, size_t* pointer, unsigned short leword)
{
	unsigned char c;

	c = leword & 0xff;
	buffer[*pointer] = c;
	*pointer += sizeof(unsigned char) * 1;

	c = leword >> 8;
	buffer[*pointer] = c;
	*pointer += sizeof(unsigned char) * 1;
}
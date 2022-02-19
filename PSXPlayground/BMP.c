/*
 * bmp2tim
 *
 * Converts a bitmap to a TIM image
 *
 * TEST output in various color depths... and check for issues on big-endian machines
 */

#include "Include/BMP.h"
#include "Include/Endian.h"

#include <stdlib.h>
#include <string.h>

enum
{
	BITMAPINFOHEADER,
	BITMAPV2INFOHEADER,
	BITMAPV3INFOHEADER,
	BITMAPV4HEADER,
	BITMAPV5HEADER,
};

typedef struct
{
	unsigned char r, g, b;
}PS_RGB;

typedef struct
{
	int w, h;
	int depth;
	// 0 = r, 1 = g, 2 = b, 3 = alpha
	unsigned int mask[4];
	unsigned int shift[4];
	unsigned int bits[4];
	unsigned char hdr_type;
	void* data;
}PS_BITMAP;

int do_clut = 0;
unsigned int clut_x, clut_y;

unsigned int org_x = 0;
unsigned int org_y = 0;

int tim_depth;

unsigned int tim_flag;

int set_stp_bit = 0;

int transparent_black = 0;
int magic_pink = 0;
int raw_flag = 0;

PS_RGB* ps_default_palette;

PS_BITMAP* ps_create_bitmap(int w, int h, int depth)
{
	PS_BITMAP* bm;

	bm = malloc(sizeof(PS_BITMAP));

	bm->w = w;
	bm->h = h;
	bm->depth = depth;

	switch (depth)
	{
	case 1:
		bm->data = malloc((w * h) / 8);
		break;
	case 4:
		bm->data = malloc((w * h) / 2);
		break;
	case 8:
		bm->data = malloc(w * h);
		break;
	case 15:
	case 16:
		bm->data = malloc((w * h) * 2);
		break;
	case 24:
		bm->data = malloc((w * h) * 3);
		break;
	case 32:
		bm->data = malloc((w * h) * 4);
		break;
	}

	return bm;
}

void ps_destroy_bitmap(PS_BITMAP* bm)
{
	free(bm->data);
	free(bm);
}


PS_BITMAP* ps_load_bitmap(FILE* bf, PS_RGB* palette)
{
	unsigned int bsize;
	unsigned int bisize;
	unsigned int bwidth, bheight, bbpp, boff, bcompr, blw;
	unsigned char* bdata;
	PS_BITMAP* bm;
	int x, y, z, i, l;

	if (bf == NULL)
		return NULL;

	if (read_le_word(bf) != 19778) // 'BM'
	{
		fclose(bf);
		return NULL;
	}

	bsize = read_le_dword(bf);

	// Read bitmap data offset
	fseek(bf, 10, SEEK_SET);
	boff = read_le_dword(bf);

	//	boff=70+;
	//printf("BOFF = %d\n", boff);

	// Read information header size, width and height	

	bisize = read_le_dword(bf);



	bwidth = read_le_dword(bf);
	bheight = read_le_dword(bf);

	//	printf("bwidth = %d, bheight = %d\n", bwidth, bheight);

	// Read BPP

	fseek(bf, 28, SEEK_SET);

	bbpp = read_le_word(bf);

	// Check if there is compression, if there is, abort

	bcompr = read_le_dword(bf);
	//printf("BCOMPR = %d\n", bcompr);

	bm = ps_create_bitmap(bwidth, bheight, bbpp);

	if (palette != NULL && bm->depth <= 8)
	{
		fseek(bf, 14 + bisize, SEEK_SET);

		if (bm->depth == 4) l = 16;
		else if (bm->depth == 8) l = 256;
		else if (bm->depth == 1) l = 2;

		for (x = 0; x < l; x++)
		{
			palette[x].b = fgetc(bf);
			palette[x].g = fgetc(bf);
			palette[x].r = fgetc(bf);
			fgetc(bf);
		}
	}

	// nextvolume FIX 2011-07-08: Now blw (line width with padding) and bwidth 
	// (line width without padding)	are calculated in a much cleaner and correct manner.

	//printf("BPP = %d\n", bbpp);

	bwidth = (bwidth * bbpp) >> 3;
	blw = bwidth;
	if (blw & 3) blw = (blw & ~3) + 4;

	bdata = (unsigned char*)bm->data;

	// Bit mask and colour stuff... Added 2011-07-09

	switch (bisize)
	{
	case 40: bm->hdr_type = BITMAPINFOHEADER; break;
	case 52: bm->hdr_type = BITMAPV2INFOHEADER; break;
	case 56: bm->hdr_type = BITMAPV3INFOHEADER; break;
	case 108: bm->hdr_type = BITMAPV4HEADER; break;
	case 124: bm->hdr_type = BITMAPV5HEADER; break;
	}

	// For now clear Alpha, it will be filled only if it will be found

	bm->mask[3] = 0;
	bm->shift[3] = 0;
	bm->bits[3] = 0;

	if (bm->hdr_type == BITMAPINFOHEADER && bbpp == 16)
	{
		// Old header type and no bitmasks specified - force defaults
		// X1 R5 G5 B5
		bm->mask[2] = 0x1f;
		bm->mask[1] = 0x1f << 5;
		bm->mask[0] = 0x1f << 10;
		bm->shift[2] = 0;
		bm->shift[1] = 5;
		bm->shift[0] = 10;
		bm->bits[2] = 5;
		bm->bits[1] = 5;
		bm->bits[0] = 5;
	}
	else if (bm->hdr_type >= BITMAPV2INFOHEADER)
	{
		fseek(bf, 54, SEEK_SET);

		// Calculate rshift and rbits

		if (bm->hdr_type >= BITMAPV3INFOHEADER)
			l = 4;
		else
			l = 3;

		for (i = 0; i < l; i++)
		{
			bm->mask[i] = read_le_dword(bf);

			y = 0; // rshift
			z = 0; // rbits

			for (x = 31; x >= 0; x--)
			{
				if (bm->mask[i] & (1 << x))
				{
					y = x;
					z++;
				}
			}

			bm->shift[i] = y;
			bm->bits[i] = z;

			//printf("shift[%d] = %d, bits[%d] = %d\n", i, bm->shift[i],
			//	i, bm->bits[i]);
		}
	}

	// Copy data in allocated memory

	for (y = 0; y < bm->h; y++)
	{
		fseek(bf, boff + (blw * (bm->h - (1 + y))), SEEK_SET);

		for (x = 0; x < bwidth; x++)
			fread(&bdata[(y * bwidth) + x], sizeof(char), 1, bf);
	}

	fclose(bf);

	return bm;
}

unsigned int ps_makecol(int r, int g, int b, int a)
{
	return (a << 24) | (r << 16) | (g << 8) | b;
}

unsigned int ps_getr(int c)
{
	return (c >> 16) & 0xff;
}

unsigned int ps_getg(int c)
{
	return (c >> 8) & 0xff;
}

unsigned int ps_getb(int c)
{
	return c & 0xff;
}

unsigned int ps_geta(int c)
{
	return (c >> 24) & 0xff;
}

unsigned int ps_getpixel(PS_BITMAP* bm, int x, int y)
{
	unsigned short shortbuf;
	unsigned int intbuf;
	unsigned char r, g, b, a;
	unsigned char* dataptrb = (unsigned char*)bm->data;
	int off;

	if (bm->depth == 16)
	{
		off = ((y * bm->w) + x) * 2;

		// Little endian, guys...

		shortbuf = dataptrb[off];
		shortbuf |= dataptrb[off + 1] << 8;

		b = ((shortbuf & bm->mask[2]) >> bm->shift[2]) << (8 - bm->bits[2]);
		g = ((shortbuf & bm->mask[1]) >> bm->shift[1]) << (8 - bm->bits[1]);
		r = ((shortbuf & bm->mask[0]) >> bm->shift[0]) << (8 - bm->bits[0]);
		a = ((shortbuf & bm->mask[3]) >> bm->shift[3]) << (8 - bm->bits[3]);

		return ps_makecol(r, g, b, a);
	}
	else if (bm->depth == 24)
	{
		// 24-bit bitmaps never have bitmasks.

		off = ((y * bm->w) + x) * 3;
		r = dataptrb[off + 2];
		g = dataptrb[off + 1];
		b = dataptrb[off];
		a = 255;

		return ps_makecol(r, g, b, 255);
	}
	else if (bm->depth == 32)
	{
		off = ((y * bm->w) + x) * 4;
		/*	r = dataptrb[off+3];
			g = dataptrb[off+2];
			b = dataptrb[off+1];*/
		intbuf = dataptrb[off];
		intbuf |= dataptrb[off + 1] << 8;
		intbuf |= dataptrb[off + 2] << 16;
		intbuf |= dataptrb[off + 3] << 24;

		r = ((intbuf & bm->mask[0]) >> bm->shift[0]) << (8 - bm->bits[0]);
		g = ((intbuf & bm->mask[1]) >> bm->shift[1]) << (8 - bm->bits[1]);
		b = ((intbuf & bm->mask[2]) >> bm->shift[2]) << (8 - bm->bits[2]);
		a = ((intbuf & bm->mask[3]) >> bm->shift[3]) << (8 - bm->bits[3]);

		return ps_makecol(r, g, b, a);
	}
	else if (bm->depth == 8)
	{
		r = dataptrb[(y * bm->w) + x];

		return ps_makecol(ps_default_palette[r].r,
			ps_default_palette[r].g, ps_default_palette[r].b, 255);
	}
	else if (bm->depth == 4)
	{
		off = (y * bm->w) + x;
		off /= 2;

		if (x & 1)
			r = dataptrb[off] & 0xf;
		else
			r = dataptrb[off] >> 4;

		return ps_makecol(ps_default_palette[r].r,
			ps_default_palette[r].g, ps_default_palette[r].b, 255);
	}
	else if (bm->depth == 1)
	{
		off = (y * bm->w) + x;
		off /= 8;

		r = (dataptrb[off] & (1 << (7 - (x & 7)))) ? 1 : 0;

		return ps_makecol(ps_default_palette[r].r,
			ps_default_palette[r].g, ps_default_palette[r].b, 255);
	}

	return 0;
}

unsigned int ps_getpixel_pal(PS_BITMAP* bm, int x, int y)
{
	unsigned char* dataptrb = (unsigned char*)bm->data;
	int off;

	if (bm->depth == 8)
	{
		return dataptrb[(y * bm->w) + x];
	}
	else if (bm->depth == 4)
	{
		off = (y * bm->w) + x;
		off /= 2;

		if (x & 1)
			return dataptrb[off] & 0xf;
		else
			return dataptrb[off] >> 4;
	}
	else if (bm->depth == 1)
	{
		off = (y * bm->w) + x;
		off /= 8;

		return (dataptrb[off] & (1 << (7 - (x & 7)))) ? 1 : 0;
	}

	return 0;
}

unsigned short rgb24_to_rgbpsx(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned short c;

	c = r >> 3;
	c |= (g >> 3) << 5;
	c |= (b >> 3) << 10;

	/*if(set_stp_bit) c|=0x8000;*/
// this code is a bit messy, tidy it up.

	if (c == 0 && !transparent_black)
		c |= 0x8000;

	if (c == ((31) | (31 << 10)) && magic_pink)
		c = 0;

	if (set_stp_bit)
	{
		if (transparent_black && c == 0)
			return c;

		if (magic_pink && c == ((31) | (31 << 10)))
			return c;

		c |= 0x8000;
	}

	return c;
}

unsigned char* loadBMP(FILE* input, int doClut, int magicPink, int transparentBlack, int timDepth, int* w, int* h)
{
	PS_BITMAP* in_bitmap;
	PS_RGB in_palette[256];
	int x, y, z, c, c2;
	unsigned short shortbuf;
	int cx_out = 0;

	do_clut = doClut;
	tim_depth = timDepth;
	magic_pink = magicPink;
	transparent_black = transparentBlack;

	ps_default_palette = in_palette;

	if (do_clut && tim_depth >= 16)
	{
		printf("Images with depths higher than 8-bit can't have a color look up table.\n");
		return 0;
	}

	if (clut_x & 0xf)
	{
		printf("The X position of the CLUT in the framebuffer must be a multiplier of 16.\n");
		return 0;
	}

	switch (tim_depth)
	{
	case 4:
		if (clut_x > (1024 - 16))
			cx_out = 1;
		break;
	case 8:
		if (clut_x > (1024 - 256))
			cx_out = 1;
		break;
	}

	if (cx_out)
	{
		printf("X position specified for CLUT out of bounds.\n");
		return 0;
	}

	if (clut_y >= 512)
	{
		printf("Y position specified for CLUT out of bounds.\n");
		return 0;
	}

	if (do_clut)
		//printf("Generating a Color Look Up Table (CLUT)\n");

	if (tim_depth != 4 && tim_depth != 8 && tim_depth != 16 && tim_depth != 24)
	{
		printf("Invalid color depth specified!\n");
		return 0;
	}

	in_bitmap = ps_load_bitmap(input, in_palette);

	if (in_bitmap == NULL)
	{
		printf("Unable to load bitmap. Unsupported format or file is unreadable or does not exist.\n");
		return 0;
	}

	if (tim_depth == 4 && in_bitmap->depth > 4)
	{
		printf("Error: Only a 4-bit bitmap or a bitmap of lower depth can be used to obtain a 4-bit TIM!\n");
		return 0;
	}

	if (tim_depth == 8 && in_bitmap->depth > 8)
	{
		printf("Error: Only a 8-bit or a bitmap of lower depth can be used to obtain a 8-bit TIM!\n");
		return 0;
	}


	if (in_bitmap == NULL)
	{
		printf("Could not open bitmap. Aborting.\n");
		return 0;
	}

	switch (tim_depth)
	{
	case 4:
		z = in_bitmap->w / 4;
		break;
	case 8:
		z = in_bitmap->w / 2;
		break;
	case 16:
		z = in_bitmap->w;
		break;
	}

	if ((org_x + z) > 1024)
	{
		printf("X position specified for image data out of bounds.\n");
		return 0;
	}

	switch (tim_depth)
	{
	case 4:
		z = in_bitmap->h / 4;
		break;
	case 8:
		z = in_bitmap->h / 2;
		break;
	case 16:
		z = in_bitmap->h;
		break;
	}

	if ((org_y + z) > 512)
	{
		printf("Y position specified for image data out of bounds.\n");
		return 0;
	}

	size_t size;
	unsigned char* outputBuffer = 0;
	size_t pointer = 0;
	switch (tim_depth)
	{
	case 24:
		size = sizeof(unsigned char) * in_bitmap->w * in_bitmap->h * 2 * 3;
		outputBuffer = malloc(size);
		memset(outputBuffer, 0, size);
		for (y = 0; y < in_bitmap->h; y++)
		{
			for (x = 0; x < in_bitmap->w; x += 2)
			{
				c = ps_getpixel(in_bitmap, x, y);
				c2 = ps_getpixel(in_bitmap, x + 1, y);

				write_le_word(outputBuffer, &pointer, (ps_getg(c) << 8) | ps_getr(c));
				write_le_word(outputBuffer, &pointer, (ps_getr(c2) << 8) | ps_getb(c));
				write_le_word(outputBuffer, &pointer, (ps_getb(c2) << 8) | ps_getg(c2));
			}
		}
		break;
	case 16:
		size = sizeof(unsigned char) * in_bitmap->w * in_bitmap->h * 2 * 1;
		outputBuffer = malloc(size);
		memset(outputBuffer, 0, size);
		for (y = 0; y < in_bitmap->h; y++)
		{
			for (x = 0; x < in_bitmap->w; x++)
			{
				c = ps_getpixel(in_bitmap, x, y);
				shortbuf = rgb24_to_rgbpsx(ps_getr(c), ps_getg(c), ps_getb(c));
				write_le_word(outputBuffer, &pointer, shortbuf);
			}
		}
		break;
	case 4:
		size = sizeof(unsigned char) * in_bitmap->w * in_bitmap->h * 2 * 1;
		outputBuffer = malloc(size);
		memset(outputBuffer, 0, size);
		for (y = 0; y < in_bitmap->h; y++)
		{
			for (x = 0; x < in_bitmap->w; x += 4)
			{
				shortbuf = 0;
				for (c = 0; c < 4; c++)
					shortbuf |= (ps_getpixel_pal(in_bitmap, x + c, y) & 0xf) << (c << 2);

				write_le_word(outputBuffer, &pointer, shortbuf);
			}
		}
		break;
	case 8:
		size = sizeof(unsigned char) * in_bitmap->w * in_bitmap->h * 2 * 1;
		outputBuffer = malloc(size);
		memset(outputBuffer, 0, size);
		for (y = 0; y < in_bitmap->h; y++)
		{
			for (x = 0; x < in_bitmap->w; x += 2)
			{
				shortbuf = 0;
				for (c = 0; c < 2; c++)
					shortbuf |= (ps_getpixel_pal(in_bitmap, x + c, y) & 0xff) << (c << 3);

				write_le_word(outputBuffer, &pointer, shortbuf);
			}
		}
		break;
	}

	//printf("Bitmap converted to TIM file successfully!\n");

	*w = in_bitmap->w;
	*h = in_bitmap->h;

	return outputBuffer;
}
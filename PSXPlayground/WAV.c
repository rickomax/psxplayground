/*
 * wav2vag
 *
 * Converts a WAV file to a PlayStation VAG file.
 * Based on PSX VAG-Packer 0.1 by bITmASTER.
 *
 */

#include "Include/WAV.h"
#include "Include/Endian.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#define BUFFER_SIZE 128*28

short wave[BUFFER_SIZE];

void find_predict(short* samples, double* d_samples, int* predict_nr, int* shift_factor);
void pack(double* d_samples, short* four_bit, int predict_nr, int shift_factor);
void fputi(int d, FILE* fp);

unsigned char* loadWAV(const char* filename, PSX_INT_TYPE enable_looping, int* vag_size, PSX_INT_TYPE* pitch)
{
	short* ptr;
	double d_samples[28];
	short four_bit[28];
	int predict_nr;
	int shift_factor;
	int flags;
	int size;
	int i, j;
	unsigned char d;
	char s[4];
	int chunk_data;
	short e;
	int sample_freq = 0, sample_len;
	short sample_size;
	unsigned char c;

	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		printf("Can't open %s. Aborting.\n", filename);
		return 0;
	}

	fread(s, 1, 4, fp);
	if (strncmp(s, "RIFF", 4))
	{
		printf("%s is not in WAV format\n", filename);
		return 0;
	}

	fseek(fp, 8, SEEK_SET);
	fread(s, 1, 4, fp);

	if (strncmp(s, "WAVE", 4))
	{
		printf("%s is not in WAV format\n", filename);
		return 0;
	}

	fseek(fp, 8 + 4, SEEK_SET);
	fread(s, 1, 4, fp);

	if (strncmp(s, "fmt", 3))
	{
		printf("%s is not in WAV format\n", filename);
		return 0;
	}

	chunk_data = read_le_dword(fp);
	chunk_data += ftell(fp);

	e = read_le_word(fp);

	if (e != 1)
	{
		printf("No PCM found in %s. Aborting.\n", filename);
		return 0;
	}

	e = read_le_word(fp);

	if (e != 1)
	{
		printf("WAV file must have only one channel. Aborting.\n");
		return 0;
	}

	sample_freq = read_le_dword(fp);

	fseek(fp, 4 + 2, SEEK_CUR);

	sample_size = read_le_word(fp);

	fseek(fp, chunk_data, SEEK_SET);

	while (1)
	{
		const size_t read = fread(s, 1, 4, fp);
		if (read == 0)
		{
			return 0;
		}
		if (!strncmp(s, "data", 4)) {
			break;
		}
		const unsigned int chunk_size = read_be_dword(fp);
		fseek(fp, chunk_size, SEEK_CUR);
	}

	sample_len = read_le_dword(fp);

	if (sample_size == 16)
		sample_len /= 2;

	size_t vag_index = 0;

	int data_size = sample_len / 28;
	if (sample_len % 28)
		data_size++;
	*vag_size = 16 * (data_size + 2);

	unsigned char* vag = malloc(*vag_size);
	memset(vag, 0, *vag_size);

	*pitch = (sample_freq << 12) / 44100;

	if (vag == NULL)
	{
		printf("Can't open output file. Aborting.\n");
		return 0;
	}

	if (enable_looping)
		flags = 6;
	else
		flags = 0;

	while (sample_len > 0) {
		size = (sample_len >= BUFFER_SIZE) ? BUFFER_SIZE : sample_len;

		if (sample_size == 8)
		{
			for (i = 0; i < size; i++)
			{
				c = fgetc(fp);
				wave[i] = c;
				wave[i] ^= 0x80;
				wave[i] <<= 8;
			}
		}
		else
		{
			for (i = 0; i < size; i++)
				wave[i] = read_le_word(fp);
		}

		i = size / 28;
		if (size % 28) {
			for (j = size % 28; j < 28; j++)
				wave[28 * i + j] = 0;
			i++;
		}

		for (j = 0; j < i; j++) {                                     // pack 28 samples
			ptr = wave + j * 28;
			find_predict(ptr, d_samples, &predict_nr, &shift_factor);
			pack(d_samples, four_bit, predict_nr, shift_factor);
			d = (predict_nr << 4) | shift_factor;
			vag[vag_index++] = d;
			vag[vag_index++] = flags;
			for (int k = 0; k < 28; k += 2) {
				d = ((four_bit[k + 1] >> 8) & 0xf0) | ((four_bit[k] >> 12) & 0xf);
				vag[vag_index++] = d;
			}
			sample_len -= 28;
			if (sample_len < 28 && enable_looping == 0)
				flags = 1;

			if (enable_looping)
				flags = 2;
		}
	}
	vag[vag_index++] = ((predict_nr << 4) | shift_factor);

	if (enable_looping)
		vag[vag_index++] = 3;
	else
		vag[vag_index++] = 7;

	fclose(fp);

	return vag;
}


void find_predict(short* samples, double* d_samples, int* predict_nr, int* shift_factor)
{
	int i, j;
	double buffer[28][5];
	double min = 1e10;
	double max[5];
	double ds;
	int min2;
	int shift_mask;
	static double _s_1 = 0.0;                            // s[t-1]
	static double _s_2 = 0.0;                            // s[t-2]
	double s_0, s_1, s_2;

	for (i = 0; i < 5; i++) {
		max[i] = 0.0;
		s_1 = _s_1;
		s_2 = _s_2;
		for (j = 0; j < 28; j++) {
			s_0 = (double)samples[j];                      // s[t-0]
			if (s_0 > 30719.0)
				s_0 = 30719.0;
			if (s_0 < -30720.0)
				s_0 = -30720.0;
			ds = s_0 + s_1 * f[i][0] + s_2 * f[i][1];
			buffer[j][i] = ds;
			if (fabs(ds) > max[i])
				max[i] = fabs(ds);
			//                printf( "%+5.2f\n", s2 );
			s_2 = s_1;                                  // new s[t-2]
			s_1 = s_0;                                  // new s[t-1]
		}

		if (max[i] < min) {
			min = max[i];
			*predict_nr = i;
		}
		if (min <= 7) {
			*predict_nr = 0;
			break;
		}

	}

	// store s[t-2] and s[t-1] in a static variable
	// these than used in the next function call

	_s_1 = s_1;
	_s_2 = s_2;

	for (i = 0; i < 28; i++)
		d_samples[i] = buffer[i][*predict_nr];

	//  if ( min > 32767.0 )
	//      min = 32767.0;

	min2 = (int)min;
	shift_mask = 0x4000;
	*shift_factor = 0;

	while (*shift_factor < 12) {
		if (shift_mask & (min2 + (shift_mask >> 3)))
			break;
		(*shift_factor)++;
		shift_mask = shift_mask >> 1;
	}

}

void pack(double* d_samples, short* four_bit, int predict_nr, int shift_factor)
{
	double ds;
	int di;
	double s_0;
	static double s_1 = 0.0;
	static double s_2 = 0.0;
	int i;

	for (i = 0; i < 28; i++) {
		s_0 = d_samples[i] + s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
		ds = s_0 * (double)(1 << shift_factor);

		di = ((int)ds + 0x800) & 0xfffff000;

		if (di > 32767)
			di = 32767;
		if (di < -32768)
			di = -32768;

		four_bit[i] = (short)di;

		di = di >> shift_factor;
		s_2 = s_1;
		s_1 = (double)di - s_0;

	}
}

#include "byteswap.h"
#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>

#include <altivec.h>

int is_apt_for_exercise(bmp_t *bmp)
{
	int is_argb =
		(bmp->channels == 4) &&
		(bswap_32(bmp->compression) == BMP_BI_BITFIELDS) &&
		(bswap_32(bmp->redMask)     == BMP_ARGB8888_R_MASK) &&
		(bswap_32(bmp->greenMask)   == BMP_ARGB8888_G_MASK) &&
		(bswap_32(bmp->blueMask)    == BMP_ARGB8888_B_MASK) &&
		(bswap_32(bmp->alphaMask)   == BMP_ARGB8888_A_MASK);
	int is_simdable =
		((bmp->width * bmp->height) % 4 == 0);
	return is_argb && is_simdable;
}

/*----------------------------------------------------------------------------
  FOR EXERCISE 1: You will reuse this program in a later exercise.
                  Ignore the functions taskA-C for this moment.
  ----------------------------------------------------------------------------*/

void taskA(uint8_t *in, uint8_t *out, int num_pixels)
{
	vector unsigned char vnegative = { 255, 255, 255, 254,
									    255, 255, 255, 254,
									    255, 255, 255, 254,
									    255, 255, 255, 254 };

	vector unsigned char *vinput = (vector unsigned char *)in;
	vector unsigned char *voutput = (vector unsigned char *)out;

	for (int p = 0; p < num_pixels / 4; p++) {
		voutput[p] = vec_sub(vnegative, vinput[p]);
	}
}

void taskB(uint8_t *in, uint8_t *out, int num_pixels)
{
	pixel_t value;

	for (int p = 0; p < num_pixels; p++) {
		value = ((pixel_t *) in)[p];

		vector unsigned int vpixel = { value.r, value.g, value.b, 0 };
		vector float vfpixel = vec_ctf(vpixel, 0);
		vector float vfconverter = { 0.29891f, 0.58661f, 0.11448f, 0 };
		vector float vfgrayscale_part = vec_mul(vfpixel, vfconverter);

		float fresult = vfgrayscale_part[0] + vfgrayscale_part[1] + vfgrayscale_part[2];
		value.r = value.g = value.b = (unsigned char) fresult;
		((pixel_t *) out)[p] = value;
	}
}

void taskC(uint8_t *in, uint8_t *out, int num_pixels)
{
	vector unsigned char *vinput = (vector unsigned char *)in;
	vector unsigned char *voutput = (vector unsigned char *)out;

	unsigned char *grayscale = malloc(num_pixels * 4);
	vector unsigned char *vgrayscale = (vector unsigned char *)grayscale;

	taskB(in, grayscale, num_pixels);
	
	vector unsigned char vperm_pattern_red  = { 0x02, 0x02, 0x10, 0x03,
										        0x06, 0x06, 0x10, 0x07,
										        0x0A, 0x0A, 0x10, 0x0B,
										        0x0E, 0x0E, 0x10, 0x0F };
	vector unsigned char vone_char 			= { 0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF,
												0xFF, 0xFF, 0xFF, 0xFF };
	vector unsigned char vred;
	vector bool char vdominantred;
	vector bool char vandresult;

	for (int p = 0; p < num_pixels / 4; p++) {
		vred = vec_perm(vinput[p], vone_char, vperm_pattern_red);
		vdominantred = vec_cmpgt(vred, vinput[p]);

		for (int r = 0; r < 4; r++) {
			if (vdominantred[r * 4] && vdominantred[r * 4 + 1] && vdominantred[r * 4 + 2]) {
				vandresult[r * 4] = vandresult[r * 4 + 1] = vandresult[r * 4 + 2] = vandresult[r * 4 + 3] = 0xFF;
			} else {
				vandresult[r * 4] = vandresult[r * 4 + 1] = vandresult[r * 4 + 2] = vandresult[r * 4 + 3] = 0x00;
			}
		}

		voutput[p] = vec_sel(vgrayscale[p], vinput[p], vandresult);
	}

	free(vgrayscale);
	vgrayscale = NULL;
}


/*----------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	bmp_t bmp_in, bmp_out;
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s [task = a] bmp-file\n", argv[0]);
		exit(1);
	}

	char *filename = argv[1];
	char task = 'a';
	if (argc == 3) {
		filename = argv[2];
		task = argv[1][0];
	}

	FILE *f = bmp_openFile(filename, "r");
	bmp_readHeader(&bmp_in, f);
	bmp_copyHeaderAndPrepareForRead(&bmp_out, &bmp_in);
	bmp_readData(&bmp_in, f);
	bmp_closeFile(f);

	if (!is_apt_for_exercise(&bmp_in)) {
		fprintf(stderr, "For the sake simplicity please provide a ARGB8888 image with a pixel count divisible by four.\n");
		exit(4);
	}

	switch (task) {
		case 'a':
			taskA(bmp_in.data, bmp_out.data, bmp_in.width * bmp_in.height);
			break;
		case 'b':
			taskB(bmp_in.data, bmp_out.data, bmp_in.width * bmp_in.height);
			break;
		case 'c':
			taskC(bmp_in.data, bmp_out.data, bmp_in.width * bmp_in.height);
			break;
		default:
			fprintf(stderr, "Invalid task.\n");
			exit(5);
	}

	bmp_write(&bmp_out, "output.bmp");
	bmp_free(&bmp_in);
	bmp_free(&bmp_out);

	return 0;
}

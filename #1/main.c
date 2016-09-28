#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>

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
		((bswap_32(bmp->width) * bswap_32(bmp->height)) % 4 == 0);
	return is_argb && is_simdable;
}

/*----------------------------------------------------------------------------
  FOR EXERCISE 1: You will reuse this program in a later exercise.
                  Ignore the functions taskA-C for this moment.
  ----------------------------------------------------------------------------*/

void taskA(uint8_t *in, uint8_t *out, int num_pixels)
{
	pixel_t value;
	int p;

	for (p = 0; p < num_pixels; p++) {
		value = ((pixel_t *)in)[p];
		value.r = bswap_8(255 - bswap_8(value.r));
		value.g = bswap_8(255 - bswap_8(value.g));
		value.b = bswap_8(255 - bswap_8(value.b)); 
		((pixel_t *)out)[p] = value;
	}
}

void taskB(uint8_t *in, uint8_t *out, int num_pixels)
{
	(void)in;         /* UNUSED */
	(void)out;        /* UNUSED */
	(void)num_pixels; /* UNUSED */
}

void taskC(uint8_t *in, uint8_t *out, int num_pixels)
{
	(void)in;         /* UNUSED */
	(void)out;        /* UNUSED */
	(void)num_pixels; /* UNUSED */
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

	bmp_read(&bmp_in, filename);
	bmp_copyHeader(&bmp_out, &bmp_in);
	bmp_assign(&bmp_in);

	if (!is_apt_for_exercise(&bmp_in)) {
		fprintf(stderr, "For the sake simplicity please provide a ARGB8888 image with a pixel count divisible by four.\n");
		exit(4);
	}

	switch (task) {
		case 'a':
			taskA(bmp_in.data, bmp_out.data, bswap_32(bmp_in.width) * bswap_32(bmp_in.height));
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


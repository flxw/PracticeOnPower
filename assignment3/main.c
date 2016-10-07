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
	vector int vnegative = {255,255,255,0}; // 0 for 32-bit alignment
	vector int vnegativepixel=vnegative;
	int p;

	for (p = 0; p < num_pixels; p++) {
		value = ((pixel_t *)in)[p];
		vector int vpixel = {bswap_8(value.r), bswap_8(value.g), bswap_8(value.b), 0};
		vnegativepixel = vec_sub(vnegative, vpixel);

		value.r = bswap_8(vnegativepixel[0]);
		value.g = bswap_8(vnegativepixel[1]);
		value.b = bswap_8(vnegativepixel[2]);

		((pixel_t *)out)[p] = value;
	}
}

void taskB(uint8_t *in, uint8_t *out, int num_pixels)
{
	pixel_t value;
	int p;


	/* R1 + G1 + B1
	 * R2 + G2 + B2
	 * R3 + G3 + B3
	 * R4 + G4 + B4 */

	for (p = 0; p < num_pixels; p+=1) {
		value = ((pixel_t *)in)[p];

		// suddenly endianness is not important anymore??!
		vector unsigned int vpixel = {(value.r), (value.g), (value.b), 0};
		vector float vfconverter = {0.29891f, 0.58661f, 0.11448f, 0};
		vector float vfpixel     = vec_ctf(vpixel, 0);

		vector unsigned int vresult = vec_ctu(vec_mul(vfpixel, vfconverter), 0);

		value.r = value.g = value.b = (vresult[0]) + (vresult[1]) + (vresult[2]);

/*		int avg = (value.r + value.g + value.b)/3;
		value.r = value.g = value.b = avg;*/

		((pixel_t *)out)[p] = value;
	}
}

void taskC(uint8_t *in, uint8_t *out, int num_pixels)
{
	pixel_t value;
	int p;

	for (p = 0; p < num_pixels; p+=1) {
		value = ((pixel_t *)in)[p];

		vector unsigned int vpixel = {(value.r), (value.g), (value.b), 0};
		vector float vfconverter = {0.29891f, 0.58661f, 0.11448f, 0};
		vector float vfpixel     = vec_ctf(vpixel, 0);

		vector unsigned int vresult = vec_ctu(vec_mul(vfpixel, vfconverter), 0);
		unsigned int avgpixvalue = (vresult[0]) + (vresult[1]) + (vresult[2]);
		vector unsigned int vavgpixel = {avgpixvalue, avgpixvalue, avgpixvalue, 0};

		vector unsigned int vred = { 255, vpixel[0], vpixel[0], 0 };
		vector bool vdominantred = vec_cmpgt(vred, vpixel);
		vector unsigned int finalpixel = (vdominantred[0] && vdominantred[1] && vdominantred[2]) ? vpixel : vavgpixel;

		value.r = (finalpixel[0]);
		value.g = (finalpixel[1]);
		value.b = (finalpixel[2]);

		((pixel_t *)out)[p] = value;

		/* Alternativ (und auch mehr SIMD)
		 * vier Pixel auf einmal nehmen, einen Vektor pro Farbkanal und in Graustufe umwandeln.
		 * Mit dem Rot-Vektor auf Dominanz prüfen, daraus einen Bool-Vektor zur Wahl des Source-Vektors nehmen
		 */
	}
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

	if (!is_apt_for_exercise(&bmp_in)) {
		fprintf(stderr, "For the sake simplicity please provide a ARGB8888 image with a pixel count divisible by four.\n");
		exit(4);
	}

	switch (task) {
		case 'a':
			taskA(bmp_in.data, bmp_out.data, bswap_32(bmp_in.width) * bswap_32(bmp_in.height));
			break;
		case 'b':
			taskB(bmp_in.data, bmp_out.data, bswap_32(bmp_in.width) * bswap_32(bmp_in.height));
			break;
		case 'c':
			taskC(bmp_in.data, bmp_out.data, bswap_32(bmp_in.width) * bswap_32(bmp_in.height));
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

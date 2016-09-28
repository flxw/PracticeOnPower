#include "bmp.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/

void bmp_read(bmp_t *bmp, const char *filename)
{
	FILE *f = fopen(filename, "r");

	if (!f) {
		printf("Cannot open %s for reading.\n", filename);
		exit(2);
	}

	fread(bmp->header, sizeof(bmp->header), 1, f);

	if (!(bmp->header[0] == 'B' && bmp->header[1] == 'M' && bswap_32(bmp->infoHeaderSize) == BMP_DIB5_HEADER_SIZE)) {
		fprintf(stderr, "Unsupported bitmap format.\n");
		exit(3);
	}
	/* top-down bitmaps (top row first) are stored with negative height */
	bmp->isTopDown = bswap_32(bmp->height) < 0;

	/* To calculate right row in GET_PIXEL/SET_PIXEL */
	bmp->yDirection = bmp->isTopDown ? 1 : -1;
	bmp->yOffset = bmp->isTopDown ? 0 : bswap_32(bmp->height) - 1;

	/* We are storing 4 channel bmp in the end. Override old value */
	bmp->channels = bswap_16(bmp->bitCount) / 8;
	bmp->bitCount = bswap_16(32);

	/* Allocate memory for our pixel data and read file */
	bmp->sizeImage = bswap_32(bmp->width) * bswap_32(bmp->height) * bmp->channels;

	bmp->data = malloc(bmp->sizeImage);

	fread(bmp->data, bmp->sizeImage, 1, f);

	fclose(f);
}

void bmp_copyHeader(bmp_t *bmp, bmp_t *other)
{
	memcpy(bmp, other, sizeof(bmp_t));
	bmp->data = calloc(bmp->sizeImage, 1);
}

void bmp_write(bmp_t *bmp, const char *filename)
{
	int x, y;
	pixel_t output;
	FILE *f = fopen(filename, "w");

	fwrite(bmp->header, sizeof(bmp->header), 1, f);

	/* Store bitmap bottom-up (thus positive height) */
	for (y = bswap_32(bmp->height) - 1; y >= 0; y--) {
		for (x = 0; x < bswap_32(bmp->width); x++) {
			output = ((pixel_t *)bmp->data)[(y * bmp->yDirection + bmp->yOffset) * bswap_32(bmp->width) + x];
			if (fwrite(&output, sizeof(output), 1, f) != 1) {
				fprintf(stderr, "Cannot write to file %s.\n", filename);
				exit(6);
			}
		}
	}

	fclose(f);
}

void bmp_free(bmp_t *bmp)
{
	free(bmp->data);
	bmp->data = NULL;
}


#include "byteswap.h"
#include "bmp.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/

void bmp_readHeader(bmp_t *bmp, FILE *f)
{
	fread(bmp->header, sizeof(bmp->header), 1, f);

	if (!(bmp->header[0] == 'B' && bmp->header[1] == 'M' && bswap_32(bmp->infoHeaderSize) == BMP_DIB5_HEADER_SIZE)) {
		fprintf(stderr, "Unsupported bitmap format.\n");
		exit(3);
	}
}

void bmp_readData(bmp_t *bmp, FILE *f)
{
	bmp->data = malloc(bmp->sizeImage);
	fread(bmp->data, bmp->sizeImage, 1, f);
}

void bmp_copyHeaderAndPrepareForRead(bmp_t *bmp, bmp_t *other)
{
	/* top-down bitmaps (top row first) are stored with negative height */
	other->isTopDown = sbswap_32(other->height) < 0;
	int32_t bmp_height = abs(sbswap_32(other->height));
	other->height = sbswap_32(bmp_height);

	/* To calculate right row in GET_PIXEL/SET_PIXEL */
	other->yDirection = other->isTopDown ? 1 : -1;
	other->yOffset = other->isTopDown ? 0 : bmp_height - 1;

	memcpy(bmp, other, sizeof(bmp_t));

	/* We are storing 4 channel bmp in the end. Override old value */
	other->channels = bswap_16(other->bitCount) / 8;
	other->bitCount = bswap_16(32);

	/* Override input bitmap height and width */
	other->height = bmp_height;
	other->width = sbswap_32(bmp->width);

	/* Allocate memory for our pixel data and read file */
	other->sizeImage = other->width * other->height * other->channels;

	bmp->data = calloc(other->sizeImage, 1);
}

void bmp_write(bmp_t *bmp, const char *filename)
{
	int x, y;
	pixel_t output;
	FILE *f = bmp_openFile(filename, "w");

	/* Write Header */
	fwrite(bmp->header, sizeof(bmp->header), 1, f);

	bmp->height = sbswap_32(bmp->height);
	bmp->width = sbswap_32(bmp->width);

	/* Store bitmap bottom-up (thus positive height) */
	for (y = bmp->height - 1; y >= 0; y--) {
		for (x = 0; x < bmp->width; x++) {
			output = ((pixel_t *)bmp->data)[(y * bmp->yDirection + bmp->yOffset) * bmp->width + x];
			if (fwrite(&output, sizeof(output), 1, f) != 1) {
				fprintf(stderr, "Cannot write to file %s.\n", filename);
				exit(6);
			}
		}
	}

	bmp_closeFile(f);
}

FILE * bmp_openFile(const char *filename, const char *RWmode)
{
	FILE *f = fopen(filename, RWmode);

	if (!f) {
		printf("Cannot open %s, requested read/write mode: %s.\n", filename, RWmode);
		exit(2);
	}
	return f;
}

void bmp_closeFile(FILE *f)
{
	fclose(f);
}

void bmp_free(bmp_t *bmp)
{
	free(bmp->data);
	bmp->data = NULL;
}


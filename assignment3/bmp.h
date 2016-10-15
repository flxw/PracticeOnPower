#ifndef _BMP_H
#define _BMP_H

#include <stdint.h>

#define BMP_FILE_HEADER_SIZE	14
#define BMP_DIB5_HEADER_SIZE	124

#define BMP_BI_BITFIELDS 3  /* uncompressed, masks are specified */

#define BMP_ARGB8888_A_MASK 0xFF000000
#define BMP_ARGB8888_R_MASK 0x00FF0000
#define BMP_ARGB8888_G_MASK 0x0000FF00
#define BMP_ARGB8888_B_MASK 0x000000FF


/*----------------------------------------------------------------------------*/

#if __BIG_ENDIAN__ == 1

#define bswap_16(value) \
((((value) & 0xff) << 8) | ((value) >> 8))
#define bswap_32(value) \
(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
(uint32_t)bswap_16((uint16_t)((value) >> 16)))
#define bswap_64(value) \
(((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) \
<< 32) | \
(uint64_t)bswap_32((uint32_t)((value) >> 32)))

#elif __BIG_ENDIAN__ != 1

#define bswap_16(value) value
#define bswap_32(value) value
#define bswap_64(value) value

#endif

/*----------------------------------------------------------------------------*/


typedef struct _pixel {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} pixel_t;

typedef struct _ciexyz_triple
{
	int32_t redX;
	int32_t redY;
	int32_t redZ;
	int32_t greenX;
	int32_t greenY;
	int32_t greenZ;
	int32_t blueX;
	int32_t blueY;
	int32_t blueZ;
} ciexyz_triple_t;

typedef struct _bmp {
	union {
		char header[BMP_FILE_HEADER_SIZE + BMP_DIB5_HEADER_SIZE];
		struct __attribute__((packed)) {
			// Bitmap file header
			uint16_t signature;
			uint32_t bytes;
			uint32_t reserved;
			uint32_t offset;
			// DIB v5 Header
			uint32_t infoHeaderSize;
			int32_t  width;
			int32_t  height;
			uint16_t planes;
			uint16_t bitCount;
			uint32_t compression;
			uint32_t sizeImage;
			int32_t  xPPM;
			int32_t  yPPM;
			uint32_t clrUsed;
			uint32_t clrImportant;
			uint32_t redMask;
			uint32_t greenMask;
			uint32_t blueMask;
			uint32_t alphaMask;
			uint32_t csType;
			ciexyz_triple_t endpoints;
			uint32_t gammaRed;
			uint32_t gammaGreen;
			uint32_t gammaBlue;
			uint32_t intent;
			uint32_t profileData;
			uint32_t profileSize;
			uint32_t reserved_v5;
		};
	};

	/* Cached to ease up our file handling */
	int isTopDown;
	int yOffset;
	int yDirection;
	int channels;

	/* Pixel data */
	uint8_t *data;
} bmp_t;


/*----------------------------------------------------------------------------*/

extern void bmp_read(bmp_t *bmp, const char *filename);
extern void bmp_assign(bmp_t *bmp);
extern void bmp_write(bmp_t *bmp, const char *filename);
extern void bmp_copyHeader(bmp_t *bmp, bmp_t *other);
extern void bmp_free(bmp_t *bmp);

#endif /* _BMP_H */

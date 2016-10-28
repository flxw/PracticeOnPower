#if __BIG_ENDIAN__ == 1


// unsigned bit swap
#define bswap_16(value) \
((((uint16_t) value << 8) & 0xFF00) | \
 (((uint16_t) value >> 8) & 0x00FF))

#define bswap_32(value) \
((((uint32_t) value << 24) & 0xFF000000) | \
 (((uint32_t) value <<  8) & 0x00FF0000) | \
 (((uint32_t) value >>  8) & 0x0000FF00) | \
 (((uint32_t) value >> 24) & 0x000000FF))

// signed bit swap
#define sbswap_16(value) \
(int16_t) bswap_16(value)

#define sbswap_32(value) \
(int32_t) bswap_32(value)


#elif __BIG_ENDIAN__ != 1


#define  bswap_16(value) value
#define  bswap_32(value) value
#define sbswap_16(value) value
#define sbswap_32(value) value


#endif

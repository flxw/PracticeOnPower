#if __BIG_ENDIAN__ == 1

#define bswap_16(value) \
((((value) & 0xff) << 8) | ((value) >> 8))

// unsigned byte swap
#define bswap_32(value) \
(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
(uint32_t)bswap_16((uint16_t)((value) >> 16)))
#define bswap_64(value) \
(((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) << 32) | \
(uint64_t)bswap_32((uint32_t)((value) >> 32)))

// signed byte swap
#define sbswap_32(value) (int32_t) bswap_32(value)
#define sbswap_64(value) (int64_t) bswap_64(value)

#elif __BIG_ENDIAN__ != 1

#define bswap_16(value) value
#define bswap_32(value) value
#define bswap_64(value) value
#define sbswap_32(value) value
#define sbswap_64(value) value

#endif

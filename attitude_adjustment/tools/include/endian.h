#ifndef __endian_compat_h
#define __endian_compat_h

#if defined(__linux__) || defined(__CYGWIN__)
#include <byteswap.h>
#include_next <endian.h>
#elif defined(__APPLE__)
#include <machine/endian.h>
#include <machine/byte_order.h>
#define bswap_16(x) NXSwapShort(x)
#define bswap_32(x) NXSwapInt(x)
#define bswap_64(x) NXSwapLongLong(x)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#else
#include <machine/endian.h>
#define bswap_16(x) swap16(x)
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)
#endif

#ifndef __BYTE_ORDER
#define __BYTE_ORDER BYTE_ORDER
#endif
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN BIG_ENDIAN
#endif
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#endif

#endif


#ifdef __BIG_ENDIAN__
# define BB_BIG_ENDIAN 1
# define BB_LITTLE_ENDIAN 0
#elif __BYTE_ORDER == __BIG_ENDIAN
# define BB_BIG_ENDIAN 1
# define BB_LITTLE_ENDIAN 0
#else
# define BB_BIG_ENDIAN 0
# define BB_LITTLE_ENDIAN 1
#endif

#if BB_BIG_ENDIAN
#define SWAP_BE16(x) (x)
#define SWAP_BE32(x) (x)
#define SWAP_BE64(x) (x)
#define SWAP_LE16(x) bswap_16(x)
#define SWAP_LE32(x) bswap_32(x)
#define SWAP_LE64(x) bswap_64(x)
#else
#define SWAP_BE16(x) bswap_16(x)
#define SWAP_BE32(x) bswap_32(x)
#define SWAP_BE64(x) bswap_64(x)
#define SWAP_LE16(x) (x)
#define SWAP_LE32(x) (x)
#define SWAP_LE64(x) (x)
#endif

typedef struct _md5_ctx_t_ {
	__uint32_t A;
	__uint32_t B;
	__uint32_t C;
	__uint32_t D;
	__uint64_t total;
	__uint32_t buflen;
	char buffer[128];
} md5_ctx_t;

void md5_begin(md5_ctx_t *ctx);
void md5_hash(const void *data, size_t length, md5_ctx_t *ctx);
void *md5_end(void *resbuf, md5_ctx_t *ctx);

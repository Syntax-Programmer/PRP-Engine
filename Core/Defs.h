#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#define PRP_BITS_IN_TYPE(T) (sizeof(T) * CHAR_BIT)

#define PRP_INVALID_SIZE ((PRP_size)(~0ULL))
#define PRP_INVALID_IDX  ((PRP_size)(~0ULL))
#define PRP_INVALID_POS  ((PRP_size)0ull)

#define PRP_POS_TO_IDX(pos) ((pos) - 1)
#define PRP_IDX_TO_POS(idx) ((idx) + 1)

/**
 * Aligns x upward to align
 *
 * align must be a nonzero power of two.
 * x + align - 1 should znot overflow.
 */
#define PRP_ALIGNUP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define PRP_ALIGNAS(x)        alignas(x)

#ifdef __cplusplus
}
#endif

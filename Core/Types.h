#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

/* ---- INT TYPES ---- */

typedef uint64_t PRP_u64;
typedef int64_t  PRP_i64;

typedef uint32_t PRP_u32;
typedef int32_t  PRP_i32;

typedef uint16_t PRP_u16;
typedef int16_t  PRP_i16;

typedef uint8_t PRP_u8;
typedef int8_t  PRP_i8;

typedef unsigned _BitInt(24) PRP_u24;
typedef _BitInt(24) PRP_i24;

typedef unsigned _BitInt(40) PRP_u40;
typedef _BitInt(40) PRP_i40;

typedef unsigned _BitInt(48) PRP_u48;
typedef _BitInt(48) PRP_i48;

typedef unsigned _BitInt(56) PRP_u56;
typedef _BitInt(56) PRP_i56;

typedef float  PRP_f32;
typedef double PRP_f64;

/* ---- CHAR TYPES ---- */

typedef char     PRP_char8u;
typedef uint16_t PRP_char16u;
typedef uint32_t PRP_char32u;

typedef char     PRP_char8;
typedef uint16_t PRP_char16;
typedef uint32_t PRP_char32;

/* ---- POINTERS AND SIZES ---- */

typedef size_t PRP_size;

typedef nullptr_t PRP_nullptrT;
typedef ptrdiff_t PRP_ptrdiff;
typedef uintptr_t PRP_uintptr;

/* ---- BOOLEAN ---- */

typedef bool PRP_bool;

/* ---- STD FUNCTION RETURN ---- */

typedef PRP_u64 PRP_rslt;

#define PRP_UNKNOWN_ERROR      (0ull)
#define PRP_SUCCESS            (1ull << 0)
#define PRP_ERR_INV_ARG        (1ull << 1)
#define PRP_ERR_INV_STATE      (1ull << 2)
#define PRP_ERR_OOM            (1ull << 3)
#define PRP_ERR_OOB            (1ull << 4)
#define PRP_ERR_RES_EXHAUSTED  (1ull << 5)
#define PRP_ERR_IO             (1ull << 6)
#define PRP_ERR_PARSE          (1ull << 7)
#define PRP_ERR_NOT_FOUND      (1ull << 8)
#define PRP_ERR_ALREADY_EXISTS (1ull << 9)
#define PRP_ERR_UNSUPPORTED    (1ull << 10)
#define PRP_ERR_BUSY           (1ull << 11)
#define PRP_ERR_TIMEOUT        (1ull << 12)
#define PRP_ERR_CORRUPTED      (1ull << 13)
#define PRP_ERR_INTERNAL       (1ull << 14)

/* ----  MIN MAX VALUES  ---- */

#define PRP_I8_MAX ((PRP_I8)0X7F)
#define PRP_I8_MIN ((PRP_I8)(-0X80))

#define PRP_U8_MAX ((PRP_U8)0XFF)
#define PRP_U8_MIN ((PRP_U8)0)

#define PRP_I16_MAX ((PRP_I16)0X7FFF)
#define PRP_I16_MIN ((PRP_I16)(-0X8000))

#define PRP_U16_MAX ((PRP_U16)0XFFFF)
#define PRP_U16_MIN ((PRP_U16)0)

#define PRP_I32_MAX ((PRP_I32)0X7FFFFFFF)
#define PRP_I32_MIN ((PRP_I32)(-0X80000000))

#define PRP_U32_MAX ((PRP_U32)0XFFFFFFFFu)
#define PRP_U32_MIN ((PRP_U32)0u)

#define PRP_I64_MAX ((PRP_I64)0X7FFFFFFFFFFFFFFFll)
#define PRP_I64_MIN ((PRP_I64)(-0X8000000000000000ll))

#define PRP_U64_MAX ((PRP_U64)0XFFFFFFFFFFFFFFFFull)
#define PRP_U64_MIN ((PRP_U64)0ull)

#define PRP_SIZE_MAX        (SIZE_MAX)
#define PRP_BITINT_MAXWIDTH (BITINT_MAXWIDTH)

#ifdef __cplusplus
}
#endif

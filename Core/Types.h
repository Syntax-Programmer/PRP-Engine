#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
// So that all using this get the access to malloc/realloc/free.
#include <stdlib.h>

typedef uint64_t PRP_U64;
typedef int64_t PRP_I64;

typedef uint32_t PRP_U32;
typedef int32_t PRP_I32;

typedef uint16_t PRP_U16;
typedef int16_t PRP_I16;

typedef uint8_t PRP_U8;
typedef int8_t PRP_I8;

typedef float PRP_F32;
typedef double PRP_F64;

typedef char PRP_Char8;
typedef uint16_t PRP_Char16;
typedef uint32_t PRP_Char32;

/**
 * This is an system dependent type.
 *
 * For 64 bit systems it is most likely 8 bytes.
 *
 * For 32 bit systems it is most likely 4 bytes.
 */
typedef size_t PRP_Size;
typedef ptrdiff_t PRP_PtrDiff;

typedef bool PRP_Bool;
#define PRP_True (true)
#define PRP_False (false)

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

#define PRP_SIZE_MAX (SIZE_MAX)

#ifdef __cplusplus
}
#endif

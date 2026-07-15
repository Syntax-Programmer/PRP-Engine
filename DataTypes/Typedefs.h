#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Type defs to be used in the engine for consistency purposes.
 */
typedef uint64_t DT_u64;
typedef int64_t DT_i64;

typedef uint32_t DT_u32;
typedef int32_t DT_i32;

typedef uint16_t DT_u16;
typedef int16_t DT_i16;

typedef uint8_t DT_u8;
typedef int8_t DT_i8;

typedef float DT_f32;
typedef double DT_f64;

/* ----  MIN MAX VALUES  ---- */

#define DT_I8_MAX ((DT_i8)0X7F)
#define DT_I8_MIN ((DT_i8)(-0X80))

#define DT_U8_MAX ((DT_u8)0XFF)
#define DT_U8_MIN ((DT_u8)0)

#define DT_I16_MAX ((DT_i16)0X7FFF)
#define DT_I16_MIN ((DT_i16)(-0X8000))

#define DT_U16_MAX ((DT_u16)0XFFFF)
#define DT_U16_MIN ((DT_u16)0)

#define DT_I32_MAX ((DT_i32)0X7FFFFFFF)
#define DT_I32_MIN ((DT_i32)(-0X80000000))

#define DT_U32_MAX ((DT_u32)0XFFFFFFFFu)
#define DT_U32_MIN ((DT_u32)0u)

#define DT_I64_MAX ((DT_i64)0X7FFFFFFFFFFFFFFFll)
#define DT_I64_MIN ((DT_i64)(-0X8000000000000000ll))

#define DT_U64_MAX ((DT_u64)0XFFFFFFFFFFFFFFFFull)
#define DT_U64_MIN ((DT_u64)0ull)

typedef char DT_char;

typedef void DT_void;

typedef FILE *DT_file;

/**
 * This is an system dependent type.
 *
 * For 64 bit systems it is most likely 8 bytes.
 *
 * For 32 bit systems it is most likely 4 bytes.
 */
typedef size_t DT_size;
#define DT_SIZE_MAX (SIZE_MAX)

#define DT_null (NULL)

typedef enum {
    DT_false = 0,
    DT_true = 1,
} DT_bool;

#ifdef __cplusplus
}
#endif

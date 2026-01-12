#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
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

typedef char DT_char;

typedef void DT_void;

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

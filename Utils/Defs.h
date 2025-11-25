#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef uint64_t PRP_u64;
typedef int64_t PRP_i64;

typedef uint32_t PRP_u32;
typedef int32_t PRP_i32;

typedef uint16_t PRP_u16;
typedef int16_t PRP_i16;

typedef uint8_t PRP_u8;
typedef int8_t PRP_i8;

typedef float PRP_f32;
typedef double PRP_f64;
typedef long double PRP_f128;

typedef char PRP_char;

typedef void PRP_void;

/**
 * This is an system dependent type.
 *
 * For 64 bit systems it is most likely 8 bytes.
 *
 * For 32 bit systems it is most likely 4 bytes.
 */
typedef size_t PRP_size;

#define PRP_Null (NULL)

#define PRP_BIN_SET(x, y) (x) |= (y)
#define PRP_BIN_CLR(x, y) (x) &= ~(y)
#define PRP_BIN_TOGGLE(x, y) (x) ^= (y)
#define PRP_BIN_IS_SET (((x) & (y)) != 0)

#ifdef _WIN32
#ifdef PRP_EXPORTS
#define PRP_FN_API __declspec(dllexport)
#else
#define PRP_FN_API __declspec(dllimport)
#endif
#define PRP_FN_CALL __stdcall
#else
#define PRP_FN_API
#define PRP_FN_CALL
#endif

typedef enum {
  PRP_FN_SUCCESS,
  PRP_FN_WARNING,
  PRP_FN_FAILURE,
  PRP_FN_NULL_ERROR,
  PRP_FN_OOB_ERROR,
  PRP_FN_MALLOC_ERROR,
  PRP_FN_UAF_ERROR,
  PRP_FN_INV_ARG_ERROR,
  PRP_FN_RES_EXHAUSTED_ERROR,
  PRP_FN_FILE_IO_ERROR
} PRP_FN_Codes;

#ifdef __cplusplus
}
#endif
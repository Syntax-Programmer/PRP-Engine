#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Type defs to be used in the engine for consistency purposes.
 */
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

#define PRP_INVALID_SIZE ((PRP_size)-1)

#define PRP_null (NULL)

typedef enum {
  PRP_false = 0,
  PRP_true = 1,
} PRP_bool;

/**
 * Basic binary ops encapsulated for user ease of use.
 */
#define PRP_BIN_SET(x, y) (x) |= (y)
#define PRP_BIN_CLR(x, y) (x) &= ~(y)
#define PRP_BIN_TOGGLE(x, y) (x) ^= (y)
#define PRP_BIN_IS_SET (((x) & (y)) != 0)

/**
 * These are added to a function to mark it as being exposed as dlls
 *
 * Syntax for using it:
 * PRP_FN_API <ret-type> PRP_FN_CALL <fn_name>(<fn-args>);
 */
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

/**
 * These are the return codes a function can return.
 * These have two types of enums:
 *
 * . Generic Code: Tells in general what happened to the function when executed.
 *   1) PRP_FN_SUCCESS: Tells that the function succeeded without any errors.
 *   2) PRP_FN_WARNING: Meant to be a log-only code that is not returned but
 *                      logged using logger utility to tell users what is the
 *                      warning.
 *   3) PRP_FN_FAILURE: Generic failure condition that can not be explained
 *                      directly by the Specific-Error-Codes. These can be
 *                      logical errors that is per project specific.
 *
 * . Specific Error Codes: Codes other than the Generic Codes are the specific
 *                         error codes telling what exactly went wrong in the
 *                         function.
 *   1) PRP_FN_NULL_ERROR: NULL error is thrown when we have unexpected NULL
 *                         pointer in our code that is not the prelim argument
 *                         check.
 *   2) PRP_FN_OOB_ERROR: OOB(Out Of Bounds) error is thrown when something
 *                        tries to access data beyond its predetermined bounds.
 *   3) PRP_FN_MALLOC_ERROR: MALLOC error is thrown in any process where a
 *                           allocator function of any type tries and fails to
 *                           allocate memory to the caller.
 *   4) PRP_FN_UAF_ERROR: UAF(Use After Free) error is thrown when we detect an
 *                        object is being used after being freed, either
 *                        physically or semantically.
 *   5) PRP_FN_INV_ARG_ERROR: INV_ARG(Invalid Argument) error is thrown when a
 *                            function detects that the arguments passed to it
 *                            are in some way invalid. During invalid arg
 *                            detection sometime we can also throw UAF or OOB
 *                            error just to specify what exactly went wrong.
 *   6) PRP_FN_RES_EXHAUSTED_ERROR: RES_EXHAUSTED(Resource Exhausted) error is
 *                                  thrown when a function that is suppose to
 *                                  create something "runs out of it", and
 *                                  cannot further create it.
 *   7) PRP_FN_FILE_IO_ERROR: FILE_IO(File input-output) error is thrown when
 *                            interacting with a file fails in some way.
 */
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
} PRP_FnCode;

#ifdef __cplusplus
}
#endif

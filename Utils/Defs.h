#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>

#define PRP_INVALID_SIZE ((DT_size)(-1))
#define PRP_INVALID_INDEX ((DT_size)(-1))
// Pos is like a 1 based index.
#define PRP_INVALID_POS ((DT_size)(0))

#define PRP_POS_TO_I(pos) ((pos) - 1)
#define PRP_I_TO_POS(i) ((i) + 1)

/**
 * Basic binary ops encapsulated for user ease of use.
 */
#define PRP_BIT_SET(x, y) (x) |= (y)
#define PRP_BIT_CLR(x, y) (x) &= ~(y)
#define PRP_BIT_TOGGLE(x, y) (x) ^= (y)
#define PRP_BIT_IS_SET(x, y) (((x) & (y)) == (y))

#define PRP_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define PRP_MIN(x, y) (((x) > (y)) ? (y) : (x))

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

typedef enum {
    PRP_OK = 0,
    PRP_ERR_INV_ARG,
    PRP_ERR_INV_STATE,
    PRP_ERR_UNSUPPORTED,
    PRP_ERR_OOM,
    PRP_ERR_OOB,
    PRP_ERR_RES_EXHAUSTED,
    PRP_ERR_NOT_FOUND,
    PRP_ERR_ALREADY_EXISTS,
    PRP_ERR_IO,
    PRP_ERR_PARSE,
    PRP_ERR_CORRUPTED,
    PRP_ERR_BUSY,
    PRP_ERR_TIMEOUT,
    PRP_ERR_INTERNAL,
} PRP_Result;

#ifdef __cplusplus
}
#endif

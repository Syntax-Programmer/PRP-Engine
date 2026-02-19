#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Log.h"
#include <pthread.h>
#include <stdlib.h>

// Platform specific debug break.
#if defined(_MSC_VER)
#define DIAG_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DIAG_DEBUG_BREAK() __builtin_trap()
#else
#define DIAG_DEBUG_BREAK() abort()
#endif

/* ----  ALWAYS ON ---- */

#define DIAG_VERIFY(expr)                                                      \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_INVARIANT_VOILATION,  \
                     __FILE__, __LINE__, __func__, "VERIFY failed: %s",        \
                     #expr);                                                   \
            DIAG_DEBUG_BREAK();                                                \
        }                                                                      \
    } while (0)

#define DIAG_VERIFY_MSG(expr, msg, ...)                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_INVARIANT_VOILATION,  \
                     __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__);        \
            DIAG_DEBUG_BREAK();                                                \
        }                                                                      \
    } while (0)

/* ----  PANIC ---- */

#define DIAG_PANIC(msg, ...)                                                   \
    do {                                                                       \
        DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_RUNTIME_FAIL, __FILE__,   \
                 __LINE__, __func__, msg, ##__VA_ARGS__);                      \
        DIAG_DEBUG_BREAK();                                                    \
    } while (0)

/* ----  UNREACHABLE---- */

#if defined(__clang__) || defined(__GNUC__)
#define DIAG_UNREACHABLE()                                                     \
    do {                                                                       \
        DIAG_PANIC("UNREACHABLE reached");                                     \
        __builtin_unreachable();                                               \
    } while (0)
#elif defined(_MSC_VER)
#define DIAG_UNREACHABLE()                                                     \
    do {                                                                       \
        DIAG_PANIC("UNREACHABLE reached");                                     \
        __assume(0);                                                           \
    } while (0)
#else
#define DIAG_UNREACHABLE() DIAG_PANIC("UNREACHABLE reached")
#endif

/* ----  STATIC ASSERT ---- */

#define DIAG_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)

/* ----  DEBUG ONLY ASSERTS ---- */

#if defined(PRP_NDEBUG)

#define DIAG_ASSERT(expr) ((DT_void)0)
#define DIAG_ASSERT_MSG(e, msg) ((DT_void)0)

#else

#define DIAG_ASSERT(expr)                                                      \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_ASSERT, __FILE__,     \
                     __LINE__, __func__, "ASSERT failed: %s", #expr);          \
            DIAG_DEBUG_BREAK();                                                \
        }                                                                      \
    } while (0)

#define DIAG_ASSERT_MSG(expr, msg, ...)                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_ASSERT, __FILE__,     \
                     __LINE__, __func__, msg, ##__VA_ARGS__);                  \
            DIAG_DEBUG_BREAK();                                                \
        }                                                                      \
    } while (0)

#endif

/**
 * This will be separate for each T.U., this is done so that functions that
 * don't return result can set it if something went worng.
 * This is done here since this is the only file that is in evrey T.U. and in no
 * header file.
 */
static PRP_Result last_err_code = PRP_OK;
static pthread_mutex_t last_err_code_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * This is a thread save way of setting last err code.
 */
#define SET_LAST_ERR_CODE(code)                                                \
    do {                                                                       \
        pthread_mutex_lock(&last_err_code_mutex);                              \
        last_err_code = code;                                                  \
        pthread_mutex_unlock(&last_err_code_mutex);                            \
    } while (0)

#ifdef __cplusplus
}
#endif

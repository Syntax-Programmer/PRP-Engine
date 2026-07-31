#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Log.h"

/* ----  ALWAYS ON ---- */

#define DIAG_VERIFY(expr)                                                      \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_INVARIANT_VOILATION,  \
                     __FILE__, __LINE__, __func__, "VERIFY failed: %s",        \
                     #expr);                                                   \
            PRP_DEBUG_BREAK();                                                 \
        }                                                                      \
    } while (0)

#define DIAG_VERIFY_MSG(expr, msg, ...)                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_INVARIANT_VOILATION,  \
                     __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__);        \
            PRP_DEBUG_BREAK();                                                 \
        }                                                                      \
    } while (0)

/* ----  PANIC ---- */

#define DIAG_PANIC(msg, ...)                                                   \
    do {                                                                       \
        DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_RUNTIME_FAIL, __FILE__,   \
                 __LINE__, __func__, msg, ##__VA_ARGS__);                      \
        PRP_DEBUG_BREAK();                                                     \
    } while (0)

/* ----  STATIC ASSERT ---- */

#define DIAG_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)

/* ----  DEBUG ONLY ASSERTS ---- */

#ifdef PRP_RELEASE_MODE
#define DIAG_ASSERT(expr) ((void)0)
#define DIAG_ASSERT_MSG(e, msg, ...) ((void)0)

#elif defined(PRP_DEBUG_MODE)
#define DIAG_ASSERT(expr)                                                      \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_ASSERT, __FILE__,     \
                     __LINE__, __func__, "ASSERT failed: %s", #expr);          \
            PRP_DEBUG_BREAK();                                                 \
        }                                                                      \
    } while (0)
#define DIAG_ASSERT_MSG(expr, msg, ...)                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DIAG_Log(DIAG_LOG_LEVEL_FATAL, DIAG_LOG_CODE_ASSERT, __FILE__,     \
                     __LINE__, __func__, msg, ##__VA_ARGS__);                  \
            PRP_DEBUG_BREAK();                                                 \
        }                                                                      \
    } while (0)

#endif

#ifdef __cplusplus
}
#endif

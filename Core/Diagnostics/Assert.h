#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Platform.h"
#include "Core/Types.h"

typedef enum DIAG_FailureType : PRP_u8 {
    DIAG_FAILURE_TYPE_VERIFY        = 0,
    DIAG_FAILURE_TYPE_PANIC         = 1,
    DIAG_FAILURE_TYPE_ASSERTION     = 2,
    DIAG_FAILURE_TYPE_UNREACHABLE   = 3,
    DIAG_FAILURE_TYPE_UNIMPLEMENTED = 4,
} DIAG_FailureType;

typedef struct DIAG_FailureInfo {
    DIAG_FailureType type;
    const PRP_char8 *pExpr;
    const PRP_char8 *pMsg;
    const PRP_char8 *pFile;
    const PRP_char8 *pFunc;
    PRP_u32          line;
    // Future implementation of a stack trace possibility.
} DIAG_FailureInfo;

#define DIAG_FAILURE_INFO_TMPL(type, pExpr, pMsg)                              \
    (DIAG_FailureInfo) {                                                       \
        .type = (type), .pExpr = (pExpr), .pMsg = (pMsg), pFile = __FILE__,    \
        .pFunc = __func__, .line = __LINE__                                    \
    }

#define DIAG_FAILURE_DEFAULT_LOG_FILE (stderr)

/**
 * Logs all type of failure inside a single convention.
 *
 * @param pFailure_info The info describing details of the failure.
 *
 * @note Behavior:
 * - If pFail_info is invalid, it will not do anything and just return early.
 */
PRP_API void PRP_CALL DIAG_FailureLog(const DIAG_FailureInfo *pFail_info);

/* ----  VERIFY ---- */

#define DIAG_VERIFY(expr)                                                      \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DiagFailureInfo info = DIAG_FAILURE_INFO_TMPL(                     \
                DIAG_FAILURE_TYPE_VERIFY, #expr, nullptr);                     \
            DIAG_FailureLog(&info);                                            \
        }                                                                      \
    } while (0)

#define DIAG_VERIFY_MSG(expr, pMsg)                                            \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DiagFailureInfo info =                                             \
                DIAG_FAILURE_INFO_TMPL(DIAG_FAILURE_TYPE_VERIFY, #expr, pMsg); \
            DIAG_FailureLog(&info);                                            \
        }                                                                      \
    } while (0)

/* ----  PANIC ---- */

#define DIAG_PANIC()                                                           \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DiagFailureInfo info = DIAG_FAILURE_INFO_TMPL(                     \
                DIAG_FAILURE_TYPE_PANIC, nullptr, nullptr);                    \
            DIAG_FailureLog(&info);                                            \
            PRP_DEBUG_BREAK();                                                 \
            PRP_ABORT();                                                       \
        }                                                                      \
    } while (0)

#define DIAG_PANIC_MSG(pMsg)                                                   \
    do {                                                                       \
        if (!(expr)) {                                                         \
            DiagFailureInfo info = DIAG_FAILURE_INFO_TMPL(                     \
                DIAG_FAILURE_TYPE_PANIC, nullptr, pMsg);                       \
            DIAG_FailureLog(&info);                                            \
            PRP_DEBUG_BREAK();                                                 \
            PRP_ABORT();                                                       \
        }                                                                      \
    } while (0)

/* ----  UNIMPLEMENTED/UNREACHABLE ---- */

#define PRP_DIAG_UNIMPLEMENTED()                                               \
    do {                                                                       \
        DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                        \
            DIAG_FAILURE_TYPE_UNIMPLEMENTED, nullptr, nullptr);                \
        DIAG_FailureLog(&info);                                                \
        PRP_DEBUG_BREAK();                                                     \
    } while (0)

#if defined(PRP_RELEASE_MODE)
    #define DIAG_UNREACHABLE() PRP_UNREACHABLE()

#elif defined(PRP_DEBUG_MODE)
    #define DIAG_UNREACHABLE()                                                 \
        do {                                                                   \
            DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                    \
                DIAG_FAILURE_TYPE_UNREACHABLE, nullptr, nullptr);              \
            DIAG_LogFailure(&info);                                            \
            PRP_DEBUG_BREAK();                                                 \
            PRP_ABORT();                                                       \
        } while (0)
#endif

/* ----  STATIC ASSERT ---- */

#define DIAG_STATIC_ASSERT(expr, pMsg) static_assert(expr, pMsg);

/* ----  ASSERT ---- */

#ifdef PRP_RELEASE_MODE
    #define DIAG_ASSERT(expr)           ((void)0)
    #define DIAG_ASSERT_MSG(expr, pMsg) ((void)0)

#elifdef PRP_DEBUG_MODE
    #define DIAG_ASSERT(expr)                                                  \
        do {                                                                   \
            if (!(expr)) {                                                     \
                DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                \
                    DIAG_FAILURE_TYPE_ASSERTION, #expr, nullptr);              \
                DIAG_LogFailure(&info);                                        \
                PRP_DEBUG_BREAK();                                             \
                PRP_ABORT();                                                   \
            }                                                                  \
        } while (0);

    #define DIAG_ASSERT_MSG(expr, pMsg)                                        \
        do {                                                                   \
            if (!(expr)) {                                                     \
                DIAG_FailureInfo info = DIAG_FAILURE_INFO_TMPL(                \
                    DIAG_FAILURE_TYPE_ASSERTION, #expr, pMsg);                 \
                DIAG_LogFailure(&info);                                        \
                PRP_DEBUG_BREAK();                                             \
                PRP_ABORT();                                                   \
            }                                                                  \
        } while (0);
#endif

#ifdef __cplusplus
}
#endif

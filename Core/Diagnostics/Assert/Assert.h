#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

typedef enum PRP_DiagFailureType {
    PRP_DIAG_FAILURE_TYPE_VERIFY,
    PRP_DIAG_FAILURE_TYPE_PANIC,
    PRP_DIAG_FAILURE_TYPE_ASSERTION,
    PRP_DIAG_FAILURE_TYPE_UNREACHABLE,
    PRP_DIAG_FAILURE_TYPE_UNIMPLEMENTED,
} PRP_DiagFailureType;

typedef struct PRP_DiagFailureInfo {
    PRP_DiagFailureType type;
    const PRP_Char8 *pExpr;
    const PRP_Char8 *pMsg;
    const PRP_Char8 *pFile;
    const PRP_Char8 *pFunc;
    PRP_U32 line;

    // Future stack trace using platform apis.
} PRP_DiagFailureInfo;

#define PRP_DIAG_FAILURE_INFO_TEMPLATE(type_, expr_, pMsg_)                    \
    (PRP_DiagFailureInfo) {                                                    \
        .type = (type_), .pExpr = (expr_), .pMsg = (pMsg_), .pFile = __FILE__, \
        .pFunc = __func__, .line = __LINE__                                    \
    }

#define PRP_DIAG_DEFAULT_FAILURE_LOG_FILE (stderr)

/**
 * Handles all type of failure inside a single convention.
 *
 * @param pFailure_info The info describing details of the failure.
 *
 * @note Invariant:
 * - If pFailure_info is NULL, it will not do anything and just return early.
 */
PRP_API void PRP_CALL
PRP_DiagLogFailure(const PRP_DiagFailureInfo *pFailure_info);

/* ----  VERIFY ---- */

#define PRP_DIAG_VERIFY(expr)                                                  \
    do {                                                                       \
        if (!(expr)) {                                                         \
            PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(         \
                PRP_DIAG_FAILURE_TYPE_VERIFY, #expr, NULL);                    \
            PRP_DiagLogFailure(&info);                                         \
        }                                                                      \
    } while (0)

#define PRP_DIAG_VERIFY_MSG(expr, pMsg)                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(         \
                PRP_DIAG_FAILURE_TYPE_VERIFY, #expr, pMsg);                    \
            PRP_DiagLogFailure(&info);                                         \
        }                                                                      \
    } while (0)

/* ----  PANIC ---- */

#define PRP_DIAG_PANIC()                                                       \
    do {                                                                       \
        PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(             \
            PRP_DIAG_FAILURE_TYPE_PANIC, NULL, NULL);                          \
        PRP_DiagLogFailure(&info);                                             \
        PRP_DEBUG_BREAK();                                                     \
        PRP_ABORT();                                                           \
    } while (0)

#define PRP_DIAG_PANIC_MSG(pMsg)                                               \
    do {                                                                       \
        PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(             \
            PRP_DIAG_FAILURE_TYPE_PANIC, NULL, pMsg);                          \
        PRP_DiagLogFailure(&info);                                             \
        PRP_DEBUG_BREAK();                                                     \
        PRP_ABORT();                                                           \
    } while (0)

/* ----  UNIMPLEMENTED/UNREACHABLE ---- */

#define PRP_DIAG_UNIMPLEMENTED()                                               \
    do {                                                                       \
        PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(             \
            PRP_DIAG_FAILURE_TYPE_UNIMPLEMENTED, NULL, NULL);                  \
        PRP_DiagLogFailure(&info);                                             \
        PRP_DEBUG_BREAK();                                                     \
    } while (0)

#if defined(PRP_RELEASE_MODE)
#define PRP_DIAG_UNREACHABLE() PRP_UNREACHABLE()

#elif defined(PRP_DEBUG_MODE)
#define PRP_DIAG_UNREACHABLE()                                                 \
    do {                                                                       \
        PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(             \
            PRP_DIAG_FAILURE_TYPE_UNREACHABLE, NULL, NULL);                    \
        PRP_DiagLogFailure(&info);                                             \
        PRP_DEBUG_BREAK();                                                     \
        PRP_ABORT();                                                           \
    } while (0)
#endif

/* ----  STATIC ASSERT ---- */

#define PRP_DIAG_STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)

/* ----  ASSERT ---- */

#if defined(PRP_RELEASE_MODE)
#define PRP_DIAG_ASSERT(expr) ((void)0)
#define PRP_DIAG_ASSERT_MSG(expr, pMsg) ((void)0)

#elif defined(PRP_DEBUG_MODE)
#define PRP_DIAG_ASSERT(expr)                                                  \
    do {                                                                       \
        if (!(expr)) {                                                         \
            PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(         \
                PRP_DIAG_FAILURE_TYPE_ASSERTION, #expr, NULL);                 \
            PRP_DiagLogFailure(&info);                                         \
            PRP_DEBUG_BREAK();                                                 \
            PRP_ABORT();                                                       \
        }                                                                      \
    } while (0)
#define PRP_DIAG_ASSERT_MSG(expr, pMsg)                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            PRP_DiagFailureInfo info = PRP_DIAG_FAILURE_INFO_TEMPLATE(         \
                PRP_DIAG_FAILURE_TYPE_ASSERTION, #expr, pMsg);                 \
            PRP_DiagLogFailure(&info);                                         \
            PRP_DEBUG_BREAK();                                                 \
            PRP_ABORT();                                                       \
        }                                                                      \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

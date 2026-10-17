#include "Log.h"
#include <pthread.h>
#include <stdarg.h>

/**
 * This is a placeholder tech so that the logger can work before the true
 * threading library can be completed.
 */
static pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER;

#ifdef PRP_DEBUG_MODE
    #warning                                                                   \
        "The current implementation of Log.c, uses pthread.h as the thread interface. In future to be replaced with dedicated Threading library."
#endif

PRP_API PRP_i32 PRP_CALL PRP_PRINTF_FORMAT(3, 4)
    DIAG_LogFormat(PRP_char8 *pDest, PRP_size size, const PRP_char8 *pMsg,
                   ...) {
    if (!pDest || size == 0 || !pMsg) {
        return -1;
    }

    va_list args;
    va_start(args, pMsg);
    PRP_i32 rslt = vsnprintf(pDest, size, pMsg, args);
    va_end(args);

    return rslt;
}

PRP_API PRP_rslt PRP_CALL DIAG_LogWriteChar(FILE *pFile, PRP_char8 c) {
    if (!pFile) {
        return PRP_ERR_INV_ARG;
    }

    PRP_rslt code = PRP_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (fwrite(&c, 1, 1, pFile) != 1) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    return code;
}

PRP_API PRP_rslt PRP_CALL DIAG_LogWriteLn(FILE *pFile, const PRP_char8 *pMsg,
                                          PRP_size len) {
    if (!pFile || !pMsg) {
        return PRP_ERR_INV_ARG;
    }

    PRP_rslt code = PRP_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (fwrite(pMsg, 1, len, pFile) != len || fwrite("\n", 1, 1, pFile) != 1) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    return code;
}

PRP_API PRP_rslt PRP_CALL PRP_PRINTF_FORMAT(2, 3)
    DIAG_LogWriteLnFmt(FILE *pFile, const PRP_char8 *pMsg, ...) {
    if (!pFile || !pMsg) {
        return PRP_ERR_INV_ARG;
    }

    va_list args;
    va_start(args, pMsg);
    PRP_rslt code = PRP_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (vfprintf(pFile, pMsg, args) < 0 || fwrite("\n", 1, 1, pFile) != 1) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    va_end(args);

    return code;
}

PRP_API PRP_rslt PRP_CALL DIAG_LogWrite(FILE *pFile, const PRP_char8 *pMsg,
                                        PRP_size len) {
    if (!pFile || !pMsg || !len) {
        return PRP_ERR_INV_ARG;
    }

    PRP_rslt code = PRP_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (fwrite(pMsg, 1, len, pFile) != len) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    return code;
}

PRP_API PRP_rslt PRP_CALL PRP_PRINTF_FORMAT(2, 3)
    DIAG_LogWriteFmt(FILE *pFile, const PRP_char8 *pMsg, ...) {
    if (!pFile || !pMsg) {
        return PRP_ERR_INV_ARG;
    }

    va_list args;
    va_start(args, pMsg);
    PRP_rslt code = PRP_SUCCESS;

    pthread_mutex_lock(&log_mtx);
    if (vfprintf(pFile, pMsg, args) < 0) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mtx);

    va_end(args);

    return code;
}

/* ----  LOGGING ABSTRACTIONS ---- */

PRP_API PRP_rslt PRP_CALL PRP_PRINTF_FORMAT(6, 7)
    DIAG_Log(FILE *pF_out, DIAG_LogLvl lvl, const PRP_char8 *pFile,
             const PRP_char8 *pFunc, PRP_size line, const PRP_char8 *pMsg,
             ...) {
    if (!pF_out) {
        return PRP_ERR_INV_ARG;
    }

    pFile = (!pFile) ? "---" : pFile;
    pFunc = (!pFunc) ? "---" : pFunc;
    pMsg  = (!pMsg) ? "---" : pMsg;

    const PRP_char8 *pLvl_str;
    switch (lvl) {
    case DIAG_LOG_LVL_TRACE:
        pLvl_str = "TRACE";
        break;
    case DIAG_LOG_LVL_DEBUG:
        pLvl_str = "DEBUG";
        break;
    case DIAG_LOG_LVL_INFO:
        pLvl_str = "INFO";
        break;
    case DIAG_LOG_LVL_WARN:
        pLvl_str = "WARN";
        break;
    case DIAG_LOG_LVL_ERROR:
        pLvl_str = "ERROR";
        break;
    case DIAG_LOG_LVL_FATAL:
        pLvl_str = "FATAL";
        break;
    default:
        pLvl_str = "UNKNOWN";
        break;
    }

    va_list args;
    va_start(args, pMsg);

    // +1 to account for the nul-terminator implicitly put by the function.
    PRP_char8 bffr[DIAG_LOG_MSG_MAX_LEN + 1];
    PRP_i32   fmt_rslt = vsnprintf(bffr, DIAG_LOG_MSG_MAX_LEN + 1, pMsg, args);

    va_end(args);

    if (fmt_rslt < 0) {
        return PRP_ERR_IO;
    }

    return DIAG_LogWriteFmt(pF_out, "%s: [File: %s]{Func %s :-: Line %zu} %s\n",
                            pLvl_str, pFile, pFunc, line, bffr);
}

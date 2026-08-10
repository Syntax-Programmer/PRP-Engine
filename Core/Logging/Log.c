#include "Log.h"
#include "Core/Defs.h"
#include "Core/Types.h"
#include <pthread.h>
#include <stdarg.h>

/**
 * This is a placeholder tech so that the logger can work before the true
 * threading library can be completed.
 */
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

PRP_API PRP_I32 PRP_CALL PRP_PRINTF_FORMAT(3, 4)
    PRP_LogFormat(PRP_Char8 *pDest, PRP_Size dest_size, const PRP_Char8 *pMsg,
                  ...) {
    if (!pDest || !pMsg || !dest_size) {
        return -1;
    }

    va_list args;
    va_start(args, pMsg);

    pthread_mutex_lock(&log_mutex);
    PRP_I32 result = vsnprintf(pDest, dest_size, pMsg, args);
    pthread_mutex_unlock(&log_mutex);

    va_end(args);

    return result;
}

PRP_API PRP_Result PRP_CALL PRP_PRINTF_FORMAT(2, 3)
    PRP_LogFormatF(FILE *pDest, const PRP_Char8 *pMsg, ...) {
    if (!pDest || !pMsg) {
        return PRP_ERR_INV_ARG;
    }

    PRP_Result code = PRP_OK;
    va_list args;
    va_start(args, pMsg);

    pthread_mutex_lock(&log_mutex);
    if (vfprintf(pDest, pMsg, args) < 0) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mutex);

    va_end(args);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_LogWrite(FILE *pFile, const PRP_Char8 *pMsg,
                                         PRP_Size len) {
    if (!pMsg || !len || !pFile) {
        return PRP_ERR_INV_ARG;
    }

    pthread_mutex_lock(&log_mutex);

    PRP_Result code = PRP_OK;
    if (fwrite(pMsg, 1, len, pFile) != len) {
        code = PRP_ERR_IO;
    }

    pthread_mutex_unlock(&log_mutex);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_PRINTF_FORMAT(2, 3)
    PRP_LogWriteFmt(FILE *pFile, const PRP_Char8 *pMsg, ...) {
    if (!pMsg || !pFile) {
        return PRP_ERR_INV_ARG;
    }

    PRP_Result code = PRP_OK;
    va_list args;
    va_start(args, pMsg);

    pthread_mutex_lock(&log_mutex);
    if (vfprintf(pFile, pMsg, args) < 0) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mutex);

    va_end(args);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_LogWriteLn(FILE *pFile, const PRP_Char8 *pMsg,
                                           PRP_Size len) {
    if (!pMsg || !len || !pFile) {
        return PRP_ERR_INV_ARG;
    }

    pthread_mutex_lock(&log_mutex);

    PRP_Result code = PRP_OK;
    if (fwrite(pMsg, 1, len, pFile) != len) {
        code = PRP_ERR_IO;
    }
    if (fwrite("\n", 1, 1, pFile) != 1) {
        code = PRP_ERR_IO;
    }

    pthread_mutex_unlock(&log_mutex);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_PRINTF_FORMAT(2, 3)
    PRP_LogWriteLnFmt(FILE *pFile, const PRP_Char8 *pMsg, ...) {
    if (!pMsg || !pFile) {
        return PRP_ERR_INV_ARG;
    }

    PRP_Result code = PRP_OK;
    va_list args;
    va_start(args, pMsg);

    pthread_mutex_lock(&log_mutex);
    if (vfprintf(pFile, pMsg, args) < 0) {
        code = PRP_ERR_IO;
    }
    if (fwrite("\n", 1, 1, pFile) != 1) {
        code = PRP_ERR_IO;
    }
    pthread_mutex_unlock(&log_mutex);

    va_end(args);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_LogWriteChar(FILE *pFile, PRP_Char8 c) {
    if (!pFile) {
        return PRP_ERR_INV_ARG;
    }

    pthread_mutex_lock(&log_mutex);

    PRP_Result code = PRP_OK;
    if (fwrite(&c, 1, 1, pFile) != 1) {
        code = PRP_ERR_IO;
    }

    pthread_mutex_unlock(&log_mutex);

    return code;
}

/* ----  LOGGING ABSTRACTIONS ---- */

PRP_API PRP_Result PRP_CALL PRP_Log(FILE *pLog_file, PRP_LogLevel level,
                                    const PRP_Char8 *pFile,
                                    const PRP_Char8 *pFunc, PRP_Size line,
                                    const PRP_Char8 *pMsg, ...) {
    if (!pLog_file) {
        return PRP_ERR_INV_ARG;
    }
    if (!pFile) {
        pFile = "Unspecified File";
    }
    if (!pFunc) {
        pFunc = "Unspecified Func";
    }
    if (!pMsg) {
        pMsg = "";
    }
    const PRP_Char8 *pLvl_str;
    switch (level) {
    case PRP_LOG_LVL_TRACE:
        pLvl_str = "TRACE";
        break;
    case PRP_LOG_LVL_DEBUG:
        pLvl_str = "DEBUG";
        break;
    case PRP_LOG_LVL_INFO:
        pLvl_str = "INFO";
        break;
    case PRP_LOG_LVL_WARN:
        pLvl_str = "WARN";
        break;
    case PRP_LOG_LVL_ERROR:
        pLvl_str = "ERROR";
        break;
    case PRP_LOG_LVL_FATAL:
        pLvl_str = "FATAL";
        break;
    default:
        pLvl_str = "UNKNOWN";
        break;
    }

    va_list args;
    va_start(args, pMsg);
    // +1 to account for the nul terminator impliclty put by the function.
    PRP_Char8 bffr[PRP_LOG_MSG_MAX_LEN + 1];
    PRP_I32 fmt_rslt = vsnprintf(bffr, PRP_LOG_MSG_MAX_LEN + 1, pMsg, args);
    va_end(args);
    if (fmt_rslt < 0) {
        return PRP_ERR_IO;
    }

    return PRP_LogFormatF(pLog_file,
                          "%s: [File: %s][Func: %s][Line: %zu]: %s\n", pLvl_str,
                          pFile, pFunc, line, bffr);
}

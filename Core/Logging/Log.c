#include "Log.h"
#include <pthread.h>
#include <stdarg.h>

/**
 * This is a placeholder tech so that the logger can work before the true
 * threading library can be completed.
 */
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

PRP_API PRP_I32 PRP_CALL PRP_LogFormat(PRP_Char8 *pDest, PRP_Size dest_size,
                                       const PRP_Char8 *pMsg, ...) {
    if (!pDest || !pMsg || !dest_size) {
        return -1;
    }

    va_list args;
    va_start(args, pMsg);
    PRP_I32 result = vsnprintf(pDest, dest_size, pMsg, args);
    va_end(args);

    return result;
}

PRP_API PRP_Result PRP_CALL PRP_LogFormatF(FILE *pDest, const PRP_Char8 *pMsg,
                                           ...) {
    if (!pDest || !pMsg) {
        return PRP_ERR_INV_ARG;
    }

    va_list args;
    va_start(args, pMsg);
    if (vfprintf(pDest, pMsg, args) < 0) {
        return PRP_ERR_IO;
    }
    va_end(args);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_LogWrite(const PRP_Char8 *pMsg, PRP_Size len) {
    if (!pMsg || !len) {
        return PRP_ERR_INV_ARG;
    }

    pthread_mutex_lock(&log_mutex);

    PRP_Result code = PRP_OK;
    if (fwrite(pMsg, 1, len, PRP_LOG_DEFAULT_LOG_FILE) != len) {
        code = PRP_ERR_IO;
    }

    pthread_mutex_unlock(&log_mutex);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_LogWriteLn(const PRP_Char8 *pMsg,
                                           PRP_Size len) {
    if (!pMsg || !len) {
        return PRP_ERR_INV_ARG;
    }

    pthread_mutex_lock(&log_mutex);

    PRP_Result code = PRP_OK;
    if (fwrite(pMsg, 1, len, PRP_LOG_DEFAULT_LOG_FILE) != len) {
        code = PRP_ERR_IO;
    }
    if (fwrite("\n", 1, 1, PRP_LOG_DEFAULT_LOG_FILE) != 1) {
        code = PRP_ERR_IO;
    }

    pthread_mutex_unlock(&log_mutex);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_LogWriteChar(PRP_Char8 c) {
    pthread_mutex_lock(&log_mutex);

    PRP_Result code = PRP_OK;
    if (fwrite(&c, 1, 1, PRP_LOG_DEFAULT_LOG_FILE) != 1) {
        code = PRP_ERR_IO;
    }

    pthread_mutex_unlock(&log_mutex);

    return code;
}

PRP_API PRP_Result PRP_CALL PRP_LogWriteF(const PRP_Char8 *pMsg, PRP_Size len,
                                          FILE *pFile) {
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

PRP_API PRP_Result PRP_CALL PRP_LogWriteLnF(const PRP_Char8 *pMsg, PRP_Size len,
                                            FILE *pFile) {
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

PRP_API PRP_Result PRP_CALL PRP_LogWriteCharF(PRP_Char8 c, FILE *pFile) {
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

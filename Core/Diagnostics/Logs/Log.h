#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"
#include <stdio.h>

/**
 * Formats the given string into pDest.
 *
 * @param pDest Output pointer of the formatted string.
 * @param size  The size of the pDest buffer allocated.
 * @param pMsg  The message to format.
 *
 * @return -1 if arguments are invalid.
 * @return Negative number if formatting fails.
 * @return The total size of the formatted string on success.
 *
 * @note Behaviour:
 * - If size is less than the required size, the message will be truncated.
 */
PRP_API PRP_I32 PRP_CALL PRP_DiagLogFormat(PRP_Char8 *pDest, PRP_Size size,
                                           const PRP_Char8 *pMsg, ...)
    PRP_PRINTF_FORMAT(3, 4);

/**
 * Writes the given character to the file dest.
 *
 * @param pFile The destination file to log into.
 * @param c     The character to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_DiagLogWriteChar(FILE *pFile, PRP_Char8 c);

/**
 * Writes the given string message plus a new line character to the file dest.
 *
 * @param pFile The destination file to log into.
 * @param pMsg  The message to log.
 * @param len   The explicit len of the message.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_DiagLogWriteLn(FILE *pFile,
                                               const PRP_Char8 *pMsg,
                                               PRP_Size len);
/**
 * Writes the given varadic argument message plus a new line character to the
 * file dest.
 *
 * @param pFile The destination file to log into.
 * @param pMsg The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_DiagLogWriteLnFmt(FILE *pFile,
                                                  const PRP_Char8 *pMsg, ...)
    PRP_PRINTF_FORMAT(2, 3);

/**
 * Writes the given string message to the file dest.
 *
 * @param pFile The destination file to log into.
 * @param pMsg  The message to log.
 * @param len   The explicit len of the message.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_DiagLogWrite(FILE *pFile, const PRP_Char8 *pMsg,
                                             PRP_Size len);
/**
 * Writes the given varadic argument message to the file dest.
 *
 * @param pFile The destination file to log into
 * @param pMsg  The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_DiagLogWriteFmt(FILE *pFile,
                                                const PRP_Char8 *pMsg, ...)
    PRP_PRINTF_FORMAT(2, 3);

/* ----  LOGGING ABSTRACTIONS ---- */

constexpr PRP_Size PRP_DIAG_LOG_MSG_MAX_LEN = 2047;

typedef enum PRP_DiagLogLvl : PRP_U8 {
    PRP_DIAG_LOG_LVL_TRACE = 0,
    PRP_DIAG_LOG_LVL_DEBUG = 1,
    PRP_DIAG_LOG_LVL_INFO = 2,
    PRP_DIAG_LOG_LVL_WARN = 3,
    PRP_DIAG_LOG_LVL_ERROR = 4,
    PRP_DIAG_LOG_LVL_FATAL = 5,
} PRP_DiagLogLvl;

/**
 * Logging abstraction that logs to the log file.
 *
 * @param pLog_file The destination file to log into.
 * @param lvl       The severity of the log.
 * @param pFile     The name of the file in which the log function was called.
 * @param pFunc     The name of the function which called the log function.
 * @param line      The exact line where the log function was called in file.
 * @param pMsg      The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if formatting into the file failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_DiagLog(FILE *pLog_file, PRP_DiagLogLvl lvl,
                                        const PRP_Char8 *pFile,
                                        const PRP_Char8 *pFunc, PRP_Size line,
                                        const PRP_Char8 *pMsg, ...)
    PRP_PRINTF_FORMAT(6, 7);

#define PRP_DIAG_LOG_TRACE(pLog_file, pMsg, ...)                               \
    PRP_Log((pLog_file), PRP_LOG_LVL_TRACE, __FILE__, __func__, __LINE__,      \
            (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define PRP_DIAG_LOG_DEBUG(pLog_file, pMsg, ...)                               \
    PRP_Log((pLog_file), PRP_LOG_LVL_DEBUG, __FILE__, __func__, __LINE__,      \
            (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define PRP_DIAG_LOG_INFO(pLog_file, pMsg, ...)                                \
    PRP_Log((pLog_file), PRP_LOG_LVL_INFO, __FILE__, __func__, __LINE__,       \
            (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define PRP_DIAG_LOG_WARN(pLog_file, pMsg, ...)                                \
    PRP_Log((pLog_file), PRP_LOG_LVL_WARN, __FILE__, __func__, __LINE__,       \
            (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define PRP_DIAG_LOG_ERROR(pLog_file, pMsg, ...)                               \
    PRP_Log((pLog_file), PRP_LOG_LVL_ERROR, __FILE__, __func__, __LINE__,      \
            (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define PRP_DIAG_LOG_FATAL(pLog_file, pMsg, ...)                               \
    PRP_Log((pLog_file), PRP_LOG_LVL_FATAL, __FILE__, __func__, __LINE__,      \
            (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

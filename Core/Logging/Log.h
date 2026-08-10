#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"
#include <stdio.h>

#define PRP_LOG_DEFAULT_LOG_FILE (stdout)

/**
 * Formats the given string into pDest.
 *
 * @oaram pDest     Output pointer of the formatted string.
 * @param dest_size The size of the pDest buffer allocated.
 * @param pMsg      The message to format.
 *
 * @return Negative number if arguments are invalid.
 * @return Negative number if formatting fails.
 * @return The total size of the formatted string on success.
 *
 * @note Behaviour:
 * - If dest_size is less than the required size, the message will be truncated.
 *
 */
PRP_API PRP_I32 PRP_CALL PRP_LogFormat(PRP_Char8 *pDest, PRP_Size dest_size,
                                       const PRP_Char8 *pMsg, ...)
    PRP_PRINTF_FORMAT(3, 4);
/**
 * Formats the given string into the pDest file.
 *
 * @param pDest Output file pointer.
 * @param pMsg  The message to format.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if the formatting into the file failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogFormatF(FILE *pDest, const PRP_Char8 *pMsg,
                                           ...) PRP_PRINTF_FORMAT(2, 3);
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
PRP_API PRP_Result PRP_CALL PRP_LogWrite(FILE *pFile, const PRP_Char8 *pMsg,
                                         PRP_Size len);
/**
 * Writes the given varadic argument messgae to the file dest.
 *
 * @param pFile The destination file to log into
 * @param pMsg  The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteFmt(FILE *pFile, const PRP_Char8 *pMsg,
                                            ...) PRP_PRINTF_FORMAT(2, 3);
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
PRP_API PRP_Result PRP_CALL PRP_LogWriteLn(FILE *pFile, const PRP_Char8 *pMsg,
                                           PRP_Size len);
/**
 * Writes the given varadic argument messgae plus a new line character to the
 * file dest.
 *
 * @param pFile The destination file to log into.
 * @param pMsg The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteLnFmt(FILE *pFile,
                                              const PRP_Char8 *pMsg, ...)
    PRP_PRINTF_FORMAT(2, 3);
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
PRP_API PRP_Result PRP_CALL PRP_LogWriteChar(FILE *pFile, PRP_Char8 c);

/* ----  LOGGING ABSTRACTIONS ---- */

#define PRP_LOG_MSG_MAX_LEN (2047)

typedef enum PRP_LogLevel {
    PRP_LOG_LVL_TRACE,
    PRP_LOG_LVL_DEBUG,
    PRP_LOG_LVL_INFO,
    PRP_LOG_LVL_WARN,
    PRP_LOG_LVL_ERROR,
    PRP_LOG_LVL_FATAL,
} PRP_LogLevel;

/**
 * Logging abstraction that logs to the log file.
 *
 * @param pLog_file The destination file to log into.
 * @param level     The severity of the log.
 * @param pFile     The name of the file in which the log function was called.
 * @param pFunc     The name of the function which called the log function.
 * @param line      The exact line where the log function was called in file.
 * @param pMsg      The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if formatting into the file failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_Log(FILE *pLog_file, PRP_LogLevel level,
                                    const PRP_Char8 *pFile,
                                    const PRP_Char8 *pFunc, PRP_Size line,
                                    const PRP_Char8 *pMsg, ...);

#define PRP_LOG_TRACE(pLog_file, pMsg, ...)                                    \
    PRP_Log((pLog_file), PRP_LOG_LVL_TRACE, __FILE__, __func__, __LINE__,      \
            (pMsg), ##__VA_ARGS__)
#define PRP_LOG_DEBUG(pLog_file, pMsg, ...)                                    \
    PRP_Log((pLog_file), PRP_LOG_LVL_DEBUG, __FILE__, __func__, __LINE__,      \
            (pMsg), ##__VA_ARGS__)
#define PRP_LOG_INFO(pLog_file, pMsg, ...)                                     \
    PRP_Log((pLog_file), PRP_LOG_LVL_INFO, __FILE__, __func__, __LINE__,       \
            (pMsg), ##__VA_ARGS__)
#define PRP_LOG_WARN(pLog_file, pMsg, ...)                                     \
    PRP_Log((pLog_file), PRP_LOG_LVL_WARN, __FILE__, __func__, __LINE__,       \
            (pMsg), ##__VA_ARGS__)
#define PRP_LOG_ERROR(pLog_file, pMsg, ...)                                    \
    PRP_Log((pLog_file), PRP_LOG_LVL_ERROR, __FILE__, __func__, __LINE__,      \
            (pMsg), ##__VA_ARGS__)
#define PRP_LOG_FATAL(pLog_file, pMsg, ...)                                    \
    PRP_Log((pLog_file), PRP_LOG_LVL_FATAL, __FILE__, __func__, __LINE__,      \
            (pMsg), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

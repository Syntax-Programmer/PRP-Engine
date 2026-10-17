#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Platform.h"
#include "Core/Types.h"
#include <stdio.h>

#define PRP_DIAG_LOG_DEFAULT_LOG_FILE (stdout)

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
 * @note Behavior:
 * - If size is less than the required size, the message will be truncated.
 */
PRP_API PRP_i32 PRP_CALL DIAG_LogFormat(PRP_char8 *pDest, PRP_size size,
                                        const PRP_char8 *pMsg, ...)
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
PRP_API PRP_rslt PRP_CALL DIAG_LogWriteChar(FILE *pFile, PRP_char8 c);
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
PRP_API PRP_rslt PRP_CALL DIAG_LogWriteLn(FILE *pFile, const PRP_char8 *pMsg,
                                          PRP_size len);
/**
 * Writes the given va argument message plus a new line character to the
 * file dest.
 *
 * @param pFile The destination file to log into.
 * @param pMsg The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_rslt PRP_CALL DIAG_LogWriteLnFmt(FILE *pFile, const PRP_char8 *pMsg,
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
PRP_API PRP_rslt PRP_CALL DIAG_LogWrite(FILE *pFile, const PRP_char8 *pMsg,
                                        PRP_size len);
/**
 * Writes the given va argument message to the file dest.
 *
 * @param pFile The destination file to log into
 * @param pMsg  The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_rslt PRP_CALL DIAG_LogWriteFmt(FILE *pFile, const PRP_char8 *pMsg,
                                           ...) PRP_PRINTF_FORMAT(2, 3);

/* ----  LOGGING ABSTRACTIONS ---- */

#define DIAG_LOG_MSG_MAX_LEN (2047)

typedef enum DIAG_LogLvl : PRP_u8 {
    DIAG_LOG_LVL_TRACE = 0,
    DIAG_LOG_LVL_DEBUG = 1,
    DIAG_LOG_LVL_INFO  = 2,
    DIAG_LOG_LVL_WARN  = 3,
    DIAG_LOG_LVL_ERROR = 4,
    DIAG_LOG_LVL_FATAL = 5,
} DIAG_LogLvl;

/**
 * Logging abstraction that logs to the log file.
 *
 * @param pF_out The destination file to log into.
 * @param lvl    The severity of the log.
 * @param pFile  The name of the file in which the log function was called.
 * @param pFunc  The name of the function which called the log function.
 * @param line   The exact line where the log function was called in file.
 * @param pMsg   The message to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if formatting into the file failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_rslt PRP_CALL DIAG_Log(FILE *pF_out, DIAG_LogLvl lvl,
                                   const PRP_char8 *pFile,
                                   const PRP_char8 *pFunc, PRP_size line,
                                   const PRP_char8 *pMsg, ...);
PRP_PRINTF_FORMAT(6, 7);

#define DIAG_LOG_TRACE(pF_out, pMsg, ...)                                      \
    DIAG_Log((pF_out), DIAG_LOG_LVL_TRACE, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define DIAG_LOG_DEBUG(pF_out, pMsg, ...)                                      \
    DIAG_Log((pF_out), DIAG_LOG_LVL_DEBUG, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define DIAG_LOG_INFO(pF_out, pMsg, ...)                                       \
    DIAG_Log((pF_out), DIAG_LOG_LVL_INFO, __FILE__, __func__, __LINE__,        \
             (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define DIAG_LOG_WARN(pF_out, pMsg, ...)                                       \
    DIAG_Log((pF_out), DIAG_LOG_LVL_WARN, __FILE__, __func__, __LINE__,        \
             (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define DIAG_LOG_ERROR(pF_out, pMsg, ...)                                      \
    DIAG_Log((pF_out), DIAG_LOG_LVL_ERROR, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#define DIAG_LOG_FATAL(pF_out, pMsg, ...)                                      \
    DIAG_Log((pF_out), DIAG_LOG_LVL_FATAL, __FILE__, __func__, __LINE__,       \
             (pMsg)__VA_OPT__(, ) __VA_ARGS__)

#ifdef __cplusplus
}
#endif

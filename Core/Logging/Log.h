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
                                       const PRP_Char8 *pMsg, ...);
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
                                           ...);

/**
 * Writes the given string message to the default log dest.
 *
 * @param pMsg The message to log.
 * @param len  The explicit len of the message.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWrite(const PRP_Char8 *pMsg, PRP_Size len);
/**
 * Writes the given string message plus a new line character to the default log
 * dest.
 *
 * @param pMsg The message to log.
 * @param len  The explicit len of the message.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteLn(const PRP_Char8 *pMsg, PRP_Size len);
/**
 * Writes the given character to the default log dest.
 *
 * @param c The character to log.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteChar(PRP_Char8 c);

/**
 * Writes the given string message to the file dest.
 *
 * @param pMsg  The message to log.
 * @param len   The explicit len of the message.
 * @param pFile The destination file to log into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteF(const PRP_Char8 *pMsg, PRP_Size len,
                                          FILE *pFile);
/**
 * Writes the given string message plus a new line character to the default log
 * dest.
 *
 * @param pMsg  The message to log.
 * @param len   The explicit len of the message.
 * @param pFile The destination file to log into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteLnF(const PRP_Char8 *pMsg, PRP_Size len,
                                            FILE *pFile);
/**
 * Writes the given character to the default log dest.
 *
 * @param c     The character to log.
 * @param pFile The destination file to log into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_IO if write operation failed.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
PRP_API PRP_Result PRP_CALL PRP_LogWriteCharF(PRP_Char8 c, FILE *pFile);

#ifdef __cplusplus
}
#endif

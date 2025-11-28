#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Defs.h"

/* ----  GENERIC LOGGER  ---- */

/**
 * A generic, thread-safe logging function that formats and displays your logs.
 *
 * @param file: The file from which the log originated, can be achieved by
 * passing in __FILE__ as the param.
 * @param func: The func that madde the logs, can be achieved by passing in
 * __func__ as the param.
 * @param line: The exact code line where the log function was called, can be
 * achieved by passing in __LINE__ as the param.
 * @param fmt: The user written message, that is to be logged.
 */
PRP_FN_API PRP_void PRP_FN_CALL PRP_Log(const PRP_char *file,
                                        const PRP_char *func, PRP_u32 line,
                                        const PRP_char *fmt, ...);

/**
 * A macro that abstracts away the file, func, and line requirement of the
 * PRP_Log function and allows you to just write the log and not worry about
 * other stuff.
 *
 * @param fmt: The user written message, that is to be logged.
 */
#define PRP_LOG(fmt, ...)                                                      \
  PRP_Log(__FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/* ----  CODE LOGGER  ---- */

/**
 * A thread-safe way to log FN_CODES, the function formats the codes into a
 * good-looking log.
 *
 * @param code: The PRP_FnCode for which the log is generated. This will be
 * displayed in your logs.
 * @param file: The file from which the log originated, can be achieved by
 * passing in __FILE__ as the param.
 * @param func: The func that madde the logs, can be achieved by passing in
 * __func__ as the param.
 * @param line: The exact code line where the log function was called, can be
 * achieved by passing in __LINE__ as the param.
 * @param fmt: The user written message, that is to be logged.
 *
 */
PRP_FN_API PRP_void PRP_FN_CALL PRP_LogFnCode(PRP_FnCode code,
                                              const PRP_char *file,
                                              const PRP_char *func,
                                              PRP_u32 line, const PRP_char *fmt,
                                              ...);

/**
 * A macro that abstracts away the file, func, and line requirement of the
 * PRP_LogFnCode function and allows you to just write the log and not worry
 * about other stuff.
 *
 * @param code: The PRP_FnCode for which the log is generated. This will be
 * displayed in your logs.
 * @param fmt: The user written message, that is to be logged.
 */
#define PRP_LOG_FN_CODE(code, fmt, ...)                                        \
  PRP_LogFnCode(code, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * A macro that logs a null error due to the var param.
 *
 * @param var: The variable that casued the error.
 */
#define PRP_LOG_FN_NULL_ERROR(var)                                             \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Unexpected PRP_null value: '%s' encountered.", #var)

/**
 * A macro that logs a malloc error due to the var param.
 *
 * @param var: The variable that casued the error.
 */
#define PRP_LOG_FN_MALLOC_ERROR(var)                                           \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Failed to allocate mem for the var: '%s'.", #var)

/**
 * A macro that logs an UAF(Use After Free) error due to the var param.
 *
 * @param var: The variable that casued the error.
 */
#define PRP_LOG_FN_UAF_ERROR(var)                                              \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Tried using an already freed var: '%s'.", #var)

/**
 * A macro that logs an invalid argument error due to the var param.
 *
 * @param var: The variable that casued the error.
 */
#define PRP_LOG_FN_INV_ARG_ERROR(var)                                          \
  PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                           \
                  "Invalid/Corrupted function argument: '%s' encountered.",    \
                  #var)

#ifdef __cplusplus
}
#endif

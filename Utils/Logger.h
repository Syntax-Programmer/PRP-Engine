#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "Defs.h"

/* ----  GENERIC LOGGER  ---- */

/**
 * A generic, thread-safe logging function that formats and displays your logs.
 *
 * @param file: The file from which the log originated, can be achieved by
 * passing in __FILE__ as the param.
 * @param fn: The fn that made the logs, can be achieved by passing in
 * __func__ as the param.
 * @param line: The exact code line where the log function was called, can be
 * achieved by passing in __LINE__ as the param.
 * @param fmt: The user written message, that is to be logged.
 */
PRP_FN_API DT_void PRP_FN_CALL PRP_Log(const DT_char *file, const DT_char *fn,
                                       DT_u32 line, const DT_char *fmt, ...);

/**
 * A macro that abstracts away the file, fn, and line requirement of the
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
 * @param fn: The fn that made the logs, can be achieved by passing in
 * __func__ as the param.
 * @param line: The exact code line where the log function was called, can be
 * achieved by passing in __LINE__ as the param.
 * @param fmt: The user written message, that is to be logged.
 *
 */
PRP_FN_API DT_void PRP_FN_CALL PRP_LogFnCode(PRP_FnCode code,
                                             const DT_char *file,
                                             const DT_char *fn, DT_u32 line,
                                             const DT_char *fmt, ...);

/**
 * A macro that abstracts away the file, fn, and line requirement of the
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
 * @param var: The variable that caused the error.
 */
#define PRP_LOG_FN_NULL_ERROR(var)                                             \
    PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                         \
                    "Unexpected DT_null value: '%s' encountered.", #var)

/**
 * A macro that logs a malloc error due to the var param.
 *
 * @param var: The variable that caused the error.
 */
#define PRP_LOG_FN_MALLOC_ERROR(var)                                           \
    PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                         \
                    "Failed to allocate mem for the var: '%s'.", #var)

/**
 * A macro that logs an UAF(Use After Free) error due to the var param.
 *
 * @param var: The variable that caused the error.
 */
#define PRP_LOG_FN_UAF_ERROR(var)                                              \
    PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                         \
                    "Tried using an already freed var: '%s'.", #var)

/**
 * A macro that logs an invalid argument error due to the var param.
 *
 * @param var: The variable that caused the error.
 */
#define PRP_LOG_FN_INV_ARG_ERROR(var)                                          \
    PRP_LOG_FN_CODE(PRP_FN_NULL_ERROR,                                         \
                    "Invalid/Corrupted function argument: '%s' encountered.",  \
                    #var)

#define PRP_NULL_ARG_CHECK(arg, ret)                                           \
    do {                                                                       \
        if (!arg) {                                                            \
            PRP_LOG_FN_INV_ARG_ERROR(arg);                                     \
            return ret;                                                        \
        }                                                                      \
    } while (0)

#ifdef __cplusplus
}
#endif

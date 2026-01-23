#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"
#include <stdio.h>

typedef enum {
    DIAG_LOG_CODE_NONE,
    DIAG_LOG_CODE_ASSERT,
    DIAG_LOG_CODE_INVARIANT_VOILATION,
    DIAG_LOG_CODE_CORRUPTION,
    DIAG_LOG_CODE_UAF,
    DIAG_LOG_CODE_OOM,
    DIAG_LOG_CODE_OOR,
    DIAG_LOG_CODE_NULL_ARG,
    DIAG_LOG_CODE_INVALID_ARG,
    DIAG_LOG_CODE_INVALID_STATE,
    DIAG_LOG_CODE_INIT_FAIL,
    DIAG_LOG_CODE_RUNTIME_FAIL,
    DIAG_LOG_CODE_EXIT_FAIL,
    DIAG_LOG_CODE_FALLBACK_USED,
    DIAG_LOG_CODE_DEGRADED_MODE,
} DIAG_LogCode;

typedef enum {
    DIAG_LOG_LEVEL_TRACE,
    DIAG_LOG_LEVEL_DEBUG,
    DIAG_LOG_LEVEL_INFO,
    DIAG_LOG_LEVEL_WARN,
    DIAG_LOG_LEVEL_ERROR,
    DIAG_LOG_LEVEL_FATAL,
} DIAG_LogLevel;

#define DIAG_DEFAULT_LOG_DEST (stderr)

/*
 * A simple thread-safe write utility for the engine.
 *
 * @param dest: The file to output the logs to.
 * @parma msg: The msg to output to the file.
 */
PRP_FN_API DT_void PRP_FN_CALL DIAG_Write(FILE *dest, const DT_char *msg);

/**
 * A thread-safe way to output logs in the engine
 *
 * @param lvl: The severity level of the log being outputted.
 * @param code: The error code representing what went wrong for which the log is
 * generated. This will be displayed in your logs.
 * @param file: The file from which the log originated, can be achieved by
 * passing in __FILE__ as the param.
 * @param line: The exact code line where the log function was called, can be
 * achieved by passing in __LINE__ as the param.
 *  @param fn: The fn that made the logs, can be achieved by passing in
 * __func__ as the param.
 * @param fmt: The user written message, that is to be logged.
 */
PRP_FN_API DT_void PRP_FN_CALL DIAG_Log(DIAG_LogLevel lvl, DIAG_LogCode code,
                                        const DT_char *file, DT_size line,
                                        const DT_char *func, const DT_char *msg,
                                        ...);

#define DIAG_LOG_TRACE(code, msg, ...)                                         \
    DIAG_Log(DIAG_LOG_LEVEL_TRACE, code, __FILE__, __LINE__, __func__, msg,    \
             ##__VA_ARGS__)
#define DIAG_LOG_DEBUG(code, msg, ...)                                         \
    DIAG_Log(DIAG_LOG_LEVEL_DEBUG, code, __FILE__, __LINE__, __func__, msg,    \
             ##__VA_ARGS__)
#define DIAG_LOG_INFO(code, msg, ...)                                          \
    DIAG_Log(DIAG_LOG_LEVEL_INFO, code, __FILE__, __LINE__, __func__, msg,     \
             ##__VA_ARGS__)
#define DIAG_LOG_WARN(code, msg, ...)                                          \
    DIAG_Log(DIAG_LOG_LEVEL_WARN, code, __FILE__, __LINE__, __func__, msg,     \
             ##__VA_ARGS__)
#define DIAG_LOG_ERROR(code, msg, ...)                                         \
    DIAG_Log(DIAG_LOG_LEVEL_ERROR, code, __FILE__, __LINE__, __func__, msg,    \
             ##__VA_ARGS__)
#define DIAG_LOG_FATAL(code, msg, ...)                                         \
    DIAG_Log(DIAG_LOG_LEVEL_FATAL, code, __FILE__, __LINE__, __func__, msg,    \
             ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

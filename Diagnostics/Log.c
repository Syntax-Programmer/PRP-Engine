#include "Log.h"
#include <pthread.h>
#include <stdarg.h>

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

#define DIAG_COLOR_RESET "\x1b[0m"

#define DIAG_COLOR_TRACE "\x1b[37m" // light gray
#define DIAG_COLOR_DEBUG "\x1b[36m" // cyan
#define DIAG_COLOR_INFO "\x1b[32m"  // green
#define DIAG_COLOR_WARN "\x1b[33m"  // yellow
#define DIAG_COLOR_ERROR "\x1b[31m" // red
#define DIAG_COLOR_FATAL "\x1b[41m" // red background

/**
 * Converts the DIAG_LogCode to their corresponding string, to be displayed in
 * the logs.
 *
 * @param code: The code to get the string equivalent.
 *
 * @return The string equivalent of the code, if the code is invalid we get
 * "UNKNOWN DIAGNOSTIC CODE"
 */
static const DT_char *CodeToStr(DIAG_LogCode code);
/**
 * Gives appropriate colors for the different log levels.
 *
 * @param lvl: The level to get the color for.
 *
 * @return The ascii codes for the colors of the log.
 */
static const DT_char *LevelToColor(DIAG_LogLevel lvl);
/**
 * Converts the DIAG_LogLevel to their corresponding string, to be displayed in
 * the logs.
 *
 * @param lvl: The lvl to get the string equivalent.
 *
 * @return The string equivalent of the code, if the code is invalid we classify
 * it as a info log.
 */
static const DT_char *LevelToStr(DIAG_LogLevel lvl);

PRP_FN_API DT_void PRP_FN_CALL DIAG_Write(FILE *dest, const DT_char *msg) {
    if (!dest) {
        dest = DIAG_DEFAULT_LOG_DEST;
    }
    pthread_mutex_lock(&log_mutex);
    fputs(msg, dest);
    pthread_mutex_unlock(&log_mutex);
}

static const DT_char *CodeToStr(DIAG_LogCode code) {
    switch (code) {
    case DIAG_LOG_CODE_NONE:
        return "NO DIAGNOSTIC CONDITION";
    case DIAG_LOG_CODE_ASSERT:
        return "ASSERTION FAILED";
    case DIAG_LOG_CODE_INVARIANT_VOILATION:
        return "ENGINE INVARIANT VIOLATED";
    case DIAG_LOG_CODE_CORRUPTION:
        return "DATA OR MEMORY CORRUPTION DETECTED";
    case DIAG_LOG_CODE_UAF:
        return "USE-AFTER-FREE DETECTED";
    case DIAG_LOG_CODE_OOM:
        return "OUT OF MEMORY";
    case DIAG_LOG_CODE_OOR:
        return "OUT OF RESOURCE CAPACITY";
    case DIAG_LOG_CODE_NULL_ARG:
        return "NULL ARGUMENT PASSED";
    case DIAG_LOG_CODE_INVALID_ARG:
        return "INVALID ARGUMENT VALUE";
    case DIAG_LOG_CODE_INVALID_STATE:
        return "INVALID OBJECT OR SYSTEM STATE";
    case DIAG_LOG_CODE_INIT_FAIL:
        return "INITIALIZATION FAILED";
    case DIAG_LOG_CODE_RUNTIME_FAIL:
        return "RUNTIME OPERATION FAILED";
    case DIAG_LOG_CODE_EXIT_FAIL:
        return "SHUTDOWN OR CLEANUP FAILED";
    case DIAG_LOG_CODE_FALLBACK_USED:
        return "FALLBACK PATH USED";
    case DIAG_LOG_CODE_DEGRADED_MODE:
        return "RUNNING IN DEGRADED MODE";
    default:
        return "UNKNOWN DIAGNOSTIC CODE";
    }
}

static const DT_char *LevelToColor(DIAG_LogLevel lvl) {
    switch (lvl) {
    case DIAG_LOG_LEVEL_TRACE:
        return DIAG_COLOR_TRACE;
    case DIAG_LOG_LEVEL_DEBUG:
        return DIAG_COLOR_DEBUG;
    case DIAG_LOG_LEVEL_INFO:
        return DIAG_COLOR_INFO;
    case DIAG_LOG_LEVEL_WARN:
        return DIAG_COLOR_WARN;
    case DIAG_LOG_LEVEL_ERROR:
        return DIAG_COLOR_ERROR;
    case DIAG_LOG_LEVEL_FATAL:
        return DIAG_COLOR_FATAL;
    default:
        return DIAG_COLOR_RESET;
    }
}

static const DT_char *LevelToStr(DIAG_LogLevel lvl) {
    switch (lvl) {
    case DIAG_LOG_LEVEL_TRACE:
        return "TRACE";
    case DIAG_LOG_LEVEL_DEBUG:
        return "DEBUG";
    case DIAG_LOG_LEVEL_INFO:
        return "INFO";
    case DIAG_LOG_LEVEL_WARN:
        return "WARN";
    case DIAG_LOG_LEVEL_ERROR:
        return "ERROR";
    case DIAG_LOG_LEVEL_FATAL:
        return "FATAL";
    default:
        return "INFO";
    }
}

PRP_FN_API DT_void PRP_FN_CALL DIAG_Log(DIAG_LogLevel lvl, DIAG_LogCode code,
                                        const DT_char *file, DT_size line,
                                        const DT_char *func, const DT_char *msg,
                                        ...) {
    va_list args;
    va_start(args, msg);
    DT_char bffr[512];
    vsnprintf(bffr, sizeof(bffr), msg, args);

    DT_char log[512];
    snprintf(log, sizeof(log), "%s[%s]  %s::%zu :: %s --[%s]-- %s%s\n",
             LevelToColor(lvl), LevelToStr(lvl), file, line, func,
             CodeToStr(code), bffr, DIAG_COLOR_RESET);

    DIAG_Write(DIAG_DEFAULT_LOG_DEST, log);
    if (lvl == DIAG_LOG_LEVEL_FATAL) {
        fflush(DIAG_DEFAULT_LOG_DEST);
    }
}

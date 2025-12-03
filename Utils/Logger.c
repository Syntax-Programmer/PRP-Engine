#include "Logger.h"
#include "Defs.h"
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

#define OUT_FILE (stdout)

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ----  GENERIC LOGGER  ---- */

PRP_FN_API DT_void PRP_FN_CALL PRP_Log(const DT_char *file, const DT_char *func,
                                       DT_u32 line, const DT_char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    DT_char bfr[512];
    vsnprintf(bfr, sizeof(bfr), fmt, args);
    va_end(args);

    DT_char log[512];
    snprintf(log, sizeof(log), "%s::%d (%s): %s\n", file, line, func, bfr);

    pthread_mutex_lock(&log_mutex);
    fputs(log, OUT_FILE);
    fflush(OUT_FILE);
    pthread_mutex_unlock(&log_mutex);
}

/* ----  CODE LOGGER  ---- */

static const DT_char *FNCodeToStr(PRP_FnCode code) {
    switch (code) {
    case PRP_FN_SUCCESS:
        return "Success";
    case PRP_FN_WARNING:
        return "Warning";
    case PRP_FN_FAILURE:
        return "Failure";
    case PRP_FN_OOB_ERROR:
        return "Out Of Bounds Access Error";
    case PRP_FN_MALLOC_ERROR:
        return "Mem Alloc Error";
    case PRP_FN_NULL_ERROR:
        return "DT_null Encountered Error";
    case PRP_FN_UAF_ERROR:
        return "Use After Free Error";
    case PRP_FN_INV_ARG_ERROR:
        return "Invalid Function Argument Error";
    case PRP_FN_RES_EXHAUSTED_ERROR:
        return "Resources Exhausted Error";
    case PRP_FN_FILE_IO_ERROR:
        return "File I/O Error";
    default:
        return "Unknown Code";
    }
}

PRP_FN_API DT_void PRP_FN_CALL PRP_LogCode(PRP_FnCode code, const DT_char *file,
                                           const DT_char *func, DT_u32 line,
                                           const DT_char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    DT_char bfr[512];
    vsnprintf(bfr, sizeof(bfr), fmt, args);
    va_end(args);

    DT_char log[512];
    snprintf(log, sizeof(log), "[%s]: %s\n", FNCodeToStr(code), bfr);

    PRP_Log(file, func, line, log);
}

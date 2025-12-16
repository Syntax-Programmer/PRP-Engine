#include "Defs.h"

#if !(defined(_POSIX_C_SOURCE))
#define _POSIX_C_SOURCE 199309L // Enable clock_gettime on POSIX systems
#endif

#include "Logger.h"
#include "Timer.h"

/* ----  TIME MEASUREMENT  ---- */

/**
 * Get the current time in nanoseconds.
 *
 * @return The current time in the double f64 format of TimeMeasure.
 */
static PRP_TimeMeasure GetTimeNs(DT_void);

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

static LARGE_INTEGER freq;
static DT_i32 initialized = 0;

static PRP_TimeMeasure GetTimeNs(DT_void) {
    LARGE_INTEGER counter;
    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }
    QueryPerformanceCounter(&counter);
    return (PRP_TimeMeasure)counter.QuadPart * 1e9 / freq.QuadPart;
}

#else
#include <time.h>

static PRP_TimeMeasure GetTimeNs(DT_void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}
#endif

static const DT_f64 TIME_UNIT_SCALE[] = {1, 1e3, 1e6, 1e9};

PRP_FN_API PRP_TimeMeasure PRP_FN_CALL PRP_TimerGetTime(PRP_TimeUnit unit) {
    if (unit < 0 || unit > PRP_TIME_S) {
        unit = PRP_TIME_NS; // fallback
    }

    return GetTimeNs() / TIME_UNIT_SCALE[unit];
}

/* ----  TIMER  ---- */

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_TimerStart(PRP_Timer *timer) {
    PRP_NULL_ARG_CHECK(timer, PRP_FN_INV_ARG_ERROR);

    timer->start = PRP_TimerGet(timer->unit);

    return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_TimeMeasure PRP_FN_CALL PRP_TimerElapsed(PRP_Timer *timer) {
    PRP_NULL_ARG_CHECK(timer, PRP_INVALID_TIME_MEASURE);

    return PRP_TimerGetTime(timer->unit) - timer->start;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_TimerChangeUnit(PRP_Timer *timer,
                                                      PRP_TimeUnit unit) {
    PRP_NULL_ARG_CHECK(timer, PRP_FN_INV_ARG_ERROR);
    if (unit < 0 || unit > PRP_TIME_S) {
        unit = PRP_TIME_NS; // fallback
    }

    timer->unit = unit;
    timer->start = PRP_TimerGetTime(timer->unit);

    return PRP_FN_SUCCESS;
}

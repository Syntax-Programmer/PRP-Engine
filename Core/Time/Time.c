#include "Time.h"

/**
 * Including platform specific headers that provide the time utility.
 *
 * The PRP_TIME_TICKS_PER_SEC define a constant freq of 1 billion ticks per sec.
 *
 * The time_s_freq is the hardware freq given by windows/apple.
 */
#if defined(PRP_PLATFORM_WEB)
#include <emscripten/emscripten.h>

static PRP_TimeTicks time_s_freq = 1000000000ULL;

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID)
#include <errno.h>
#include <sched.h>
#include <time.h>

static PRP_TimeTicks time_s_freq = 1000000000ULL;

#elif defined(PRP_PLATFORM_WINDOWS) && defined(PRP_HAS_INCLUDE_WINDOWS)
#include <windows.h>

static PRP_TimeTicks time_s_freq = 0;

#elif defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
#include <errno.h>
#include <mach/mach_time.h>
#include <sched.h>

static PRP_TimeTicks time_s_freq = 0;

#else
#error Unsupported Platform Detected
#endif

/* ---- TIMING ---- */

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeNow(void) {
#if defined(PRP_PLATFORM_WEB)
    // 1 tick == 1ns. doing /1000.0 since emscripten_get_now() returns in ms.
    return (PRP_TimeTicks)(emscripten_get_now() * (time_s_freq / 1000.0));

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // 1 tick == 1ns.
    return (PRP_TimeTicks)ts.tv_sec * time_s_freq + (PRP_TimeTicks)ts.tv_nsec;

#elif defined(PRP_PLATFORM_WINDOWS) && defined(PRP_HAS_INCLUDE_WINDOWS)
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (PRP_TimeTicks)counter.QuadPart;

#elif defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
    return (PRP_TimeTicks)mach_absolute_time();

#else
#error Unsupported Platform Detected
#endif
}

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeFreq(void) {
#if defined(PRP_PLATFORM_WINDOWS) && defined(PRP_HAS_INCLUDE_WINDOWS)
    if (time_s_freq == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

        time_s_freq = (PRP_TimeTicks)freq.QuadPart;
    }

#elif defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
    if (time_s_freq == 0) {
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);

        time_s_freq = (1000000000ULL * timebase.denom) / timebase.numer;
    }

#endif

    return time_s_freq;
}

PRP_API void PRP_CALL PRP_TimeSleep(PRP_F64 ms) {
    if (ms <= 0.0) {
        return;
    }

#if defined(PRP_PLATFORM_WINDOWS)
    /*
     * Sleep() only accepts whole milliseconds.
     * Round upward so a positive sub-ms request doesn't become Sleep(0).
     */
    PRP_U64 whole_ms = (PRP_U64)ms;
    if ((PRP_F64)whole_ms < ms) {
        whole_ms++;
    }

    while (whole_ms > PRP_U32_MAX) {
        Sleep(PRP_U32_MAX);
        whole_ms -= PRP_U32_MAX;
    }
    Sleep((DWORD)whole_ms);

#elif defined(PRP_PLATFORM_WEB)
    /*
     * Sleep() only accepts whole milliseconds.
     * Round upward so a positive sub-ms request doesn't become Sleep(0).
     */
    PRP_U64 whole_ms = (PRP_U64)ms;
    if ((PRP_F64)whole_ms < ms) {
        whole_ms++;
    }

    while (whole_ms > PRP_U32_MAX) {
        emscripten_sleep(PRP_U32_MAX);
        whole_ms -= PRP_U32_MAX;
    }
    emscripten_sleep((DWORD)whole_ms);

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID) ||          \
    defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)

    PRP_F64 seconds = ms / 1000.0;
    struct timespec req;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - (PRP_F64)req.tv_sec) * 1000000000.0);

    while (nanosleep(&req, &req) == -1 && errno == EINTR) {
    }

#else
#error Unsupported Platform Detected
#endif
}

PRP_API void PRP_CALL PRP_TimeSleepTicks(PRP_TimeTicks ticks) {
    if (ticks == 0) {
        return;
    }

    PRP_F64 ms = PRP_TimeTicksToTimeUnits(ticks, PRP_TIME_UNIT_MS);
    PRP_TimeSleep(ms);
}

PRP_API void PRP_CALL PRP_TimeYield(void) {
#if defined(PRP_PLATFORM_WINDOWS)
    (void)SwitchToThread();

#elif defined(PRP_PLATFORM_WEB)
#if defined(__EMSCRIPTEN_PTHREADS__)
    (void)sched_yield();
#else
    /*
     * No meaningful synchronous thread-yield operation
     * on the browser main thread.
     */
#endif

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID) ||          \
    defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
    (void)sched_yield();

#else
#error Unsupported Platform Detected
#endif
}

PRP_API PRP_F64 PRP_CALL PRP_TimeTicksToTimeUnits(PRP_TimeTicks ticks,
                                                  PRP_TimeUnit unit) {
    switch (unit) {
    case PRP_TIME_UNIT_NS:
        return ((PRP_F64)(ticks) * 1000000000.0) / (PRP_F64)PRP_TimeFreq();
    case PRP_TIME_UNIT_US:
        return ((PRP_F64)(ticks) * 1000000.0) / (PRP_F64)PRP_TimeFreq();
    case PRP_TIME_UNIT_MS:
        return ((PRP_F64)(ticks) * 1000.0) / (PRP_F64)PRP_TimeFreq();
    case PRP_TIME_UNIT_S:
        return ((PRP_F64)(ticks)) / (PRP_F64)PRP_TimeFreq();
    default:
        return 0;
    }
}

/* ---- TIMER ---- */

PRP_API PRP_TimeTimer PRP_CALL PRP_TimeTimerCreate(void) {
    return PRP_TimeNow();
}

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsedTicks(PRP_TimeTimer timer) {
    return PRP_TimeNow() - timer;
}

PRP_API PRP_F64 PRP_CALL PRP_TimeTimerElapsed(PRP_TimeTimer timer,
                                              PRP_TimeUnit unit) {
    return PRP_TimeTicksToTimeUnits(PRP_TimeNow() - timer, unit);
}

/* ---- STOPWATCH ---- */

PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchStart(PRP_TimeStopwatch *pSw) {
    if (!pSw) {
        return PRP_ERR_INV_ARG;
    }
    if (pSw->is_running) {
        return PRP_ERR_INV_STATE;
    }

    pSw->start = PRP_TimeNow();
    pSw->is_running = PRP_True;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchStop(PRP_TimeStopwatch *pSw) {
    if (!pSw) {
        return PRP_ERR_INV_ARG;
    }
    if (!pSw->is_running) {
        return PRP_ERR_INV_STATE;
    }

    pSw->elapsed += PRP_TimeNow() - pSw->start;
    pSw->start = 0;
    pSw->is_running = PRP_False;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchReset(PRP_TimeStopwatch *pSw) {
    if (!pSw) {
        return PRP_ERR_INV_ARG;
    }

    *pSw = PRP_TIME_STOPWATCH_CREATE;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchElapsedTicks(
    const PRP_TimeStopwatch *pSw, PRP_TimeTicks *pTicks) {
    if (!pSw || !pTicks) {
        return PRP_ERR_INV_ARG;
    }

    *pTicks = pSw->elapsed;
    if (pSw->is_running) {
        *pTicks += PRP_TimeNow() - pSw->start;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchElapsed(
    const PRP_TimeStopwatch *pSw, PRP_TimeUnit unit, PRP_F64 *pTime) {
    if (!pSw || !pTime) {
        return PRP_ERR_INV_ARG;
    }

    PRP_TimeTicks ticks = pSw->elapsed;
    if (pSw->is_running) {
        ticks += PRP_TimeNow() - pSw->start;
    }
    *pTime = PRP_TimeTicksToTimeUnits(ticks, unit);

    return PRP_OK;
}

/* ---- FRAME TIMING ---- */

PRP_API
PRP_Result PRP_CALL PRP_TimeFrameTimerBegin(PRP_TimeFrameTimer *pF_timer) {
    if (!pF_timer) {
        return PRP_ERR_INV_ARG;
    }

    pF_timer->frame_count++;

    PRP_TimeTicks now = PRP_TimeNow();
    pF_timer->frame_delta = now - pF_timer->frame_start;
    pF_timer->frame_start = now;

    pF_timer->fps_accumulator += pF_timer->frame_delta;
    pF_timer->accumulated_frame_count++;
    if (pF_timer->fps_accumulator >= PRP_TIME_FRAME_TIMER_ACCUMULATION_TICKS) {
        // FPS = (freq * frame_count) / accumulator
        pF_timer->fps = ((PRP_F64)PRP_TimeFreq() *
                         (PRP_F64)pF_timer->accumulated_frame_count) /
                        (PRP_F64)pF_timer->fps_accumulator;

        pF_timer->fps_accumulator = 0;
        pF_timer->accumulated_frame_count = 0;
    }

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL
PRP_TimeFrameTimerReset(PRP_TimeFrameTimer *pF_timer) {
    if (!pF_timer) {
        return PRP_ERR_INV_ARG;
    }

    *pF_timer = PRP_TIME_FRAME_TIMER_CREATE;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeFrameTimerDeltaTicks(
    const PRP_TimeFrameTimer *pF_timer, PRP_TimeTicks *pTicks) {
    if (!pF_timer || !pTicks) {
        return PRP_ERR_INV_ARG;
    }

    *pTicks = pF_timer->frame_delta;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeFrameTimerDeltaTime(
    const PRP_TimeFrameTimer *pF_timer, PRP_TimeUnit unit, PRP_F64 *pTime) {
    if (!pF_timer || !pTime) {
        return PRP_ERR_INV_ARG;
    }

    *pTime = PRP_TimeTicksToTimeUnits(pF_timer->frame_delta, unit);

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL
PRP_TimeFrameTimerFPS(const PRP_TimeFrameTimer *pF_timer, PRP_F64 *pFPS) {
    if (!pF_timer || !pFPS) {
        return PRP_ERR_INV_ARG;
    }

    *pFPS = pF_timer->fps;

    return PRP_OK;
}

PRP_API PRP_Result PRP_CALL PRP_TimeFrameTimerFrameCount(
    const PRP_TimeFrameTimer *pF_timer, PRP_U64 *pFrame_count) {
    if (!pF_timer || !pFrame_count) {
        return PRP_ERR_INV_ARG;
    }

    *pFrame_count = pF_timer->frame_count;

    return PRP_OK;
}

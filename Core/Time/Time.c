#include "Time.h"
#include "Core/Types.h"
#include <bits/time.h>

#ifdef PRP_PLATFORM_WEB
    #include <emscripten/emscripten.h>

static PRP_TimeTicks time_s_freq = 1'000'000'000ULL;

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID)
    #include <errno.h>
    #include <sched.h>
    #include <time.h>

static PRP_TimeTicks time_s_freq = 1'000'000'000ULL;

#elifdef PRP_PLATFORM_WINDOWS
    #include <windows.h>

static PRP_TimeTicks time_s_freq = 0;

#elif defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
    #include <errno.h>
    #include <mach/mach_time.h>
    #include <sched.h>

static PRP_TimeTicks time_s_freq = 0;

#else
    #error Unsupported platform detected.
#endif

/* ---- TIMING ---- */

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeNow(void) {
#ifdef PRP_PLATFORM_WEB
    return (PRP_TimeTicks)(emscripten_get_now() * (time_s_freq / 1000.0));

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ((PRP_TimeTicks)ts.tv_sec * time_s_freq) + (PRP_TimeTicks)ts.tv_nsec;

#elifdef PRP_PLATFORM_WINDOWS
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (PRP_TimeTicks)counter.QuadPart;

#elif defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
    return (PRP_TimeTicks)mach_absolute_time();

#else
    #error Unsupported platform detected.
#endif
}

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeFreq(void) {
#ifdef PRP_PLATFORM_WINDOWS
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

PRP_API PRP_f64 PRP_CALL PRP_TimeTicksToUnits(PRP_TimeTicks ticks,
                                              PRP_TimeUnit  unit) {
    switch (unit) {
    case PRP_TIME_UNIT_NS:
        return ((PRP_f64)ticks * 1'000'000'000.0) / (PRP_f64)PRP_TimeFreq();
    case PRP_TIME_UNIT_US:
        return ((PRP_f64)ticks * 1'000'000.0) / (PRP_f64)PRP_TimeFreq();
    case PRP_TIME_UNIT_MS:
        return ((PRP_f64)ticks * 1'000.0) / (PRP_f64)PRP_TimeFreq();
    case PRP_TIME_UNIT_S:
        return (PRP_f64)ticks / (PRP_f64)PRP_TimeFreq();
    default:
        return -1;
    }
}

PRP_API void PRP_CALL PRP_TimeSleep(PRP_f64 ms) {
    if (ms <= 0.0) {
        return;
    }

#ifdef PRP_PLATFORM_WINDOWS
    PRP_u64 whole_ms = (PRP_u64)ms;
    if ((PRP_f64)whole_ms < ms) {
        whole_ms++;
    }
    while (whole_ms > PRP_U32_MAX) {
        Sleep(PRP_U32_MAX);
        whole_ms -= PRP_U32_MAX;
    }
    Sleep((DWORD)whole_ms);

#elifdef PRP_PLATFORM_WEB
    PRP_u64 whole_ms = (PRP_u64)ms;
    if ((PRP_f64)whole_ms < ms) {
        whole_ms++;
    }
    while (whole_ms > PRP_U32_MAX) {
        emscripten_sleep(PRP_U32_MAX);
        whole_ms -= PRP_U32_MAX;
    }
    emscripten_sleep((DWORD)whole_ms);

#elif defined(PRP_PLATFORM_LINUX) || defined(PRP_PLATFORM_ANDROID) ||          \
    defined(PRP_PLATFORM_MACOS) || defined(PRP_PLATFORM_IOS)
    PRP_f64         sec = ms / 1000.0f;
    struct timespec req;

    req.tv_sec  = (time_t)sec;
    req.tv_nsec = (long)((sec - (PRP_f64)req.tv_sec) * 1'000'000'000.0);
    while (nanosleep(&req, &req) == -1 && errno == EINTR)
        ;

#else
    #error Unsupported platform detected.
#endif
}

PRP_API void PRP_CALL PRP_TimeSleepTicks(PRP_TimeTicks ticks) {
    if (ticks == 0) {
        return;
    }

    PRP_f64 ms = PRP_TimeTicksToUnits(ticks, PRP_TIME_UNIT_MS);
    PRP_TimeSleep(ms);
}

PRP_API void PRP_CALL PRP_TimeYield(void) {
#ifdef PRP_PLATFORM_WINDOWS
    (void)SwitchToThread();

#elifdef PRP_PLATFORM_WEB
    #ifdef __EMSCRIPTEN_PTHREADS__
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
    #error Unsupported platform detected.
#endif
}

/* ---- TIMER ---- */

PRP_API PRP_TimeTimer PRP_CALL PRP_TimeTimerCreate(void) {
    return PRP_TimeNow();
}

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsedTicks(PRP_TimeTimer timer) {
    return PRP_TimeNow() - timer;
}

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsed(PRP_TimeTimer timer,
                                                    PRP_TimeUnit  unit) {
    return PRP_TimeTicksToUnits(PRP_TimeNow() - timer, unit);
}

/* ---- STOPWATCH ---- */

PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchStart(PRP_TimeStopwatch *pSw) {
    if (!pSw) {
        return PRP_ERR_INV_ARG;
    } else if (pSw->is_running) {
        return PRP_ERR_INV_STATE;
    }

    pSw->start      = PRP_TimeNow();
    pSw->is_running = true;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchStop(PRP_TimeStopwatch *pSw) {
    if (!pSw) {
        return PRP_ERR_INV_ARG;
    } else if (!pSw->is_running) {
        return PRP_ERR_INV_STATE;
    }

    pSw->elapsed    += PRP_TimeNow() - pSw->start;
    pSw->start       = 0;
    pSw->is_running  = false;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchReset(PRP_TimeStopwatch *pSw) {
    if (!pSw) {
        return PRP_ERR_INV_ARG;
    }

    *pSw = PRP_TIME_STOPWATCH_CREATE;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchElapsedTicks(
    const PRP_TimeStopwatch *pSw, PRP_TimeTicks *pTicks) {
    if (!pSw || !pTicks) {
        return PRP_ERR_INV_ARG;
    }

    *pTicks = pSw->elapsed;
    if (pSw->is_running) {
        *pTicks += PRP_TimeNow() - pSw->start;
    }

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchElapsed(const PRP_TimeStopwatch *pSw,
                                                   PRP_f64     *pTime,
                                                   PRP_TimeUnit unit) {
    if (!pSw || !pTime) {
        return PRP_ERR_INV_ARG;
    }

    PRP_TimeTicks ticks = pSw->elapsed;
    if (pSw->is_running) {
        ticks += PRP_TimeNow() - pSw->start;
    }
    *pTime = PRP_TimeTicksToUnits(ticks, unit);

    return PRP_SUCCESS;
}

/* ---- FRAME TIMING ---- */

PRP_API PRP_rslt PRP_CALL
PRP_TimeFrameTimerBegin(PRP_TimeFrameTimer *pF_timer) {
    if (!pF_timer) {
        return PRP_ERR_INV_ARG;
    }

    PRP_TimeTicks now          = PRP_TimeNow();
    pF_timer->frame_delta      = now - pF_timer->frame_start;
    pF_timer->frame_start      = now;
    pF_timer->fps_accumulator += pF_timer->frame_delta;
    pF_timer->accumulated_frame_count++;

    if (pF_timer->fps_accumulator >= PRP_TIME_FRAME_TIMER_ACCUMULATION_TICKS) {
        // FPS = (freq * frame_count) / accumulator
        pF_timer->fps = ((PRP_f64)PRP_TimeFreq() *
                         (PRP_f64)pF_timer->accumulated_frame_count /
                         (PRP_f64)pF_timer->fps_accumulator);
        pF_timer->fps_accumulator         = 0;
        pF_timer->accumulated_frame_count = 0;
    }
    pF_timer->frame_count++;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL
PRP_TimeFrameTimerReset(PRP_TimeFrameTimer *pF_timer) {
    if (!pF_timer) {
        return PRP_ERR_INV_ARG;
    }

    *pF_timer = PRP_TIME_FRAME_TIMER_CREATE;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL PRP_TimeFrameTimerDeltaTicks(
    const PRP_TimeFrameTimer *pF_timer, PRP_TimeTicks *pTicks) {
    if (!pF_timer || !pTicks) {
        return PRP_ERR_INV_ARG;
    }

    *pTicks = pF_timer->frame_delta;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL
PRP_TimeFrameTimerDeltaTime(const PRP_TimeFrameTimer *pF_timer,
                            PRP_TimeTicks *pTime, PRP_TimeUnit unit) {
    if (!pF_timer || !pTime) {
        return PRP_ERR_INV_ARG;
    }

    *pTime = PRP_TimeTicksToUnits(pF_timer->frame_delta, unit);

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL
PRP_TimeFrameTimerFPS(const PRP_TimeFrameTimer *pF_timer, PRP_f64 *pFPS) {
    if (!pF_timer || !pFPS) {
        return PRP_ERR_INV_ARG;
    }

    *pFPS = pF_timer->fps;

    return PRP_SUCCESS;
}

PRP_API PRP_rslt PRP_CALL PRP_TimeFrameTimerFrameCount(
    const PRP_TimeFrameTimer *pF_timer, PRP_u64 *pFrame_count) {
    if (!pF_timer || !pFrame_count) {
        return PRP_ERR_INV_ARG;
    }

    *pFrame_count = pF_timer->frame_count;

    return PRP_SUCCESS;
}

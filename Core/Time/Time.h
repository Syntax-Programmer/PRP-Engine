#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/* ---- TIMING ---- */

/**
 * 1 tick represents a singular incrementation into the platform's monotonic
 * clock.
 *
 * This is an int value used to reduce the floating point error accumulation.
 */
typedef PRP_U64 PRP_TimeTicks;

/**
 * Defines all the time units our time api supports.
 *
 * PRP_TIME_UNIT_NS: nanoseconds.
 * PRP_TIME_UNIT_US: microseconds.
 * PRP_TIME_UNIT_MS: milliseconds.
 * PRP_TIME_UNIT_S: seconds.
 */
typedef enum PRP_TimeUnit {
    PRP_TIME_UNIT_NS,
    PRP_TIME_UNIT_US,
    PRP_TIME_UNIT_MS,
    PRP_TIME_UNIT_S,
} PRP_TimeUnit;

/**
 * Returns the value of the platform's monotonic clock.
 *
 * @return The value of the monotonic clock in ticks.
 */
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeNow(void);
/**
 * Finds the platform's monotonic clock's frequency(Number of ticks per second)
 *
 * @return The number of ticks in 1 physical second of the monotonic clock  .
 */
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeFreq(void);

/**
 * Suspends the execution of the calling thread for at least given milliseconds.
 *
 * @param ms The duration to sleep for in milliseconds.
 */
PRP_API void PRP_CALL PRP_TimeSleep(PRP_F64 ms);
/**
 * Suspends the execution of the calling thread for at least given ticks.
 *
 * @param ticks The duration to sleep for in ticks.
 */
PRP_API void PRP_CALL PRP_TimeSleepTicks(PRP_TimeTicks ticks);

/**
 * Yields execution of the calling thread to allow another runnable thread
 * to execute.
 *
 * This function is a scheduling hint and may return immediately if no other
 * thread is available to run. On platforms without explicit thread yielding,
 * this operation may have no effect.
 */
PRP_API void PRP_CALL PRP_TimeYield(void);

/**
 * Changes the platform's monotonic clock ticks into real world units.
 *
 * @param ticks The ticks to convert to physical units.
 * @param unit  The unit to convert the ticks to.
 *
 * @return Converted ticks into the defined time unit based on the platform's
 *         frequency.
 *
 * @note: Invalid unit case
 * - If the given unit is invalid, we default to returning 0.
 */
PRP_API PRP_F64 PRP_CALL PRP_TimeTicksToTimeUnits(PRP_TimeTicks ticks,
                                                  PRP_TimeUnit unit);

/* ---- TIMER ---- */

/**
 * A timer object.
 *
 * This only defines at what tick the timer was started at.
 */
typedef PRP_TimeTicks PRP_TimeTimer;

/**
 * Creates/Starts a new timer.
 *
 * @return The timer created.
 */
PRP_API PRP_TimeTimer PRP_CALL PRP_TimeTimerCreate(void);
/**
 * Finds the number of elapsed ticks since the timer was created.
 *
 * @param timer The timer to find elapsed ticks of.
 *
 * @return The number of ticks elapsed since the timer creation.
 */
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsedTicks(PRP_TimeTimer timer);
/**
 * Finds the elapsed time since the timer was created in the specified units.
 *
 * @param timer The timer to find elapsed time of.
 * @param unit  The unit of time to return in.
 *
 * @return The time elapsed since the timer creation in the specified unit.
 */
PRP_API PRP_F64 PRP_CALL PRP_TimeTimerElapsed(PRP_TimeTimer timer,
                                              PRP_TimeUnit unit);

/* ---- STOPWATCH ---- */

/**
 * A stopwatch implementation that can be started and stopped at will.
 *
 * Stores as state the total ticks the stopwatch has been running for,
 * regardless of how many times it was started/stopped.
 */
typedef struct PRP_TimeStopwatch {
    PRP_TimeTicks start;
    PRP_TimeTicks elapsed;
    PRP_Bool is_running;
} PRP_TimeStopwatch;

/**
 * Creates a new stopwatch making sure it is correctly initialized.
 */
#define PRP_TIME_STOPWATCH_CREATE ((PRP_TimeStopwatch){0})

/**
 * Starts an already created stopwatch.
 *
 * @param pSw PRP_TimeStopwatch instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pSw is provided as NULL.
 * @return PRP_ERR_INV_STATE if you try to start an already running stopwatch.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchStart(PRP_TimeStopwatch *pSw);
/**
 * Stops an already running stopwatch.
 *
 * @param pSw PRP_TimeStopwatch instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pSw is provided as NULL.
 * @return PRP_ERR_INV_STATE if you try to stop an already stopped stopwatch.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchStop(PRP_TimeStopwatch *pSw);
/**
 * Resets the stopwatch to a new state. All previous state is lost.
 *
 * @param pSw PRP_TimeStopwatch instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pSw is provided as NULL.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchReset(PRP_TimeStopwatch *pSw);
/**
 * Finds the accumulated elapsed ticks of the stopwatch running.
 *
 * @param pSw    PRP_TimeStopwatch instance.
 * @param pTicks Output pointer to store the elapsed ticks.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments is/are NULL.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchElapsedTicks(
    const PRP_TimeStopwatch *pSw, PRP_TimeTicks *pTicks);
/**
 * Finds the accumulated elapsed time of the stopwatch running in units asked.
 *
 * @param pSw   PRP_TimeStopwatch instance.
 * @param unit  The unit to get the time into.
 * @param pTime Output pointer to store the elapsed time.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments is/are NULL.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeStopwatchElapsed(
    const PRP_TimeStopwatch *pSw, PRP_TimeUnit unit, PRP_F64 *pTime);

/* ---- FRAME TIMING ---- */

/**
 * Defines the tick interval the frame accumulator averages out the fps for.
 *
 * This is used to smooth out random jitter in fps caused by system noise.
 */
#define PRP_TIME_FRAME_TIMER_ACCUMULATION_TICKS (500000000)

/**
 * A frame timer object
 *
 * Stores all the info related to frame_count, delta_time and dps of the program
 * running.
 *
 * The fps stored is smoothed out over a specified amount of ticks.
 */
typedef struct PRP_TimeFrameTimer {
    // The tick at the beginning of the frame.
    PRP_TimeTicks frame_start;
    // Duration of the previous frame.
    PRP_TimeTicks frame_delta;
    /**
     * Total number of frames since initialization.
     *
     * Fun Fact:
     * If the application ran at 32kfps
     * For the frame count to overflow it will take the app to be running for
     * 18279.450542346 millennia(1 Millennium = 1000 years).
     */
    PRP_U64 frame_count;

    /*
     * Accumulates frame deltas to average them out for a smoother fps curve.
     *
     * This accumulator accumulates frames till a predefined averaging interval
     * has passed.
     */
    PRP_TimeTicks fps_accumulator;
    // Number of frames it took to accumulaate 1 sec of delta times.
    PRP_U64 accumulated_frame_count;
    // Averaged out fps.
    PRP_F64 fps;
} PRP_TimeFrameTimer;

/**
 * Creates a new frame timer making sure it is correctly initialized.
 */
#define PRP_TIME_FRAME_TIMER_CREATE                                            \
    ((PRP_TimeFrameTimer){.frame_start = PRP_TimeNow()})

/**
 * Starts a new frame inside the frame timer.
 * Starting a new frame updates all the accumulation and metadatas.
 *
 * @param pF_timer PRP_TimeFrameTimer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pF_timer is provided as NULL.
 */
PRP_API
PRP_Result PRP_CALL PRP_TimeFrameTimerBegin(PRP_TimeFrameTimer *pF_timer);
/**
 * Resets the frame timer to a new state. All previous state is lost.
 *
 * @param pF_timer PRP_TimeFrameTimer instance.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pF_timer is provided as NULL.
 */
PRP_API PRP_Result PRP_CALL
PRP_TimeFrameTimerReset(PRP_TimeFrameTimer *pF_timer);
/**
 * Queries the delta ticks b/w start and end of the previous frame.
 *
 * @param pF_timer PRP_TimeFrameTimer instance.
 * @param pTicks   Output pointer the delta ticks.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments is/are NULL.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeFrameTimerDeltaTicks(
    const PRP_TimeFrameTimer *pF_timer, PRP_TimeTicks *pTicks);
/**
 * Queries the delta time b/w start and end of the previous frame in the
 * requested units.
 *
 * @param pF_timer PRP_TimeFrameTimer instance.
 * @param unit     The unit to get the delta time into.
 * @param pTime    Output pointer the delta time.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments is/are NULL.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeFrameTimerDeltaTime(
    const PRP_TimeFrameTimer *pF_timer, PRP_TimeUnit unit, PRP_F64 *pTime);
/**
 * Queries the averaged out smoothed fps the frame timer is storing.
 *
 * @param pF_timer PRP_TimeFrameTimer instance.
 * @param pFPS     Output pointer the smoothed fps.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments is/are NULL.
 *
 * @note Accumulation cycle:
 * - Before the first smoothing out cycle of the frame timer completes, the fps
 * is 0.
 */
PRP_API PRP_Result PRP_CALL
PRP_TimeFrameTimerFPS(const PRP_TimeFrameTimer *pF_timer, PRP_F64 *pFPS);
/**
 * Queries the total number of frames elapesed since the creation of the frame
 * counter.
 *
 * @param pF_timer     PRP_TimeFrameTimer instance.
 * @param pFrame_count Output pointer the frame count.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments is/are NULL.
 */
PRP_API PRP_Result PRP_CALL PRP_TimeFrameTimerFrameCount(
    const PRP_TimeFrameTimer *pF_timer, PRP_U64 *pFrame_count);

#ifdef __cplusplus
}
#endif

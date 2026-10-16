#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Platform.h"
#include "Core/Types.h"

/* ---- TIMING ---- */

/**
 * 1 tick represents a singular increment into the platform's monotonic
 * clock.
 *
 * This is an int value used to reduce the floating point error accumulation.
 */
typedef PRP_u64 PRP_TimeTicks;

/**
 * Defines all the time units our time api supports.
 *
 * PRP_TIME_UNIT_NS: nanoseconds
 * PRP_TIME_UNIT_US: microseconds
 * PRP_TIME_UNIT_MS: milliseconds
 * PRP_TIME_UNIT_S : seconds
 */
typedef enum PRP_TimeUnit : PRP_u8 {
    PRP_TIME_UNIT_NS = 0,
    PRP_TIME_UNIT_US = 1,
    PRP_TIME_UNIT_MS = 2,
    PRP_TIME_UNIT_S  = 3,
} PRP_TimeUnit;

/**
 * Queries the value of the platform's monotonic clock.
 *
 * @return The value of the monotonic clock in ticks.
 */
[[nodiscard("Called PRP_TimeNow without using the result.")]]
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeNow(void);
/**
 * Queries the platform's monotonic clock's frequency(ticks per second).
 *
 * @return The number of ticks in 1 physical second of the monotonic clock.
 */
[[nodiscard("Called PRP_TimeFreq without using the result.")]]
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeFreq(void);

/**
 * Changes the time ticks into described units.
 *
 * @param ticks The ticks to convert to physical units.
 * @param unit  The unit to convert the ticks to.
 *
 * @return -1 if the unit is invalid.
 * @return Converted ticks into the time unit based on the platform's frequency.
 */
[[nodiscard("Called PRP_TimeTicksToUnits without using the result.")]]
PRP_API PRP_f64 PRP_CALL PRP_TimeTicksToUnits(PRP_TimeTicks ticks,
                                              PRP_TimeUnit  unit);

/**
 * Suspends the execution of the calling thread for at least given milliseconds.
 *
 * @param ms The duration to sleep for in milliseconds.
 */
PRP_API void PRP_CALL PRP_TimeSleep(PRP_f64 ms);
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
 * @note:
 * -This function is a scheduling hint and may return immediately if no other
 *  thread is available to run. On platforms without explicit thread yielding,
 *  this operation may have no effect.
 */
PRP_API void PRP_CALL PRP_TimeYield(void);

/* ---- TIMER ---- */

/**
 * A standard timer.
 */
typedef PRP_TimeTicks PRP_TimeTimer;

/**
 * Creates or starts a new timer.
 *
 * @return The timer created.
 */
[[nodiscard("Called PRP_TimeTimerCreate without using the result.")]]
PRP_API PRP_TimeTimer PRP_CALL PRP_TimeTimerCreate(void);
/**
 * Finds the number of elapsed ticks since the timer was created.
 *
 * @param timer The timer to find elapsed ticks of.
 *
 * @return The number of ticks elapsed since the timer creation.
 */
[[nodiscard("Called PRP_TimeTimerElapsedTicks without using the result.")]]
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsedTicks(PRP_TimeTimer timer);
/**
 * Finds the elapsed time since the timer was created in the described units.
 *
 * @param timer The timer to find elapsed time of.
 * @param unit  The unit of time to return in.
 *
 * @return The time elapsed since the timer creation in the specified unit.
 */
[[nodiscard("Called PRP_TimeTimerElapsed without using the result.")]]
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsed(PRP_TimeTimer timer,
                                                    PRP_TimeUnit  unit);

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
    PRP_bool      is_running;
} PRP_TimeStopwatch;

/**
 * Creates a new stopwatch making sure it is correctly initialized.
 */
#define PRP_TIME_STOPWATCH_CREATE ((PRP_TimeStopwatch){0})

/**
 * Starts an already created stopwatch.
 *
 * @param pSw The stopwatch object to start.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if pSw is provided as NULL.
 * @return PRP_ERR_INV_STATE if you try to start an already running stopwatch.
 */
[[nodiscard("PRP_TimeStopwatchStart can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchStart(PRP_TimeStopwatch *pSw);
/**
 * Stops an already running stopwatch.
 *
 * @param pSw The stopwatch object to stop.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 * @return PRP_ERR_INV_STATE if you try to stop an already stopped stopwatch.
 */
[[nodiscard("PRP_TimeStopwatchStop can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchStop(PRP_TimeStopwatch *pSw);
/**
 * Resets the stopwatch to a new state. All previous state is lost.
 *
 * @param pSw The stopwatch object to reset.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard("PRP_TimeStopwatchReset can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchReset(PRP_TimeStopwatch *pSw);
/**
 * Gets the accumulated elapsed ticks of the stopwatch running.
 *
 * @param pSw    The stopwatch object to query into.
 * @param pTicks Output pointer to store the elapsed ticks.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeStopwatchElapsedTicks can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchElapsedTicks(
    const PRP_TimeStopwatch *pSw, PRP_TimeTicks *pTicks);
/**
 * Gets the accumulated elapsed time of the stopwatch running in described
 * units.
 *
 * @param pSw   The stopwatch object to query into.
 * @param pTime Output pointer to store the elapsed time.
 * @param unit  The unit to get the time into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeStopwatchElapsed can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeStopwatchElapsed(const PRP_TimeStopwatch *pSw,
                                                   PRP_f64     *pTime,
                                                   PRP_TimeUnit unit);

/* ---- FRAME TIMING ---- */

/**
 * Defines the tick interval the frame accumulator averages out the fps for.
 *
 * This is used to smooth out random jitter in fps caused by system noise.
 */
#define PRP_TIME_FRAME_TIMER_ACCUMULATION_TICKS (500'000'000)

/**
 * A frame timer object
 *
 * Stores all the info related to frame_count, delta_time and dps of the program
 * running.
 *
 * The fps stored is smoothed out over a specified amount of ticks.
 */
typedef struct PRP_TimeFrameTimer {
    PRP_TimeTicks frame_start;     // The tick at the starting of a frame.
    PRP_TimeTicks frame_delta;     // Duration of the previous frame.
    PRP_u64       frame_count;     // Total frames since initialization.
    PRP_TimeTicks fps_accumulator; // Frame delta accumulate for smooth fps.
    PRP_u64       accumulated_frame_count; // Frame to accumulate set delta.
    PRP_f64       fps;                     // Averages out fps.
} PRP_TimeFrameTimer;

/**
 * Creates a new frame timer making sure it is correctly initialized.
 */
#define PRP_TIME_FRAME_TIMER_CREATE                                            \
    ((PRP_TimeFrameTimer){.frame_start = PRP_TimeNow()})

/**
 * Starts a new frame inside the frame timer.
 * Starting a new frame updates all the accumulation and metadata.
 *
 * @param pF_timer The frame timer object to begin.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeFrameTimerBegin can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeFrameTimerBegin(PRP_TimeFrameTimer *pF_timer);
/**
 * Resets the frame timer to a new state. All previous state is lost.
 *
 * @param pF_timer The frame timer object to reset.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeFrameTimerReset can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeFrameTimerReset(PRP_TimeFrameTimer *pF_timer);
/**
 * Queries the delta ticks b/w start and end of the previous frame.
 *
 * @param pF_timer The frame timer object to query into.
 * @param pTicks   Output pointer the delta ticks.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeFrameTimerDeltaTicks can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeFrameTimerDeltaTicks(
    const PRP_TimeFrameTimer *pF_timer, PRP_TimeTicks *pTicks);
/**
 * Queries the delta time b/w start and end of the previous frame in the
 * described unit.
 *
 * @param pF_timer The frame timer object to query into.
 * @param pTime    Output pointer the delta time.
 * @param unit     The unit to get the delta time into.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeFrameTimerDeltaTime can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL
PRP_TimeFrameTimerDeltaTime(const PRP_TimeFrameTimer *pF_timer,
                            PRP_TimeTicks *pTime, PRP_TimeUnit unit);
/**
 * Queries the averaged out fps the frame timer is storing.
 *
 * @param pF_timer The frame timer object to query into.
 * @param pFPS     Output pointer the smoothed fps.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 *
 * @note Accumulation cycle:
 * - Before the first smoothing out cycle of the frame timer completes, the fps
 * is 0.
 */
[[nodiscard("PRP_TimeFrameTimerFPS can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL
PRP_TimeFrameTimerFPS(const PRP_TimeFrameTimer *pF_timer, PRP_f64 *pFPS);
/**
 * Queries the total number of frames elapsed since the creation of the frame
 * counter.
 *
 * @param pF_timer     The frame timer object to query into.
 * @param pFrame_count Output pointer the frame count.
 *
 * @return PRP_OK on success.
 * @return PRP_ERR_INV_ARG if arguments are invalid.
 */
[[nodiscard(
    "PRP_TimeFrameTimerFrameCount can fail and lead to undefined behavior.")]]
PRP_API PRP_rslt PRP_CALL PRP_TimeFrameTimerFrameCount(
    const PRP_TimeFrameTimer *pF_timer, PRP_u64 *pFrame_count);

#ifdef __cplusplus
}
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Defs.h"

/* ----  TIME MEASUREMENT  ---- */

/**
 * The units of time that the timer supports.
 *
 * PRP_TIME_NS: Nanoseconds
 * PRP_TIME_US: Microseconds
 * PRP_TIME_MS: Milliseconds
 * PRP_TIME_S: Seconds
 */
typedef enum { PRP_TIME_NS, PRP_TIME_US, PRP_TIME_MS, PRP_TIME_S } PRP_TimeUnit;

// The actual time measure that we will get in float.
typedef DT_f64 PRP_TimeMeasure;
#define PRP_INVALID_TIME_MEASURE (-1)

/**
 * Gets the current time in the specified unit.
 *
 * @param unit: The unit to get the result into.
 *
 * @return The time in the given unit, with PRP_TIME_NS as a fallback if unit is
 * invalid.
 */
PRP_FN_API PRP_TimeMeasure PRP_FN_CALL PRP_TimerGet(PRP_TimeUnit unit);

/* ----  TIMER  ---- */

/**
 * Keeps track of the start and the current unit of the timer.
 */
typedef struct {
    PRP_TimeMeasure start;
    PRP_TimeUnit unit;
} PRP_Timer;

#define PRP_TIMER(unit) ((PRP_Timer){.unit = (unit)})

/**
 * Starts the timer by setting the current time. To set current time to the
 * timer simply recall this same function on the timer and it will update the
 * timer to the current time.
 *
 * @param timer: The timer to start.
 *
 * @return PRP_FN_INV_ARG_ERROR if the timer is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_TimerStart(PRP_Timer *timer);
/**
 * Gets the total elapsed time from the timer start till the call time.
 *
 * @param timer: The timer to get elapsed time for.
 *
 * @return PRP_INVALID_TIME_MEASURE if the timer is invalid, otherwise the
 * elapsed time in the timer's units.
 */
PRP_FN_API PRP_TimeMeasure PRP_FN_CALL PRP_TimerElapsed(PRP_Timer *timer);
/**
 * Changes the unit of the timer and updates the currently set time safely.
 *
 * @param timer: The timer to change the unit for.
 * @param unit: The new unit of the timer to set.
 *
 * @return PRP_INVALID_TIME_MEASURE if the timer is invalid, otherwise
 * PRP_FN_SUCCESS.
 */
PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_TimerChangeUnit(PRP_Timer *timer,
                                                      PRP_TimeUnit unit);

#ifdef __cplusplus
}
#endif

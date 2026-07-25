#pragma once

#include "Core/Types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

/* ---- TIMING ---- */

typedef PRP_U64 PRP_TimeTicks;

typedef enum PRP_TimeUnit {
    PRP_TIME_UNIT_NS,
    PRP_TIME_UNIT_US,
    PRP_TIME_UNIT_MS,
    PRP_TIME_UNIT_S,
} PRP_TimeUnit;

PRP_API PRP_TimeTicks PRP_CALL PRP_TimeNow(void);
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeFreq(void);

PRP_API PRP_F64 PRP_CALL PRP_TimeToNS(PRP_TimeTicks ticks);
PRP_API PRP_F64 PRP_CALL PRP_TimeToUS(PRP_TimeTicks ticks);
PRP_API PRP_F64 PRP_CALL PRP_TimeToMS(PRP_TimeTicks ticks);
PRP_API PRP_F64 PRP_CALL PRP_TimeToS(PRP_TimeTicks ticks);

/* ---- TIMER ---- */

typedef PRP_TimeTicks PRP_TimeTimer;

PRP_API PRP_TimeTimer PRP_CALL PRP_TimeTimerStart(void);
PRP_API PRP_TimeTicks PRP_CALL PRP_TimeTimerElapsedTicks(PRP_TimeTimer timer);
PRP_API PRP_F64 PRP_CALL PRP_TimeTimerElapsed(PRP_TimeTimer timer,
                                              PRP_TimeUnit unit);

/* ---- STOPWATCH ---- */

typedef struct PRP_TimeStopwatch {

} PRP_TimeStopwatch;

#ifdef __cplusplus
}
#endif

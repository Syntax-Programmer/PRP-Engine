#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include <float.h>
#include <math.h>

/* ----  FUNDAMENTAL CONSTS  ---- */

#define MATH_PI_F32 ((DT_f32)3.14159265358979323846f)
#define MATH_PI_F64 ((DT_f64)3.14159265358979323846264338327950288)

#define MATH_TAU_F32 ((DT_f32)6.28318530717958647692f)
#define MATH_TAU_F64 ((DT_f64)6.28318530717958647692528676655900576)

#define MATH_HALF_PI_F32 ((DT_f32)1.57079632679489661923f)
#define MATH_HALF_PI_F64 ((DT_f64)1.57079632679489661923132169163975144)

#define MATH_QUARTER_PI_F32 ((DT_f32)0.78539816339744830962f)
#define MATH_QUARTER_PI_F64 ((DT_f64)0.78539816339744830961566084581987572)

#define MATH_INV_PI_F32 ((DT_f32)(1.0f / MATH_PI_F32))
#define MATH_INV_PI_F64 ((DT_f64)(1.0 / MATH_PI_F64))

#define MATH_INV_TAU_F32 ((DT_f32)(1.0f / MATH_TAU_F32))
#define MATH_INV_TAU_F64 ((DT_f64)(1.0 / MATH_TAU_F64))

/* ----  ANGLE CONVERSIONS  ---- */

#define MATH_DEG2RAD_F32 (MATH_PI_F32 / (DT_f32)180.0f)
#define MATH_RAD2DEG_F32 ((DT_f32)180.0f / MATH_PI_F32)

#define MATH_DEG2RAD_F64 (MATH_PI_F64 / (DT_f64)180.0)
#define MATH_RAD2DEG_F64 ((DT_f64)180.0 / MATH_PI_F64)

/* ----  FLOAT POLICY  ---- */

// General float comparison tolerance.
#define MATH_COMPARE_EPS_F32 ((DT_f32)1e-6f)
#define MATH_COMPARE_EPS_F64 ((DT_f64)1e-12)

// Used when guarding normalization/division.
#define MATH_NORMALIZE_EPS_F32 ((DT_f32)1e-8f)
#define MATH_NORMALIZE_EPS_F64 ((DT_f64)1e-14)

// Squared epsilon (avoids sqrt).
#define MATH_EPSILON_SQ_F32 ((DT_f32)1e-12f)
#define MATH_EPSILON_SQ_F64 ((DT_f64)1e-24)

// Denormal guard threshold.
#define MATH_TINY_F32 ((DT_f32)1e-8f)
#define MATH_TINY_F64 ((DT_f64)1e-14)

/* ----  SPECIAL VALUES  ---- */

#define MATH_INFINITY_F32 ((DT_f32)INFINITY)
#define MATH_INFINITY_F64 ((DT_f64)INFINITY)

#define MATH_NAN_F32 ((DT_f32)NAN)
#define MATH_NAN_F64 ((DT_f64)NAN)

/* ----  FLOAT LIMITS  ---- */

#define MATH_F32_MAX ((DT_f32)FLT_MAX)
#define MATH_F32_MIN ((DT_f32)FLT_MIN)

#define MATH_F64_MAX ((DT_f64)DBL_MAX)
#define MATH_F64_MIN ((DT_f64)DBL_MIN)

/* ----  NUMERIC POLICY  ---- */

// Default world up direction.
#define MATH_WORLD_UP_X ((DT_f32)0.0f)
#define MATH_WORLD_UP_Y ((DT_f32)1.0f)
#define MATH_WORLD_UP_Z ((DT_f32)0.0f)

// Default gravity magnitude.
#define MATH_GRAVITY ((DT_f32)(-9.80665f))

#ifdef __cplusplus
}
#endif

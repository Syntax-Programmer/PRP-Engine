#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Data-Types/Typedefs.h"
#include <math.h>

/* ----  GENERIC FUNCTIONS  ---- */

static inline DT_f32 MATH_MinF32(DT_f32 a, DT_f32 b) { return (a > b) ? b : a; }

static inline DT_f64 MATH_MinF64(DT_f64 a, DT_f64 b) { return (a > b) ? b : a; }

static inline DT_f32 MATH_MaxF32(DT_f32 a, DT_f32 b) { return (a > b) ? a : b; }

static inline DT_f64 MATH_MaxF64(DT_f64 a, DT_f64 b) { return (a > b) ? a : b; }

static inline DT_f32 MATH_ClampF32(DT_f32 a, DT_f32 min, DT_f32 max) {
    return MATH_MinF32(max, MATH_MaxF32(min, a));
}

static inline DT_f64 MATH_ClampF64(DT_f64 a, DT_f64 min, DT_f64 max) {
    return MATH_MinF64(max, MATH_MaxF64(min, a));
}

static inline DT_f32 MATH_SaturateF32(DT_f32 a) {
    return MATH_ClampF32(a, 0, 1);
}
static inline DT_f64 MATH_SaturateF64(DT_f64 a) {
    return MATH_ClampF64(a, 0, 1);
}

static inline DT_f32 MATH_AbsF32(DT_f32 a) {
    return (a < (DT_f32)0.0f) ? -a : a;
}

static inline DT_f64 MATH_AbsF64(DT_f64 a) {
    return (a < (DT_f64)0.0f) ? -a : a;
}

static inline DT_f32 MATH_SignF32(DT_f32 a) {
    DT_f32 zero = 0.0;

    return (DT_f32)((a > zero) - (a < zero));
}

static inline DT_f64 MATH_SignF64(DT_f64 a) {
    DT_f64 zero = 0.0;

    return (DT_f64)((a > zero) - (a < zero));
}

static inline DT_f32 MATH_FloorF32(DT_f32 a) { return floorf(a); }
static inline DT_f64 MATH_FloorF64(DT_f64 a) { return floor(a); }

static inline DT_f32 MATH_CeilF32(DT_f32 a) { return ceilf(a); }
static inline DT_f64 MATH_CeilF64(DT_f64 a) { return ceil(a); }

static inline DT_f32 MATH_TruncF32(DT_f32 a) { return truncf(a); }
static inline DT_f64 MATH_TruncF64(DT_f64 a) { return trunc(a); }

static inline DT_f32 MATH_RoundF32(DT_f32 a) { return roundf(a); }
static inline DT_f64 MATH_RoundF64(DT_f64 a) { return round(a); }

static inline DT_f32 MATH_FracF32(DT_f32 a) { return (a - MATH_FloorF32(a)); }
static inline DT_f64 MATH_FracF64(DT_f64 a) { return (a - MATH_FloorF64(a)); }

static inline DT_bool MATH_InRangeIncF32(DT_f32 a, DT_f32 min, DT_f32 max) {
    return (DT_bool)((a >= min) && (a <= max));
}

static inline DT_bool MATH_InRangeIncF64(DT_f64 a, DT_f64 min, DT_f64 max) {
    return (DT_bool)((a >= min) && (a <= max));
}

static inline DT_bool MATH_InRangeExcF32(DT_f32 a, DT_f32 min, DT_f32 max) {
    return (DT_bool)((a > min) && (a < max));
}

static inline DT_bool MATH_InRangeExcF64(DT_f64 a, DT_f64 min, DT_f64 max) {
    return (DT_bool)((a > min) && (a < max));
}

static inline DT_f32 MATH_CopySignF32(DT_f32 mag, DT_f32 sign) {
    return copysignf(mag, sign);
}

static inline DT_f64 MATH_CopySignF64(DT_f64 mag, DT_f64 sign) {
    return copysign(mag, sign);
}

// Move the below functions to interpolation sub-directory

// /* ----  INTERPOLATION  ---- */

// static inline DT_f32 MATH_LerpF32(DT_f32 a, DT_f32 b, DT_f32 t);
// static inline DT_f64 MATH_LerpF64(DT_f64 a, DT_f64 b, DT_f64 t);

// static inline DT_f32 MATH_InvLerpF32(DT_f32 a, DT_f32 b, DT_f32 x);
// static inline DT_f64 MATH_InvLerpF64(DT_f64 a, DT_f64 b, DT_f64 x);

// /* ----  RANGE MAPPING  ---- */

// static inline DT_f32 MATH_RemapF32(DT_f32 in_min, DT_f32 in_max, DT_f32
// out_min,
//                                    DT_f32 out_max, DT_f32 x);
// static inline DT_f64 MATH_RemapF64(DT_f64 in_min, DT_f64 in_max, DT_f64
// out_min,
//                                    DT_f64 out_max, DT_f64 x);
// static inline DT_f32 MATH_RemapClampedF32(DT_f32 in_min, DT_f32 in_max,
//                                           DT_f32 out_min, DT_f32 out_max,
//                                           DT_f32 x);
// static inline DT_f64 MATH_RemapClampedF64(DT_f64 in_min, DT_f64 in_max,
//                                           DT_f64 out_min, DT_f64 out_max,
//                                           DT_f64 x);

// /* ----  STEP  ---- */

// static inline DT_f32 MATH_StepF32(DT_f32 edge, DT_f32 x);
// static inline DT_f64 MATH_StepF64(DT_f64 edge, DT_f64 x);

// static inline DT_f32 MATH_CubicHermiteStepF32(DT_f32 a, DT_f32 b, DT_f32 x);
// static inline DT_f64 MATH_CubicHermiteStepF64(DT_f64 a, DT_f64 b, DT_f64 x);

// static inline DT_f32 MATH_QuinticHermiteStepF32(DT_f32 a, DT_f32 b, DT_f32
// x); static inline DT_f64 MATH_QuinticHermiteStepF64(DT_f64 a, DT_f64 b,
// DT_f64 x);

#ifdef __cplusplus
}
#endif

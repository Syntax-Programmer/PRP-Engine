#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Types.h"
#include <math.h>

/* ----  GENERIC FUNCTIONS  ---- */

static inline PRP_F32 MATH_MinF32(PRP_F32 a, PRP_F32 b) {
    return (a > b) ? b : a;
}

static inline PRP_F64 MATH_MinF64(PRP_F64 a, PRP_F64 b) {
    return (a > b) ? b : a;
}

static inline PRP_F32 MATH_MaxF32(PRP_F32 a, PRP_F32 b) {
    return (a > b) ? a : b;
}

static inline PRP_F64 MATH_MaxF64(PRP_F64 a, PRP_F64 b) {
    return (a > b) ? a : b;
}

static inline PRP_F32 MATH_ClampF32(PRP_F32 a, PRP_F32 min, PRP_F32 max) {
    return MATH_MinF32(max, MATH_MaxF32(min, a));
}

static inline PRP_F64 MATH_ClampF64(PRP_F64 a, PRP_F64 min, PRP_F64 max) {
    return MATH_MinF64(max, MATH_MaxF64(min, a));
}

static inline PRP_F32 MATH_SaturateF32(PRP_F32 a) {
    return MATH_ClampF32(a, 0, 1);
}
static inline PRP_F64 MATH_SaturateF64(PRP_F64 a) {
    return MATH_ClampF64(a, 0, 1);
}

static inline PRP_F32 MATH_AbsF32(PRP_F32 a) {
    return (a < (PRP_F32)0.0f) ? -a : a;
}

static inline PRP_F64 MATH_AbsF64(PRP_F64 a) {
    return (a < (PRP_F64)0.0f) ? -a : a;
}

static inline PRP_F32 MATH_SignF32(PRP_F32 a) {
    PRP_F32 zero = 0.0;

    return (PRP_F32)((a > zero) - (a < zero));
}

static inline PRP_F64 MATH_SignF64(PRP_F64 a) {
    PRP_F64 zero = 0.0;

    return (PRP_F64)((a > zero) - (a < zero));
}

static inline PRP_F32 MATH_FloorF32(PRP_F32 a) { return floorf(a); }
static inline PRP_F64 MATH_FloorF64(PRP_F64 a) { return floor(a); }

static inline PRP_F32 MATH_CeilF32(PRP_F32 a) { return ceilf(a); }
static inline PRP_F64 MATH_CeilF64(PRP_F64 a) { return ceil(a); }

static inline PRP_F32 MATH_TruncF32(PRP_F32 a) { return truncf(a); }
static inline PRP_F64 MATH_TruncF64(PRP_F64 a) { return trunc(a); }

static inline PRP_F32 MATH_RoundF32(PRP_F32 a) { return roundf(a); }
static inline PRP_F64 MATH_RoundF64(PRP_F64 a) { return round(a); }

static inline PRP_F32 MATH_FracF32(PRP_F32 a) { return (a - MATH_FloorF32(a)); }
static inline PRP_F64 MATH_FracF64(PRP_F64 a) { return (a - MATH_FloorF64(a)); }

static inline PRP_Bool MATH_InRangeIncF32(PRP_F32 a, PRP_F32 min, PRP_F32 max) {
    return (PRP_Bool)((a >= min) && (a <= max));
}

static inline PRP_Bool MATH_InRangeIncF64(PRP_F64 a, PRP_F64 min, PRP_F64 max) {
    return (PRP_Bool)((a >= min) && (a <= max));
}

static inline PRP_Bool MATH_InRangeExcF32(PRP_F32 a, PRP_F32 min, PRP_F32 max) {
    return (PRP_Bool)((a > min) && (a < max));
}

static inline PRP_Bool MATH_InRangeExcF64(PRP_F64 a, PRP_F64 min, PRP_F64 max) {
    return (PRP_Bool)((a > min) && (a < max));
}

static inline PRP_F32 MATH_CopySignF32(PRP_F32 mag, PRP_F32 sign) {
    return copysignf(mag, sign);
}

static inline PRP_F64 MATH_CopySignF64(PRP_F64 mag, PRP_F64 sign) {
    return copysign(mag, sign);
}

// Move the below functions to interpolation sub-directory

// /* ----  INTERPOLATION  ---- */

// static inline PRP_F32 MATH_LerpF32(PRP_F32 a, PRP_F32 b, PRP_F32 t);
// static inline PRP_F64 MATH_LerpF64(PRP_F64 a, PRP_F64 b, PRP_F64 t);

// static inline PRP_F32 MATH_InvLerpF32(PRP_F32 a, PRP_F32 b, PRP_F32 x);
// static inline PRP_F64 MATH_InvLerpF64(PRP_F64 a, PRP_F64 b, PRP_F64 x);

// /* ----  RANGE MAPPING  ---- */

// static inline PRP_F32 MATH_RemapF32(PRP_F32 in_min, PRP_F32 in_max, PRP_F32
// out_min,
//                                    PRP_F32 out_max, PRP_F32 x);
// static inline PRP_F64 MATH_RemapF64(PRP_F64 in_min, PRP_F64 in_max, PRP_F64
// out_min,
//                                    PRP_F64 out_max, PRP_F64 x);
// static inline PRP_F32 MATH_RemapClampedF32(PRP_F32 in_min, PRP_F32 in_max,
//                                           PRP_F32 out_min, PRP_F32 out_max,
//                                           PRP_F32 x);
// static inline PRP_F64 MATH_RemapClampedF64(PRP_F64 in_min, PRP_F64 in_max,
//                                           PRP_F64 out_min, PRP_F64 out_max,
//                                           PRP_F64 x);

// /* ----  STEP  ---- */

// static inline PRP_F32 MATH_StepF32(PRP_F32 edge, PRP_F32 x);
// static inline PRP_F64 MATH_StepF64(PRP_F64 edge, PRP_F64 x);

// static inline PRP_F32 MATH_CubicHermiteStepF32(PRP_F32 a, PRP_F32 b, PRP_F32
// x); static inline PRP_F64 MATH_CubicHermiteStepF64(PRP_F64 a, PRP_F64 b,
// PRP_F64 x);

// static inline PRP_F32 MATH_QuinticHermiteStepF32(PRP_F32 a, PRP_F32 b,
// PRP_F32 x); static inline PRP_F64 MATH_QuinticHermiteStepF64(PRP_F64 a,
// PRP_F64 b, PRP_F64 x);

#ifdef __cplusplus
}
#endif

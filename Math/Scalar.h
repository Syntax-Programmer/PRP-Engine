#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Consts.h"

/* ----  MIN / MAX  ---- */

static inline DT_f32 MATH_MinF32(DT_f32 a, DT_f32 b) { return (a < b) ? a : b; }

static inline DT_f64 MATH_MinF64(DT_f64 a, DT_f64 b) { return (a < b) ? a : b; }

static inline DT_f32 MATH_MaxF32(DT_f32 a, DT_f32 b) { return (a > b) ? a : b; }

static inline DT_f64 MATH_MaxF64(DT_f64 a, DT_f64 b) { return (a > b) ? a : b; }

/* ----  CLAMP / SATURATE  ---- */

static inline DT_f32 MATH_ClampF32(DT_f32 a, DT_f32 min, DT_f32 max) {
    return MATH_MinF32(MATH_MaxF32(a, min), max);
}

static inline DT_f64 MATH_ClampF64(DT_f64 a, DT_f64 min, DT_f64 max) {
    return MATH_MinF64(MATH_MaxF64(a, min), max);
}

static inline DT_f32 MATH_SaturateF32(DT_f32 a) {
    return MATH_ClampF32(a, (DT_f32)0.0f, (DT_f32)1.0f);
}

static inline DT_f64 MATH_SaturateF64(DT_f64 a) {
    return MATH_ClampF64(a, (DT_f64)0.0, (DT_f64)1.0);
}

/* ----  ABS / SIGN  ---- */

static inline DT_f32 MATH_AbsF32(DT_f32 a) {
    return (a < (DT_f32)0.0f) ? -a : a;
}

static inline DT_f64 MATH_AbsF64(DT_f64 a) {
    return (a < (DT_f64)0.0) ? -a : a;
}

static inline DT_i32 MATH_SignI32(DT_i32 a) { return (a > 0) - (a < 0); }

static inline DT_f32 MATH_SignF32(DT_f32 a) {
    return (DT_f32)((a > 0.0f) - (a < 0.0f));
}

static inline DT_f64 MATH_SignF64(DT_f64 a) {
    return (DT_f64)((a > 0.0) - (a < 0.0));
}

/* ----  LERP  ---- */

static inline DT_f32 MATH_LerpF32(DT_f32 a, DT_f32 b, DT_f32 t) {
    return a + (b - a) * t;
}

static inline DT_f64 MATH_LerpF64(DT_f64 a, DT_f64 b, DT_f64 t) {
    return a + (b - a) * t;
}

/* ----  SMOOTHSTEP  ---- */

static inline DT_f32 MATH_SmoothStepF32(DT_f32 a, DT_f32 b, DT_f32 t) {
    DT_f32 denom = b - a;

    if (MATH_AbsF32(denom) <= MATH_NORMALIZE_EPS_F32)
        return a;

    t = MATH_SaturateF32((t - a) / denom);
    return t * t * ((DT_f32)3.0f - (DT_f32)2.0f * t);
}

static inline DT_f64 MATH_SmoothStepF64(DT_f64 a, DT_f64 b, DT_f64 t) {
    DT_f64 denom = b - a;

    if (MATH_AbsF64(denom) <= MATH_NORMALIZE_EPS_F64)
        return a;

    t = MATH_SaturateF64((t - a) / denom);
    return t * t * ((DT_f64)3.0 - (DT_f64)2.0 * t);
}

/* ----  REMAP  ---- */

static inline DT_f32 MATH_RemapF32(DT_f32 inMin, DT_f32 inMax, DT_f32 outMin,
                                   DT_f32 outMax, DT_f32 v) {
    DT_f32 denom = inMax - inMin;

    if (MATH_AbsF32(denom) <= MATH_NORMALIZE_EPS_F32)
        return outMin;

    DT_f32 t = (v - inMin) / denom;
    return MATH_LerpF32(outMin, outMax, t);
}

static inline DT_f64 MATH_RemapF64(DT_f64 inMin, DT_f64 inMax, DT_f64 outMin,
                                   DT_f64 outMax, DT_f64 v) {
    DT_f64 denom = inMax - inMin;

    if (MATH_AbsF64(denom) <= MATH_NORMALIZE_EPS_F64)
        return outMin;

    DT_f64 t = (v - inMin) / denom;
    return MATH_LerpF64(outMin, outMax, t);
}

/* ----  ROUNDING  ---- */

static inline DT_f32 MATH_FloorF32(DT_f32 v) { return floorf(v); }
static inline DT_f32 MATH_CeilF32(DT_f32 v) { return ceilf(v); }
static inline DT_f32 MATH_RoundF32(DT_f32 v) { return roundf(v); }

static inline DT_f64 MATH_FloorF64(DT_f64 v) { return floor(v); }
static inline DT_f64 MATH_CeilF64(DT_f64 v) { return ceil(v); }
static inline DT_f64 MATH_RoundF64(DT_f64 v) { return round(v); }

/* ----  SAFE DIVISION  ---- */

static inline DT_f32 MATH_SafeDivF32(DT_f32 a, DT_f32 b) {
    return (MATH_AbsF32(b) > MATH_NORMALIZE_EPS_F32) ? a / b : (DT_f32)0.0f;
}

static inline DT_f64 MATH_SafeDivF64(DT_f64 a, DT_f64 b) {
    return (MATH_AbsF64(b) > MATH_NORMALIZE_EPS_F64) ? a / b : (DT_f64)0.0;
}

/* ----  FLOAT COMPARISON  ---- */

static inline DT_bool MATH_AlmostEqualAbsF32(DT_f32 a, DT_f32 b) {
    return (MATH_AbsF32(a - b) <= MATH_COMPARE_EPS_F32) ? DT_true : DT_false;
}

static inline DT_bool MATH_AlmostEqualRelF32(DT_f32 a, DT_f32 b) {
    DT_f32 diff = MATH_AbsF32(a - b);
    DT_f32 largest = MATH_MaxF32(MATH_AbsF32(a), MATH_AbsF32(b));

    return (diff <= largest * MATH_COMPARE_EPS_F32) ? DT_true : DT_false;
}

static inline DT_bool MATH_AlmostEqualF32(DT_f32 a, DT_f32 b) {
    return (MATH_AlmostEqualAbsF32(a, b) || MATH_AlmostEqualRelF32(a, b))
               ? DT_true
               : DT_false;
}

static inline DT_bool MATH_AlmostEqualAbsF64(DT_f64 a, DT_f64 b) {
    return (MATH_AbsF64(a - b) <= MATH_COMPARE_EPS_F64) ? DT_true : DT_false;
}

static inline DT_bool MATH_AlmostEqualRelF64(DT_f64 a, DT_f64 b) {
    DT_f64 diff = MATH_AbsF64(a - b);
    DT_f64 largest = MATH_MaxF64(MATH_AbsF64(a), MATH_AbsF64(b));

    return (diff <= largest * MATH_COMPARE_EPS_F64) ? DT_true : DT_false;
}

static inline DT_bool MATH_AlmostEqualF64(DT_f64 a, DT_f64 b) {
    return (MATH_AlmostEqualAbsF64(a, b) || MATH_AlmostEqualRelF64(a, b))
               ? DT_true
               : DT_false;
}

#ifdef __cplusplus
}
#endif

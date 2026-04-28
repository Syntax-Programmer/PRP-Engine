#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Basic.h"

/* ----  FLOATING POINT LIMITS  ---- */

#define MATH_F32_MIN ((DT_f32)FLT_MIN)
#define MATH_F32_MAX ((DT_f32)FLT_MAX)
#define MATH_F32_LOWEST ((DT_f32)(-FLT_MAX))

#define MATH_F64_MIN ((DT_f64)DBL_MIN)
#define MATH_F64_MAX ((DT_f64)DBL_MAX)
#define MATH_F64_LOWEST ((DT_f64)(-DBL_MAX))

/* ----  FLOATING POINT POLICIES  ---- */

#define MATH_F32_EPS ((DT_f32)FLT_EPSILON)
#define MATH_F64_EPS ((DT_f64)DBL_EPSILON)

// Squared epsilon: useful for squared comparisons (avoid sqrt)
/* (2^-23)^2 = 2^-46 */
#define MATH_F32_SQ_EPS ((DT_f32)1.4210854715202004e-14f)
/* (2^-52)^2 = 2^-104 */
#define MATH_F64_SQ_EPS ((DT_f64)4.930380657631324e-32)

/*
 * Safe epsilon: practical tolerance for comparisons (this is NOT a standard
 * constant)
 */
#define MATH_F32_SAFE_EPS (1e-6f)
#define MATH_F64_SAFE_EPS (1e-12)

#define MATH_F32_INF ((DT_f32)INFINITY)
#define MATH_F64_INF ((DT_f64)INFINITY)

#define MATH_F32_NAN ((DT_f32)NAN)
#define MATH_F64_NAN ((DT_f64)NAN)

/* ----  FLOATING POINT FUNCTIONS  ---- */

static inline DT_bool MATH_IsZeroF32(DT_f32 x) {
    return (DT_bool)(MATH_AbsF32(x) <= MATH_F32_SAFE_EPS);
}

static inline DT_bool MATH_IsZeroF64(DT_f64 x) {
    return (DT_bool)(MATH_AbsF64(x) <= MATH_F64_SAFE_EPS);
}

static inline DT_bool MATH_IsNaNF32(DT_f32 x) { return (DT_bool)isnanf(x); }
static inline DT_bool MATH_IsNaNF64(DT_f64 x) { return (DT_bool)isnan(x); }

static inline DT_bool MATH_IsInfF32(DT_f32 x) { return (DT_bool)isinff(x); }
static inline DT_bool MATH_IsInfF64(DT_f64 x) { return (DT_bool)isinf(x); }

static inline DT_bool MATH_AlmostEqAbsF32(DT_f32 a, DT_f32 b) {
    return (DT_bool)(MATH_AbsF32(a - b) <= MATH_F32_SAFE_EPS);
}

static inline DT_bool MATH_AlmostEqAbsF64(DT_f64 a, DT_f64 b) {
    return (DT_bool)(MATH_AbsF64(a - b) <= MATH_F64_SAFE_EPS);
}

static inline DT_bool MATH_AlmostEqRelF32(DT_f32 a, DT_f32 b) {
    DT_f32 diff = MATH_AbsF32(a - b);
    DT_f32 largest = MATH_MaxF32(MATH_AbsF32(a), MATH_AbsF32(b));

    return (DT_bool)(diff <= MATH_F32_SAFE_EPS * largest);
}

static inline DT_bool MATH_AlmostEqRelF64(DT_f64 a, DT_f64 b) {
    DT_f64 diff = MATH_AbsF64(a - b);
    DT_f64 largest = MATH_MaxF64(MATH_AbsF64(a), MATH_AbsF64(b));

    return (DT_bool)(diff <= MATH_F64_SAFE_EPS * largest);
}

static inline DT_bool MATH_AlmostEqF32(DT_f32 a, DT_f32 b) {
    return (DT_bool)(MATH_AlmostEqAbsF32(a, b) || MATH_AlmostEqRelF32(a, b));
}

static inline DT_bool MATH_AlmostEqF64(DT_f64 a, DT_f64 b) {
    return (DT_bool)(MATH_AlmostEqAbsF64(a, b) || MATH_AlmostEqRelF64(a, b));
}

static inline DT_f32 MATH_SafeDivF32(DT_f32 a, DT_f32 b, DT_f32 fallback) {
    if (MATH_IsZeroF32(b)) {
        return fallback;
    }

    return a / b;
}
static inline DT_f64 MATH_SafeDivF64(DT_f64 a, DT_f64 b, DT_f64 fallback) {
    if (MATH_IsZeroF64(b)) {
        return fallback;
    }

    return a / b;
}

#ifdef __cplusplus
}
#endif

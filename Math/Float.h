#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Basic.h"

/* ----  FLOATING POINT LIMITS  ---- */

#define MATH_F32_MIN ((PRP_F32)FLT_MIN)
#define MATH_F32_MAX ((PRP_F32)FLT_MAX)
#define MATH_F32_LOWEST ((PRP_F32)(-FLT_MAX))

#define MATH_F64_MIN ((PRP_F64)DBL_MIN)
#define MATH_F64_MAX ((PRP_F64)DBL_MAX)
#define MATH_F64_LOWEST ((PRP_F64)(-DBL_MAX))

/* ----  FLOATING POINT POLICIES  ---- */

#define MATH_F32_EPS ((PRP_F32)FLT_EPSILON)
#define MATH_F64_EPS ((PRP_F64)DBL_EPSILON)

// Squared epsilon: useful for squared comparisons (avoid sqrt)
/* (2^-23)^2 = 2^-46 */
#define MATH_F32_SQ_EPS ((PRP_F32)1.4210854715202004e-14f)
/* (2^-52)^2 = 2^-104 */
#define MATH_F64_SQ_EPS ((PRP_F64)4.930380657631324e-32)

/*
 * Safe epsilon: practical tolerance for comparisons (this is NOT a standard
 * constant)
 */
#define MATH_F32_SAFE_EPS (1e-6f)
#define MATH_F64_SAFE_EPS (1e-12)

#define MATH_F32_INF ((PRP_F32)INFINITY)
#define MATH_F64_INF ((PRP_F64)INFINITY)

#define MATH_F32_NAN ((PRP_F32)NAN)
#define MATH_F64_NAN ((PRP_F64)NAN)

/* ----  FLOATING POINT FUNCTIONS  ---- */

static inline PRP_Bool MATH_IsZeroF32(PRP_F32 x) {
    return (PRP_Bool)(MATH_AbsF32(x) <= MATH_F32_SAFE_EPS);
}

static inline PRP_Bool MATH_IsZeroF64(PRP_F64 x) {
    return (PRP_Bool)(MATH_AbsF64(x) <= MATH_F64_SAFE_EPS);
}

static inline PRP_Bool MATH_IsNaNF32(PRP_F32 x) { return (PRP_Bool)isnan(x); }
static inline PRP_Bool MATH_IsNaNF64(PRP_F64 x) { return (PRP_Bool)isnan(x); }

static inline PRP_Bool MATH_IsInfF32(PRP_F32 x) { return (PRP_Bool)isinf(x); }
static inline PRP_Bool MATH_IsInfF64(PRP_F64 x) { return (PRP_Bool)isinf(x); }

static inline PRP_Bool MATH_AlmostEqAbsF32(PRP_F32 a, PRP_F32 b) {
    return (PRP_Bool)(MATH_AbsF32(a - b) <= MATH_F32_SAFE_EPS);
}

static inline PRP_Bool MATH_AlmostEqAbsF64(PRP_F64 a, PRP_F64 b) {
    return (PRP_Bool)(MATH_AbsF64(a - b) <= MATH_F64_SAFE_EPS);
}

static inline PRP_Bool MATH_AlmostEqRelF32(PRP_F32 a, PRP_F32 b) {
    PRP_F32 diff = MATH_AbsF32(a - b);
    PRP_F32 largest = MATH_MaxF32(MATH_AbsF32(a), MATH_AbsF32(b));

    return (PRP_Bool)(diff <= MATH_F32_SAFE_EPS * largest);
}

static inline PRP_Bool MATH_AlmostEqRelF64(PRP_F64 a, PRP_F64 b) {
    PRP_F64 diff = MATH_AbsF64(a - b);
    PRP_F64 largest = MATH_MaxF64(MATH_AbsF64(a), MATH_AbsF64(b));

    return (PRP_Bool)(diff <= MATH_F64_SAFE_EPS * largest);
}

static inline PRP_Bool MATH_AlmostEqF32(PRP_F32 a, PRP_F32 b) {
    return (PRP_Bool)(MATH_AlmostEqAbsF32(a, b) || MATH_AlmostEqRelF32(a, b));
}

static inline PRP_Bool MATH_AlmostEqF64(PRP_F64 a, PRP_F64 b) {
    return (PRP_Bool)(MATH_AlmostEqAbsF64(a, b) || MATH_AlmostEqRelF64(a, b));
}

static inline PRP_F32 MATH_SafeDivF32(PRP_F32 a, PRP_F32 b, PRP_F32 fallback) {
    if (MATH_IsZeroF32(b)) {
        return fallback;
    }

    return a / b;
}
static inline PRP_F64 MATH_SafeDivF64(PRP_F64 a, PRP_F64 b, PRP_F64 fallback) {
    if (MATH_IsZeroF64(b)) {
        return fallback;
    }

    return a / b;
}

#ifdef __cplusplus
}
#endif

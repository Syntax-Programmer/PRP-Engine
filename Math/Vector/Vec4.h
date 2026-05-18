#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Math/Float.h"
#include "Math/Transcendental.h"

typedef struct {
    DT_f32 x;
    DT_f32 y;
    DT_f32 z;
    DT_f32 w;
} MATH_Vec4;

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Vec4 MATH_Vec4Create(DT_f32 x, DT_f32 y, DT_f32 z,
                                        DT_f32 w) {
    return (MATH_Vec4){.x = x, .y = y, .z = z, .w = w};
}

static inline MATH_Vec4 MATH_Vec4CreateZero(DT_void) {
    return (MATH_Vec4){0, 0, 0, 0};
}

static inline MATH_Vec4 MATH_Vec4CreateOne(DT_void) {
    return (MATH_Vec4){1, 1, 1, 1};
}

static inline MATH_Vec4 MATH_Vec4CreateUnitX(DT_void) {
    return (MATH_Vec4){.x = 1, .y = 0, .z = 0, .w = 0};
}

static inline MATH_Vec4 MATH_Vec4CreateUnitY(DT_void) {
    return (MATH_Vec4){.x = 0, .y = 1, .z = 0, .w = 0};
}

static inline MATH_Vec4 MATH_Vec4CreateUnitZ(DT_void) {
    return (MATH_Vec4){.x = 0, .y = 0, .z = 1, .w = 0};
}

static inline MATH_Vec4 MATH_Vec4CreateUnitW(DT_void) {
    return (MATH_Vec4){.x = 0, .y = 0, .z = 0, .w = 1};
}

static inline MATH_Vec4 MATH_Vec4CreateScalar(DT_f32 s) {
    return (MATH_Vec4){s, s, s, s};
}

static inline MATH_Vec4 MATH_Vec4Homogenize(MATH_Vec4 a) {
    return (MATH_Vec4){
        .x = a.x / a.w, .y = a.y / a.w, .z = a.z / a.w, .w = (DT_f32)1.0};
}

static inline MATH_Vec4 MATH_Vec4HomogenizeSafe(MATH_Vec4 a, DT_f32 fallback) {
    a.x = MATH_SafeDivF32(a.x, a.w, fallback);
    a.y = MATH_SafeDivF32(a.y, a.w, fallback);
    a.z = MATH_SafeDivF32(a.z, a.w, fallback);
    a.w = 1.0;

    return a;
}

static inline MATH_Vec4 MATH_Vec4MinComps(MATH_Vec4 a, MATH_Vec4 b) {
    return (MATH_Vec4){.x = MATH_MinF32(a.x, b.x),
                       .y = MATH_MinF32(a.y, b.y),
                       .z = MATH_MinF32(a.z, b.z),
                       .w = MATH_MinF32(a.w, b.w)};
}

static inline MATH_Vec4 MATH_Vec4MaxComps(MATH_Vec4 a, MATH_Vec4 b) {
    return (MATH_Vec4){.x = MATH_MaxF32(a.x, b.x),
                       .y = MATH_MaxF32(a.y, b.y),
                       .z = MATH_MaxF32(a.z, b.z),
                       .w = MATH_MaxF32(a.w, b.w)};
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_f32 MATH_Vec4Min(MATH_Vec4 a) {
    return MATH_MinF32(MATH_MinF32(a.x, a.y), MATH_MinF32(a.z, a.w));
}

static inline DT_f32 MATH_Vec4Max(MATH_Vec4 a) {
    return MATH_MaxF32(MATH_MaxF32(a.x, a.y), MATH_MaxF32(a.z, a.w));
}

static inline DT_bool MATH_Vec4Eq(MATH_Vec4 a, MATH_Vec4 b) {
    return (DT_bool)(a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

static inline DT_bool MATH_Vec4AlmostEq(MATH_Vec4 a, MATH_Vec4 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.x, b.x) && MATH_AlmostEqF32(a.y, b.y) &&
                     MATH_AlmostEqF32(a.z, b.z) && MATH_AlmostEqF32(a.w, b.w));
}

static inline DT_bool MATH_Vec4IsZero(MATH_Vec4 a) {
    return (DT_bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y) &&
                     MATH_IsZeroF32(a.z) && MATH_IsZeroF32(a.w));
}

static inline DT_bool MATH_Vec4IsNaN(MATH_Vec4 a) {
    return (DT_bool)(MATH_IsNaNF32(a.x) || MATH_IsNaNF32(a.y) ||
                     MATH_IsNaNF32(a.z) || MATH_IsNaNF32(a.w));
}

static inline DT_bool MATH_Vec4IsInf(MATH_Vec4 a) {
    return (DT_bool)(MATH_IsInfF32(a.x) || MATH_IsInfF32(a.y) ||
                     MATH_IsInfF32(a.z) || MATH_IsInfF32(a.w));
}

/* ----  BASIC OPS  ---- */

static inline MATH_Vec4 MATH_Vec4Abs(MATH_Vec4 a) {
    return (MATH_Vec4){MATH_AbsF32(a.x), MATH_AbsF32(a.y), MATH_AbsF32(a.z),
                       MATH_AbsF32(a.w)};
}

static inline MATH_Vec4 MATH_Vec4Sign(MATH_Vec4 a) {
    return (MATH_Vec4){MATH_SignF32(a.x), MATH_SignF32(a.y), MATH_SignF32(a.z),
                       MATH_SignF32(a.w)};
}

static inline MATH_Vec4 MATH_Vec4Neg(MATH_Vec4 a) {
    return (MATH_Vec4){.x = -a.x, .y = -a.y, .z = -a.z, .w = -a.w};
}

static inline MATH_Vec4 MATH_Vec4AddComps(MATH_Vec4 a, MATH_Vec4 b) {
    return (MATH_Vec4){
        .x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z, .w = a.w + b.w};
}

static inline MATH_Vec4 MATH_Vec4SubComps(MATH_Vec4 a, MATH_Vec4 b) {
    return (MATH_Vec4){
        .x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z, .w = a.w - b.w};
}

static inline MATH_Vec4 MATH_Vec4MulComps(MATH_Vec4 a, MATH_Vec4 b) {
    return (MATH_Vec4){
        .x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z, .w = a.w * b.w};
}

static inline MATH_Vec4 MATH_Vec4DivComps(MATH_Vec4 a, MATH_Vec4 b) {
    return (MATH_Vec4){
        .x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z, .w = a.w / b.w};
}

static inline MATH_Vec4 MATH_Vec4DivCompsSafe(MATH_Vec4 a, MATH_Vec4 b,
                                              DT_f32 fallback) {
    return (MATH_Vec4){
        .x = MATH_SafeDivF32(a.x, b.x, fallback),
        .y = MATH_SafeDivF32(a.y, b.y, fallback),
        .z = MATH_SafeDivF32(a.z, b.z, fallback),
        .w = MATH_SafeDivF32(a.w, b.w, fallback),
    };
}

static inline MATH_Vec4 MATH_Vec4AddScalar(MATH_Vec4 a, DT_f32 s) {
    return (MATH_Vec4){.x = a.x + s, .y = a.y + s, .z = a.z + s, .w = a.w + s};
}

static inline MATH_Vec4 MATH_Vec4SubScalar(MATH_Vec4 a, DT_f32 s) {
    return (MATH_Vec4){.x = a.x - s, .y = a.y - s, .z = a.z - s, .w = a.w - s};
}

static inline MATH_Vec4 MATH_Vec4MulScalar(MATH_Vec4 a, DT_f32 s) {
    return (MATH_Vec4){.x = a.x * s, .y = a.y * s, .z = a.z * s, .w = a.w * s};
}

static inline MATH_Vec4 MATH_Vec4DivScalar(MATH_Vec4 a, DT_f32 s) {
    return (MATH_Vec4){.x = a.x / s, .y = a.y / s, .z = a.z / s, .w = a.w / s};
}

static inline MATH_Vec4 MATH_Vec4DivScalarSafe(MATH_Vec4 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return (MATH_Vec4){fallback, fallback, fallback, fallback};
    }

    return MATH_Vec4DivScalar(a, s);
}

static inline DT_f32 MATH_Vec4Sum(MATH_Vec4 a) { return a.x + a.y + a.z + a.w; }

static inline DT_f32 MATH_Vec4Avg(MATH_Vec4 a) {
    return (a.x + a.y + a.z + a.w) * (DT_f32)(0.25);
}

static inline MATH_Vec4 MATH_Vec4SclarMulAdd(MATH_Vec4 a, MATH_Vec4 b,
                                             DT_f32 s) {
    return MATH_Vec4AddComps(a, MATH_Vec4MulScalar(b, s));
}

static inline MATH_Vec4 MATH_Vec4SclarAddMul(MATH_Vec4 a, MATH_Vec4 b,
                                             DT_f32 s) {
    return MATH_Vec4AddComps(MATH_Vec4MulScalar(a, s), b);
}

/* ----  BASIC ALGEBRA  ---- */

static inline DT_f32 MATH_Vec4Dot(MATH_Vec4 a, MATH_Vec4 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

static inline DT_f32 MATH_Vec4LenSq(MATH_Vec4 a) { return MATH_Vec4Dot(a, a); }

static inline DT_f32 MATH_Vec4Len(MATH_Vec4 a) {
    return MATH_SqrtF32(MATH_Vec4Dot(a, a));
}

static inline MATH_Vec4 MATH_Vec4ClampLen(MATH_Vec4 a, DT_f32 min, DT_f32 max) {
    DT_f32 len = MATH_Vec4Len(a);
    DT_f32 clamp = MATH_ClampF32(len, min, max);
    DT_f32 new_len_ratio = clamp / len;

    return MATH_Vec4MulScalar(a, new_len_ratio);
}

static inline MATH_Vec4 MATH_Vec4ClampLenSafe(MATH_Vec4 a, DT_f32 min,
                                              DT_f32 max, MATH_Vec4 fallback) {
    DT_f32 len = MATH_Vec4Len(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }
    if (min > max) {
        DT_f32 tmp = min;
        min = max;
        max = tmp;
    }

    DT_f32 clamp = MATH_ClampF32(len, min, max);

    return MATH_Vec4MulScalar(a, (clamp / len));
}

static inline MATH_Vec4 MATH_Vec4Clamp(MATH_Vec4 a, MATH_Vec4 min,
                                       MATH_Vec4 max) {
    return (MATH_Vec4){.x = MATH_ClampF32(a.x, min.x, max.x),
                       .y = MATH_ClampF32(a.y, min.y, max.y),
                       .z = MATH_ClampF32(a.z, min.z, max.z),
                       .w = MATH_ClampF32(a.w, min.w, max.w)};
}

static inline MATH_Vec4 MATH_Vec4Reciprocal(MATH_Vec4 a) {
    return (MATH_Vec4){
        .x = 1.0f / a.x, .y = 1.0f / a.y, .z = 1.0f / a.z, .w = 1.0f / a.w};
}

static inline MATH_Vec4 MATH_Vec4ReciprocalSafe(MATH_Vec4 a, DT_f32 fallback) {
    return (MATH_Vec4){.x = MATH_SafeDivF32(1.0f, a.x, fallback),
                       .y = MATH_SafeDivF32(1.0f, a.y, fallback),
                       .z = MATH_SafeDivF32(1.0f, a.z, fallback),
                       .w = MATH_SafeDivF32(1.0f, a.w, fallback)};
}

static inline MATH_Vec4 MATH_Vec4Normalize(MATH_Vec4 a) {
    DT_f32 inv_len = 1.0f / MATH_Vec4Len(a);

    return MATH_Vec4MulScalar(a, inv_len);
}

static inline MATH_Vec4 MATH_Vec4NormalizeSafe(MATH_Vec4 a,
                                               MATH_Vec4 fallback) {
    DT_f32 len = MATH_Vec4Len(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }

    return MATH_Vec4MulScalar(a, 1.0f / len);
}

static inline DT_bool MATH_Vec4IsNormalized(MATH_Vec4 a) {
    return (DT_bool)(MATH_AlmostEqF32(MATH_Vec4LenSq(a), 1.0f));
}

static inline DT_f32 MATH_Vec4Distance(MATH_Vec4 a, MATH_Vec4 b) {
    return MATH_Vec4Len(MATH_Vec4SubComps(a, b));
}

static inline DT_f32 MATH_Vec4DistanceSq(MATH_Vec4 a, MATH_Vec4 b) {
    return MATH_Vec4LenSq(MATH_Vec4SubComps(a, b));
}

/* ----  SPECIAL OPS  ---- */

static inline MATH_Vec4 MATH_Vec4Lerp(MATH_Vec4 a, MATH_Vec4 b, DT_f32 t) {
    MATH_Vec4 sub = MATH_Vec4SubComps(b, a);
    MATH_Vec4 sub_with_mul = MATH_Vec4MulScalar(sub, t);

    return MATH_Vec4AddComps(a, sub_with_mul);
}

static inline MATH_Vec4 MATH_Vec4NLerp(MATH_Vec4 a, MATH_Vec4 b, DT_f32 t) {
    MATH_Vec4 sub = MATH_Vec4SubComps(b, a);
    MATH_Vec4 sub_with_mul = MATH_Vec4MulScalar(sub, t);
    MATH_Vec4 lerp = MATH_Vec4AddComps(a, sub_with_mul);

    return MATH_Vec4Normalize(lerp);
}

static inline MATH_Vec4 MATH_Vec4Project(MATH_Vec4 a, MATH_Vec4 b) {
    DT_f32 denom = MATH_Vec4Dot(b, b);

    return MATH_Vec4MulScalar(b, MATH_Vec4Dot(a, b) / denom);
}

static inline MATH_Vec4 MATH_Vec4ProjectSafe(MATH_Vec4 a, MATH_Vec4 b,
                                             MATH_Vec4 fallback) {
    DT_f32 denom = MATH_Vec4Dot(b, b);
    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }

    DT_f32 s = MATH_Vec4Dot(a, b) / denom;
    return MATH_Vec4MulScalar(b, s);
}

static inline MATH_Vec4 MATH_Vec4Reject(MATH_Vec4 a, MATH_Vec4 b) {
    return MATH_Vec4SubComps(a, MATH_Vec4Project(a, b));
}

static inline MATH_Vec4 MATH_Vec4RejectSafe(MATH_Vec4 a, MATH_Vec4 b,
                                            MATH_Vec4 fallback) {
    DT_f32 denom = MATH_Vec4Dot(b, b);

    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }

    return MATH_Vec4SubComps(a,
                             MATH_Vec4MulScalar(b, MATH_Vec4Dot(a, b) / denom));
}

static inline MATH_Vec4 MATH_Vec4Reflect(MATH_Vec4 v, MATH_Vec4 n) {
    // n is supposed to be normalized.
    DT_f32 d = 2.0f * MATH_Vec4Dot(v, n);

    return MATH_Vec4SubComps(v, MATH_Vec4MulScalar(n, d));
}

#ifdef __cplusplus
}
#endif

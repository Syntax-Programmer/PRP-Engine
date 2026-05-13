#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Float.h"
#include "../Transcendental.h"

typedef struct {
    DT_f32 x;
    DT_f32 y;
} MATH_Vec2;

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Vec2 MATH_Vec2Create(DT_f32 x, DT_f32 y) {
    return (MATH_Vec2){.x = x, .y = y};
}

static inline MATH_Vec2 MATH_Vec2CreateZero(DT_void) {
    return (MATH_Vec2){0, 0};
}

static inline MATH_Vec2 MATH_Vec2CreateOne(DT_void) {
    return (MATH_Vec2){1, 1};
}

static inline MATH_Vec2 MATH_Vec2CreateUnitX(DT_void) {
    return (MATH_Vec2){.x = 1, .y = 0};
}

static inline MATH_Vec2 MATH_Vec2CreateUnitY(DT_void) {
    return (MATH_Vec2){.x = 0, .y = 1};
}

static inline MATH_Vec2 MATH_Vec2CreateScalar(DT_f32 s) {
    return (MATH_Vec2){s, s};
}

static inline MATH_Vec2 MATH_Vec2MinComps(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = MATH_MinF32(a.x, b.x), .y = MATH_MinF32(a.y, b.y)};
}

static inline MATH_Vec2 MATH_Vec2MaxComps(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = MATH_MaxF32(a.x, b.x), .y = MATH_MaxF32(a.y, b.y)};
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_f32 MATH_Vec2Min(MATH_Vec2 a) { return MATH_MinF32(a.x, a.y); }

static inline DT_f32 MATH_Vec2Max(MATH_Vec2 a) { return MATH_MaxF32(a.x, a.y); }

static inline DT_bool MATH_Vec2Eq(MATH_Vec2 a, MATH_Vec2 b) {
    return (DT_bool)(a.x == b.x && a.y == b.y);
}

static inline DT_bool MATH_Vec2AlmostEq(MATH_Vec2 a, MATH_Vec2 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.x, b.x) && MATH_AlmostEqF32(a.y, b.y));
}

static inline DT_bool MATH_Vec2IsZero(MATH_Vec2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y));
}

static inline DT_bool MATH_Vec2IsNaN(MATH_Vec2 a) {
    return (DT_bool)(MATH_IsNaNF32(a.x) || MATH_IsNaNF32(a.y));
}

static inline DT_bool MATH_Vec2IsInf(MATH_Vec2 a) {
    return (DT_bool)(MATH_IsInfF32(a.x) || MATH_IsInfF32(a.y));
}

/* ----  BASIC OPS  ---- */

static inline MATH_Vec2 MATH_Vec2Abs(MATH_Vec2 a) {
    return (MATH_Vec2){MATH_AbsF32(a.x), MATH_AbsF32(a.y)};
}

static inline MATH_Vec2 MATH_Vec2Sign(MATH_Vec2 a) {
    return (MATH_Vec2){MATH_SignF32(a.x), MATH_SignF32(a.y)};
}

static inline MATH_Vec2 MATH_Vec2Neg(MATH_Vec2 a) {
    return (MATH_Vec2){.x = -a.x, .y = -a.y};
}

static inline MATH_Vec2 MATH_Vec2AddComps(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x + b.x, .y = a.y + b.y};
}

static inline MATH_Vec2 MATH_Vec2SubComps(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x - b.x, .y = a.y - b.y};
}

static inline MATH_Vec2 MATH_Vec2MulComps(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x * b.x, .y = a.y * b.y};
}

static inline MATH_Vec2 MATH_Vec2DivComps(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x / b.x, .y = a.y / b.y};
}

static inline MATH_Vec2 MATH_Vec2DivCompsSafe(MATH_Vec2 a, MATH_Vec2 b,
                                              DT_f32 fallback) {
    return (MATH_Vec2){
        .x = MATH_SafeDivF32(a.x, b.x, fallback),
        .y = MATH_SafeDivF32(a.y, b.y, fallback),
    };
}

static inline MATH_Vec2 MATH_Vec2AddScalar(MATH_Vec2 a, DT_f32 s) {
    return (MATH_Vec2){.x = a.x + s, .y = a.y + s};
}

static inline MATH_Vec2 MATH_Vec2SubScalar(MATH_Vec2 a, DT_f32 s) {
    return (MATH_Vec2){.x = a.x - s, .y = a.y - s};
}

static inline MATH_Vec2 MATH_Vec2MulScalar(MATH_Vec2 a, DT_f32 s) {
    return (MATH_Vec2){.x = a.x * s, .y = a.y * s};
}

static inline MATH_Vec2 MATH_Vec2DivScalar(MATH_Vec2 a, DT_f32 s) {
    return (MATH_Vec2){.x = a.x / s, .y = a.y / s};
}

static inline MATH_Vec2 MATH_Vec2DivScalarSafe(MATH_Vec2 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return (MATH_Vec2){fallback, fallback};
    }

    return MATH_Vec2DivScalar(a, s);
}

static inline DT_f32 MATH_Vec2Sum(MATH_Vec2 a) { return a.x + a.y; }

static inline DT_f32 MATH_Vec2Avg(MATH_Vec2 a) {
    return (a.x + a.y) * (DT_f32)(0.5f);
}

static inline MATH_Vec2 MATH_Vec2SclarMulAdd(MATH_Vec2 a, MATH_Vec2 b,
                                             DT_f32 s) {
    return MATH_Vec2AddComps(a, MATH_Vec2MulScalar(b, s));
}

static inline MATH_Vec2 MATH_Vec2SclarAddMul(MATH_Vec2 a, MATH_Vec2 b,
                                             DT_f32 s) {
    return MATH_Vec2AddComps(MATH_Vec2MulScalar(a, s), b);
}

/* ----  BASIC ALGEBRA  ---- */

static inline DT_f32 MATH_Vec2Dot(MATH_Vec2 a, MATH_Vec2 b) {
    return (a.x * b.x) + (a.y * b.y);
}

static inline DT_f32 MATH_Vec2Cross(MATH_Vec2 a, MATH_Vec2 b) {
    return a.x * b.y - a.y * b.x;
}

static inline DT_f32 MATH_Vec2LenSq(MATH_Vec2 a) { return MATH_Vec2Dot(a, a); }

static inline DT_f32 MATH_Vec2Len(MATH_Vec2 a) {
    return MATH_SqrtF32(MATH_Vec2Dot(a, a));
}

static inline DT_f32 MATH_Vec2Angle(MATH_Vec2 a, MATH_Vec2 b) {
    DT_f32 denom = MATH_Vec2Len(a) * MATH_Vec2Len(b);
    DT_f32 c = MATH_Vec2Dot(a, b) / denom;
    c = MATH_ClampF32(c, -1.0f, 1.0f);

    return MATH_ACosF32(c);
}

static inline DT_f32 MATH_Vec2AngleSafe(MATH_Vec2 a, MATH_Vec2 b,
                                        DT_f32 fallback) {
    DT_f32 denom = MATH_Vec2Len(a) * MATH_Vec2Len(b);
    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }
    DT_f32 c = MATH_Vec2Dot(a, b) / denom;
    c = MATH_ClampF32(c, -1.0f, 1.0f);

    return MATH_ACosF32(c);
}

static inline MATH_Vec2 MATH_Vec2ClampLen(MATH_Vec2 a, DT_f32 min, DT_f32 max) {
    DT_f32 len = MATH_Vec2Len(a);
    DT_f32 clamp = MATH_ClampF32(len, min, max);
    DT_f32 new_len_ratio = clamp / len;

    return MATH_Vec2MulScalar(a, new_len_ratio);
}

static inline MATH_Vec2 MATH_Vec2ClampLenSafe(MATH_Vec2 a, DT_f32 min,
                                              DT_f32 max, MATH_Vec2 fallback) {
    DT_f32 len = MATH_Vec2Len(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }
    if (min > max) {
        DT_f32 tmp = min;
        min = max;
        max = tmp;
    }

    DT_f32 clamp = MATH_ClampF32(len, min, max);

    return MATH_Vec2MulScalar(a, (clamp / len));
}

static inline MATH_Vec2 MATH_Vec2Clamp(MATH_Vec2 a, MATH_Vec2 min,
                                       MATH_Vec2 max) {
    return (MATH_Vec2){.x = MATH_ClampF32(a.x, min.x, max.x),
                       .y = MATH_ClampF32(a.y, min.y, max.y)};
}

static inline MATH_Vec2 MATH_Vec2Reciprocal(MATH_Vec2 a) {
    return (MATH_Vec2){.x = 1.0f / a.x, .y = 1.0f / a.y};
}

static inline MATH_Vec2 MATH_Vec2ReciprocalSafe(MATH_Vec2 a, DT_f32 fallback) {
    return (MATH_Vec2){.x = MATH_SafeDivF32(1.0f, a.x, fallback),
                       .y = MATH_SafeDivF32(1.0f, a.y, fallback)};
}

static inline MATH_Vec2 MATH_Vec2Normalize(MATH_Vec2 a) {
    DT_f32 inv_len = 1.0f / MATH_Vec2Len(a);

    return MATH_Vec2MulScalar(a, inv_len);
}

static inline MATH_Vec2 MATH_Vec2NormalizeSafe(MATH_Vec2 a,
                                               MATH_Vec2 fallback) {
    DT_f32 len = MATH_Vec2Len(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }

    return MATH_Vec2MulScalar(a, 1.0f / len);
}

static inline DT_f32 MATH_Vec2Distance(MATH_Vec2 a, MATH_Vec2 b) {
    return MATH_Vec2Len(MATH_Vec2SubComps(a, b));
}

static inline DT_f32 MATH_Vec2DistanceSq(MATH_Vec2 a, MATH_Vec2 b) {
    return MATH_Vec2LenSq(MATH_Vec2SubComps(a, b));
}

static inline MATH_Vec2 MATH_Vec2Perp(MATH_Vec2 v) {
    return (MATH_Vec2){-v.y, v.x};
}

static inline MATH_Vec2 MATH_Vec2PerpCW(MATH_Vec2 v) {
    return (MATH_Vec2){v.y, -v.x};
}

static inline DT_f32 MATH_Vec2AngleOf(MATH_Vec2 v) {
    return MATH_ATan2F32(v.y, v.x);
}

/* ----  SPECIAL OPS  ---- */

static inline MATH_Vec2 MATH_Vec2Rotate(MATH_Vec2 v, DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Vec2){v.x * c - v.y * s, v.x * s + v.y * c};
}

static inline MATH_Vec2 MATH_Vec2Lerp(MATH_Vec2 a, MATH_Vec2 b, DT_f32 t) {
    MATH_Vec2 sub = MATH_Vec2SubComps(b, a);
    MATH_Vec2 sub_with_mul = MATH_Vec2MulScalar(sub, t);

    return MATH_Vec2AddComps(a, sub_with_mul);
}

static inline MATH_Vec2 MATH_Vec2NLerp(MATH_Vec2 a, MATH_Vec2 b, DT_f32 t) {
    MATH_Vec2 sub = MATH_Vec2SubComps(b, a);
    MATH_Vec2 sub_with_mul = MATH_Vec2MulScalar(sub, t);
    MATH_Vec2 lerp = MATH_Vec2AddComps(a, sub_with_mul);

    return MATH_Vec2Normalize(lerp);
}

static inline MATH_Vec2 MATH_Vec2Project(MATH_Vec2 a, MATH_Vec2 b) {
    DT_f32 denom = MATH_Vec2Dot(b, b);

    return MATH_Vec2MulScalar(b, MATH_Vec2Dot(a, b) / denom);
}

static inline MATH_Vec2 MATH_Vec2ProjectSafe(MATH_Vec2 a, MATH_Vec2 b,
                                             MATH_Vec2 fallback) {
    DT_f32 denom = MATH_Vec2Dot(b, b);
    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }
    DT_f32 s = MATH_Vec2Dot(a, b) / denom;

    return MATH_Vec2MulScalar(b, s);
}

static inline MATH_Vec2 MATH_Vec2Reject(MATH_Vec2 a, MATH_Vec2 b) {
    return MATH_Vec2SubComps(a, MATH_Vec2Project(a, b));
}

static inline MATH_Vec2 MATH_Vec2RejectSafe(MATH_Vec2 a, MATH_Vec2 b,
                                            MATH_Vec2 fallback) {
    DT_f32 denom = MATH_Vec2Dot(b, b);

    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }

    return MATH_Vec2SubComps(a,
                             MATH_Vec2MulScalar(b, MATH_Vec2Dot(a, b) / denom));
}

static inline MATH_Vec2 MATH_Vec2Reflect(MATH_Vec2 v, MATH_Vec2 n) {
    // n is supposed to be normalized.
    DT_f32 d = 2.0f * MATH_Vec2Dot(v, n);

    return MATH_Vec2SubComps(v, MATH_Vec2MulScalar(n, d));
}

#ifdef __cplusplus
}
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Math/Float.h"
#include "Math/Transcendental.h"

typedef struct {
    PRP_F32 x;
    PRP_F32 y;
    PRP_F32 z;
} MATH_Vec3;

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Vec3 MATH_Vec3Create(PRP_F32 x, PRP_F32 y, PRP_F32 z) {
    return (MATH_Vec3){.x = x, .y = y, .z = z};
}

static inline MATH_Vec3 MATH_Vec3CreateZero(void) {
    return (MATH_Vec3){0, 0, 0};
}

static inline MATH_Vec3 MATH_Vec3CreateOne(void) {
    return (MATH_Vec3){1, 1, 1};
}

static inline MATH_Vec3 MATH_Vec3CreateUnitX(void) {
    return (MATH_Vec3){.x = 1, .y = 0, .z = 0};
}

static inline MATH_Vec3 MATH_Vec3CreateUnitY(void) {
    return (MATH_Vec3){.x = 0, .y = 1, .z = 0};
}

static inline MATH_Vec3 MATH_Vec3CreateUnitZ(void) {
    return (MATH_Vec3){.x = 0, .y = 0, .z = 1};
}

static inline MATH_Vec3 MATH_Vec3CreateScalar(PRP_F32 s) {
    return (MATH_Vec3){s, s, s};
}

static inline MATH_Vec3 MATH_Vec3MinComps(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = MATH_MinF32(a.x, b.x),
                       .y = MATH_MinF32(a.y, b.y),
                       .z = MATH_MinF32(a.z, b.z)};
}

static inline MATH_Vec3 MATH_Vec3MaxComps(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = MATH_MaxF32(a.x, b.x),
                       .y = MATH_MaxF32(a.y, b.y),
                       .z = MATH_MaxF32(a.z, b.z)};
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline PRP_F32 MATH_Vec3Min(MATH_Vec3 a) {
    return MATH_MinF32(MATH_MinF32(a.x, a.y), a.z);
}

static inline PRP_F32 MATH_Vec3Max(MATH_Vec3 a) {
    return MATH_MaxF32(MATH_MaxF32(a.x, a.y), a.z);
}

static inline PRP_Bool MATH_Vec3Eq(MATH_Vec3 a, MATH_Vec3 b) {
    return (PRP_Bool)(a.x == b.x && a.y == b.y && a.z == b.z);
}

static inline PRP_Bool MATH_Vec3AlmostEq(MATH_Vec3 a, MATH_Vec3 b) {
    return (PRP_Bool)(MATH_AlmostEqF32(a.x, b.x) &&
                      MATH_AlmostEqF32(a.y, b.y) && MATH_AlmostEqF32(a.z, b.z));
}

static inline PRP_Bool MATH_Vec3IsZero(MATH_Vec3 a) {
    return (PRP_Bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y) &&
                      MATH_IsZeroF32(a.z));
}

static inline PRP_Bool MATH_Vec3IsNaN(MATH_Vec3 a) {
    return (PRP_Bool)(MATH_IsNaNF32(a.x) || MATH_IsNaNF32(a.y) ||
                      MATH_IsNaNF32(a.z));
}

static inline PRP_Bool MATH_Vec3IsInf(MATH_Vec3 a) {
    return (PRP_Bool)(MATH_IsInfF32(a.x) || MATH_IsInfF32(a.y) ||
                      MATH_IsInfF32(a.z));
}

/* ----  BASIC OPS  ---- */

static inline MATH_Vec3 MATH_Vec3Abs(MATH_Vec3 a) {
    return (MATH_Vec3){MATH_AbsF32(a.x), MATH_AbsF32(a.y), MATH_AbsF32(a.z)};
}

static inline MATH_Vec3 MATH_Vec3Sign(MATH_Vec3 a) {
    return (MATH_Vec3){MATH_SignF32(a.x), MATH_SignF32(a.y), MATH_SignF32(a.z)};
}

static inline MATH_Vec3 MATH_Vec3Neg(MATH_Vec3 a) {
    return (MATH_Vec3){.x = -a.x, .y = -a.y, .z = -a.z};
}

static inline MATH_Vec3 MATH_Vec3AddComps(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

static inline MATH_Vec3 MATH_Vec3SubComps(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

static inline MATH_Vec3 MATH_Vec3MulComps(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

static inline MATH_Vec3 MATH_Vec3DivComps(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = a.x / b.x, .y = a.y / b.y, .z = a.z / b.z};
}

static inline MATH_Vec3 MATH_Vec3DivCompsSafe(MATH_Vec3 a, MATH_Vec3 b,
                                              PRP_F32 fallback) {
    return (MATH_Vec3){
        .x = MATH_SafeDivF32(a.x, b.x, fallback),
        .y = MATH_SafeDivF32(a.y, b.y, fallback),
        .z = MATH_SafeDivF32(a.z, b.z, fallback),
    };
}

static inline MATH_Vec3 MATH_Vec3AddScalar(MATH_Vec3 a, PRP_F32 s) {
    return (MATH_Vec3){.x = a.x + s, .y = a.y + s, .z = a.z + s};
}

static inline MATH_Vec3 MATH_Vec3SubScalar(MATH_Vec3 a, PRP_F32 s) {
    return (MATH_Vec3){.x = a.x - s, .y = a.y - s, .z = a.z - s};
}

static inline MATH_Vec3 MATH_Vec3MulScalar(MATH_Vec3 a, PRP_F32 s) {
    return (MATH_Vec3){.x = a.x * s, .y = a.y * s, .z = a.z * s};
}

static inline MATH_Vec3 MATH_Vec3DivScalar(MATH_Vec3 a, PRP_F32 s) {
    return (MATH_Vec3){.x = a.x / s, .y = a.y / s, .z = a.z / s};
}

static inline MATH_Vec3 MATH_Vec3DivScalarSafe(MATH_Vec3 a, PRP_F32 s,
                                               PRP_F32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return (MATH_Vec3){fallback, fallback, fallback};
    }

    return MATH_Vec3DivScalar(a, s);
}

static inline PRP_F32 MATH_Vec3Sum(MATH_Vec3 a) { return a.x + a.y + a.z; }

static inline PRP_F32 MATH_Vec3Avg(MATH_Vec3 a) {
    return (a.x + a.y + a.z) * (PRP_F32)(1.0f / 3.0f);
}

static inline MATH_Vec3 MATH_Vec3SclarMulAdd(MATH_Vec3 a, MATH_Vec3 b,
                                             PRP_F32 s) {
    return MATH_Vec3AddComps(a, MATH_Vec3MulScalar(b, s));
}

static inline MATH_Vec3 MATH_Vec3SclarAddMul(MATH_Vec3 a, MATH_Vec3 b,
                                             PRP_F32 s) {
    return MATH_Vec3AddComps(MATH_Vec3MulScalar(a, s), b);
}

/* ----  BASIC ALGEBRA  ---- */

static inline PRP_F32 MATH_Vec3Dot(MATH_Vec3 a, MATH_Vec3 b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline MATH_Vec3 MATH_Vec3Cross(MATH_Vec3 a, MATH_Vec3 b) {
    return (MATH_Vec3){.x = a.y * b.z - a.z * b.y,
                       .y = a.z * b.x - a.x * b.z,
                       .z = a.x * b.y - a.y * b.x};
}

static inline PRP_F32 MATH_Vec3LenSq(MATH_Vec3 a) { return MATH_Vec3Dot(a, a); }

static inline PRP_F32 MATH_Vec3Len(MATH_Vec3 a) {
    return MATH_SqrtF32(MATH_Vec3Dot(a, a));
}

static inline PRP_F32 MATH_Vec3Angle(MATH_Vec3 a, MATH_Vec3 b) {
    PRP_F32 denom = MATH_Vec3Len(a) * MATH_Vec3Len(b);
    PRP_F32 c = MATH_Vec3Dot(a, b) / denom;
    c = MATH_ClampF32(c, -1.0f, 1.0f);

    return MATH_ACosF32(c);
}

static inline PRP_F32 MATH_Vec3AngleSafe(MATH_Vec3 a, MATH_Vec3 b,
                                         PRP_F32 fallback) {
    PRP_F32 denom = MATH_Vec3Len(a) * MATH_Vec3Len(b);
    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }

    return MATH_ACosF32(MATH_Vec3Dot(a, b) / denom);
}

static inline PRP_F32 MATH_Vec3AngleSigned(MATH_Vec3 a, MATH_Vec3 b,
                                           MATH_Vec3 axis) {
    MATH_Vec3 cross = MATH_Vec3Cross(a, b);
    PRP_F32 sin_term = MATH_Vec3Dot(cross, axis);
    PRP_F32 cos_term = MATH_Vec3Dot(a, b);

    return MATH_ATan2F32(sin_term, cos_term);
}

static inline PRP_F32 MATH_Vec3AngleSignedSafe(MATH_Vec3 a, MATH_Vec3 b,
                                               MATH_Vec3 axis,
                                               PRP_F32 fallback) {
    PRP_F32 len_a = MATH_Vec3Len(a);
    PRP_F32 len_b = MATH_Vec3Len(b);
    if (MATH_IsZeroF32(len_a) || MATH_IsZeroF32(len_b)) {
        return fallback;
    }
    MATH_Vec3 cross = MATH_Vec3Cross(a, b);
    PRP_F32 sin_term = MATH_Vec3Dot(cross, axis);
    PRP_F32 cos_term = MATH_Vec3Dot(a, b);

    return MATH_ATan2F32(sin_term, cos_term);
}

static inline PRP_F32 MATH_Vec3TripleScalar(MATH_Vec3 a, MATH_Vec3 b,
                                            MATH_Vec3 c) {
    return MATH_Vec3Dot(a, MATH_Vec3Cross(b, c));
}

static inline MATH_Vec3 MATH_Vec3ClampLen(MATH_Vec3 a, PRP_F32 min,
                                          PRP_F32 max) {
    PRP_F32 len = MATH_Vec3Len(a);
    PRP_F32 clamp = MATH_ClampF32(len, min, max);
    PRP_F32 new_len_ratio = clamp / len;

    return MATH_Vec3MulScalar(a, new_len_ratio);
}

static inline MATH_Vec3 MATH_Vec3ClampLenSafe(MATH_Vec3 a, PRP_F32 min,
                                              PRP_F32 max, MATH_Vec3 fallback) {
    PRP_F32 len = MATH_Vec3Len(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }
    if (min > max) {
        PRP_F32 tmp = min;
        min = max;
        max = tmp;
    }

    PRP_F32 clamp = MATH_ClampF32(len, min, max);

    return MATH_Vec3MulScalar(a, (clamp / len));
}

static inline MATH_Vec3 MATH_Vec3Clamp(MATH_Vec3 a, MATH_Vec3 min,
                                       MATH_Vec3 max) {
    return (MATH_Vec3){.x = MATH_ClampF32(a.x, min.x, max.x),
                       .y = MATH_ClampF32(a.y, min.y, max.y),
                       .z = MATH_ClampF32(a.z, min.z, max.z)};
}

static inline MATH_Vec3 MATH_Vec3Reciprocal(MATH_Vec3 a) {
    return (MATH_Vec3){.x = 1.0f / a.x, .y = 1.0f / a.y, .z = 1.0f / a.z};
}

static inline MATH_Vec3 MATH_Vec3ReciprocalSafe(MATH_Vec3 a, PRP_F32 fallback) {
    return (MATH_Vec3){.x = MATH_SafeDivF32(1.0f, a.x, fallback),
                       .y = MATH_SafeDivF32(1.0f, a.y, fallback),
                       .z = MATH_SafeDivF32(1.0f, a.z, fallback)};
}

static inline MATH_Vec3 MATH_Vec3Normalize(MATH_Vec3 a) {
    PRP_F32 inv_len = 1.0f / MATH_Vec3Len(a);

    return MATH_Vec3MulScalar(a, inv_len);
}

static inline MATH_Vec3 MATH_Vec3NormalizeSafe(MATH_Vec3 a,
                                               MATH_Vec3 fallback) {
    PRP_F32 len = MATH_Vec3Len(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }

    return MATH_Vec3MulScalar(a, 1.0f / len);
}

static inline PRP_Bool MATH_Vec3IsNormalized(MATH_Vec3 a) {
    return (PRP_Bool)(MATH_AlmostEqF32(MATH_Vec3LenSq(a), 1.0f));
}

static inline PRP_F32 MATH_Vec3Distance(MATH_Vec3 a, MATH_Vec3 b) {
    return MATH_Vec3Len(MATH_Vec3SubComps(a, b));
}

static inline PRP_F32 MATH_Vec3DistanceSq(MATH_Vec3 a, MATH_Vec3 b) {
    return MATH_Vec3LenSq(MATH_Vec3SubComps(a, b));
}

static inline PRP_F32 MATH_Vec3DistanceToLine(MATH_Vec3 p, MATH_Vec3 p0,
                                              MATH_Vec3 d) {
    MATH_Vec3 diff = MATH_Vec3SubComps(p, p0);
    MATH_Vec3 cross = MATH_Vec3Cross(diff, d);

    return MATH_Vec3Len(cross) / MATH_Vec3Len(d);
}

static inline PRP_F32 MATH_Vec3DistanceToLineSafe(MATH_Vec3 p, MATH_Vec3 p0,
                                                  MATH_Vec3 d,
                                                  PRP_F32 fallback) {
    PRP_F32 len_d = MATH_Vec3Len(d);
    if (MATH_IsZeroF32(len_d)) {
        return fallback;
    }

    MATH_Vec3 diff = MATH_Vec3SubComps(p, p0);
    MATH_Vec3 cross = MATH_Vec3Cross(diff, d);

    return MATH_Vec3Len(cross) / len_d;
}

static inline PRP_F32 MATH_Vec3DistanceToPlane(MATH_Vec3 p, MATH_Vec3 p0,
                                               MATH_Vec3 n) {
    MATH_Vec3 diff = MATH_Vec3SubComps(p, p0);

    return MATH_Vec3Dot(diff, n) / MATH_Vec3Len(n);
}

static inline PRP_F32 MATH_Vec3DistanceToPlaneSafe(MATH_Vec3 p, MATH_Vec3 p0,
                                                   MATH_Vec3 n,
                                                   PRP_F32 fallback) {
    PRP_F32 len_n = MATH_Vec3Len(n);
    if (MATH_IsZeroF32(len_n)) {
        return fallback;
    }

    MATH_Vec3 diff = MATH_Vec3SubComps(p, p0);
    return MATH_Vec3Dot(diff, n) / len_n;
}

static inline MATH_Vec3 MATH_Vec3Perpendicular(MATH_Vec3 v) {
    if (MATH_AbsF32(v.x) < MATH_AbsF32(v.z)) {
        return MATH_Vec3Cross(v, (MATH_Vec3){.x = 1, .y = 0, .z = 0});
    } else {
        return MATH_Vec3Cross(v, (MATH_Vec3){.x = 0, .y = 0, .z = 1});
    }
}

static inline void MATH_Vec3OrthonormalBasis(MATH_Vec3 n, MATH_Vec3 *t,
                                             MATH_Vec3 *b) {
    *t = MATH_Vec3Normalize(MATH_Vec3Perpendicular(n));
    *b = MATH_Vec3Cross(n, *t);
}

/* ----  SPECIAL OPS  ---- */

static inline MATH_Vec3 MATH_Vec3Lerp(MATH_Vec3 a, MATH_Vec3 b, PRP_F32 t) {
    MATH_Vec3 sub = MATH_Vec3SubComps(b, a);
    MATH_Vec3 sub_with_mul = MATH_Vec3MulScalar(sub, t);

    return MATH_Vec3AddComps(a, sub_with_mul);
}

static inline MATH_Vec3 MATH_Vec3NLerp(MATH_Vec3 a, MATH_Vec3 b, PRP_F32 t) {
    MATH_Vec3 sub = MATH_Vec3SubComps(b, a);
    MATH_Vec3 sub_with_mul = MATH_Vec3MulScalar(sub, t);
    MATH_Vec3 lerp = MATH_Vec3AddComps(a, sub_with_mul);

    return MATH_Vec3Normalize(lerp);
}

static inline MATH_Vec3 MATH_Vec3Project(MATH_Vec3 a, MATH_Vec3 b) {
    PRP_F32 denom = MATH_Vec3Dot(b, b);

    return MATH_Vec3MulScalar(b, MATH_Vec3Dot(a, b) / denom);
}

static inline MATH_Vec3 MATH_Vec3ProjectSafe(MATH_Vec3 a, MATH_Vec3 b,
                                             MATH_Vec3 fallback) {
    PRP_F32 denom = MATH_Vec3Dot(b, b);
    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }
    PRP_F32 s = MATH_Vec3Dot(a, b) / denom;

    return MATH_Vec3MulScalar(b, s);
}

static inline MATH_Vec3 MATH_Vec3Reject(MATH_Vec3 a, MATH_Vec3 b) {
    return MATH_Vec3SubComps(a, MATH_Vec3Project(a, b));
}

static inline MATH_Vec3 MATH_Vec3RejectSafe(MATH_Vec3 a, MATH_Vec3 b,
                                            MATH_Vec3 fallback) {
    PRP_F32 denom = MATH_Vec3Dot(b, b);

    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }

    return MATH_Vec3SubComps(a,
                             MATH_Vec3MulScalar(b, MATH_Vec3Dot(a, b) / denom));
}

static inline MATH_Vec3 MATH_Vec3ProjectOnPlane(MATH_Vec3 v, MATH_Vec3 n) {
    return MATH_Vec3SubComps(v, MATH_Vec3Project(v, n));
}

static inline MATH_Vec3 MATH_Vec3ProjectOnPlaneSafe(MATH_Vec3 v, MATH_Vec3 n,
                                                    MATH_Vec3 fallback) {
    PRP_F32 denom = MATH_Vec3Dot(n, n);
    if (MATH_IsZeroF32(denom)) {
        return fallback;
    }

    return MATH_Vec3SubComps(v,
                             MATH_Vec3MulScalar(n, MATH_Vec3Dot(v, n) / denom));
}

static inline MATH_Vec3 MATH_Vec3Reflect(MATH_Vec3 v, MATH_Vec3 n) {
    // n is supposed to be normalized.
    PRP_F32 d = 2.0f * MATH_Vec3Dot(v, n);

    return MATH_Vec3SubComps(v, MATH_Vec3MulScalar(n, d));
}

#ifdef __cplusplus
}
#endif

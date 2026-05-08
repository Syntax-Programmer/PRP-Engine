#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Diagnostics/Assert.h"
#include "../Vector/Vec2.h"

#define MATH_MAT2_SIZE (2)
#define MATH_MAT2_ELEM_COUNT (4)

// Its a column major matrix.
typedef struct {
    DT_f32 m[MATH_MAT2_ELEM_COUNT];
} MATH_Mat2;

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat2 MATH_Mat2CreateZero(DT_void) { return (MATH_Mat2){0}; }

static inline MATH_Mat2 MATH_Mat2CreateIdentity(DT_void) {
    return (MATH_Mat2){.m = {1.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat2 MATH_Mat2CreateDiagScalar(DT_f32 s) {
    return (MATH_Mat2){.m = {s, 0.0f, 0.0f, s}};
}

static inline MATH_Mat2 MATH_Mat2CreateFillScalar(DT_f32 s) {
    return (MATH_Mat2){.m = {s, s, s, s}};
}

static inline MATH_Mat2 MATH_Mat2CreateFromCols(MATH_Vec2 c1, MATH_Vec2 c2) {
    return (MATH_Mat2){.m = {c1.x, c1.y, c2.x, c2.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateFromRows(MATH_Vec2 r1, MATH_Vec2 r2) {
    return (MATH_Mat2){.m = {r1.x, r2.x, r1.y, r2.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateRotation(DT_f32 rad) {
    DT_f32 c = MATH_CosF32(rad);
    DT_f32 s = MATH_SinF32(rad);

    return (MATH_Mat2){.m = {c, s, -s, c}};
}

static inline MATH_Mat2 MATH_Mat2CreateScale(MATH_Vec2 scale) {
    return (MATH_Mat2){.m = {scale.x, 0.0f, 0.0f, scale.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateRS(DT_f32 rot_rad, MATH_Vec2 scale) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat2){
        .m = {c * scale.x, s * scale.x, -s * scale.y, c * scale.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateShear(MATH_Vec2 shear) {
    return (MATH_Mat2){.m = {1.0f, shear.x, shear.y, 1.0f}};
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline MATH_Mat2 MATH_Mat2MinElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.m[0] = MATH_MinF32(a.m[0], b.m[0]);
    a.m[1] = MATH_MinF32(a.m[1], b.m[1]);
    a.m[2] = MATH_MinF32(a.m[2], b.m[2]);
    a.m[3] = MATH_MinF32(a.m[3], b.m[3]);

    return a;
}

static inline MATH_Mat2 MATH_Mat2MaxElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.m[0] = MATH_MaxF32(a.m[0], b.m[0]);
    a.m[1] = MATH_MaxF32(a.m[1], b.m[1]);
    a.m[2] = MATH_MaxF32(a.m[2], b.m[2]);
    a.m[3] = MATH_MaxF32(a.m[3], b.m[3]);

    return a;
}

static inline DT_f32 MATH_Mat2Min(MATH_Mat2 a) {
    DT_f32 min1 = MATH_MinF32(a.m[0], a.m[1]);
    DT_f32 min2 = MATH_MinF32(a.m[2], a.m[3]);

    return MATH_MinF32(min1, min2);
}

static inline DT_f32 MATH_Mat2Max(MATH_Mat2 a) {
    DT_f32 min1 = MATH_MaxF32(a.m[0], a.m[1]);
    DT_f32 min2 = MATH_MaxF32(a.m[2], a.m[3]);

    return MATH_MaxF32(min1, min2);
}

static inline DT_bool MATH_Mat2Eq(MATH_Mat2 a, MATH_Mat2 b) {
    return (DT_bool)((a.m[0] == b.m[0]) && (a.m[1] == b.m[1]) &&
                     (a.m[2] == b.m[2]) && (a.m[3] == b.m[3]));
}

static inline DT_bool MATH_Mat2AlmostEq(MATH_Mat2 a, MATH_Mat2 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.m[0], b.m[0]) &&
                     MATH_AlmostEqF32(a.m[1], b.m[1]) &&
                     MATH_AlmostEqF32(a.m[2], b.m[2]) &&
                     MATH_AlmostEqF32(a.m[3], b.m[3]));
}

static inline DT_bool MATH_Mat2IsZero(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[0]) && MATH_IsZeroF32(a.m[1]) &&
                     MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[3]));
}

static inline DT_bool MATH_Mat2IsNaN(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsNaNF32(a.m[0]) || MATH_IsNaNF32(a.m[1]) ||
                     MATH_IsNaNF32(a.m[2]) || MATH_IsNaNF32(a.m[3]));
}

static inline DT_bool MATH_Mat2IsInf(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsInfF32(a.m[0]) || MATH_IsInfF32(a.m[1]) ||
                     MATH_IsInfF32(a.m[2]) || MATH_IsInfF32(a.m[3]));
}

static inline DT_bool MATH_Mat2IsSymmetric(MATH_Mat2 a) {
    return MATH_AlmostEqF32(a.m[1], a.m[2]);
}

static inline DT_bool MATH_Mat2IsDiagonal(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[1]) && MATH_IsZeroF32(a.m[2]));
}

static inline DT_bool MATH_Mat2IsIdentity(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[1]) && MATH_IsZeroF32(a.m[2]) &&
                     MATH_AlmostEqF32(a.m[0], 1.0f) &&
                     MATH_AlmostEqF32(a.m[3], 1.0f));
}

static inline DT_bool MATH_Mat2IsOrthonormal(MATH_Mat2 a) {
    MATH_Vec2 c0 = {.x = a.m[0], .y = a.m[1]};
    MATH_Vec2 c1 = {.x = a.m[2], .y = a.m[3]};
    DT_f32 dot = MATH_Vec2Dot(c0, c1);
    DT_f32 len0 = MATH_Vec2Len(c0);
    DT_f32 len1 = MATH_Vec2Len(c1);

    return (DT_bool)(MATH_IsZeroF32(dot) && MATH_AlmostEqF32(len0, 1.0f) &&
                     MATH_AlmostEqF32(len1, 1.0f));
}

/* ----  BASIC OPS  ---- */

static inline MATH_Mat2 MATH_Mat2Abs(MATH_Mat2 a) {
    a.m[0] = MATH_AbsF32(a.m[0]);
    a.m[1] = MATH_AbsF32(a.m[1]);
    a.m[2] = MATH_AbsF32(a.m[2]);
    a.m[3] = MATH_AbsF32(a.m[3]);

    return a;
}

static inline MATH_Mat2 MATH_Mat2Sign(MATH_Mat2 a) {
    a.m[0] = MATH_SignF32(a.m[0]);
    a.m[1] = MATH_SignF32(a.m[1]);
    a.m[2] = MATH_SignF32(a.m[2]);
    a.m[3] = MATH_SignF32(a.m[3]);

    return a;
}

static inline MATH_Mat2 MATH_Mat2AddElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.m[0] += b.m[0];
    a.m[1] += b.m[1];
    a.m[2] += b.m[2];
    a.m[3] += b.m[3];

    return a;
}

static inline MATH_Mat2 MATH_Mat2SubElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.m[0] -= b.m[0];
    a.m[1] -= b.m[1];
    a.m[2] -= b.m[2];
    a.m[3] -= b.m[3];

    return a;
}

static inline MATH_Mat2 MATH_Mat2MulElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.m[0] *= b.m[0];
    a.m[1] *= b.m[1];
    a.m[2] *= b.m[2];
    a.m[3] *= b.m[3];

    return a;
}

static inline MATH_Mat2 MATH_Mat2DivElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.m[0] /= b.m[0];
    a.m[1] /= b.m[1];
    a.m[2] /= b.m[2];
    a.m[3] /= b.m[3];

    return a;
}

static inline MATH_Mat2 MATH_Mat2DivElemsSafe(MATH_Mat2 a, MATH_Mat2 b,
                                              DT_f32 fallback) {
    a.m[0] = MATH_SafeDivF32(a.m[0], b.m[0], fallback);
    a.m[1] = MATH_SafeDivF32(a.m[1], b.m[1], fallback);
    a.m[2] = MATH_SafeDivF32(a.m[2], b.m[2], fallback);
    a.m[3] = MATH_SafeDivF32(a.m[3], b.m[3], fallback);

    return a;
}

static inline MATH_Mat2 MATH_Mat2AddScalar(MATH_Mat2 a, DT_f32 s) {
    a.m[0] += s;
    a.m[1] += s;
    a.m[2] += s;
    a.m[3] += s;

    return a;
}

static inline MATH_Mat2 MATH_Mat2SubScalar(MATH_Mat2 a, DT_f32 s) {
    a.m[0] -= s;
    a.m[1] -= s;
    a.m[2] -= s;
    a.m[3] -= s;

    return a;
}

static inline MATH_Mat2 MATH_Mat2MulScalar(MATH_Mat2 a, DT_f32 s) {
    a.m[0] *= s;
    a.m[1] *= s;
    a.m[2] *= s;
    a.m[3] *= s;

    return a;
}

static inline MATH_Mat2 MATH_Mat2DivScalar(MATH_Mat2 a, DT_f32 s) {
    DT_f32 inv_scalar = 1.0f / s;

    a.m[0] *= inv_scalar;
    a.m[1] *= inv_scalar;
    a.m[2] *= inv_scalar;
    a.m[3] *= inv_scalar;

    return a;
}

static inline MATH_Mat2 MATH_Mat2DivScalarSafe(MATH_Mat2 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return MATH_Mat2CreateFillScalar(fallback);
    }

    return MATH_Mat2DivScalar(a, s);
}

static inline MATH_Mat2 MATH_Mat2Neg(MATH_Mat2 a) {
    a.m[0] = -a.m[0];
    a.m[1] = -a.m[1];
    a.m[2] = -a.m[2];
    a.m[3] = -a.m[3];

    return a;
}

static inline MATH_Mat2 MATH_Mat2Mul(MATH_Mat2 a, MATH_Mat2 b) {
    DT_f32 a00 = a.m[0], a10 = a.m[1];
    DT_f32 a01 = a.m[2], a11 = a.m[3];

    DT_f32 b00 = b.m[0], b10 = b.m[1];
    DT_f32 b01 = b.m[2], b11 = b.m[3];

    // Unrolled mat mul calculation.
    a.m[0] = a00 * b00 + a01 * b10;
    a.m[1] = a10 * b00 + a11 * b10;
    a.m[2] = a00 * b01 + a01 * b11;
    a.m[3] = a10 * b01 + a11 * b11;

    return a;
}

static inline MATH_Mat2 MATH_Mat2Outer(MATH_Vec2 u, MATH_Vec2 v) {

    return (MATH_Mat2){.m = {u.x * v.x, u.y * v.x, u.x * v.y, u.y * v.y}};
}

static inline MATH_Vec2 MATH_Mat2MulVec2(MATH_Mat2 a, MATH_Vec2 v) {
    return (MATH_Vec2){
        .x = (a.m[0] * v.x) + (a.m[2] * v.y),
        .y = (a.m[1] * v.x) + (a.m[3] * v.y),
    };
}

static inline DT_f32 MATH_Mat2Trace(MATH_Mat2 a) { return a.m[0] + a.m[3]; }

/* ----  ACCESSORS  ---- */

static inline MATH_Vec2 MATH_Mat2GetRow(MATH_Mat2 a, DT_size row) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);

    return (MATH_Vec2){
        .x = a.m[0 * MATH_MAT2_SIZE + row],
        .y = a.m[1 * MATH_MAT2_SIZE + row],
    };
}

static inline MATH_Vec2 MATH_Mat2GetCol(MATH_Mat2 a, DT_size col) {
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    return (MATH_Vec2){
        .x = a.m[col * MATH_MAT2_SIZE + 0],
        .y = a.m[col * MATH_MAT2_SIZE + 1],
    };
}

static inline MATH_Mat2 MATH_Mat2SetRow(MATH_Mat2 a, DT_size row, MATH_Vec2 v) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);

    a.m[0 * MATH_MAT2_SIZE + row] = v.x;
    a.m[1 * MATH_MAT2_SIZE + row] = v.y;

    return a;
}

static inline MATH_Mat2 MATH_Mat2SetCol(MATH_Mat2 a, DT_size col, MATH_Vec2 v) {
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    a.m[col * MATH_MAT2_SIZE + 0] = v.x;
    a.m[col * MATH_MAT2_SIZE + 1] = v.y;

    return a;
}

static inline DT_f32 MATH_Mat2GetAt(MATH_Mat2 a, DT_size row, DT_size col) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    return a.m[col * MATH_MAT2_SIZE + row];
}

static inline MATH_Mat2 MATH_Mat2SetAt(MATH_Mat2 a, DT_size row, DT_size col,
                                       DT_f32 val) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    a.m[col * MATH_MAT2_SIZE + row] = val;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

static inline MATH_Mat2 MATH_Mat2Transpose(MATH_Mat2 a) {
    DT_f32 a01 = a.m[2];
    a.m[2] = a.m[1];
    a.m[1] = a01;

    return a;
}

static inline MATH_Mat2 MATH_Mat2Adjoint(MATH_Mat2 a) {
    return (MATH_Mat2){.m = {a.m[3], -a.m[1], -a.m[2], a.m[0]}};
}

static inline DT_f32 MATH_Mat2Det(MATH_Mat2 a) {
    DT_f32 a00 = a.m[0];
    DT_f32 a10 = a.m[1];
    DT_f32 a01 = a.m[2];
    DT_f32 a11 = a.m[3];

    return (a00 * a11) - (a01 * a10);
}

static inline DT_bool MATH_Mat2IsFlipped(MATH_Mat2 a) {
    return (DT_bool)(MATH_Mat2Det(a) < 0.0f);
}

static inline MATH_Mat2 MATH_Mat2InvOrthonormal(MATH_Mat2 a) {
    return MATH_Mat2Transpose(a);
}

static inline MATH_Mat2 MATH_Mat2Inv(MATH_Mat2 a, MATH_Mat2 det_zero_fallback) {
    DT_f32 a00 = a.m[0];
    DT_f32 a10 = a.m[1];
    DT_f32 a01 = a.m[2];
    DT_f32 a11 = a.m[3];

    // Determinant calculation
    DT_f32 det = a00 * a11 - a01 * a10;
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    // Adjoint and inverse calculation together
    a.m[0] = a11 * inv_det;
    a.m[1] = -a10 * inv_det;
    a.m[2] = -a01 * inv_det;
    a.m[3] = a00 * inv_det;

    return a;
}

static inline DT_f32 MATH_Mat2ExtractRotation(MATH_Mat2 a) {
    DT_f32 a00 = a.m[0], a10 = a.m[1];
    MATH_Vec2 c0 = {.x = a00, .y = a10};
    DT_f32 len = MATH_Vec2Len(c0);
    if (MATH_IsZeroF32(len))
        return 0.0f;
    DT_f32 inv_len = 1.0f / len;

    DT_f32 x = a00 * inv_len;
    DT_f32 y = a10 * inv_len;

    return MATH_ATan2F32(y, x);
}

static inline MATH_Vec2 MATH_Mat2ExtractScale(MATH_Mat2 a) {
    MATH_Vec2 c0 = {.x = a.m[0], .y = a.m[1]};
    MATH_Vec2 c1 = {.x = a.m[2], .y = a.m[3]};

    return (MATH_Vec2){
        .x = MATH_Vec2Len(c0),
        .y = MATH_Vec2Len(c1),
    };
}

static inline MATH_Mat2 MATH_Mat2NormBasis(MATH_Mat2 a) {
    MATH_Vec2 s = MATH_Mat2ExtractScale(a);
    if (!MATH_IsZeroF32(s.x)) {
        a.m[0] /= s.x;
        a.m[1] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.m[2] /= s.y;
        a.m[3] /= s.y;
    }

    return a;
}

static inline DT_f32 MATH_Mat2FrobeniusNorm(MATH_Mat2 a) {
    return MATH_SqrtF32((a.m[0] * a.m[0]) + (a.m[1] * a.m[1]) +
                        (a.m[2] * a.m[2]) + (a.m[3] * a.m[3]));
}

static inline DT_f32 MATH_Mat2FrobeniusNormSq(MATH_Mat2 a) {
    return (a.m[0] * a.m[0]) + (a.m[1] * a.m[1]) + (a.m[2] * a.m[2]) +
           (a.m[3] * a.m[3]);
}

static inline MATH_Mat2 MATH_Mat2Orthonormalize(MATH_Mat2 a) {
    MATH_Vec2 c0 = {a.m[0], a.m[1]};
    MATH_Vec2 c1 = {a.m[2], a.m[3]};
    c0 = MATH_Vec2Normalize(c0);

    DT_f32 proj = MATH_Vec2Dot(c1, c0);
    c1.x -= c0.x * proj;
    c1.y -= c0.y * proj;

    c1 = MATH_Vec2Normalize(c1);

    a.m[0] = c0.x;
    a.m[1] = c0.y;
    a.m[2] = c1.x;
    a.m[3] = c1.y;

    return a;
}

static inline MATH_Vec2 MATH_Mat2GetRight(MATH_Mat2 a) {
    return (MATH_Vec2){
        .x = a.m[0],
        .y = a.m[1],
    };
}

static inline MATH_Vec2 MATH_Mat2GetUp(MATH_Mat2 a) {
    return (MATH_Vec2){
        .x = a.m[2],
        .y = a.m[3],
    };
}

#ifdef __cplusplus
}
#endif

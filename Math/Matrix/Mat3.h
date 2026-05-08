#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Diagnostics/Assert.h"
#include "../Vector/Vec2.h"
#include "../Vector/Vec3.h"

#define MATH_MAT3_SIZE (3)
#define MATH_MAT3_ELEM_COUNT (9)

// Its a column major matrix.
typedef struct {
    DT_f32 m[MATH_MAT3_ELEM_COUNT];
} MATH_Mat3;

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat3 MATH_Mat3CreateZero(DT_void) { return (MATH_Mat3){0}; }

static inline MATH_Mat3 MATH_Mat3CreateIdentity(DT_void) {
    return (MATH_Mat3){
        .m = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateDiagScalar(DT_f32 s) {
    return (MATH_Mat3){.m = {s, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f, s}};
}

static inline MATH_Mat3 MATH_Mat3CreateFillScalar(DT_f32 s) {
    return (MATH_Mat3){.m = {s, s, s, s, s, s, s, s, s}};
}

static inline MATH_Mat3 MATH_Mat3CreateFromCols(MATH_Vec3 c1, MATH_Vec3 c2,
                                                MATH_Vec3 c3) {
    return (MATH_Mat3){
        .m = {c1.x, c1.y, c1.z, c2.x, c2.y, c2.z, c3.x, c3.y, c3.z}};
}

static inline MATH_Mat3 MATH_Mat3CreateFromRows(MATH_Vec3 r1, MATH_Vec3 r2,
                                                MATH_Vec3 r3) {
    return (MATH_Mat3){
        .m = {r1.x, r2.x, r3.x, r1.y, r2.y, r3.y, r1.z, r2.z, r3.z}};
}

static inline MATH_Mat3 MATH_Mat3CreateTranslation(MATH_Vec2 pos) {
    return (MATH_Mat3){.m = {1.0f, 0.0f, 0.0f,

                             0.0f, 1.0f, 0.0f,

                             pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateRotation(DT_f32 rad) {
    DT_f32 c = MATH_CosF32(rad);
    DT_f32 s = MATH_SinF32(rad);

    return (MATH_Mat3){.m = {c, s, 0.0f,

                             -s, c, 0.0f,

                             0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateScale(MATH_Vec2 scale) {
    return (MATH_Mat3){.m = {scale.x, 0.0f, 0.0f,

                             0.0f, scale.y, 0.0f,

                             0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateTR(MATH_Vec2 pos, DT_f32 rot_rad) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat3){.m = {c, s, 0.0f,

                             -s, c, 0.0f,

                             pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateTS(MATH_Vec2 pos, MATH_Vec2 scale) {
    return (MATH_Mat3){.m = {scale.x, 0.0f, 0.0f,

                             0.0f, scale.y, 0.0f,

                             pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateRS(DT_f32 rot_rad, MATH_Vec2 scale) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat3){.m = {c * scale.x, s * scale.x, 0.0f,

                             -s * scale.y, c * scale.y, 0.0f,

                             0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateTRS(MATH_Vec2 pos, DT_f32 rot_rad,
                                           MATH_Vec2 scale) {
    DT_f32 c = MATH_CosF32(rot_rad);
    DT_f32 s = MATH_SinF32(rot_rad);

    return (MATH_Mat3){.m = {c * scale.x, s * scale.x, 0.0f,

                             -s * scale.y, c * scale.y, 0.0f,

                             pos.x, pos.y, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateShear(MATH_Vec2 shear) {

    return (MATH_Mat3){.m = {1.0f, shear.x, 0.0f,

                             shear.y, 1.0f, 0.0f,

                             0.0f, 0.0f, 1.0f}};
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline MATH_Mat3 MATH_Mat3MinElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.m[0] = MATH_MinF32(a.m[0], b.m[0]);
    a.m[1] = MATH_MinF32(a.m[1], b.m[1]);
    a.m[2] = MATH_MinF32(a.m[2], b.m[2]);
    a.m[3] = MATH_MinF32(a.m[3], b.m[3]);
    a.m[4] = MATH_MinF32(a.m[4], b.m[4]);
    a.m[5] = MATH_MinF32(a.m[5], b.m[5]);
    a.m[6] = MATH_MinF32(a.m[6], b.m[6]);
    a.m[7] = MATH_MinF32(a.m[7], b.m[7]);
    a.m[8] = MATH_MinF32(a.m[8], b.m[8]);

    return a;
}

static inline MATH_Mat3 MATH_Mat3MaxElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.m[0] = MATH_MaxF32(a.m[0], b.m[0]);
    a.m[1] = MATH_MaxF32(a.m[1], b.m[1]);
    a.m[2] = MATH_MaxF32(a.m[2], b.m[2]);
    a.m[3] = MATH_MaxF32(a.m[3], b.m[3]);
    a.m[4] = MATH_MaxF32(a.m[4], b.m[4]);
    a.m[5] = MATH_MaxF32(a.m[5], b.m[5]);
    a.m[6] = MATH_MaxF32(a.m[6], b.m[6]);
    a.m[7] = MATH_MaxF32(a.m[7], b.m[7]);
    a.m[8] = MATH_MaxF32(a.m[8], b.m[8]);

    return a;
}

static inline DT_f32 MATH_Mat3Min(MATH_Mat3 a) {
    DT_f32 min1 = MATH_MinF32(a.m[0], a.m[1]);
    DT_f32 min2 = MATH_MinF32(a.m[2], a.m[3]);
    DT_f32 min3 = MATH_MinF32(a.m[4], a.m[5]);
    DT_f32 min4 = MATH_MinF32(a.m[6], a.m[7]);

    DT_f32 min5 = MATH_MinF32(min1, min2);
    DT_f32 min6 = MATH_MinF32(min3, min4);

    DT_f32 min7 = MATH_MinF32(min5, min6);

    return MATH_MinF32(min7, a.m[8]);
}

static inline DT_f32 MATH_Mat3Max(MATH_Mat3 a) {
    DT_f32 min1 = MATH_MaxF32(a.m[0], a.m[1]);
    DT_f32 min2 = MATH_MaxF32(a.m[2], a.m[3]);
    DT_f32 min3 = MATH_MaxF32(a.m[4], a.m[5]);
    DT_f32 min4 = MATH_MaxF32(a.m[6], a.m[7]);

    DT_f32 min5 = MATH_MaxF32(min1, min2);
    DT_f32 min6 = MATH_MaxF32(min3, min4);

    DT_f32 min7 = MATH_MaxF32(min5, min6);

    return MATH_MaxF32(min7, a.m[8]);
}

static inline DT_bool MATH_Mat3Eq(MATH_Mat3 a, MATH_Mat3 b) {
    return (DT_bool)((a.m[0] == b.m[0]) && (a.m[1] == b.m[1]) &&
                     (a.m[2] == b.m[2]) && (a.m[3] == b.m[3]) &&
                     (a.m[4] == b.m[4]) && (a.m[5] == b.m[5]) &&
                     (a.m[6] == b.m[6]) && (a.m[7] == b.m[7]) &&
                     (a.m[8] == b.m[8]));
}

static inline DT_bool MATH_Mat3AlmostEq(MATH_Mat3 a, MATH_Mat3 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.m[0], b.m[0]) &&
                     MATH_AlmostEqF32(a.m[1], b.m[1]) &&
                     MATH_AlmostEqF32(a.m[2], b.m[2]) &&
                     MATH_AlmostEqF32(a.m[3], b.m[3]) &&
                     MATH_AlmostEqF32(a.m[4], b.m[4]) &&
                     MATH_AlmostEqF32(a.m[5], b.m[5]) &&
                     MATH_AlmostEqF32(a.m[6], b.m[6]) &&
                     MATH_AlmostEqF32(a.m[7], b.m[7]) &&
                     MATH_AlmostEqF32(a.m[8], b.m[8]));
}

static inline DT_bool MATH_Mat3IsZero(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[0]) && MATH_IsZeroF32(a.m[1]) &&
                     MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[3]) &&
                     MATH_IsZeroF32(a.m[4]) && MATH_IsZeroF32(a.m[5]) &&
                     MATH_IsZeroF32(a.m[6]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_IsZeroF32(a.m[8]));
}

static inline DT_bool MATH_Mat3IsNaN(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsNaNF32(a.m[0]) || MATH_IsNaNF32(a.m[1]) ||
                     MATH_IsNaNF32(a.m[2]) || MATH_IsNaNF32(a.m[3]) ||
                     MATH_IsNaNF32(a.m[4]) || MATH_IsNaNF32(a.m[5]) ||
                     MATH_IsNaNF32(a.m[6]) || MATH_IsNaNF32(a.m[7]) ||
                     MATH_IsNaNF32(a.m[8]));
}

static inline DT_bool MATH_Mat3IsInf(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsInfF32(a.m[0]) || MATH_IsInfF32(a.m[1]) ||
                     MATH_IsInfF32(a.m[2]) || MATH_IsInfF32(a.m[3]) ||
                     MATH_IsInfF32(a.m[4]) || MATH_IsInfF32(a.m[5]) ||
                     MATH_IsInfF32(a.m[6]) || MATH_IsInfF32(a.m[7]) ||
                     MATH_IsInfF32(a.m[8]));
}

static inline DT_bool MATH_Mat3IsOrthonormal(MATH_Mat3 a) {
    MATH_Vec3 c0 = {.x = a.m[0], .y = a.m[1], .z = a.m[2]};
    MATH_Vec3 c1 = {.x = a.m[3], .y = a.m[4], .z = a.m[5]};
    MATH_Vec3 c2 = {.x = a.m[6], .y = a.m[7], .z = a.m[8]};

    DT_f32 d01 = MATH_Vec3Dot(c0, c1);
    DT_f32 d02 = MATH_Vec3Dot(c0, c2);
    DT_f32 d12 = MATH_Vec3Dot(c1, c2);

    DT_f32 l0 = MATH_Vec3LenSq(c0);
    DT_f32 l1 = MATH_Vec3LenSq(c1);
    DT_f32 l2 = MATH_Vec3LenSq(c2);

    return (DT_bool)(MATH_IsZeroF32(d01) && MATH_IsZeroF32(d02) &&
                     MATH_IsZeroF32(d12) && MATH_AlmostEqF32(l0, 1.0f) &&
                     MATH_AlmostEqF32(l1, 1.0f) && MATH_AlmostEqF32(l2, 1.0f));
}

static inline DT_bool MATH_Mat3IsAffine(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[5]) &&
                     MATH_AlmostEqF32(a.m[8], 1.0f));
}

static inline DT_bool MATH_Mat3IsAffineOrthonormal(MATH_Mat3 a) {
    MATH_Vec2 c0 = {.x = a.m[0], .y = a.m[1]};
    MATH_Vec2 c1 = {.x = a.m[3], .y = a.m[4]};

    DT_f32 d01 = MATH_Vec2Dot(c0, c1);

    DT_f32 l0 = MATH_Vec2LenSq(c0);
    DT_f32 l1 = MATH_Vec2LenSq(c1);

    return (DT_bool)(MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[5]) &&
                     MATH_AlmostEqF32(a.m[8], 1.0f) && MATH_IsZeroF32(d01) &&
                     MATH_AlmostEqF32(l0, 1.0f) && MATH_AlmostEqF32(l1, 1.0f));
}

static inline DT_bool MATH_Mat3IsSymmetric(MATH_Mat3 a) {
    return (DT_bool)(MATH_AlmostEqF32(a.m[1], a.m[3]) &&
                     MATH_AlmostEqF32(a.m[2], a.m[6]) &&
                     MATH_AlmostEqF32(a.m[5], a.m[7]));
}

static inline DT_bool MATH_Mat3IsDiagonal(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[1]) && MATH_IsZeroF32(a.m[3]) &&
                     MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[6]) &&
                     MATH_IsZeroF32(a.m[5]) && MATH_IsZeroF32(a.m[7]));
}

static inline DT_bool MATH_Mat3IsIdentity(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[1]) && MATH_IsZeroF32(a.m[3]) &&
                     MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[6]) &&
                     MATH_IsZeroF32(a.m[5]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_AlmostEqF32(a.m[0], 1.0f) &&
                     MATH_AlmostEqF32(a.m[4], 1.0f) &&
                     MATH_AlmostEqF32(a.m[8], 1.0f));
}

/* ----  BASIC OPS  ---- */

static inline MATH_Mat3 MATH_Mat3Abs(MATH_Mat3 a) {
    a.m[0] = MATH_AbsF32(a.m[0]);
    a.m[1] = MATH_AbsF32(a.m[1]);
    a.m[2] = MATH_AbsF32(a.m[2]);
    a.m[3] = MATH_AbsF32(a.m[3]);
    a.m[4] = MATH_AbsF32(a.m[4]);
    a.m[5] = MATH_AbsF32(a.m[5]);
    a.m[6] = MATH_AbsF32(a.m[6]);
    a.m[7] = MATH_AbsF32(a.m[7]);
    a.m[8] = MATH_AbsF32(a.m[8]);

    return a;
}

static inline MATH_Mat3 MATH_Mat3Sign(MATH_Mat3 a) {
    a.m[0] = MATH_SignF32(a.m[0]);
    a.m[1] = MATH_SignF32(a.m[1]);
    a.m[2] = MATH_SignF32(a.m[2]);
    a.m[3] = MATH_SignF32(a.m[3]);
    a.m[4] = MATH_SignF32(a.m[4]);
    a.m[5] = MATH_SignF32(a.m[5]);
    a.m[6] = MATH_SignF32(a.m[6]);
    a.m[7] = MATH_SignF32(a.m[7]);
    a.m[8] = MATH_SignF32(a.m[8]);

    return a;
}

static inline MATH_Mat3 MATH_Mat3AddElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.m[0] += b.m[0];
    a.m[1] += b.m[1];
    a.m[2] += b.m[2];
    a.m[3] += b.m[3];
    a.m[4] += b.m[4];
    a.m[5] += b.m[5];
    a.m[6] += b.m[6];
    a.m[7] += b.m[7];
    a.m[8] += b.m[8];

    return a;
}

static inline MATH_Mat3 MATH_Mat3SubElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.m[0] -= b.m[0];
    a.m[1] -= b.m[1];
    a.m[2] -= b.m[2];
    a.m[3] -= b.m[3];
    a.m[4] -= b.m[4];
    a.m[5] -= b.m[5];
    a.m[6] -= b.m[6];
    a.m[7] -= b.m[7];
    a.m[8] -= b.m[8];

    return a;
}

static inline MATH_Mat3 MATH_Mat3MulElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.m[0] *= b.m[0];
    a.m[1] *= b.m[1];
    a.m[2] *= b.m[2];
    a.m[3] *= b.m[3];
    a.m[4] *= b.m[4];
    a.m[5] *= b.m[5];
    a.m[6] *= b.m[6];
    a.m[7] *= b.m[7];
    a.m[8] *= b.m[8];

    return a;
}

static inline MATH_Mat3 MATH_Mat3DivElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.m[0] /= b.m[0];
    a.m[1] /= b.m[1];
    a.m[2] /= b.m[2];
    a.m[3] /= b.m[3];
    a.m[4] /= b.m[4];
    a.m[5] /= b.m[5];
    a.m[6] /= b.m[6];
    a.m[7] /= b.m[7];
    a.m[8] /= b.m[8];

    return a;
}

static inline MATH_Mat3 MATH_Mat3DivElemsSafe(MATH_Mat3 a, MATH_Mat3 b,
                                              DT_f32 fallback) {
    a.m[0] = MATH_SafeDivF32(a.m[0], b.m[0], fallback);
    a.m[1] = MATH_SafeDivF32(a.m[1], b.m[1], fallback);
    a.m[2] = MATH_SafeDivF32(a.m[2], b.m[2], fallback);
    a.m[3] = MATH_SafeDivF32(a.m[3], b.m[3], fallback);
    a.m[4] = MATH_SafeDivF32(a.m[4], b.m[4], fallback);
    a.m[5] = MATH_SafeDivF32(a.m[5], b.m[5], fallback);
    a.m[6] = MATH_SafeDivF32(a.m[6], b.m[6], fallback);
    a.m[7] = MATH_SafeDivF32(a.m[7], b.m[7], fallback);
    a.m[8] = MATH_SafeDivF32(a.m[8], b.m[8], fallback);

    return a;
}

static inline MATH_Mat3 MATH_Mat3AddScalar(MATH_Mat3 a, DT_f32 s) {
    a.m[0] += s;
    a.m[1] += s;
    a.m[2] += s;
    a.m[3] += s;
    a.m[4] += s;
    a.m[5] += s;
    a.m[6] += s;
    a.m[7] += s;
    a.m[8] += s;

    return a;
}

static inline MATH_Mat3 MATH_Mat3SubScalar(MATH_Mat3 a, DT_f32 s) {
    a.m[0] -= s;
    a.m[1] -= s;
    a.m[2] -= s;
    a.m[3] -= s;
    a.m[4] -= s;
    a.m[5] -= s;
    a.m[6] -= s;
    a.m[7] -= s;
    a.m[8] -= s;

    return a;
}

static inline MATH_Mat3 MATH_Mat3MulScalar(MATH_Mat3 a, DT_f32 s) {
    a.m[0] *= s;
    a.m[1] *= s;
    a.m[2] *= s;
    a.m[3] *= s;
    a.m[4] *= s;
    a.m[5] *= s;
    a.m[6] *= s;
    a.m[7] *= s;
    a.m[8] *= s;

    return a;
}

static inline MATH_Mat3 MATH_Mat3DivScalar(MATH_Mat3 a, DT_f32 s) {
    DT_f32 inv_scalar = 1.0f / s;

    a.m[0] *= inv_scalar;
    a.m[1] *= inv_scalar;
    a.m[2] *= inv_scalar;
    a.m[3] *= inv_scalar;
    a.m[4] *= inv_scalar;
    a.m[5] *= inv_scalar;
    a.m[6] *= inv_scalar;
    a.m[7] *= inv_scalar;
    a.m[8] *= inv_scalar;

    return a;
}

static inline MATH_Mat3 MATH_Mat3DivScalarSafe(MATH_Mat3 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return MATH_Mat3CreateFillScalar(fallback);
    }

    return MATH_Mat3DivScalar(a, s);
}

static inline MATH_Mat3 MATH_Mat3Neg(MATH_Mat3 a) {
    a.m[0] = -a.m[0];
    a.m[1] = -a.m[1];
    a.m[2] = -a.m[2];
    a.m[3] = -a.m[3];
    a.m[4] = -a.m[4];
    a.m[5] = -a.m[5];
    a.m[6] = -a.m[6];
    a.m[7] = -a.m[7];
    a.m[8] = -a.m[8];

    return a;
}

static inline MATH_Mat3 MATH_Mat3Mul(MATH_Mat3 a, MATH_Mat3 b) {
    DT_f32 a00 = a.m[0], a10 = a.m[1], a20 = a.m[2];
    DT_f32 a01 = a.m[3], a11 = a.m[4], a21 = a.m[5];
    DT_f32 a02 = a.m[6], a12 = a.m[7], a22 = a.m[8];

    DT_f32 b00 = b.m[0], b10 = b.m[1], b20 = b.m[2];
    DT_f32 b01 = b.m[3], b11 = b.m[4], b21 = b.m[5];
    DT_f32 b02 = b.m[6], b12 = b.m[7], b22 = b.m[8];

    a.m[0] = (a00 * b00) + (a01 * b10) + (a02 * b20);
    a.m[1] = (a10 * b00) + (a11 * b10) + (a12 * b20);
    a.m[2] = (a20 * b00) + (a21 * b10) + (a22 * b20);

    a.m[3] = (a00 * b01) + (a01 * b11) + (a02 * b21);
    a.m[4] = (a10 * b01) + (a11 * b11) + (a12 * b21);
    a.m[5] = (a20 * b01) + (a21 * b11) + (a22 * b21);

    a.m[6] = (a00 * b02) + (a01 * b12) + (a02 * b22);
    a.m[7] = (a10 * b02) + (a11 * b12) + (a12 * b22);
    a.m[8] = (a20 * b02) + (a21 * b12) + (a22 * b22);

    return a;
}

static inline MATH_Mat3 MATH_Mat3Outer(MATH_Vec3 u, MATH_Vec3 v) {

    return (MATH_Mat3){.m = {(u.x * v.x), (u.y * v.x), (u.z * v.x), (u.x * v.y),
                             (u.y * v.y), (u.z * v.y), (u.x * v.z), (u.y * v.z),
                             (u.z * v.z)}};
}

static inline MATH_Vec3 MATH_Mat3MulVec3(MATH_Mat3 a, MATH_Vec3 v) {
    return (MATH_Vec3){
        .x = (a.m[0] * v.x) + (a.m[3] * v.y) + (a.m[6] * v.z),
        .y = (a.m[1] * v.x) + (a.m[4] * v.y) + (a.m[7] * v.z),
        .z = (a.m[2] * v.x) + (a.m[5] * v.y) + (a.m[8] * v.z),
    };
}

static inline MATH_Vec2 MATH_Mat3MulVec2Projective(MATH_Mat3 a, MATH_Vec2 v) {
    MATH_Vec3 mul = {
        .x = (a.m[0] * v.x) + (a.m[3] * v.y) + (a.m[6]),
        .y = (a.m[1] * v.x) + (a.m[4] * v.y) + (a.m[7]),
        .z = (a.m[2] * v.x) + (a.m[5] * v.y) + (a.m[8]),
    };
    DT_f32 inv_z = 1 / mul.z;

    return (MATH_Vec2){.x = mul.x * inv_z, .y = mul.y * inv_z};
}

static inline MATH_Vec2 MATH_Mat3MulVec2ProjectiveSafe(MATH_Mat3 a, MATH_Vec2 v,
                                                       MATH_Vec2 fallback) {
    MATH_Vec3 mul = {
        .x = (a.m[0] * v.x) + (a.m[3] * v.y) + (a.m[6]),
        .y = (a.m[1] * v.x) + (a.m[4] * v.y) + (a.m[7]),
        .z = (a.m[2] * v.x) + (a.m[5] * v.y) + (a.m[8]),
    };
    if (MATH_IsZeroF32(mul.z)) {
        return fallback;
    }
    DT_f32 inv_z = 1 / mul.z;

    return (MATH_Vec2){.x = mul.x * inv_z, .y = mul.y * inv_z};
}

static inline MATH_Vec2 MATH_Mat3MulVec2Dir(MATH_Mat3 a, MATH_Vec2 v) {
    return (MATH_Vec2){
        .x = (a.m[0] * v.x) + (a.m[3] * v.y),
        .y = (a.m[1] * v.x) + (a.m[4] * v.y),
    };
}

static inline MATH_Vec2 MATH_Mat3MulVec2Affine(MATH_Mat3 a, MATH_Vec2 v) {
    return (MATH_Vec2){
        .x = (a.m[0] * v.x) + (a.m[3] * v.y) + a.m[6],
        .y = (a.m[1] * v.x) + (a.m[4] * v.y) + a.m[7],
    };
}

static inline DT_f32 MATH_Mat3Trace(MATH_Mat3 a) {
    return a.m[0] + a.m[4] + a.m[8];
}

/* ----  ACCESSORS  ---- */

static inline MATH_Vec3 MATH_Mat3GetRow(MATH_Mat3 a, DT_size row) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);

    return (MATH_Vec3){
        .x = a.m[0 * MATH_MAT3_SIZE + row],
        .y = a.m[1 * MATH_MAT3_SIZE + row],
        .z = a.m[2 * MATH_MAT3_SIZE + row],
    };
}

static inline MATH_Vec3 MATH_Mat3GetCol(MATH_Mat3 a, DT_size col) {
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    return (MATH_Vec3){
        .x = a.m[col * MATH_MAT3_SIZE + 0],
        .y = a.m[col * MATH_MAT3_SIZE + 1],
        .z = a.m[col * MATH_MAT3_SIZE + 2],
    };
}

static inline MATH_Mat3 MATH_Mat3SetRow(MATH_Mat3 a, DT_size row, MATH_Vec3 v) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);

    a.m[0 * MATH_MAT3_SIZE + row] = v.x;
    a.m[1 * MATH_MAT3_SIZE + row] = v.y;
    a.m[2 * MATH_MAT3_SIZE + row] = v.z;

    return a;
}

static inline MATH_Mat3 MATH_Mat3SetCol(MATH_Mat3 a, DT_size col, MATH_Vec3 v) {
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    a.m[col * MATH_MAT3_SIZE + 0] = v.x;
    a.m[col * MATH_MAT3_SIZE + 1] = v.y;
    a.m[col * MATH_MAT3_SIZE + 2] = v.z;

    return a;
}

static inline DT_f32 MATH_Mat3GetAt(MATH_Mat3 a, DT_size row, DT_size col) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    return a.m[col * MATH_MAT3_SIZE + row];
}

static inline MATH_Mat3 MATH_Mat3SetAt(MATH_Mat3 a, DT_size row, DT_size col,
                                       DT_f32 val) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    a.m[col * MATH_MAT3_SIZE + row] = val;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

static inline MATH_Mat3 MATH_Mat3Transpose(MATH_Mat3 a) {
    MATH_Mat3 rslt;

    rslt.m[0 * MATH_MAT3_SIZE + 0] = a.m[0 * MATH_MAT3_SIZE + 0];
    rslt.m[0 * MATH_MAT3_SIZE + 1] = a.m[1 * MATH_MAT3_SIZE + 0];
    rslt.m[0 * MATH_MAT3_SIZE + 2] = a.m[2 * MATH_MAT3_SIZE + 0];
    rslt.m[1 * MATH_MAT3_SIZE + 0] = a.m[0 * MATH_MAT3_SIZE + 1];
    rslt.m[1 * MATH_MAT3_SIZE + 1] = a.m[1 * MATH_MAT3_SIZE + 1];
    rslt.m[1 * MATH_MAT3_SIZE + 2] = a.m[2 * MATH_MAT3_SIZE + 1];
    rslt.m[2 * MATH_MAT3_SIZE + 0] = a.m[0 * MATH_MAT3_SIZE + 2];
    rslt.m[2 * MATH_MAT3_SIZE + 1] = a.m[1 * MATH_MAT3_SIZE + 2];
    rslt.m[2 * MATH_MAT3_SIZE + 2] = a.m[2 * MATH_MAT3_SIZE + 2];

    return rslt;
}

static inline MATH_Mat3 MATH_Mat3Adjoint(MATH_Mat3 a) {
    DT_f32 a00 = a.m[0], a10 = a.m[1], a20 = a.m[2];
    DT_f32 a01 = a.m[3], a11 = a.m[4], a21 = a.m[5];
    DT_f32 a02 = a.m[6], a12 = a.m[7], a22 = a.m[8];

    DT_f32 c00 = (a11 * a22 - a12 * a21);
    DT_f32 c01 = -(a10 * a22 - a12 * a20);
    DT_f32 c02 = (a10 * a21 - a11 * a20);
    DT_f32 c10 = -(a01 * a22 - a02 * a21);
    DT_f32 c11 = (a00 * a22 - a02 * a20);
    DT_f32 c12 = -(a00 * a21 - a01 * a20);
    DT_f32 c20 = (a01 * a12 - a02 * a11);
    DT_f32 c21 = -(a00 * a12 - a02 * a10);
    DT_f32 c22 = (a00 * a11 - a01 * a10);

    return (MATH_Mat3){.m = {c00, c01, c02, c10, c11, c12, c20, c21, c22}};
}

static inline DT_f32 MATH_Mat3Det(MATH_Mat3 a) {
    DT_f32 a00 = a.m[0], a10 = a.m[1], a20 = a.m[2];
    DT_f32 a01 = a.m[3], a11 = a.m[4], a21 = a.m[5];
    DT_f32 a02 = a.m[6], a12 = a.m[7], a22 = a.m[8];

    DT_f32 m0 = (a11 * a22) - (a12 * a21);
    DT_f32 m1 = (a01 * a22) - (a02 * a21);
    DT_f32 m2 = (a01 * a12) - (a02 * a11);

    return (a00 * m0) - (a10 * m1) + (a20 * m2);
}

static inline DT_bool MATH_Mat3IsFlipped(MATH_Mat3 a) {
    return (DT_bool)(MATH_Mat3Det(a) < 0.0f);
}

static inline MATH_Mat3 MATH_Mat3Inv(MATH_Mat3 a, MATH_Mat3 det_zero_fallback) {
    DT_f32 a00 = a.m[0], a10 = a.m[1], a20 = a.m[2];
    DT_f32 a01 = a.m[3], a11 = a.m[4], a21 = a.m[5];
    DT_f32 a02 = a.m[6], a12 = a.m[7], a22 = a.m[8];

    // Cofactors
    DT_f32 c00 = (a11 * a22 - a12 * a21);
    DT_f32 c01 = -(a10 * a22 - a12 * a20);
    DT_f32 c02 = (a10 * a21 - a11 * a20);
    DT_f32 c10 = -(a01 * a22 - a02 * a21);
    DT_f32 c11 = (a00 * a22 - a02 * a20);
    DT_f32 c12 = -(a00 * a21 - a01 * a20);
    DT_f32 c20 = (a01 * a12 - a02 * a11);
    DT_f32 c21 = -(a00 * a12 - a02 * a10);
    DT_f32 c22 = (a00 * a11 - a01 * a10);

    // Determinant
    DT_f32 det = a00 * c00 + a10 * c10 + a20 * c20;
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    // Adjoint and inverse calculation together
    a.m[0] = c00 * inv_det;
    a.m[1] = c10 * inv_det;
    a.m[2] = c20 * inv_det;
    a.m[3] = c01 * inv_det;
    a.m[4] = c11 * inv_det;
    a.m[5] = c21 * inv_det;
    a.m[6] = c02 * inv_det;
    a.m[7] = c12 * inv_det;
    a.m[8] = c22 * inv_det;

    return a;
}

static inline MATH_Mat3 MATH_Mat3InvAffine(MATH_Mat3 a,
                                           MATH_Mat3 det_zero_fallback) {
    DT_f32 a00 = a.m[0], a10 = a.m[1];
    DT_f32 a01 = a.m[3], a11 = a.m[4];
    DT_f32 tx = a.m[6], ty = a.m[7];
    // determinant of upper-left 2x2
    DT_f32 det = (a00 * a11) - (a01 * a10);
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    // inverse 2x2 basis
    DT_f32 r00 = a11 * inv_det;
    DT_f32 r01 = -a01 * inv_det;
    DT_f32 r10 = -a10 * inv_det;
    DT_f32 r11 = a00 * inv_det;
    // inverse translation
    DT_f32 itx = -(r00 * tx + r01 * ty);
    DT_f32 ity = -(r10 * tx + r11 * ty);

    return (MATH_Mat3){.m = {r00, r10, 0.0f, r01, r11, 0.0f, itx, ity, 1.0f}};
}

static inline MATH_Vec2 MATH_Mat3ExtractTranslation(MATH_Mat3 a) {
    return (MATH_Vec2){
        .x = a.m[6],
        .y = a.m[7],
    };
}

static inline DT_f32 MATH_Mat3ExtractRotation(MATH_Mat3 a) {
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

static inline MATH_Vec2 MATH_Mat3ExtractScale(MATH_Mat3 a) {
    MATH_Vec2 c0 = {.x = a.m[0], .y = a.m[1]};
    MATH_Vec2 c1 = {.x = a.m[3], .y = a.m[4]};

    return (MATH_Vec2){
        .x = MATH_Vec2Len(c0),
        .y = MATH_Vec2Len(c1),
    };
}

static inline MATH_Mat3 MATH_Mat3NormBasis(MATH_Mat3 a) {
    MATH_Vec2 s = MATH_Mat3ExtractScale(a);
    if (!MATH_IsZeroF32(s.x)) {
        a.m[0] /= s.x;
        a.m[1] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.m[3] /= s.y;
        a.m[4] /= s.y;
    }

    return a;
}

static inline DT_f32 MATH_Mat3FrobeniusNorm(MATH_Mat3 a) {
    return MATH_SqrtF32(
        (a.m[0] * a.m[0]) + (a.m[1] * a.m[1]) + (a.m[2] * a.m[2]) +
        (a.m[3] * a.m[3]) + (a.m[4] * a.m[4]) + (a.m[5] * a.m[5]) +
        (a.m[6] * a.m[6]) + (a.m[7] * a.m[7]) + (a.m[8] * a.m[8]));
}

static inline DT_f32 MATH_Mat3FrobeniusNormSq(MATH_Mat3 a) {
    return (a.m[0] * a.m[0]) + (a.m[1] * a.m[1]) + (a.m[2] * a.m[2]) +
           (a.m[3] * a.m[3]) + (a.m[4] * a.m[4]) + (a.m[5] * a.m[5]) +
           (a.m[6] * a.m[6]) + (a.m[7] * a.m[7]) + (a.m[8] * a.m[8]);
}

static inline MATH_Vec2 MATH_Mat3GetRight(MATH_Mat3 a) {
    return (MATH_Vec2){
        .x = a.m[0],
        .y = a.m[1],
    };
}

static inline MATH_Vec2 MATH_Mat3GetUp(MATH_Mat3 a) {
    return (MATH_Vec2){
        .x = a.m[3],
        .y = a.m[4],
    };
}

#ifdef __cplusplus
}
#endif

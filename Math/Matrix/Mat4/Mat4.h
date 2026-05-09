#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Diagnostics/Assert.h"
#include "../../../Utils/Defs.h"
#include "../../Vector/Vec3.h"
#include "../../Vector/Vec4.h"
#include "Defs.h"

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat4 MATH_Mat4CreateZero(DT_void) { return (MATH_Mat4){0}; }

static inline MATH_Mat4 MATH_Mat4CreateIdentity(DT_void) {
    return (MATH_Mat4){.membs = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                 1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateDiagScalar(DT_f32 s) {
    return (MATH_Mat4){.membs = {s, 0.0f, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f,
                                 0.0f, s, 0.0f, 0.0f, 0.0f, 0.0f, s}};
}

static inline MATH_Mat4 MATH_Mat4CreateFillScalar(DT_f32 s) {
    return (MATH_Mat4){
        .membs = {s, s, s, s, s, s, s, s, s, s, s, s, s, s, s, s}};
}

static inline MATH_Mat4 MATH_Mat4CreateFromCols(MATH_Vec4 c1, MATH_Vec4 c2,
                                                MATH_Vec4 c3, MATH_Vec4 c4) {
    return (MATH_Mat4){.membs = {c1.x, c1.y, c1.z, c1.w, c2.x, c2.y, c2.z, c2.w,
                                 c3.x, c3.y, c3.z, c3.w, c4.x, c4.y, c4.z,
                                 c4.w}};
}

static inline MATH_Mat4 MATH_Mat4CreateFromRows(MATH_Vec4 r1, MATH_Vec4 r2,
                                                MATH_Vec4 r3, MATH_Vec4 r4) {
    return (MATH_Mat4){.membs = {r1.x, r2.x, r3.x, r4.x, r1.y, r2.y, r3.y, r4.y,
                                 r1.z, r2.z, r3.z, r4.z, r1.w, r2.w, r3.w,
                                 r4.w}};
}

static inline MATH_Mat4 MATH_Mat4MinElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.membs[0] = MATH_MinF32(a.membs[0], b.membs[0]);
    a.membs[1] = MATH_MinF32(a.membs[1], b.membs[1]);
    a.membs[2] = MATH_MinF32(a.membs[2], b.membs[2]);
    a.membs[3] = MATH_MinF32(a.membs[3], b.membs[3]);
    a.membs[4] = MATH_MinF32(a.membs[4], b.membs[4]);
    a.membs[5] = MATH_MinF32(a.membs[5], b.membs[5]);
    a.membs[6] = MATH_MinF32(a.membs[6], b.membs[6]);
    a.membs[7] = MATH_MinF32(a.membs[7], b.membs[7]);
    a.membs[8] = MATH_MinF32(a.membs[8], b.membs[8]);
    a.membs[0] = MATH_MinF32(a.membs[8], b.membs[9]);
    a.membs[1] = MATH_MinF32(a.membs[10], b.membs[10]);
    a.membs[2] = MATH_MinF32(a.membs[11], b.membs[11]);
    a.membs[3] = MATH_MinF32(a.membs[12], b.membs[12]);
    a.membs[4] = MATH_MinF32(a.membs[13], b.membs[13]);
    a.membs[5] = MATH_MinF32(a.membs[14], b.membs[14]);
    a.membs[6] = MATH_MinF32(a.membs[15], b.membs[15]);

    return a;
}

static inline MATH_Mat4 MATH_Max4MaxElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.membs[0] = MATH_MaxF32(a.membs[0], b.membs[0]);
    a.membs[1] = MATH_MaxF32(a.membs[1], b.membs[1]);
    a.membs[2] = MATH_MaxF32(a.membs[2], b.membs[2]);
    a.membs[3] = MATH_MaxF32(a.membs[3], b.membs[3]);
    a.membs[4] = MATH_MaxF32(a.membs[4], b.membs[4]);
    a.membs[5] = MATH_MaxF32(a.membs[5], b.membs[5]);
    a.membs[6] = MATH_MaxF32(a.membs[6], b.membs[6]);
    a.membs[7] = MATH_MaxF32(a.membs[7], b.membs[7]);
    a.membs[8] = MATH_MaxF32(a.membs[8], b.membs[8]);
    a.membs[0] = MATH_MaxF32(a.membs[8], b.membs[9]);
    a.membs[1] = MATH_MaxF32(a.membs[10], b.membs[10]);
    a.membs[2] = MATH_MaxF32(a.membs[11], b.membs[11]);
    a.membs[3] = MATH_MaxF32(a.membs[12], b.membs[12]);
    a.membs[4] = MATH_MaxF32(a.membs[13], b.membs[13]);
    a.membs[5] = MATH_MaxF32(a.membs[14], b.membs[14]);
    a.membs[6] = MATH_MaxF32(a.membs[15], b.membs[15]);

    return a;
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_f32 MATH_Mat4Min(MATH_Mat4 a) {
    DT_f32 min1 = MATH_MinF32(a.membs[0], a.membs[1]);
    DT_f32 min2 = MATH_MinF32(a.membs[2], a.membs[3]);
    DT_f32 min3 = MATH_MinF32(a.membs[4], a.membs[5]);
    DT_f32 min4 = MATH_MinF32(a.membs[6], a.membs[7]);
    DT_f32 min5 = MATH_MinF32(a.membs[8], a.membs[9]);
    DT_f32 min6 = MATH_MinF32(a.membs[10], a.membs[11]);
    DT_f32 min7 = MATH_MinF32(a.membs[12], a.membs[13]);
    DT_f32 min8 = MATH_MinF32(a.membs[14], a.membs[15]);

    DT_f32 min9 = MATH_MinF32(min1, min2);
    DT_f32 min10 = MATH_MinF32(min3, min4);
    DT_f32 min11 = MATH_MinF32(min5, min6);
    DT_f32 min12 = MATH_MinF32(min7, min8);

    DT_f32 min13 = MATH_MinF32(min9, min10);
    DT_f32 min14 = MATH_MinF32(min11, min12);

    return MATH_MinF32(min13, min14);
}

static inline DT_f32 MATH_Mat4Max(MATH_Mat4 a) {
    DT_f32 min1 = MATH_MaxF32(a.membs[0], a.membs[1]);
    DT_f32 min2 = MATH_MaxF32(a.membs[2], a.membs[3]);
    DT_f32 min3 = MATH_MaxF32(a.membs[4], a.membs[5]);
    DT_f32 min4 = MATH_MaxF32(a.membs[6], a.membs[7]);
    DT_f32 min5 = MATH_MaxF32(a.membs[8], a.membs[9]);
    DT_f32 min6 = MATH_MaxF32(a.membs[10], a.membs[11]);
    DT_f32 min7 = MATH_MaxF32(a.membs[12], a.membs[13]);
    DT_f32 min8 = MATH_MaxF32(a.membs[14], a.membs[15]);

    DT_f32 min9 = MATH_MaxF32(min1, min2);
    DT_f32 min10 = MATH_MaxF32(min3, min4);
    DT_f32 min11 = MATH_MaxF32(min5, min6);
    DT_f32 min12 = MATH_MaxF32(min7, min8);

    DT_f32 min13 = MATH_MaxF32(min9, min10);
    DT_f32 min14 = MATH_MaxF32(min11, min12);

    return MATH_MaxF32(min13, min14);
}

static inline DT_bool MATH_Mat4Eq(MATH_Mat4 a, MATH_Mat4 b) {
    return (
        DT_bool)((a.membs[0] == b.membs[0]) && (a.membs[1] == b.membs[1]) &&
                 (a.membs[2] == b.membs[2]) && (a.membs[3] == b.membs[3]) &&
                 (a.membs[4] == b.membs[4]) && (a.membs[5] == b.membs[5]) &&
                 (a.membs[6] == b.membs[6]) && (a.membs[7] == b.membs[7]) &&
                 (a.membs[8] == b.membs[8]) && (a.membs[9] == b.membs[9]) &&
                 (a.membs[10] == b.membs[10]) && (a.membs[11] == b.membs[11]) &&
                 (a.membs[12] == b.membs[12]) && (a.membs[12] == b.membs[14]) &&
                 (a.membs[14] == b.membs[14]) && (a.membs[15] == b.membs[15]));
}

static inline DT_bool MATH_Mat4AlmostEq(MATH_Mat4 a, MATH_Mat4 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.membs[0], b.membs[0]) &&
                     MATH_AlmostEqF32(a.membs[1], b.membs[1]) &&
                     MATH_AlmostEqF32(a.membs[2], b.membs[2]) &&
                     MATH_AlmostEqF32(a.membs[3], b.membs[3]) &&
                     MATH_AlmostEqF32(a.membs[4], b.membs[4]) &&
                     MATH_AlmostEqF32(a.membs[5], b.membs[5]) &&
                     MATH_AlmostEqF32(a.membs[6], b.membs[6]) &&
                     MATH_AlmostEqF32(a.membs[7], b.membs[7]) &&
                     MATH_AlmostEqF32(a.membs[8], b.membs[8]) &&
                     MATH_AlmostEqF32(a.membs[9], b.membs[9]) &&
                     MATH_AlmostEqF32(a.membs[10], b.membs[10]) &&
                     MATH_AlmostEqF32(a.membs[11], b.membs[11]) &&
                     MATH_AlmostEqF32(a.membs[12], b.membs[12]) &&
                     MATH_AlmostEqF32(a.membs[13], b.membs[13]) &&
                     MATH_AlmostEqF32(a.membs[14], b.membs[14]) &&
                     MATH_AlmostEqF32(a.membs[15], b.membs[15]));
}

static inline DT_bool MATH_Mat4IsZero(MATH_Mat4 a) {
    return (
        DT_bool)(MATH_IsZeroF32(a.membs[0]) && MATH_IsZeroF32(a.membs[1]) &&
                 MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[3]) &&
                 MATH_IsZeroF32(a.membs[4]) && MATH_IsZeroF32(a.membs[5]) &&
                 MATH_IsZeroF32(a.membs[6]) && MATH_IsZeroF32(a.membs[7]) &&
                 MATH_IsZeroF32(a.membs[8]) && MATH_IsZeroF32(a.membs[9]) &&
                 MATH_IsZeroF32(a.membs[10]) && MATH_IsZeroF32(a.membs[11]) &&
                 MATH_IsZeroF32(a.membs[12]) && MATH_IsZeroF32(a.membs[13]) &&
                 MATH_IsZeroF32(a.membs[14]) && MATH_IsZeroF32(a.membs[15]));
}

static inline DT_bool MATH_Mat4IsNaN(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsNaNF32(a.membs[0]) || MATH_IsNaNF32(a.membs[1]) ||
                     MATH_IsNaNF32(a.membs[2]) || MATH_IsNaNF32(a.membs[3]) ||
                     MATH_IsNaNF32(a.membs[4]) || MATH_IsNaNF32(a.membs[5]) ||
                     MATH_IsNaNF32(a.membs[6]) || MATH_IsNaNF32(a.membs[7]) ||
                     MATH_IsNaNF32(a.membs[8]) || MATH_IsNaNF32(a.membs[9]) ||
                     MATH_IsNaNF32(a.membs[10]) || MATH_IsNaNF32(a.membs[11]) ||
                     MATH_IsNaNF32(a.membs[12]) || MATH_IsNaNF32(a.membs[13]) ||
                     MATH_IsNaNF32(a.membs[14]) || MATH_IsNaNF32(a.membs[15]));
}

static inline DT_bool MATH_Mat4IsInf(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsInfF32(a.membs[0]) || MATH_IsInfF32(a.membs[1]) ||
                     MATH_IsInfF32(a.membs[2]) || MATH_IsInfF32(a.membs[3]) ||
                     MATH_IsInfF32(a.membs[4]) || MATH_IsInfF32(a.membs[5]) ||
                     MATH_IsInfF32(a.membs[6]) || MATH_IsInfF32(a.membs[7]) ||
                     MATH_IsInfF32(a.membs[8]) || MATH_IsInfF32(a.membs[9]) ||
                     MATH_IsInfF32(a.membs[10]) || MATH_IsInfF32(a.membs[11]) ||
                     MATH_IsInfF32(a.membs[12]) || MATH_IsInfF32(a.membs[13]) ||
                     MATH_IsInfF32(a.membs[14]) || MATH_IsInfF32(a.membs[15]));
}

static inline DT_bool MATH_Mat4IsSymmetric(MATH_Mat4 a) {
    return (DT_bool)(MATH_AlmostEqF32(a.membs[1], a.membs[4]) &&
                     MATH_AlmostEqF32(a.membs[2], a.membs[8]) &&
                     MATH_AlmostEqF32(a.membs[3], a.membs[12]) &&
                     MATH_AlmostEqF32(a.membs[6], a.membs[9]) &&
                     MATH_AlmostEqF32(a.membs[7], a.membs[13]) &&
                     MATH_AlmostEqF32(a.membs[11], a.membs[14]));
}

static inline DT_bool MATH_Mat4IsDiagonal(MATH_Mat4 a) {
    return (
        DT_bool)(MATH_IsZeroF32(a.membs[1]) && MATH_IsZeroF32(a.membs[2]) &&
                 MATH_IsZeroF32(a.membs[3]) && MATH_IsZeroF32(a.membs[4]) &&
                 MATH_IsZeroF32(a.membs[6]) && MATH_IsZeroF32(a.membs[7]) &&
                 MATH_IsZeroF32(a.membs[8]) && MATH_IsZeroF32(a.membs[9]) &&
                 MATH_IsZeroF32(a.membs[11]) && MATH_IsZeroF32(a.membs[12]) &&
                 MATH_IsZeroF32(a.membs[13]) && MATH_IsZeroF32(a.membs[14]));
}

static inline DT_bool MATH_Mat4IsIdentity(MATH_Mat4 a) {
    return (
        DT_bool)(MATH_IsZeroF32(a.membs[1]) && MATH_IsZeroF32(a.membs[2]) &&
                 MATH_IsZeroF32(a.membs[3]) && MATH_IsZeroF32(a.membs[4]) &&
                 MATH_IsZeroF32(a.membs[6]) && MATH_IsZeroF32(a.membs[7]) &&
                 MATH_IsZeroF32(a.membs[8]) && MATH_IsZeroF32(a.membs[9]) &&
                 MATH_IsZeroF32(a.membs[11]) && MATH_IsZeroF32(a.membs[12]) &&
                 MATH_IsZeroF32(a.membs[13]) && MATH_IsZeroF32(a.membs[14]) &&
                 MATH_AlmostEqF32(a.membs[0], 1.0f) &&
                 MATH_AlmostEqF32(a.membs[5], 1.0f) &&
                 MATH_AlmostEqF32(a.membs[10], 1.0f) &&
                 MATH_AlmostEqF32(a.membs[15], 1.0f));
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat4IsOrthonormal(MATH_Mat4 a);

/* ----  BASIC OPS  ---- */

static inline MATH_Mat4 MATH_Mat4Abs(MATH_Mat4 a) {
    a.membs[0] = MATH_AbsF32(a.membs[0]);
    a.membs[1] = MATH_AbsF32(a.membs[1]);
    a.membs[2] = MATH_AbsF32(a.membs[2]);
    a.membs[3] = MATH_AbsF32(a.membs[3]);
    a.membs[4] = MATH_AbsF32(a.membs[4]);
    a.membs[5] = MATH_AbsF32(a.membs[5]);
    a.membs[6] = MATH_AbsF32(a.membs[6]);
    a.membs[7] = MATH_AbsF32(a.membs[7]);
    a.membs[8] = MATH_AbsF32(a.membs[8]);
    a.membs[9] = MATH_AbsF32(a.membs[9]);
    a.membs[10] = MATH_AbsF32(a.membs[10]);
    a.membs[11] = MATH_AbsF32(a.membs[11]);
    a.membs[12] = MATH_AbsF32(a.membs[12]);
    a.membs[13] = MATH_AbsF32(a.membs[13]);
    a.membs[14] = MATH_AbsF32(a.membs[14]);
    a.membs[15] = MATH_AbsF32(a.membs[15]);

    return a;
}

static inline MATH_Mat4 MATH_Mat4Sign(MATH_Mat4 a) {
    a.membs[0] = MATH_SignF32(a.membs[0]);
    a.membs[1] = MATH_SignF32(a.membs[1]);
    a.membs[2] = MATH_SignF32(a.membs[2]);
    a.membs[3] = MATH_SignF32(a.membs[3]);
    a.membs[4] = MATH_SignF32(a.membs[4]);
    a.membs[5] = MATH_SignF32(a.membs[5]);
    a.membs[6] = MATH_SignF32(a.membs[6]);
    a.membs[7] = MATH_SignF32(a.membs[7]);
    a.membs[8] = MATH_SignF32(a.membs[8]);
    a.membs[9] = MATH_SignF32(a.membs[9]);
    a.membs[10] = MATH_SignF32(a.membs[10]);
    a.membs[11] = MATH_SignF32(a.membs[11]);
    a.membs[12] = MATH_SignF32(a.membs[12]);
    a.membs[13] = MATH_SignF32(a.membs[13]);
    a.membs[14] = MATH_SignF32(a.membs[14]);
    a.membs[15] = MATH_SignF32(a.membs[15]);

    return a;
}

static inline MATH_Mat4 MATH_Mat4AddElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.membs[0] += b.membs[0];
    a.membs[1] += b.membs[1];
    a.membs[2] += b.membs[2];
    a.membs[3] += b.membs[3];
    a.membs[4] += b.membs[4];
    a.membs[5] += b.membs[5];
    a.membs[6] += b.membs[6];
    a.membs[7] += b.membs[7];
    a.membs[8] += b.membs[8];
    a.membs[9] += b.membs[9];
    a.membs[10] += b.membs[10];
    a.membs[11] += b.membs[11];
    a.membs[12] += b.membs[12];
    a.membs[13] += b.membs[13];
    a.membs[14] += b.membs[14];
    a.membs[15] += b.membs[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4SubElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.membs[0] -= b.membs[0];
    a.membs[1] -= b.membs[1];
    a.membs[2] -= b.membs[2];
    a.membs[3] -= b.membs[3];
    a.membs[4] -= b.membs[4];
    a.membs[5] -= b.membs[5];
    a.membs[6] -= b.membs[6];
    a.membs[7] -= b.membs[7];
    a.membs[8] -= b.membs[8];
    a.membs[9] -= b.membs[9];
    a.membs[10] -= b.membs[10];
    a.membs[11] -= b.membs[11];
    a.membs[12] -= b.membs[12];
    a.membs[13] -= b.membs[13];
    a.membs[14] -= b.membs[14];
    a.membs[15] -= b.membs[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4MulElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.membs[0] *= b.membs[0];
    a.membs[1] *= b.membs[1];
    a.membs[2] *= b.membs[2];
    a.membs[3] *= b.membs[3];
    a.membs[4] *= b.membs[4];
    a.membs[5] *= b.membs[5];
    a.membs[6] *= b.membs[6];
    a.membs[7] *= b.membs[7];
    a.membs[8] *= b.membs[8];
    a.membs[9] *= b.membs[9];
    a.membs[10] *= b.membs[10];
    a.membs[11] *= b.membs[11];
    a.membs[12] *= b.membs[12];
    a.membs[13] *= b.membs[13];
    a.membs[14] *= b.membs[14];
    a.membs[15] *= b.membs[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.membs[0] /= b.membs[0];
    a.membs[1] /= b.membs[1];
    a.membs[2] /= b.membs[2];
    a.membs[3] /= b.membs[3];
    a.membs[4] /= b.membs[4];
    a.membs[5] /= b.membs[5];
    a.membs[6] /= b.membs[6];
    a.membs[7] /= b.membs[7];
    a.membs[8] /= b.membs[8];
    a.membs[9] /= b.membs[9];
    a.membs[10] /= b.membs[10];
    a.membs[11] /= b.membs[11];
    a.membs[12] /= b.membs[12];
    a.membs[13] /= b.membs[13];
    a.membs[14] /= b.membs[14];
    a.membs[15] /= b.membs[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivElemsSafe(MATH_Mat4 a, MATH_Mat4 b,
                                              DT_f32 fallback) {
    a.membs[0] = MATH_SafeDivF32(a.membs[0], b.membs[0], fallback);
    a.membs[1] = MATH_SafeDivF32(a.membs[1], b.membs[1], fallback);
    a.membs[2] = MATH_SafeDivF32(a.membs[2], b.membs[2], fallback);
    a.membs[3] = MATH_SafeDivF32(a.membs[3], b.membs[3], fallback);
    a.membs[4] = MATH_SafeDivF32(a.membs[4], b.membs[4], fallback);
    a.membs[5] = MATH_SafeDivF32(a.membs[5], b.membs[5], fallback);
    a.membs[6] = MATH_SafeDivF32(a.membs[6], b.membs[6], fallback);
    a.membs[7] = MATH_SafeDivF32(a.membs[7], b.membs[7], fallback);
    a.membs[8] = MATH_SafeDivF32(a.membs[8], b.membs[8], fallback);
    a.membs[9] = MATH_SafeDivF32(a.membs[9], b.membs[9], fallback);
    a.membs[10] = MATH_SafeDivF32(a.membs[10], b.membs[10], fallback);
    a.membs[11] = MATH_SafeDivF32(a.membs[11], b.membs[11], fallback);
    a.membs[12] = MATH_SafeDivF32(a.membs[12], b.membs[12], fallback);
    a.membs[13] = MATH_SafeDivF32(a.membs[13], b.membs[13], fallback);
    a.membs[14] = MATH_SafeDivF32(a.membs[14], b.membs[14], fallback);
    a.membs[15] = MATH_SafeDivF32(a.membs[15], b.membs[15], fallback);

    return a;
}

static inline MATH_Mat4 MATH_Mat4AddScalar(MATH_Mat4 a, DT_f32 s) {
    a.membs[0] += s;
    a.membs[1] += s;
    a.membs[2] += s;
    a.membs[3] += s;
    a.membs[4] += s;
    a.membs[5] += s;
    a.membs[6] += s;
    a.membs[7] += s;
    a.membs[8] += s;
    a.membs[9] += s;
    a.membs[10] += s;
    a.membs[11] += s;
    a.membs[12] += s;
    a.membs[13] += s;
    a.membs[14] += s;
    a.membs[15] += s;

    return a;
}

static inline MATH_Mat4 MATH_Mat4SubScalar(MATH_Mat4 a, DT_f32 s) {
    a.membs[0] -= s;
    a.membs[1] -= s;
    a.membs[2] -= s;
    a.membs[3] -= s;
    a.membs[4] -= s;
    a.membs[5] -= s;
    a.membs[6] -= s;
    a.membs[7] -= s;
    a.membs[8] -= s;
    a.membs[9] -= s;
    a.membs[10] -= s;
    a.membs[11] -= s;
    a.membs[12] -= s;
    a.membs[13] -= s;
    a.membs[14] -= s;
    a.membs[15] -= s;

    return a;
}

static inline MATH_Mat4 MATH_Mat4MulScalar(MATH_Mat4 a, DT_f32 s) {
    a.membs[0] *= s;
    a.membs[1] *= s;
    a.membs[2] *= s;
    a.membs[3] *= s;
    a.membs[4] *= s;
    a.membs[5] *= s;
    a.membs[6] *= s;
    a.membs[7] *= s;
    a.membs[8] *= s;
    a.membs[9] *= s;
    a.membs[10] *= s;
    a.membs[11] *= s;
    a.membs[12] *= s;
    a.membs[13] *= s;
    a.membs[14] *= s;
    a.membs[15] *= s;

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivScalar(MATH_Mat4 a, DT_f32 s) {
    DT_f32 inv_scalar = 1.0f / s;

    a.membs[0] *= inv_scalar;
    a.membs[1] *= inv_scalar;
    a.membs[2] *= inv_scalar;
    a.membs[3] *= inv_scalar;
    a.membs[4] *= inv_scalar;
    a.membs[5] *= inv_scalar;
    a.membs[6] *= inv_scalar;
    a.membs[7] *= inv_scalar;
    a.membs[8] *= inv_scalar;
    a.membs[9] *= inv_scalar;
    a.membs[10] *= inv_scalar;
    a.membs[11] *= inv_scalar;
    a.membs[12] *= inv_scalar;
    a.membs[13] *= inv_scalar;
    a.membs[14] *= inv_scalar;
    a.membs[15] *= inv_scalar;

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivScalarSafe(MATH_Mat4 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return MATH_Mat4CreateFillScalar(fallback);
    }

    return MATH_Mat4DivScalar(a, s);
}

static inline MATH_Mat4 MATH_Mat4Neg(MATH_Mat4 a) {
    a.membs[0] = -a.membs[0];
    a.membs[1] = -a.membs[1];
    a.membs[2] = -a.membs[2];
    a.membs[3] = -a.membs[3];
    a.membs[4] = -a.membs[4];
    a.membs[5] = -a.membs[5];
    a.membs[6] = -a.membs[6];
    a.membs[7] = -a.membs[7];
    a.membs[8] = -a.membs[8];
    a.membs[9] = -a.membs[9];
    a.membs[10] = -a.membs[10];
    a.membs[11] = -a.membs[11];
    a.membs[12] = -a.membs[12];
    a.membs[13] = -a.membs[13];
    a.membs[14] = -a.membs[14];
    a.membs[15] = -a.membs[15];

    return a;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Mul(MATH_Mat4 a, MATH_Mat4 b);

static inline MATH_Mat4 MATH_Mat4Outer(MATH_Vec4 u, MATH_Vec4 v) {

    return (MATH_Mat4){
        .membs = {(u.x * v.x), (u.y * v.x), (u.z * v.x), (u.w * v.x),
                  (u.x * v.y), (u.y * v.y), (u.z * v.y), (u.w * v.y),
                  (u.x * v.z), (u.y * v.z), (u.z * v.z), (u.w * v.y),
                  (u.x * v.w), (u.y * v.w), (u.z * v.w), (u.w * v.w)}};
}

static inline MATH_Vec4 MATH_Mat4MulVec4(MATH_Mat4 a, MATH_Vec4 v) {
    return (MATH_Vec4){
        .x = (a.membs[0] * v.x) + (a.membs[4] * v.y) + (a.membs[8] * v.z) +
             (a.membs[12] * v.w),
        .y = (a.membs[1] * v.x) + (a.membs[5] * v.y) + (a.membs[9] * v.z) +
             (a.membs[13] * v.w),
        .z = (a.membs[2] * v.x) + (a.membs[6] * v.y) + (a.membs[10] * v.z) +
             (a.membs[14] * v.w),
        .w = (a.membs[3] * v.x) + (a.membs[7] * v.y) + (a.membs[11] * v.z) +
             (a.membs[15] * v.w),
    };
}

static inline MATH_Vec3 MATH_Mat4MulVec3Projective(MATH_Mat4 a, MATH_Vec3 v) {
    MATH_Vec4 mul = {
        .x = (a.membs[0] * v.x) + (a.membs[4] * v.y) + (a.membs[8] * v.z) +
             (a.membs[12]),
        .y = (a.membs[1] * v.x) + (a.membs[5] * v.y) + (a.membs[9] * v.z) +
             (a.membs[13]),
        .z = (a.membs[2] * v.x) + (a.membs[6] * v.y) + (a.membs[10] * v.z) +
             (a.membs[14]),
        .w = (a.membs[3] * v.x) + (a.membs[7] * v.y) + (a.membs[11] * v.z) +
             (a.membs[15]),
    };
    DT_f32 inv_w = 1 / mul.w;

    return (MATH_Vec3){
        .x = mul.x * inv_w, .y = mul.y * inv_w, .z = mul.z * inv_w};
}

static inline MATH_Vec3 MATH_Mat4MulVec3ProjectiveSafe(MATH_Mat4 a, MATH_Vec3 v,
                                                       MATH_Vec3 fallback) {
    MATH_Vec4 mul = {
        .x = (a.membs[0] * v.x) + (a.membs[4] * v.y) + (a.membs[8] * v.z) +
             (a.membs[12]),
        .y = (a.membs[1] * v.x) + (a.membs[5] * v.y) + (a.membs[9] * v.z) +
             (a.membs[13]),
        .z = (a.membs[2] * v.x) + (a.membs[6] * v.y) + (a.membs[10] * v.z) +
             (a.membs[14]),
        .w = (a.membs[3] * v.x) + (a.membs[7] * v.y) + (a.membs[11] * v.z) +
             (a.membs[15]),
    };
    DT_f32 inv_w = 1 / mul.w;
    if (MATH_IsZeroF32(inv_w)) {
        return fallback;
    }

    return (MATH_Vec3){
        .x = mul.x * inv_w, .y = mul.y * inv_w, .z = mul.z * inv_w};
}

static inline MATH_Vec3 MATH_Mat4MulVec3Dir(MATH_Mat4 a, MATH_Vec3 v) {
    return (MATH_Vec3){
        .x = (a.membs[0] * v.x) + (a.membs[4] * v.y) + (a.membs[8] * v.z),
        .y = (a.membs[1] * v.x) + (a.membs[5] * v.y) + (a.membs[9] * v.z),
        .z = (a.membs[2] * v.x) + (a.membs[6] * v.y) + (a.membs[10] * v.z),
    };
}

static inline DT_f32 MATH_Mat4Trace(MATH_Mat4 a) {
    return a.membs[0] + a.membs[5] + a.membs[10] + a.membs[15];
}

/* ----  ACCESSORS  ---- */

static inline MATH_Vec4 MATH_Mat4GetRow(MATH_Mat4 a, DT_size row) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);

    return (MATH_Vec4){
        .x = a.membs[0 * MATH_MAT4_SIZE + row],
        .y = a.membs[1 * MATH_MAT4_SIZE + row],
        .z = a.membs[2 * MATH_MAT4_SIZE + row],
        .w = a.membs[3 * MATH_MAT4_SIZE + row],
    };
}

static inline MATH_Vec4 MATH_Mat4GetCol(MATH_Mat4 a, DT_size col) {
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    return (MATH_Vec4){
        .x = a.membs[col * MATH_MAT4_SIZE + 0],
        .y = a.membs[col * MATH_MAT4_SIZE + 1],
        .z = a.membs[col * MATH_MAT4_SIZE + 2],
        .w = a.membs[col * MATH_MAT4_SIZE + 3],
    };
}

static inline MATH_Mat4 MATH_Mat4SetRow(MATH_Mat4 a, DT_size row, MATH_Vec4 v) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);

    a.membs[0 * MATH_MAT4_SIZE + row] = v.x;
    a.membs[1 * MATH_MAT4_SIZE + row] = v.y;
    a.membs[2 * MATH_MAT4_SIZE + row] = v.z;
    a.membs[3 * MATH_MAT4_SIZE + row] = v.w;

    return a;
}

static inline MATH_Mat4 MATH_Mat4SetCol(MATH_Mat4 a, DT_size col, MATH_Vec4 v) {
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    a.membs[col * MATH_MAT4_SIZE + 0] = v.x;
    a.membs[col * MATH_MAT4_SIZE + 1] = v.y;
    a.membs[col * MATH_MAT4_SIZE + 2] = v.z;
    a.membs[col * MATH_MAT4_SIZE + 3] = v.w;

    return a;
}

static inline DT_f32 MATH_Mat4GetAt(MATH_Mat4 a, DT_size row, DT_size col) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    return a.membs[col * MATH_MAT4_SIZE + row];
}

static inline MATH_Mat4 MATH_Mat4SetAt(MATH_Mat4 a, DT_size row, DT_size col,
                                       DT_f32 val) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    a.membs[col * MATH_MAT4_SIZE + row] = val;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Transpose(MATH_Mat4 a);
PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Adjoint(MATH_Mat4 a);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat4Det(MATH_Mat4 a);

static inline DT_bool MATH_Mat4IsFlipped(MATH_Mat4 a) {
    return (DT_bool)(MATH_Mat4Det(a) < 0.0f);
}

static inline MATH_Mat4 MATH_Mat4InvOrthonormal(MATH_Mat4 a) {
    return MATH_Mat4Transpose(a);
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Inv(MATH_Mat4 a,
                                              MATH_Mat4 det_zero_fallback);

static inline DT_f32 MATH_Mat4FrobeniusNorm(MATH_Mat4 a) {
    return MATH_SqrtF32(
        (a.membs[0] * a.membs[0]) + (a.membs[1] * a.membs[1]) +
        (a.membs[2] * a.membs[2]) + (a.membs[3] * a.membs[3]) +
        (a.membs[4] * a.membs[4]) + (a.membs[5] * a.membs[5]) +
        (a.membs[6] * a.membs[6]) + (a.membs[7] * a.membs[7]) +
        (a.membs[8] * a.membs[8]) + (a.membs[9] * a.membs[9]) +
        (a.membs[10] * a.membs[10]) + (a.membs[11] * a.membs[11]) +
        (a.membs[12] * a.membs[12]) + (a.membs[13] * a.membs[13]) +
        (a.membs[14] * a.membs[14]) + (a.membs[15] * a.membs[15]));
}

static inline DT_f32 MATH_Mat4FrobeniusNormSq(MATH_Mat4 a) {
    return (a.membs[0] * a.membs[0]) + (a.membs[1] * a.membs[1]) +
           (a.membs[2] * a.membs[2]) + (a.membs[3] * a.membs[3]) +
           (a.membs[4] * a.membs[4]) + (a.membs[5] * a.membs[5]) +
           (a.membs[6] * a.membs[6]) + (a.membs[7] * a.membs[7]) +
           (a.membs[8] * a.membs[8]) + (a.membs[9] * a.membs[9]) +
           (a.membs[10] * a.membs[10]) + (a.membs[11] * a.membs[11]) +
           (a.membs[12] * a.membs[12]) + (a.membs[13] * a.membs[13]) +
           (a.membs[14] * a.membs[14]) + (a.membs[15] * a.membs[15]);
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Orthonormalize(MATH_Mat4 a);

#ifdef __cplusplus
}
#endif

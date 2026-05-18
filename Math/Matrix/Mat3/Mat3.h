#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Defs.h"
#include "Diagnostics/Assert.h"
#include "Math/Vector/Vec2.h"
#include "Math/Vector/Vec3.h"
#include "Utils/Defs.h"

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat3 MATH_Mat3CreateZero(DT_void) { return (MATH_Mat3){0}; }

static inline MATH_Mat3 MATH_Mat3CreateIdentity(DT_void) {
    return (MATH_Mat3){
        .membs = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat3 MATH_Mat3CreateDiagScalar(DT_f32 s) {
    return (MATH_Mat3){.membs = {s, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f, s}};
}

static inline MATH_Mat3 MATH_Mat3CreateFillScalar(DT_f32 s) {
    return (MATH_Mat3){.membs = {s, s, s, s, s, s, s, s, s}};
}

static inline MATH_Mat3 MATH_Mat3CreateFromCols(MATH_Vec3 c1, MATH_Vec3 c2,
                                                MATH_Vec3 c3) {
    return (MATH_Mat3){
        .membs = {c1.x, c1.y, c1.z, c2.x, c2.y, c2.z, c3.x, c3.y, c3.z}};
}

static inline MATH_Mat3 MATH_Mat3CreateFromRows(MATH_Vec3 r1, MATH_Vec3 r2,
                                                MATH_Vec3 r3) {
    return (MATH_Mat3){
        .membs = {r1.x, r2.x, r3.x, r1.y, r2.y, r3.y, r1.z, r2.z, r3.z}};
}

static inline MATH_Mat3 MATH_Mat3MinElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.membs[0] = MATH_MinF32(a.membs[0], b.membs[0]);
    a.membs[1] = MATH_MinF32(a.membs[1], b.membs[1]);
    a.membs[2] = MATH_MinF32(a.membs[2], b.membs[2]);
    a.membs[3] = MATH_MinF32(a.membs[3], b.membs[3]);
    a.membs[4] = MATH_MinF32(a.membs[4], b.membs[4]);
    a.membs[5] = MATH_MinF32(a.membs[5], b.membs[5]);
    a.membs[6] = MATH_MinF32(a.membs[6], b.membs[6]);
    a.membs[7] = MATH_MinF32(a.membs[7], b.membs[7]);
    a.membs[8] = MATH_MinF32(a.membs[8], b.membs[8]);

    return a;
}

static inline MATH_Mat3 MATH_Mat3MaxElems(MATH_Mat3 a, MATH_Mat3 b) {
    a.membs[0] = MATH_MaxF32(a.membs[0], b.membs[0]);
    a.membs[1] = MATH_MaxF32(a.membs[1], b.membs[1]);
    a.membs[2] = MATH_MaxF32(a.membs[2], b.membs[2]);
    a.membs[3] = MATH_MaxF32(a.membs[3], b.membs[3]);
    a.membs[4] = MATH_MaxF32(a.membs[4], b.membs[4]);
    a.membs[5] = MATH_MaxF32(a.membs[5], b.membs[5]);
    a.membs[6] = MATH_MaxF32(a.membs[6], b.membs[6]);
    a.membs[7] = MATH_MaxF32(a.membs[7], b.membs[7]);
    a.membs[8] = MATH_MaxF32(a.membs[8], b.membs[8]);

    return a;
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_f32 MATH_Mat3Min(MATH_Mat3 a) {
    DT_f32 min1 = MATH_MinF32(a.membs[0], a.membs[1]);
    DT_f32 min2 = MATH_MinF32(a.membs[2], a.membs[3]);
    DT_f32 min3 = MATH_MinF32(a.membs[4], a.membs[5]);
    DT_f32 min4 = MATH_MinF32(a.membs[6], a.membs[7]);

    DT_f32 min5 = MATH_MinF32(min1, min2);
    DT_f32 min6 = MATH_MinF32(min3, min4);

    DT_f32 min7 = MATH_MinF32(min5, min6);

    return MATH_MinF32(min7, a.membs[8]);
}

static inline DT_f32 MATH_Mat3Max(MATH_Mat3 a) {
    DT_f32 min1 = MATH_MaxF32(a.membs[0], a.membs[1]);
    DT_f32 min2 = MATH_MaxF32(a.membs[2], a.membs[3]);
    DT_f32 min3 = MATH_MaxF32(a.membs[4], a.membs[5]);
    DT_f32 min4 = MATH_MaxF32(a.membs[6], a.membs[7]);

    DT_f32 min5 = MATH_MaxF32(min1, min2);
    DT_f32 min6 = MATH_MaxF32(min3, min4);

    DT_f32 min7 = MATH_MaxF32(min5, min6);

    return MATH_MaxF32(min7, a.membs[8]);
}

static inline DT_bool MATH_Mat3Eq(MATH_Mat3 a, MATH_Mat3 b) {
    return (DT_bool)((a.membs[0] == b.membs[0]) && (a.membs[1] == b.membs[1]) &&
                     (a.membs[2] == b.membs[2]) && (a.membs[3] == b.membs[3]) &&
                     (a.membs[4] == b.membs[4]) && (a.membs[5] == b.membs[5]) &&
                     (a.membs[6] == b.membs[6]) && (a.membs[7] == b.membs[7]) &&
                     (a.membs[8] == b.membs[8]));
}

static inline DT_bool MATH_Mat3AlmostEq(MATH_Mat3 a, MATH_Mat3 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.membs[0], b.membs[0]) &&
                     MATH_AlmostEqF32(a.membs[1], b.membs[1]) &&
                     MATH_AlmostEqF32(a.membs[2], b.membs[2]) &&
                     MATH_AlmostEqF32(a.membs[3], b.membs[3]) &&
                     MATH_AlmostEqF32(a.membs[4], b.membs[4]) &&
                     MATH_AlmostEqF32(a.membs[5], b.membs[5]) &&
                     MATH_AlmostEqF32(a.membs[6], b.membs[6]) &&
                     MATH_AlmostEqF32(a.membs[7], b.membs[7]) &&
                     MATH_AlmostEqF32(a.membs[8], b.membs[8]));
}

static inline DT_bool MATH_Mat3IsZero(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[0]) && MATH_IsZeroF32(a.membs[1]) &&
                     MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[3]) &&
                     MATH_IsZeroF32(a.membs[4]) && MATH_IsZeroF32(a.membs[5]) &&
                     MATH_IsZeroF32(a.membs[6]) && MATH_IsZeroF32(a.membs[7]) &&
                     MATH_IsZeroF32(a.membs[8]));
}

static inline DT_bool MATH_Mat3IsNaN(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsNaNF32(a.membs[0]) || MATH_IsNaNF32(a.membs[1]) ||
                     MATH_IsNaNF32(a.membs[2]) || MATH_IsNaNF32(a.membs[3]) ||
                     MATH_IsNaNF32(a.membs[4]) || MATH_IsNaNF32(a.membs[5]) ||
                     MATH_IsNaNF32(a.membs[6]) || MATH_IsNaNF32(a.membs[7]) ||
                     MATH_IsNaNF32(a.membs[8]));
}

static inline DT_bool MATH_Mat3IsInf(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsInfF32(a.membs[0]) || MATH_IsInfF32(a.membs[1]) ||
                     MATH_IsInfF32(a.membs[2]) || MATH_IsInfF32(a.membs[3]) ||
                     MATH_IsInfF32(a.membs[4]) || MATH_IsInfF32(a.membs[5]) ||
                     MATH_IsInfF32(a.membs[6]) || MATH_IsInfF32(a.membs[7]) ||
                     MATH_IsInfF32(a.membs[8]));
}

static inline DT_bool MATH_Mat3IsSymmetric(MATH_Mat3 a) {
    return (DT_bool)(MATH_AlmostEqF32(a.membs[1], a.membs[3]) &&
                     MATH_AlmostEqF32(a.membs[2], a.membs[6]) &&
                     MATH_AlmostEqF32(a.membs[5], a.membs[7]));
}

static inline DT_bool MATH_Mat3IsDiagonal(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[1]) && MATH_IsZeroF32(a.membs[3]) &&
                     MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[6]) &&
                     MATH_IsZeroF32(a.membs[5]) && MATH_IsZeroF32(a.membs[7]));
}

static inline DT_bool MATH_Mat3IsIdentity(MATH_Mat3 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[1]) && MATH_IsZeroF32(a.membs[3]) &&
                     MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[6]) &&
                     MATH_IsZeroF32(a.membs[5]) && MATH_IsZeroF32(a.membs[7]) &&
                     MATH_AlmostEqF32(a.membs[0], 1.0f) &&
                     MATH_AlmostEqF32(a.membs[4], 1.0f) &&
                     MATH_AlmostEqF32(a.membs[8], 1.0f));
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat3IsOrthonormal(MATH_Mat3 a);

/* ----  BASIC OPS  ---- */

#define MAT3_INTERNAL_DEFINE_UNARY_OP(op_name, op)                             \
    static inline MATH_Mat3 MATH_Mat3##op_name(MATH_Mat3 a) {                  \
        a.membs[0] = op(a.membs[0]);                                           \
        a.membs[1] = op(a.membs[1]);                                           \
        a.membs[2] = op(a.membs[2]);                                           \
        a.membs[3] = op(a.membs[3]);                                           \
        a.membs[4] = op(a.membs[4]);                                           \
        a.membs[5] = op(a.membs[5]);                                           \
        a.membs[6] = op(a.membs[6]);                                           \
        a.membs[7] = op(a.membs[7]);                                           \
        a.membs[8] = op(a.membs[8]);                                           \
                                                                               \
        return a;                                                              \
    }

MAT3_INTERNAL_DEFINE_UNARY_OP(Abs, MATH_AbsF32);
MAT3_INTERNAL_DEFINE_UNARY_OP(Sign, MATH_SignF32);
MAT3_INTERNAL_DEFINE_UNARY_OP(Neg, -);

#undef MAT3_INTERNAL_DEFINE_UNARY_OP

#define MAT3_INTERNAL_DEFINE_ELEMS_OP(op_name, op)                             \
    static inline MATH_Mat3 MATH_Mat3Elems##op_name(MATH_Mat3 a,               \
                                                    MATH_Mat3 b) {             \
        a.membs[0] = (a.membs[0])op(b.membs[0]);                               \
        a.membs[1] = (a.membs[1])op(b.membs[1]);                               \
        a.membs[2] = (a.membs[2])op(b.membs[2]);                               \
        a.membs[3] = (a.membs[3])op(b.membs[3]);                               \
        a.membs[4] = (a.membs[4])op(b.membs[4]);                               \
        a.membs[5] = (a.membs[5])op(b.membs[5]);                               \
        a.membs[6] = (a.membs[6])op(b.membs[6]);                               \
        a.membs[7] = (a.membs[7])op(b.membs[7]);                               \
        a.membs[8] = (a.membs[8])op(b.membs[8]);                               \
                                                                               \
        return a;                                                              \
    }

MAT3_INTERNAL_DEFINE_ELEMS_OP(Add, +);
MAT3_INTERNAL_DEFINE_ELEMS_OP(Sub, -);
MAT3_INTERNAL_DEFINE_ELEMS_OP(Mul, *);
MAT3_INTERNAL_DEFINE_ELEMS_OP(Div, /);

#undef MAT3_INTERNAL_DEFINE_ELEMS_OP

static inline MATH_Mat3 MATH_Mat3ElemsDivSafe(MATH_Mat3 a, MATH_Mat3 b,
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

    return a;
}

#define MAT3_INTERNAL_DEFINE_SCALAR_OP(op_name, op)                            \
    static inline MATH_Mat3 MATH_Mat3Scalar##op_name(MATH_Mat3 a, DT_f32 s) {  \
        a.membs[0] = (a.membs[0])op(s);                                        \
        a.membs[1] = (a.membs[1])op(s);                                        \
        a.membs[2] = (a.membs[2])op(s);                                        \
        a.membs[3] = (a.membs[3])op(s);                                        \
        a.membs[4] = (a.membs[4])op(s);                                        \
        a.membs[5] = (a.membs[5])op(s);                                        \
        a.membs[6] = (a.membs[6])op(s);                                        \
        a.membs[7] = (a.membs[7])op(s);                                        \
        a.membs[8] = (a.membs[8])op(s);                                        \
                                                                               \
        return a;                                                              \
    }

MAT3_INTERNAL_DEFINE_SCALAR_OP(Add, +);
MAT3_INTERNAL_DEFINE_SCALAR_OP(Sub, -);
MAT3_INTERNAL_DEFINE_SCALAR_OP(Mul, *);
MAT3_INTERNAL_DEFINE_SCALAR_OP(Div, /);

#undef MAT3_INTERNAL_DEFINE_SCALAR_OP

static inline MATH_Mat3 MATH_Mat3ScalarDivSafe(MATH_Mat3 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return MATH_Mat3CreateFillScalar(fallback);
    }

    return MATH_Mat3ScalarDiv(a, s);
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Mul(MATH_Mat3 a, MATH_Mat3 b);

static inline MATH_Mat3 MATH_Mat3Outer(MATH_Vec3 u, MATH_Vec3 v) {
    return (MATH_Mat3){.membs = {(u.x * v.x), (u.y * v.x), (u.z * v.x),
                                 (u.x * v.y), (u.y * v.y), (u.z * v.y),
                                 (u.x * v.z), (u.y * v.z), (u.z * v.z)}};
}

static inline MATH_Vec3 MATH_Mat3MulVec3(MATH_Mat3 a, MATH_Vec3 v) {
    return (MATH_Vec3){
        .x = (a.membs[0] * v.x) + (a.membs[3] * v.y) + (a.membs[6] * v.z),
        .y = (a.membs[1] * v.x) + (a.membs[4] * v.y) + (a.membs[7] * v.z),
        .z = (a.membs[2] * v.x) + (a.membs[5] * v.y) + (a.membs[8] * v.z),
    };
}

static inline MATH_Vec2 MATH_Mat3MulVec2Projective(MATH_Mat3 a, MATH_Vec2 v) {
    MATH_Vec3 mul = {
        .x = (a.membs[0] * v.x) + (a.membs[3] * v.y) + (a.membs[6]),
        .y = (a.membs[1] * v.x) + (a.membs[4] * v.y) + (a.membs[7]),
        .z = (a.membs[2] * v.x) + (a.membs[5] * v.y) + (a.membs[8]),
    };
    DT_f32 inv_z = 1 / mul.z;

    return (MATH_Vec2){.x = mul.x * inv_z, .y = mul.y * inv_z};
}

static inline MATH_Vec2 MATH_Mat3MulVec2ProjectiveSafe(MATH_Mat3 a, MATH_Vec2 v,
                                                       MATH_Vec2 fallback) {
    MATH_Vec3 mul = {
        .x = (a.membs[0] * v.x) + (a.membs[3] * v.y) + (a.membs[6]),
        .y = (a.membs[1] * v.x) + (a.membs[4] * v.y) + (a.membs[7]),
        .z = (a.membs[2] * v.x) + (a.membs[5] * v.y) + (a.membs[8]),
    };
    if (MATH_IsZeroF32(mul.z)) {
        return fallback;
    }
    DT_f32 inv_z = 1 / mul.z;

    return (MATH_Vec2){.x = mul.x * inv_z, .y = mul.y * inv_z};
}

static inline MATH_Vec2 MATH_Mat3MulVec2Dir(MATH_Mat3 a, MATH_Vec2 v) {
    return (MATH_Vec2){
        .x = (a.membs[0] * v.x) + (a.membs[3] * v.y),
        .y = (a.membs[1] * v.x) + (a.membs[4] * v.y),
    };
}

static inline DT_f32 MATH_Mat3Trace(MATH_Mat3 a) {
    return a.membs[0] + a.membs[4] + a.membs[8];
}

/* ----  ACCESSORS  ---- */

static inline MATH_Vec3 MATH_Mat3GetRow(MATH_Mat3 a, DT_size row) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);

    return (MATH_Vec3){
        .x = a.membs[0 * MATH_MAT3_SIZE + row],
        .y = a.membs[1 * MATH_MAT3_SIZE + row],
        .z = a.membs[2 * MATH_MAT3_SIZE + row],
    };
}

static inline MATH_Vec3 MATH_Mat3GetCol(MATH_Mat3 a, DT_size col) {
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    return (MATH_Vec3){
        .x = a.membs[col * MATH_MAT3_SIZE + 0],
        .y = a.membs[col * MATH_MAT3_SIZE + 1],
        .z = a.membs[col * MATH_MAT3_SIZE + 2],
    };
}

static inline MATH_Mat3 MATH_Mat3SetRow(MATH_Mat3 a, DT_size row, MATH_Vec3 v) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);

    a.membs[0 * MATH_MAT3_SIZE + row] = v.x;
    a.membs[1 * MATH_MAT3_SIZE + row] = v.y;
    a.membs[2 * MATH_MAT3_SIZE + row] = v.z;

    return a;
}

static inline MATH_Mat3 MATH_Mat3SetCol(MATH_Mat3 a, DT_size col, MATH_Vec3 v) {
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    a.membs[col * MATH_MAT3_SIZE + 0] = v.x;
    a.membs[col * MATH_MAT3_SIZE + 1] = v.y;
    a.membs[col * MATH_MAT3_SIZE + 2] = v.z;

    return a;
}

static inline DT_f32 MATH_Mat3GetAt(MATH_Mat3 a, DT_size row, DT_size col) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    return a.membs[col * MATH_MAT3_SIZE + row];
}

static inline MATH_Mat3 MATH_Mat3SetAt(MATH_Mat3 a, DT_size row, DT_size col,
                                       DT_f32 val) {
    DIAG_ASSERT(row < MATH_MAT3_SIZE);
    DIAG_ASSERT(col < MATH_MAT3_SIZE);

    a.membs[col * MATH_MAT3_SIZE + row] = val;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Transpose(MATH_Mat3 a);
PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Adjoint(MATH_Mat3 a);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat3Det(MATH_Mat3 a);

static inline DT_bool MATH_Mat3IsFlipped(MATH_Mat3 a) {
    return (DT_bool)(MATH_Mat3Det(a) < 0.0f);
}

static inline MATH_Mat3 MATH_Mat3InvOrthonormal(MATH_Mat3 a) {
    return MATH_Mat3Transpose(a);
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Inv(MATH_Mat3 a,
                                              MATH_Mat3 det_zero_fallback);

static inline DT_f32 MATH_Mat3FrobeniusNorm(MATH_Mat3 a) {
    return MATH_SqrtF32((a.membs[0] * a.membs[0]) + (a.membs[1] * a.membs[1]) +
                        (a.membs[2] * a.membs[2]) + (a.membs[3] * a.membs[3]) +
                        (a.membs[4] * a.membs[4]) + (a.membs[5] * a.membs[5]) +
                        (a.membs[6] * a.membs[6]) + (a.membs[7] * a.membs[7]) +
                        (a.membs[8] * a.membs[8]));
}

static inline DT_f32 MATH_Mat3FrobeniusNormSq(MATH_Mat3 a) {
    return (a.membs[0] * a.membs[0]) + (a.membs[1] * a.membs[1]) +
           (a.membs[2] * a.membs[2]) + (a.membs[3] * a.membs[3]) +
           (a.membs[4] * a.membs[4]) + (a.membs[5] * a.membs[5]) +
           (a.membs[6] * a.membs[6]) + (a.membs[7] * a.membs[7]) +
           (a.membs[8] * a.membs[8]);
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Orthonormalize(MATH_Mat3 a);
PRP_FN_API MATH_Mat3 PRP_FN_CALL
MATH_Mat3OrthonormalizeSafe(MATH_Mat3 a, MATH_Mat3 fallback);

#ifdef __cplusplus
}
#endif

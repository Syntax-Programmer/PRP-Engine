#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../Diagnostics/Assert.h"
#include "../../../Utils/Defs.h"
#include "../../Vector/Vec2.h"
#include "Defs.h"

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat2 MATH_Mat2CreateZero(DT_void) { return (MATH_Mat2){0}; }

static inline MATH_Mat2 MATH_Mat2CreateIdentity(DT_void) {
    return (MATH_Mat2){.membs = {1.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat2 MATH_Mat2CreateDiagScalar(DT_f32 s) {
    return (MATH_Mat2){.membs = {s, 0.0f, 0.0f, s}};
}

static inline MATH_Mat2 MATH_Mat2CreateFillScalar(DT_f32 s) {
    return (MATH_Mat2){.membs = {s, s, s, s}};
}

static inline MATH_Mat2 MATH_Mat2CreateFromCols(MATH_Vec2 c1, MATH_Vec2 c2) {
    return (MATH_Mat2){.membs = {c1.x, c1.y, c2.x, c2.y}};
}

static inline MATH_Mat2 MATH_Mat2CreateFromRows(MATH_Vec2 r1, MATH_Vec2 r2) {
    return (MATH_Mat2){.membs = {r1.x, r2.x, r1.y, r2.y}};
}

static inline MATH_Mat2 MATH_Mat2MinElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.membs[0] = MATH_MinF32(a.membs[0], b.membs[0]);
    a.membs[1] = MATH_MinF32(a.membs[1], b.membs[1]);
    a.membs[2] = MATH_MinF32(a.membs[2], b.membs[2]);
    a.membs[3] = MATH_MinF32(a.membs[3], b.membs[3]);

    return a;
}

static inline MATH_Mat2 MATH_Mat2MaxElems(MATH_Mat2 a, MATH_Mat2 b) {
    a.membs[0] = MATH_MaxF32(a.membs[0], b.membs[0]);
    a.membs[1] = MATH_MaxF32(a.membs[1], b.membs[1]);
    a.membs[2] = MATH_MaxF32(a.membs[2], b.membs[2]);
    a.membs[3] = MATH_MaxF32(a.membs[3], b.membs[3]);

    return a;
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_f32 MATH_Mat2Min(MATH_Mat2 a) {
    DT_f32 min1 = MATH_MinF32(a.membs[0], a.membs[1]);
    DT_f32 min2 = MATH_MinF32(a.membs[2], a.membs[3]);

    return MATH_MinF32(min1, min2);
}

static inline DT_f32 MATH_Mat2Max(MATH_Mat2 a) {
    DT_f32 min1 = MATH_MaxF32(a.membs[0], a.membs[1]);
    DT_f32 min2 = MATH_MaxF32(a.membs[2], a.membs[3]);

    return MATH_MaxF32(min1, min2);
}

static inline DT_bool MATH_Mat2Eq(MATH_Mat2 a, MATH_Mat2 b) {
    return (DT_bool)((a.membs[0] == b.membs[0]) && (a.membs[1] == b.membs[1]) &&
                     (a.membs[2] == b.membs[2]) && (a.membs[3] == b.membs[3]));
}

static inline DT_bool MATH_Mat2AlmostEq(MATH_Mat2 a, MATH_Mat2 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.membs[0], b.membs[0]) &&
                     MATH_AlmostEqF32(a.membs[1], b.membs[1]) &&
                     MATH_AlmostEqF32(a.membs[2], b.membs[2]) &&
                     MATH_AlmostEqF32(a.membs[3], b.membs[3]));
}

static inline DT_bool MATH_Mat2IsZero(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[0]) && MATH_IsZeroF32(a.membs[1]) &&
                     MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[3]));
}

static inline DT_bool MATH_Mat2IsNaN(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsNaNF32(a.membs[0]) || MATH_IsNaNF32(a.membs[1]) ||
                     MATH_IsNaNF32(a.membs[2]) || MATH_IsNaNF32(a.membs[3]));
}

static inline DT_bool MATH_Mat2IsInf(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsInfF32(a.membs[0]) || MATH_IsInfF32(a.membs[1]) ||
                     MATH_IsInfF32(a.membs[2]) || MATH_IsInfF32(a.membs[3]));
}

static inline DT_bool MATH_Mat2IsSymmetric(MATH_Mat2 a) {
    return MATH_AlmostEqF32(a.membs[1], a.membs[2]);
}

static inline DT_bool MATH_Mat2IsDiagonal(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[1]) && MATH_IsZeroF32(a.membs[2]));
}

static inline DT_bool MATH_Mat2IsIdentity(MATH_Mat2 a) {
    return (DT_bool)(MATH_IsZeroF32(a.membs[1]) && MATH_IsZeroF32(a.membs[2]) &&
                     MATH_AlmostEqF32(a.membs[0], 1.0f) &&
                     MATH_AlmostEqF32(a.membs[3], 1.0f));
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat2IsOrthonormal(MATH_Mat2 a);

/* ----  BASIC OPS  ---- */

#define MAT2_INTERNAL_DEFINE_UNARY_OP(op_name, op)                             \
    static inline MATH_Mat2 MATH_Mat2##op_name(MATH_Mat2 a) {                  \
        a.membs[0] = op(a.membs[0]);                                           \
        a.membs[1] = op(a.membs[1]);                                           \
        a.membs[2] = op(a.membs[2]);                                           \
        a.membs[3] = op(a.membs[3]);                                           \
                                                                               \
        return a;                                                              \
    }

MAT2_INTERNAL_DEFINE_UNARY_OP(Abs, MATH_AbsF32);
MAT2_INTERNAL_DEFINE_UNARY_OP(Sign, MATH_SignF32);
MAT2_INTERNAL_DEFINE_UNARY_OP(Neg, -);

#undef MAT2_INTERNAL_DEFINE_UNARY_OP

#define MAT2_INTERNAL_DEFINE_ELEMS_OP(op_name, op)                             \
    static inline MATH_Mat2 MATH_Mat2Elems##op_name(MATH_Mat2 a,               \
                                                    MATH_Mat2 b) {             \
        a.membs[0] = (a.membs[0])op(b.membs[0]);                               \
        a.membs[1] = (a.membs[1])op(b.membs[1]);                               \
        a.membs[2] = (a.membs[2])op(b.membs[2]);                               \
        a.membs[3] = (a.membs[3])op(b.membs[3]);                               \
                                                                               \
        return a;                                                              \
    }

MAT2_INTERNAL_DEFINE_ELEMS_OP(Add, +);
MAT2_INTERNAL_DEFINE_ELEMS_OP(Sub, -);
MAT2_INTERNAL_DEFINE_ELEMS_OP(Mul, *);
MAT2_INTERNAL_DEFINE_ELEMS_OP(Div, /);

#undef MAT2_INTERNAL_DEFINE_ELEMS_OP

static inline MATH_Mat2 MATH_Mat2ElemsDivSafe(MATH_Mat2 a, MATH_Mat2 b,
                                              DT_f32 fallback) {
    a.membs[0] = MATH_SafeDivF32(a.membs[0], b.membs[0], fallback);
    a.membs[1] = MATH_SafeDivF32(a.membs[1], b.membs[1], fallback);
    a.membs[2] = MATH_SafeDivF32(a.membs[2], b.membs[2], fallback);
    a.membs[3] = MATH_SafeDivF32(a.membs[3], b.membs[3], fallback);

    return a;
}

#define MAT2_INTERNAL_DEFINE_SCALAR_OP(op_name, op)                            \
    static inline MATH_Mat2 MATH_Mat2Scalar##op_name(MATH_Mat2 a, DT_f32 s) {  \
        a.membs[0] = (a.membs[0])op(s);                                        \
        a.membs[1] = (a.membs[1])op(s);                                        \
        a.membs[2] = (a.membs[2])op(s);                                        \
        a.membs[3] = (a.membs[3])op(s);                                        \
                                                                               \
        return a;                                                              \
    }

MAT2_INTERNAL_DEFINE_SCALAR_OP(Add, +);
MAT2_INTERNAL_DEFINE_SCALAR_OP(Sub, -);
MAT2_INTERNAL_DEFINE_SCALAR_OP(Mul, *);
MAT2_INTERNAL_DEFINE_SCALAR_OP(Div, /);

#undef MAT2_INTERNAL_DEFINE_SCALAR_OP

static inline MATH_Mat2 MATH_Mat2ScalarDivSafe(MATH_Mat2 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return MATH_Mat2CreateFillScalar(fallback);
    }

    return MATH_Mat2ScalarDiv(a, s);
}

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2Mul(MATH_Mat2 a, MATH_Mat2 b);

static inline MATH_Mat2 MATH_Mat2Outer(MATH_Vec2 u, MATH_Vec2 v) {
    return (MATH_Mat2){.membs = {u.x * v.x, u.y * v.x, u.x * v.y, u.y * v.y}};
}

static inline MATH_Vec2 MATH_Mat2MulVec2(MATH_Mat2 a, MATH_Vec2 v) {
    return (MATH_Vec2){
        .x = (a.membs[0] * v.x) + (a.membs[2] * v.y),
        .y = (a.membs[1] * v.x) + (a.membs[3] * v.y),
    };
}

static inline DT_f32 MATH_Mat2Trace(MATH_Mat2 a) {
    return a.membs[0] + a.membs[3];
}

/* ----  ACCESSORS  ---- */

static inline MATH_Vec2 MATH_Mat2GetRow(MATH_Mat2 a, DT_size row) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);

    return (MATH_Vec2){
        .x = a.membs[0 * MATH_MAT2_SIZE + row],
        .y = a.membs[1 * MATH_MAT2_SIZE + row],
    };
}

static inline MATH_Vec2 MATH_Mat2GetCol(MATH_Mat2 a, DT_size col) {
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    return (MATH_Vec2){
        .x = a.membs[col * MATH_MAT2_SIZE + 0],
        .y = a.membs[col * MATH_MAT2_SIZE + 1],
    };
}

static inline MATH_Mat2 MATH_Mat2SetRow(MATH_Mat2 a, DT_size row, MATH_Vec2 v) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);

    a.membs[0 * MATH_MAT2_SIZE + row] = v.x;
    a.membs[1 * MATH_MAT2_SIZE + row] = v.y;

    return a;
}

static inline MATH_Mat2 MATH_Mat2SetCol(MATH_Mat2 a, DT_size col, MATH_Vec2 v) {
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    a.membs[col * MATH_MAT2_SIZE + 0] = v.x;
    a.membs[col * MATH_MAT2_SIZE + 1] = v.y;

    return a;
}

static inline DT_f32 MATH_Mat2GetAt(MATH_Mat2 a, DT_size row, DT_size col) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    return a.membs[col * MATH_MAT2_SIZE + row];
}

static inline MATH_Mat2 MATH_Mat2SetAt(MATH_Mat2 a, DT_size row, DT_size col,
                                       DT_f32 val) {
    DIAG_ASSERT(row < MATH_MAT2_SIZE);
    DIAG_ASSERT(col < MATH_MAT2_SIZE);

    a.membs[col * MATH_MAT2_SIZE + row] = val;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

static inline MATH_Mat2 MATH_Mat2Transpose(MATH_Mat2 a) {
    DT_f32 a01 = a.membs[2];
    a.membs[2] = a.membs[1];
    a.membs[1] = a01;

    return a;
}

static inline MATH_Mat2 MATH_Mat2Adjoint(MATH_Mat2 a) {
    return (MATH_Mat2){
        .membs = {a.membs[3], -a.membs[1], -a.membs[2], a.membs[0]}};
}

static inline DT_f32 MATH_Mat2Det(MATH_Mat2 a) {
    DT_f32 a00 = a.membs[0];
    DT_f32 a10 = a.membs[1];
    DT_f32 a01 = a.membs[2];
    DT_f32 a11 = a.membs[3];

    return (a00 * a11) - (a01 * a10);
}

static inline DT_bool MATH_Mat2IsFlipped(MATH_Mat2 a) {
    return (DT_bool)(MATH_Mat2Det(a) < 0.0f);
}

static inline MATH_Mat2 MATH_Mat2InvOrthonormal(MATH_Mat2 a) {
    return MATH_Mat2Transpose(a);
}

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2Inv(MATH_Mat2 a,
                                              MATH_Mat2 det_zero_fallback);

static inline DT_f32 MATH_Mat2FrobeniusNorm(MATH_Mat2 a) {
    return MATH_SqrtF32((a.membs[0] * a.membs[0]) + (a.membs[1] * a.membs[1]) +
                        (a.membs[2] * a.membs[2]) + (a.membs[3] * a.membs[3]));
}

static inline DT_f32 MATH_Mat2FrobeniusNormSq(MATH_Mat2 a) {
    return (a.membs[0] * a.membs[0]) + (a.membs[1] * a.membs[1]) +
           (a.membs[2] * a.membs[2]) + (a.membs[3] * a.membs[3]);
}

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2Orthonormalize(MATH_Mat2 a);
PRP_FN_API MATH_Mat2 PRP_FN_CALL
MATH_Mat2OrthonormalizeSafe(MATH_Mat2 a, MATH_Mat2 fallback);

#ifdef __cplusplus
}
#endif

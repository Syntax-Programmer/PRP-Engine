#include "Mat2.h"
#include "Mat2-Affine.h"

/* ----  COMPARE FUNCTIONS  ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat2IsOrthonormal(MATH_Mat2 a) {
    MATH_Vec2 c0 = {.x = a.membs[0], .y = a.membs[1]};
    MATH_Vec2 c1 = {.x = a.membs[2], .y = a.membs[3]};
    DT_f32 dot = MATH_Vec2Dot(c0, c1);
    DT_f32 len0 = MATH_Vec2Len(c0);
    DT_f32 len1 = MATH_Vec2Len(c1);

    return (DT_bool)(MATH_IsZeroF32(dot) && MATH_AlmostEqF32(len0, 1.0f) &&
                     MATH_AlmostEqF32(len1, 1.0f));
}

/* ----  BASIC OPS  ---- */

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2Mul(MATH_Mat2 a, MATH_Mat2 b) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1];
    DT_f32 a01 = a.membs[2], a11 = a.membs[3];

    DT_f32 b00 = b.membs[0], b10 = b.membs[1];
    DT_f32 b01 = b.membs[2], b11 = b.membs[3];

    // Unrolled mat mul calculation.
    a.membs[0] = a00 * b00 + a01 * b10;
    a.membs[1] = a10 * b00 + a11 * b10;
    a.membs[2] = a00 * b01 + a01 * b11;
    a.membs[3] = a10 * b01 + a11 * b11;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2Inv(MATH_Mat2 a,
                                              MATH_Mat2 det_zero_fallback) {
    DT_f32 a00 = a.membs[0];
    DT_f32 a10 = a.membs[1];
    DT_f32 a01 = a.membs[2];
    DT_f32 a11 = a.membs[3];

    // Determinant calculation
    DT_f32 det = a00 * a11 - a01 * a10;
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    // Adjoint and inverse calculation together
    a.membs[0] = a11 * inv_det;
    a.membs[1] = -a10 * inv_det;
    a.membs[2] = -a01 * inv_det;
    a.membs[3] = a00 * inv_det;

    return a;
}

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2Orthonormalize(MATH_Mat2 a) {
    MATH_Vec2 c0 = {a.membs[0], a.membs[1]};
    MATH_Vec2 c1 = {a.membs[2], a.membs[3]};
    c0 = MATH_Vec2Normalize(c0);

    DT_f32 proj = MATH_Vec2Dot(c1, c0);
    c1.x -= c0.x * proj;
    c1.y -= c0.y * proj;

    c1 = MATH_Vec2Normalize(c1);

    a.membs[0] = c0.x;
    a.membs[1] = c0.y;
    a.membs[2] = c1.x;
    a.membs[3] = c1.y;

    return a;
}

/* ----  AFFINE ALGEBRAIC EXTRACTIONS  ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat2ExtractRotation(MATH_Mat2 a) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1];
    MATH_Vec2 c0 = {.x = a00, .y = a10};

    DT_f32 len = MATH_Vec2Len(c0);
    if (MATH_IsZeroF32(len))
        return 0.0f;
    DT_f32 inv_len = 1.0f / len;

    DT_f32 x = a00 * inv_len;
    DT_f32 y = a10 * inv_len;

    return MATH_ATan2F32(y, x);
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Mat2ExtractScale(MATH_Mat2 a) {
    MATH_Vec2 c0 = {.x = a.membs[0], .y = a.membs[1]};
    MATH_Vec2 c1 = {.x = a.membs[2], .y = a.membs[3]};

    return (MATH_Vec2){
        .x = MATH_Vec2Len(c0),
        .y = MATH_Vec2Len(c1),
    };
}

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2NormBasis(MATH_Mat2 a) {
    MATH_Vec2 s = MATH_Mat2ExtractScale(a);
    if (!MATH_IsZeroF32(s.x)) {
        a.membs[0] /= s.x;
        a.membs[1] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.membs[2] /= s.y;
        a.membs[3] /= s.y;
    }

    return a;
}

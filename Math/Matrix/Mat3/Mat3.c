#include "Mat3.h"
#include "Mat3-Affine.h"

/* ----  COMPARE FUNCTIONS  ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat3IsOrthonormal(MATH_Mat3 a) {
    MATH_Vec3 c0 = {.x = a.membs[0], .y = a.membs[1], .z = a.membs[2]};
    MATH_Vec3 c1 = {.x = a.membs[3], .y = a.membs[4], .z = a.membs[5]};
    MATH_Vec3 c2 = {.x = a.membs[6], .y = a.membs[7], .z = a.membs[8]};

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

/* ----  BASIC OPS  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Mul(MATH_Mat3 a, MATH_Mat3 b) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1], a20 = a.membs[2];
    DT_f32 a01 = a.membs[3], a11 = a.membs[4], a21 = a.membs[5];
    DT_f32 a02 = a.membs[6], a12 = a.membs[7], a22 = a.membs[8];

    DT_f32 b00 = b.membs[0], b10 = b.membs[1], b20 = b.membs[2];
    DT_f32 b01 = b.membs[3], b11 = b.membs[4], b21 = b.membs[5];
    DT_f32 b02 = b.membs[6], b12 = b.membs[7], b22 = b.membs[8];

    a.membs[0] = (a00 * b00) + (a01 * b10) + (a02 * b20);
    a.membs[1] = (a10 * b00) + (a11 * b10) + (a12 * b20);
    a.membs[2] = (a20 * b00) + (a21 * b10) + (a22 * b20);

    a.membs[3] = (a00 * b01) + (a01 * b11) + (a02 * b21);
    a.membs[4] = (a10 * b01) + (a11 * b11) + (a12 * b21);
    a.membs[5] = (a20 * b01) + (a21 * b11) + (a22 * b21);

    a.membs[6] = (a00 * b02) + (a01 * b12) + (a02 * b22);
    a.membs[7] = (a10 * b02) + (a11 * b12) + (a12 * b22);
    a.membs[8] = (a20 * b02) + (a21 * b12) + (a22 * b22);

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Transpose(MATH_Mat3 a) {
    MATH_Mat3 rslt;

    rslt.membs[0 * MATH_MAT3_SIZE + 0] = a.membs[0 * MATH_MAT3_SIZE + 0];
    rslt.membs[0 * MATH_MAT3_SIZE + 1] = a.membs[1 * MATH_MAT3_SIZE + 0];
    rslt.membs[0 * MATH_MAT3_SIZE + 2] = a.membs[2 * MATH_MAT3_SIZE + 0];
    rslt.membs[1 * MATH_MAT3_SIZE + 0] = a.membs[0 * MATH_MAT3_SIZE + 1];
    rslt.membs[1 * MATH_MAT3_SIZE + 1] = a.membs[1 * MATH_MAT3_SIZE + 1];
    rslt.membs[1 * MATH_MAT3_SIZE + 2] = a.membs[2 * MATH_MAT3_SIZE + 1];
    rslt.membs[2 * MATH_MAT3_SIZE + 0] = a.membs[0 * MATH_MAT3_SIZE + 2];
    rslt.membs[2 * MATH_MAT3_SIZE + 1] = a.membs[1 * MATH_MAT3_SIZE + 2];
    rslt.membs[2 * MATH_MAT3_SIZE + 2] = a.membs[2 * MATH_MAT3_SIZE + 2];

    return rslt;
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Adjoint(MATH_Mat3 a) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1], a20 = a.membs[2];
    DT_f32 a01 = a.membs[3], a11 = a.membs[4], a21 = a.membs[5];
    DT_f32 a02 = a.membs[6], a12 = a.membs[7], a22 = a.membs[8];

    DT_f32 c00 = (a11 * a22 - a12 * a21);
    DT_f32 c01 = -(a10 * a22 - a12 * a20);
    DT_f32 c02 = (a10 * a21 - a11 * a20);
    DT_f32 c10 = -(a01 * a22 - a02 * a21);
    DT_f32 c11 = (a00 * a22 - a02 * a20);
    DT_f32 c12 = -(a00 * a21 - a01 * a20);
    DT_f32 c20 = (a01 * a12 - a02 * a11);
    DT_f32 c21 = -(a00 * a12 - a02 * a10);
    DT_f32 c22 = (a00 * a11 - a01 * a10);

    return (MATH_Mat3){.membs = {c00, c01, c02, c10, c11, c12, c20, c21, c22}};
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat3Det(MATH_Mat3 a) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1], a20 = a.membs[2];
    DT_f32 a01 = a.membs[3], a11 = a.membs[4], a21 = a.membs[5];
    DT_f32 a02 = a.membs[6], a12 = a.membs[7], a22 = a.membs[8];

    DT_f32 m0 = (a11 * a22) - (a12 * a21);
    DT_f32 m1 = (a01 * a22) - (a02 * a21);
    DT_f32 m2 = (a01 * a12) - (a02 * a11);

    return (a00 * m0) - (a10 * m1) + (a20 * m2);
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Inv(MATH_Mat3 a,
                                              MATH_Mat3 det_zero_fallback) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1], a20 = a.membs[2];
    DT_f32 a01 = a.membs[3], a11 = a.membs[4], a21 = a.membs[5];
    DT_f32 a02 = a.membs[6], a12 = a.membs[7], a22 = a.membs[8];

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
    a.membs[0] = c00 * inv_det;
    a.membs[1] = c10 * inv_det;
    a.membs[2] = c20 * inv_det;
    a.membs[3] = c01 * inv_det;
    a.membs[4] = c11 * inv_det;
    a.membs[5] = c21 * inv_det;
    a.membs[6] = c02 * inv_det;
    a.membs[7] = c12 * inv_det;
    a.membs[8] = c22 * inv_det;

    return a;
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3Orthonormalize(MATH_Mat3 a) {
    MATH_Vec3 c0 = {.x = a.membs[0], .y = a.membs[1], .z = a.membs[2]};
    MATH_Vec3 c1 = {.x = a.membs[3], .y = a.membs[4], .z = a.membs[5]};
    MATH_Vec3 c2 = {.x = a.membs[6], .y = a.membs[7], .z = a.membs[8]};

    c0 = MATH_Vec3Normalize(c0);
    // Orthogonalize c1 against c0
    DT_f32 proj10 = MATH_Vec3Dot(c1, c0);
    c1.x -= c0.x * proj10;
    c1.y -= c0.y * proj10;
    c1.z -= c0.z * proj10;

    c1 = MATH_Vec3Normalize(c1);
    // Orthogonalize c2 against c0
    DT_f32 proj20 = MATH_Vec3Dot(c2, c0);
    c2.x -= c0.x * proj20;
    c2.y -= c0.y * proj20;
    c2.z -= c0.z * proj20;

    // Orthogonalize c2 against c1
    DT_f32 proj21 = MATH_Vec3Dot(c2, c1);
    c2.x -= c1.x * proj21;
    c2.y -= c1.y * proj21;
    c2.z -= c1.z * proj21;
    c2 = MATH_Vec3Normalize(c2);

    return (MATH_Mat3){
        .membs = {c0.x, c0.y, c0.z, c1.x, c1.y, c1.z, c2.x, c2.y, c2.z}};
}

/* ----  AFFINE COMPARE FUNCTIONS  ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat3IsAffineOrthonormal(MATH_Mat3 a) {
    MATH_Vec2 c0 = {.x = a.membs[0], .y = a.membs[1]};
    MATH_Vec2 c1 = {.x = a.membs[3], .y = a.membs[4]};

    DT_f32 d01 = MATH_Vec2Dot(c0, c1);

    DT_f32 l0 = MATH_Vec2LenSq(c0);
    DT_f32 l1 = MATH_Vec2LenSq(c1);

    return (DT_bool)(MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[5]) &&
                     MATH_AlmostEqF32(a.membs[8], 1.0f) &&
                     MATH_IsZeroF32(d01) && MATH_AlmostEqF32(l0, 1.0f) &&
                     MATH_AlmostEqF32(l1, 1.0f));
}

/* ----  AFFINE BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL
MATH_Mat3InvAffine(MATH_Mat3 a, MATH_Mat3 det_zero_fallback) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1];
    DT_f32 a01 = a.membs[3], a11 = a.membs[4];
    DT_f32 tx = a.membs[6], ty = a.membs[7];
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

    return (MATH_Mat3){
        .membs = {r00, r10, 0.0f, r01, r11, 0.0f, itx, ity, 1.0f}};
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3InvAffineOrthonormal(MATH_Mat3 a) {
    DT_f32 r00 = a.membs[0], r10 = a.membs[1];
    DT_f32 r01 = a.membs[3], r11 = a.membs[4];
    DT_f32 tx = a.membs[6];
    DT_f32 ty = a.membs[7];

    return (MATH_Mat3){.membs = {r00, r01, 0.0f,

                                 r10, r11, 0.0f,

                                 -((r00 * tx) + (r10 * ty)),
                                 -((r01 * tx) + (r11 * ty)), 1.0f}};
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3AffineOrthonormalize(MATH_Mat3 a) {
    MATH_Vec2 c0 = {a.membs[0], a.membs[1]};
    MATH_Vec2 c1 = {a.membs[3], a.membs[4]};
    c0 = MATH_Vec2Normalize(c0);

    DT_f32 proj = MATH_Vec2Dot(c1, c0);
    c1.x -= c0.x * proj;
    c1.y -= c0.y * proj;

    c1 = MATH_Vec2Normalize(c1);

    a.membs[0] = c0.x;
    a.membs[1] = c0.y;
    a.membs[3] = c1.x;
    a.membs[4] = c1.y;

    return a;
}

/* ----  AFFINE ALGEBRAIC EXTRACTIONS  ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat3ExtractRotation(MATH_Mat3 a) {
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

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Mat3ExtractScale(MATH_Mat3 a) {
    MATH_Vec2 c0 = {.x = a.membs[0], .y = a.membs[1]};
    MATH_Vec2 c1 = {.x = a.membs[3], .y = a.membs[4]};

    return (MATH_Vec2){
        .x = MATH_Vec2Len(c0),
        .y = MATH_Vec2Len(c1),
    };
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3NormBasis(MATH_Mat3 a) {
    MATH_Vec2 s = MATH_Mat3ExtractScale(a);
    if (!MATH_IsZeroF32(s.x)) {
        a.membs[0] /= s.x;
        a.membs[1] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.membs[3] /= s.y;
        a.membs[4] /= s.y;
    }

    return a;
}

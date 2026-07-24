#include "Mat3-Affine.h"

/* ----  COMPARE FUNCTIONS  ---- */

PRP_FN_API PRP_Bool PRP_FN_CALL MATH_Mat3IsAffineOrthonormal(MATH_Mat3 a) {
    MATH_Vec2 c0 = {.x = a.membs[0], .y = a.membs[1]};
    MATH_Vec2 c1 = {.x = a.membs[3], .y = a.membs[4]};

    PRP_F32 d01 = MATH_Vec2Dot(c0, c1);

    PRP_F32 l0 = MATH_Vec2LenSq(c0);
    PRP_F32 l1 = MATH_Vec2LenSq(c1);

    return (
        PRP_Bool)(MATH_IsZeroF32(a.membs[2]) && MATH_IsZeroF32(a.membs[5]) &&
                  MATH_AlmostEqF32(a.membs[8], 1.0f) && MATH_IsZeroF32(d01) &&
                  MATH_AlmostEqF32(l0, 1.0f) && MATH_AlmostEqF32(l1, 1.0f));
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL
MATH_Mat3InvAffine(MATH_Mat3 a, MATH_Mat3 det_zero_fallback) {
    PRP_F32 a00 = a.membs[0], a10 = a.membs[1];
    PRP_F32 a01 = a.membs[3], a11 = a.membs[4];
    PRP_F32 tx = a.membs[6], ty = a.membs[7];
    // determinant of upper-left 2x2
    PRP_F32 det = (a00 * a11) - (a01 * a10);
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    PRP_F32 inv_det = 1.0f / det;

    // inverse 2x2 basis
    PRP_F32 r00 = a11 * inv_det;
    PRP_F32 r01 = -a01 * inv_det;
    PRP_F32 r10 = -a10 * inv_det;
    PRP_F32 r11 = a00 * inv_det;
    // inverse translation
    PRP_F32 itx = -(r00 * tx + r01 * ty);
    PRP_F32 ity = -(r10 * tx + r11 * ty);

    return (MATH_Mat3){
        .membs = {r00, r10, 0.0f, r01, r11, 0.0f, itx, ity, 1.0f}};
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3InvAffineOrthonormal(MATH_Mat3 a) {
    PRP_F32 r00 = a.membs[0], r10 = a.membs[1];
    PRP_F32 r01 = a.membs[3], r11 = a.membs[4];
    PRP_F32 tx = a.membs[6];
    PRP_F32 ty = a.membs[7];

    return (MATH_Mat3){.membs = {r00, r01, 0.0f,

                                 r10, r11, 0.0f,

                                 -((r00 * tx) + (r10 * ty)),
                                 -((r01 * tx) + (r11 * ty)), 1.0f}};
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat3AffineOrthonormalize(MATH_Mat3 a) {
    MATH_Vec2 c0 = {a.membs[0], a.membs[1]};
    MATH_Vec2 c1 = {a.membs[3], a.membs[4]};
    c0 = MATH_Vec2Normalize(c0);

    PRP_F32 proj = MATH_Vec2Dot(c1, c0);
    c1.x -= c0.x * proj;
    c1.y -= c0.y * proj;

    c1 = MATH_Vec2Normalize(c1);

    a.membs[0] = c0.x;
    a.membs[1] = c0.y;
    a.membs[3] = c1.x;
    a.membs[4] = c1.y;

    return a;
}

PRP_FN_API MATH_Mat3 PRP_FN_CALL
MATH_Mat3AffineOrthonormalizeSafe(MATH_Mat3 a, MATH_Mat3 fallback) {
    if (MATH_IsZeroF32(
            MATH_Vec2LenSq((MATH_Vec2){.x = a.membs[0], .y = a.membs[1]})) ||

        MATH_IsZeroF32(
            MATH_Vec2LenSq((MATH_Vec2){.x = a.membs[3], .y = a.membs[4]}))) {
        return fallback;
    }

    return MATH_Mat3AffineOrthonormalize(a);
}

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

PRP_FN_API PRP_F32 PRP_FN_CALL MATH_Mat3ExtractRotation(MATH_Mat3 a) {
    PRP_F32 a00 = a.membs[0], a10 = a.membs[1];
    MATH_Vec2 c0 = {.x = a00, .y = a10};
    PRP_F32 len = MATH_Vec2Len(c0);
    if (MATH_IsZeroF32(len))
        return 0.0f;
    PRP_F32 inv_len = 1.0f / len;

    PRP_F32 x = a00 * inv_len;
    PRP_F32 y = a10 * inv_len;

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

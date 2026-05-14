#include "Mat4.h"

/* ----  COMPARE FUNCTIONS  ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat4IsOrthonormal(MATH_Mat4 a) {
    MATH_Vec4 c0 = {
        .x = a.membs[0], .y = a.membs[1], .z = a.membs[2], .w = a.membs[3]};
    MATH_Vec4 c1 = {
        .x = a.membs[4], .y = a.membs[5], .z = a.membs[6], .w = a.membs[7]};
    MATH_Vec4 c2 = {
        .x = a.membs[8], .y = a.membs[9], .z = a.membs[10], .w = a.membs[11]};
    MATH_Vec4 c3 = {
        .x = a.membs[12], .y = a.membs[13], .z = a.membs[14], .w = a.membs[15]};

    DT_f32 d01 = MATH_Vec4Dot(c0, c1);
    DT_f32 d02 = MATH_Vec4Dot(c0, c2);
    DT_f32 d03 = MATH_Vec4Dot(c0, c3);
    DT_f32 d12 = MATH_Vec4Dot(c1, c2);
    DT_f32 d13 = MATH_Vec4Dot(c1, c3);
    DT_f32 d23 = MATH_Vec4Dot(c2, c3);

    DT_f32 l0 = MATH_Vec4LenSq(c0);
    DT_f32 l1 = MATH_Vec4LenSq(c1);
    DT_f32 l2 = MATH_Vec4LenSq(c2);
    DT_f32 l3 = MATH_Vec4LenSq(c3);

    return (DT_bool)(MATH_IsZeroF32(d01) && MATH_IsZeroF32(d02) &&
                     MATH_IsZeroF32(d03) && MATH_IsZeroF32(d12) &&
                     MATH_IsZeroF32(d13) && MATH_IsZeroF32(d23) &&
                     MATH_AlmostEqF32(l0, 1.0f) && MATH_AlmostEqF32(l1, 1.0f) &&
                     MATH_AlmostEqF32(l2, 1.0f) && MATH_AlmostEqF32(l3, 1.0f));
}

/* ----  BASIC OPS  ---- */

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Mul(MATH_Mat4 a, MATH_Mat4 b) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1], a20 = a.membs[2],
           a30 = a.membs[3];
    DT_f32 a01 = a.membs[4], a11 = a.membs[5], a21 = a.membs[6],
           a31 = a.membs[7];
    DT_f32 a02 = a.membs[8], a12 = a.membs[9], a22 = a.membs[10],
           a32 = a.membs[11];
    DT_f32 a03 = a.membs[12], a13 = a.membs[13], a23 = a.membs[14],
           a33 = a.membs[15];

    DT_f32 b00 = b.membs[0], b10 = b.membs[1], b20 = b.membs[2],
           b30 = b.membs[3];
    DT_f32 b01 = b.membs[4], b11 = b.membs[5], b21 = b.membs[6],
           b31 = b.membs[7];
    DT_f32 b02 = b.membs[8], b12 = b.membs[9], b22 = b.membs[10],
           b32 = b.membs[11];
    DT_f32 b03 = b.membs[12], b13 = b.membs[13], b23 = b.membs[14],
           b33 = b.membs[15];

    a.membs[0] = (a00 * b00) + (a01 * b10) + (a02 * b20) + (a03 * b30);
    a.membs[1] = (a10 * b00) + (a11 * b10) + (a12 * b20) + (a13 * b30);
    a.membs[2] = (a20 * b00) + (a21 * b10) + (a22 * b20) + (a23 * b30);
    a.membs[3] = (a30 * b00) + (a31 * b10) + (a32 * b20) + (a33 * b30);

    a.membs[4] = (a00 * b01) + (a01 * b11) + (a02 * b21) + (a03 * b31);
    a.membs[5] = (a10 * b01) + (a11 * b11) + (a12 * b21) + (a13 * b31);
    a.membs[6] = (a20 * b01) + (a21 * b11) + (a22 * b21) + (a23 * b31);
    a.membs[7] = (a30 * b01) + (a31 * b11) + (a32 * b21) + (a33 * b31);

    a.membs[8] = (a00 * b02) + (a01 * b12) + (a02 * b22) + (a03 * b32);
    a.membs[9] = (a10 * b02) + (a11 * b12) + (a12 * b22) + (a13 * b32);
    a.membs[10] = (a20 * b02) + (a21 * b12) + (a22 * b22) + (a23 * b32);
    a.membs[11] = (a30 * b02) + (a31 * b12) + (a32 * b22) + (a33 * b32);

    a.membs[12] = (a00 * b03) + (a01 * b13) + (a02 * b23) + (a03 * b33);
    a.membs[13] = (a10 * b03) + (a11 * b13) + (a12 * b23) + (a13 * b33);
    a.membs[14] = (a20 * b03) + (a21 * b13) + (a22 * b23) + (a23 * b33);
    a.membs[15] = (a30 * b03) + (a31 * b13) + (a32 * b23) + (a33 * b33);

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Transpose(MATH_Mat4 a) {
    MATH_Mat4 rslt;

    rslt.membs[0 * MATH_MAT4_SIZE + 0] = a.membs[0 * MATH_MAT4_SIZE + 0];
    rslt.membs[0 * MATH_MAT4_SIZE + 1] = a.membs[1 * MATH_MAT4_SIZE + 0];
    rslt.membs[0 * MATH_MAT4_SIZE + 2] = a.membs[2 * MATH_MAT4_SIZE + 0];
    rslt.membs[0 * MATH_MAT4_SIZE + 3] = a.membs[3 * MATH_MAT4_SIZE + 0];
    rslt.membs[1 * MATH_MAT4_SIZE + 0] = a.membs[0 * MATH_MAT4_SIZE + 1];
    rslt.membs[1 * MATH_MAT4_SIZE + 1] = a.membs[1 * MATH_MAT4_SIZE + 1];
    rslt.membs[1 * MATH_MAT4_SIZE + 2] = a.membs[2 * MATH_MAT4_SIZE + 1];
    rslt.membs[1 * MATH_MAT4_SIZE + 3] = a.membs[3 * MATH_MAT4_SIZE + 1];
    rslt.membs[2 * MATH_MAT4_SIZE + 0] = a.membs[0 * MATH_MAT4_SIZE + 2];
    rslt.membs[2 * MATH_MAT4_SIZE + 1] = a.membs[1 * MATH_MAT4_SIZE + 2];
    rslt.membs[2 * MATH_MAT4_SIZE + 2] = a.membs[2 * MATH_MAT4_SIZE + 2];
    rslt.membs[2 * MATH_MAT4_SIZE + 3] = a.membs[3 * MATH_MAT4_SIZE + 2];
    rslt.membs[3 * MATH_MAT4_SIZE + 0] = a.membs[0 * MATH_MAT4_SIZE + 3];
    rslt.membs[3 * MATH_MAT4_SIZE + 1] = a.membs[1 * MATH_MAT4_SIZE + 3];
    rslt.membs[3 * MATH_MAT4_SIZE + 2] = a.membs[2 * MATH_MAT4_SIZE + 3];
    rslt.membs[3 * MATH_MAT4_SIZE + 3] = a.membs[3 * MATH_MAT4_SIZE + 3];

    return rslt;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Adjoint(MATH_Mat4 a) {
    DT_f32 a00 = a.membs[0], a10 = a.membs[1], a20 = a.membs[2],
           a30 = a.membs[3];
    DT_f32 a01 = a.membs[4], a11 = a.membs[5], a21 = a.membs[6],
           a31 = a.membs[7];
    DT_f32 a02 = a.membs[8], a12 = a.membs[9], a22 = a.membs[10],
           a32 = a.membs[11];
    DT_f32 a03 = a.membs[12], a13 = a.membs[13], a23 = a.membs[14],
           a33 = a.membs[15];

    DT_f32 c00 =
        +(a11 * (a22 * a33 - a32 * a23) - a21 * (a12 * a33 - a32 * a13) +
          a31 * (a12 * a23 - a22 * a13));
    DT_f32 c01 =
        -(a10 * (a22 * a33 - a32 * a23) - a20 * (a12 * a33 - a32 * a13) +
          a30 * (a12 * a23 - a22 * a13));
    DT_f32 c02 =
        +(a10 * (a21 * a33 - a31 * a23) - a20 * (a11 * a33 - a31 * a13) +
          a30 * (a11 * a23 - a21 * a13));
    DT_f32 c03 =
        -(a10 * (a21 * a32 - a31 * a22) - a20 * (a11 * a32 - a31 * a12) +
          a30 * (a11 * a22 - a21 * a12));

    DT_f32 c10 =
        -(a01 * (a22 * a33 - a32 * a23) - a21 * (a02 * a33 - a32 * a03) +
          a31 * (a02 * a23 - a22 * a03));
    DT_f32 c11 =
        +(a00 * (a22 * a33 - a32 * a23) - a20 * (a02 * a33 - a32 * a03) +
          a30 * (a02 * a23 - a22 * a03));
    DT_f32 c12 =
        -(a00 * (a21 * a33 - a31 * a23) - a20 * (a01 * a33 - a31 * a03) +
          a30 * (a01 * a23 - a21 * a03));
    DT_f32 c13 =
        +(a00 * (a21 * a32 - a31 * a22) - a20 * (a01 * a32 - a31 * a02) +
          a30 * (a01 * a22 - a21 * a02));

    DT_f32 c20 =
        +(a01 * (a12 * a33 - a32 * a13) - a11 * (a02 * a33 - a32 * a03) +
          a31 * (a02 * a13 - a12 * a03));
    DT_f32 c21 =
        -(a00 * (a12 * a33 - a32 * a13) - a10 * (a02 * a33 - a32 * a03) +
          a30 * (a02 * a13 - a12 * a03));
    DT_f32 c22 =
        +(a00 * (a11 * a33 - a31 * a13) - a10 * (a01 * a33 - a31 * a03) +
          a30 * (a01 * a13 - a11 * a03));
    DT_f32 c23 =
        -(a00 * (a11 * a32 - a31 * a12) - a10 * (a01 * a32 - a31 * a02) +
          a30 * (a01 * a12 - a11 * a02));

    DT_f32 c30 =
        -(a01 * (a12 * a23 - a22 * a13) - a11 * (a02 * a23 - a22 * a03) +
          a21 * (a02 * a13 - a12 * a03));
    DT_f32 c31 =
        +(a00 * (a12 * a23 - a22 * a13) - a10 * (a02 * a23 - a22 * a03) +
          a20 * (a02 * a13 - a12 * a03));
    DT_f32 c32 =
        -(a00 * (a11 * a23 - a21 * a13) - a10 * (a01 * a23 - a21 * a03) +
          a20 * (a01 * a13 - a11 * a03));
    DT_f32 c33 =
        +(a00 * (a11 * a22 - a21 * a12) - a10 * (a01 * a22 - a21 * a02) +
          a20 * (a01 * a12 - a11 * a02));

    // Adjugate = transpose of cofactor matrix
    return (MATH_Mat4){.membs = {c00, c10, c20, c30, c01, c11, c21, c31, c02,
                                 c12, c22, c32, c03, c13, c23, c33}};
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat4Det(MATH_Mat4 a) {
    const DT_f32 *m = a.membs;

    DT_f32 s0 = m[0] * m[5] - m[1] * m[4];
    DT_f32 s1 = m[0] * m[6] - m[2] * m[4];
    DT_f32 s2 = m[0] * m[7] - m[3] * m[4];
    DT_f32 s3 = m[1] * m[6] - m[2] * m[5];
    DT_f32 s4 = m[1] * m[7] - m[3] * m[5];
    DT_f32 s5 = m[2] * m[7] - m[3] * m[6];

    DT_f32 c5 = m[10] * m[15] - m[11] * m[14];
    DT_f32 c4 = m[9] * m[15] - m[11] * m[13];
    DT_f32 c3 = m[9] * m[14] - m[10] * m[13];
    DT_f32 c2 = m[8] * m[15] - m[11] * m[12];
    DT_f32 c1 = m[8] * m[14] - m[10] * m[12];
    DT_f32 c0 = m[8] * m[13] - m[9] * m[12];

    return (s0 * c5) - (s1 * c4) + (s2 * c3) + (s3 * c2) - (s4 * c1) +
           (s5 * c0);
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Inv(MATH_Mat4 a,
                                              MATH_Mat4 det_zero_fallback) {
    /**
     * Adjugate (cofactor) method for matrix inversion, in an
     optimized/unrolled
     * form
     */
    const DT_f32 *m = a.membs;
    MATH_Mat4 r;

    DT_f32 s0 = m[0] * m[5] - m[1] * m[4];
    DT_f32 s1 = m[0] * m[6] - m[2] * m[4];
    DT_f32 s2 = m[0] * m[7] - m[3] * m[4];
    DT_f32 s3 = m[1] * m[6] - m[2] * m[5];
    DT_f32 s4 = m[1] * m[7] - m[3] * m[5];
    DT_f32 s5 = m[2] * m[7] - m[3] * m[6];

    DT_f32 c5 = m[10] * m[15] - m[11] * m[14];
    DT_f32 c4 = m[9] * m[15] - m[11] * m[13];
    DT_f32 c3 = m[9] * m[14] - m[10] * m[13];
    DT_f32 c2 = m[8] * m[15] - m[11] * m[12];
    DT_f32 c1 = m[8] * m[14] - m[10] * m[12];
    DT_f32 c0 = m[8] * m[13] - m[9] * m[12];

    DT_f32 det =
        (s0 * c5) - (s1 * c4) + (s2 * c3) + (s3 * c2) - (s4 * c1) + (s5 * c0);
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    r.membs[0] = (m[5] * c5 - m[6] * c4 + m[7] * c3) * inv_det;
    r.membs[1] = (-m[1] * c5 + m[2] * c4 - m[3] * c3) * inv_det;
    r.membs[2] = (m[13] * s5 - m[14] * s4 + m[15] * s3) * inv_det;
    r.membs[3] = (-m[9] * s5 + m[10] * s4 - m[11] * s3) * inv_det;

    r.membs[4] = (-m[4] * c5 + m[6] * c2 - m[7] * c1) * inv_det;
    r.membs[5] = (m[0] * c5 - m[2] * c2 + m[3] * c1) * inv_det;
    r.membs[6] = (-m[12] * s5 + m[14] * s2 - m[15] * s1) * inv_det;
    r.membs[7] = (m[8] * s5 - m[10] * s2 + m[11] * s1) * inv_det;

    r.membs[8] = (m[4] * c4 - m[5] * c2 + m[7] * c0) * inv_det;
    r.membs[9] = (-m[0] * c4 + m[1] * c2 - m[3] * c0) * inv_det;
    r.membs[10] = (m[12] * s4 - m[13] * s2 + m[15] * s0) * inv_det;
    r.membs[11] = (-m[8] * s4 + m[9] * s2 - m[11] * s0) * inv_det;

    r.membs[12] = (-m[4] * c3 + m[5] * c1 - m[6] * c0) * inv_det;
    r.membs[13] = (m[0] * c3 - m[1] * c1 + m[2] * c0) * inv_det;
    r.membs[14] = (-m[12] * s3 + m[13] * s1 - m[14] * s0) * inv_det;
    r.membs[15] = (m[8] * s3 - m[9] * s1 + m[10] * s0) * inv_det;

    return r;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4Orthonormalize(MATH_Mat4 a) {
    MATH_Vec4 c0 = {
        .x = a.membs[0], .y = a.membs[1], .z = a.membs[2], .w = a.membs[3]};
    MATH_Vec4 c1 = {
        .x = a.membs[4], .y = a.membs[5], .z = a.membs[6], .w = a.membs[7]};
    MATH_Vec4 c2 = {
        .x = a.membs[8], .y = a.membs[9], .z = a.membs[10], .w = a.membs[11]};
    MATH_Vec4 c3 = {
        .x = a.membs[12], .y = a.membs[13], .z = a.membs[14], .w = a.membs[15]};

    // Normalize c0
    c0 = MATH_Vec4Normalize(c0);
    // Orthogonalize c1 against c0
    DT_f32 proj10 = MATH_Vec4Dot(c1, c0);
    c1.x -= c0.x * proj10;
    c1.y -= c0.y * proj10;
    c1.z -= c0.z * proj10;
    c1.w -= c0.w * proj10;
    c1 = MATH_Vec4Normalize(c1);

    // Orthogonalize c2 against c0
    DT_f32 proj20 = MATH_Vec4Dot(c2, c0);
    c2.x -= c0.x * proj20;
    c2.y -= c0.y * proj20;
    c2.z -= c0.z * proj20;
    c2.w -= c0.w * proj20;

    // Orthogonalize c2 against c1
    DT_f32 proj21 = MATH_Vec4Dot(c2, c1);
    c2.x -= c1.x * proj21;
    c2.y -= c1.y * proj21;
    c2.z -= c1.z * proj21;
    c2.w -= c1.w * proj21;
    c2 = MATH_Vec4Normalize(c2);

    // Orthogonalize c3 against c0
    DT_f32 proj30 = MATH_Vec4Dot(c3, c0);
    c3.x -= c0.x * proj30;
    c3.y -= c0.y * proj30;
    c3.z -= c0.z * proj30;
    c3.w -= c0.w * proj30;

    // Orthogonalize c3 against c1
    DT_f32 proj31 = MATH_Vec4Dot(c3, c1);
    c3.x -= c1.x * proj31;
    c3.y -= c1.y * proj31;
    c3.z -= c1.z * proj31;
    c3.w -= c1.w * proj31;

    // Orthogonalize c3 against c2
    DT_f32 proj32 = MATH_Vec4Dot(c3, c2);
    c3.x -= c2.x * proj32;
    c3.y -= c2.y * proj32;
    c3.z -= c2.z * proj32;
    c3.w -= c2.w * proj32;
    c3 = MATH_Vec4Normalize(c3);

    return (MATH_Mat4){.membs = {c0.x, c0.y, c0.z, c0.w, c1.x, c1.y, c1.z, c1.w,
                                 c2.x, c2.y, c2.z, c2.w, c3.x, c3.y, c3.z,
                                 c3.w}};
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4OrthonormalizeSafe(MATH_Mat4 a, MATH_Mat4 fallback) {
    if (MATH_IsZeroF32(MATH_Vec4LenSq((MATH_Vec4){.x = a.membs[0],
                                                  .y = a.membs[1],
                                                  .z = a.membs[2],
                                                  .w = a.membs[3]})) ||

        MATH_IsZeroF32(MATH_Vec4LenSq((MATH_Vec4){.x = a.membs[4],
                                                  .y = a.membs[5],
                                                  .z = a.membs[6],
                                                  .w = a.membs[7]})) ||

        MATH_IsZeroF32(MATH_Vec4LenSq((MATH_Vec4){.x = a.membs[8],
                                                  .y = a.membs[9],
                                                  .z = a.membs[10],
                                                  .w = a.membs[11]})) ||

        MATH_IsZeroF32(MATH_Vec4LenSq((MATH_Vec4){.x = a.membs[12],
                                                  .y = a.membs[13],
                                                  .z = a.membs[14],
                                                  .w = a.membs[15]}))) {
        return fallback;
    }

    return MATH_Mat4Orthonormalize(a);
}

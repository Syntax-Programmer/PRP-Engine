#include "Mat4-Affine.h"

/* ----  CONSTRUCTORS ---- */

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4CreateRotationAxis(MATH_Vec3 axis,
                                                             DT_f32 angle) {
    axis = MATH_Vec3Normalize(axis);
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);
    DT_f32 t = 1.0f - c;

    MATH_Mat4 m = {0};
    m.membs[0] = t * axis.x * axis.x + c;
    m.membs[1] = t * axis.x * axis.y + s * axis.z;
    m.membs[2] = t * axis.x * axis.z - s * axis.y;

    m.membs[4] = t * axis.x * axis.y - s * axis.z;
    m.membs[5] = t * axis.y * axis.y + c;
    m.membs[6] = t * axis.y * axis.z + s * axis.x;

    m.membs[8] = t * axis.x * axis.z + s * axis.y;
    m.membs[9] = t * axis.y * axis.z - s * axis.x;
    m.membs[10] = t * axis.z * axis.z + c;

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4CreateRotationAxisSafe(
    MATH_Vec3 axis, DT_f32 angle, MATH_Mat4 fallback) {
    if (MATH_IsZeroF32(MATH_Vec3LenSq(axis))) {
        return fallback;
    }

    return MATH_Mat4CreateRotationAxis(axis, angle);
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerXYZ(MATH_EulerAngle angles) {
    DT_f32 cx = MATH_CosF32(angles.x_rad), sx = MATH_SinF32(angles.x_rad);
    DT_f32 cy = MATH_CosF32(angles.y_rad), sy = MATH_SinF32(angles.y_rad);
    DT_f32 cz = MATH_CosF32(angles.z_rad), sz = MATH_SinF32(angles.z_rad);

    MATH_Mat4 m = {0};
    m.membs[0] = cy * cz;
    m.membs[1] = cy * sz;
    m.membs[2] = -sy;

    m.membs[4] = (cz * sx * sy) - (cx * sz);
    m.membs[5] = (cx * cz) + (sx * sy * sz);
    m.membs[6] = cy * sx;

    m.membs[8] = (sx * sz) + (cx * cz * sy);
    m.membs[9] = (cx * sy * sz) - (cz * sx);
    m.membs[10] = cx * cy;

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerZXY(MATH_EulerAngle angles) {
    DT_f32 cx = MATH_CosF32(angles.x_rad), sx = MATH_SinF32(angles.x_rad);
    DT_f32 cy = MATH_CosF32(angles.y_rad), sy = MATH_SinF32(angles.y_rad);
    DT_f32 cz = MATH_CosF32(angles.z_rad), sz = MATH_SinF32(angles.z_rad);

    MATH_Mat4 m = {0};
    m.membs[0] = (cx * cz) + (sx * sy * sz);
    m.membs[1] = cx * sz;
    m.membs[2] = -(sy * cz) + (sx * cy * sz);

    m.membs[4] = (cz * sx * sy) - (cx * sz);
    m.membs[5] = cx * cz;
    m.membs[6] = (sy * sz) + (sx * cy * cz);

    m.membs[8] = cx * sy;
    m.membs[9] = -sx;
    m.membs[10] = cx * cy;

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerYZX(MATH_EulerAngle angles) {
    DT_f32 cx = MATH_CosF32(angles.x_rad), sx = MATH_SinF32(angles.x_rad);
    DT_f32 cy = MATH_CosF32(angles.y_rad), sy = MATH_SinF32(angles.y_rad);
    DT_f32 cz = MATH_CosF32(angles.z_rad), sz = MATH_SinF32(angles.z_rad);

    MATH_Mat4 m = {0};
    m.membs[0] = cy * cz;
    m.membs[1] = (cx * sz * cy) + (sx * sy);
    m.membs[2] = (sx * sz * cy) - (cx * sy);

    m.membs[4] = -sz;
    m.membs[5] = cx * cz;
    m.membs[6] = sx * cz;

    m.membs[8] = cz * sy;
    m.membs[9] = (cx * sz * sy) - (sx * cy);
    m.membs[10] = (sx * sz * sy) + (cx * cy);

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerYXZ(MATH_EulerAngle angles) {
    DT_f32 cx = MATH_CosF32(angles.x_rad), sx = MATH_SinF32(angles.x_rad);
    DT_f32 cy = MATH_CosF32(angles.y_rad), sy = MATH_SinF32(angles.y_rad);
    DT_f32 cz = MATH_CosF32(angles.z_rad), sz = MATH_SinF32(angles.z_rad);

    MATH_Mat4 m = {0};
    m.membs[0] = (cy * cz) + (sx * sy * sz);
    m.membs[1] = cx * sz;
    m.membs[2] = (cy * sx * sz) - (cz * sy);

    m.membs[4] = (cz * sx * sy) - (cy * sz);
    m.membs[5] = cx * cz;
    m.membs[6] = (cy * cz * sx) + (sy * sz);

    m.membs[8] = cx * sy;
    m.membs[9] = -sx;
    m.membs[10] = cx * cy;

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerZYX(MATH_EulerAngle angles) {
    DT_f32 cx = MATH_CosF32(angles.x_rad), sx = MATH_SinF32(angles.x_rad);
    DT_f32 cy = MATH_CosF32(angles.y_rad), sy = MATH_SinF32(angles.y_rad);
    DT_f32 cz = MATH_CosF32(angles.z_rad), sz = MATH_SinF32(angles.z_rad);

    MATH_Mat4 m = {0};
    m.membs[0] = (cy * cz);
    m.membs[1] = (cx * sz) + (sx * sy * cz);
    m.membs[2] = (sx * sz) - (cx * sy * cz);

    m.membs[4] = -(cy * sz);
    m.membs[5] = (cx * cz) - (sx * sy * sz);
    m.membs[6] = (sx * cz) + (cx * sy * sz);

    m.membs[8] = sy;
    m.membs[9] = -(sx * cy);
    m.membs[10] = cx * cy;

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateRotationEulerXZY(MATH_EulerAngle angles) {
    DT_f32 cx = MATH_CosF32(angles.x_rad), sx = MATH_SinF32(angles.x_rad);
    DT_f32 cy = MATH_CosF32(angles.y_rad), sy = MATH_SinF32(angles.y_rad);
    DT_f32 cz = MATH_CosF32(angles.z_rad), sz = MATH_SinF32(angles.z_rad);

    MATH_Mat4 m = {0};
    m.membs[0] = cy * cz;
    m.membs[1] = sz;
    m.membs[2] = -(cz * sy);

    m.membs[4] = (sx * sy) - (cx * cy * sz);
    m.membs[5] = cx * cz;
    m.membs[6] = (cy * sx) + (cx * sy * sz);

    m.membs[8] = (cx * sy) + (cy * sx * sz);
    m.membs[9] = -(cz * sx);
    m.membs[10] = (cx * cy) - (sx * sy * sz);

    m.membs[15] = 1.0f;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4CreateTRSAxis(MATH_Vec3 pos,
                                                        MATH_Vec3 rot_axis,
                                                        DT_f32 rot_rad,
                                                        MATH_Vec3 scale) {
    MATH_Mat4 m = MATH_Mat4CreateRotationAxis(rot_axis, rot_rad);
    // applying scale on it.
    m.membs[0] *= scale.x;
    m.membs[1] *= scale.x;
    m.membs[2] *= scale.x;
    m.membs[4] *= scale.y;
    m.membs[5] *= scale.y;
    m.membs[6] *= scale.y;
    m.membs[8] *= scale.z;
    m.membs[9] *= scale.z;
    m.membs[10] *= scale.z;

    // Adding traslation.
    m.membs[12] = pos.x;
    m.membs[13] = pos.y;
    m.membs[14] = pos.z;

    return m;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4CreateTRSEuler(MATH_Vec3 pos, MATH_EulerAngle rot_angles,
                        MATH_EulerAngleOrder rot_order, MATH_Vec3 scale) {
    MATH_Mat4 m = MATH_Mat4CreateRotationEuler(rot_angles, rot_order);
    // applying scale on it.
    m.membs[0] *= scale.x;
    m.membs[1] *= scale.x;
    m.membs[2] *= scale.x;
    m.membs[4] *= scale.y;
    m.membs[5] *= scale.y;
    m.membs[6] *= scale.y;
    m.membs[8] *= scale.z;
    m.membs[9] *= scale.z;
    m.membs[10] *= scale.z;

    // Adding traslation.
    m.membs[12] = pos.x;
    m.membs[13] = pos.y;
    m.membs[14] = pos.z;

    return m;
}

/* ----  COMPARE FUNCTIONS  ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Mat4IsAffineOrthonormal(MATH_Mat4 a) {
    MATH_Vec3 c0 = {.x = a.membs[0], .y = a.membs[1], .z = a.membs[2]};
    MATH_Vec3 c1 = {.x = a.membs[4], .y = a.membs[5], .z = a.membs[6]};
    MATH_Vec3 c2 = {.x = a.membs[8], .y = a.membs[9], .z = a.membs[10]};

    DT_f32 d01 = MATH_Vec3Dot(c0, c1);
    DT_f32 d02 = MATH_Vec3Dot(c0, c2);
    DT_f32 d12 = MATH_Vec3Dot(c1, c2);

    DT_f32 l0 = MATH_Vec3LenSq(c0);
    DT_f32 l1 = MATH_Vec3LenSq(c1);
    DT_f32 l2 = MATH_Vec3LenSq(c2);

    return (DT_bool)(MATH_IsZeroF32(a.membs[3]) && MATH_IsZeroF32(a.membs[7]) &&
                     MATH_IsZeroF32(a.membs[11]) &&
                     MATH_AlmostEqF32(a.membs[15], 1.0f) &&
                     MATH_IsZeroF32(d01) && MATH_IsZeroF32(d02) &&
                     MATH_IsZeroF32(d12) && MATH_AlmostEqF32(l0, 1.0f) &&
                     MATH_AlmostEqF32(l1, 1.0f) && MATH_AlmostEqF32(l2, 1.0f));
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4InvAffine(MATH_Mat4 a, MATH_Mat4 det_zero_fallback) {
    // Extract 3x3 linear part
    DT_f32 a00 = a.membs[0], a01 = a.membs[4], a02 = a.membs[8];
    DT_f32 a10 = a.membs[1], a11 = a.membs[5], a12 = a.membs[9];
    DT_f32 a20 = a.membs[2], a21 = a.membs[6], a22 = a.membs[10];

    // Compute inverse of 3x3 (same pattern as your optimized det)
    DT_f32 det = a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20) +
                 a02 * (a10 * a21 - a11 * a20);
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    // Inverse 3x3
    a.membs[0] = (a11 * a22 - a12 * a21) * inv_det;
    a.membs[1] = -(a10 * a22 - a12 * a20) * inv_det;
    a.membs[2] = (a10 * a21 - a11 * a20) * inv_det;
    a.membs[3] = 0.0f;

    a.membs[4] = -(a01 * a22 - a02 * a21) * inv_det;
    a.membs[5] = (a00 * a22 - a02 * a20) * inv_det;
    a.membs[6] = -(a00 * a21 - a01 * a20) * inv_det;
    a.membs[7] = 0.0f;

    a.membs[8] = (a01 * a12 - a02 * a11) * inv_det;
    a.membs[9] = -(a00 * a12 - a02 * a10) * inv_det;
    a.membs[10] = (a00 * a11 - a01 * a10) * inv_det;
    a.membs[11] = 0.0f;

    // Translation
    DT_f32 tx = a.membs[12];
    DT_f32 ty = a.membs[13];
    DT_f32 tz = a.membs[14];
    a.membs[12] = -(a.membs[0] * tx + a.membs[4] * ty + a.membs[8] * tz);
    a.membs[13] = -(a.membs[1] * tx + a.membs[5] * ty + a.membs[9] * tz);
    a.membs[14] = -(a.membs[2] * tx + a.membs[6] * ty + a.membs[10] * tz);
    a.membs[15] = 1.0f;

    return a;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4InvAffineOrthonormal(MATH_Mat4 a) {
    DT_f32 r00 = a.membs[0];
    DT_f32 r10 = a.membs[1];
    DT_f32 r20 = a.membs[2];

    DT_f32 r01 = a.membs[4];
    DT_f32 r11 = a.membs[5];
    DT_f32 r21 = a.membs[6];

    DT_f32 r02 = a.membs[8];
    DT_f32 r12 = a.membs[9];
    DT_f32 r22 = a.membs[10];

    // Translation
    DT_f32 tx = a.membs[12];
    DT_f32 ty = a.membs[13];
    DT_f32 tz = a.membs[14];

    // -(R^T * t)
    DT_f32 itx = -((r00 * tx) + (r10 * ty) + (r20 * tz));
    DT_f32 ity = -((r01 * tx) + (r11 * ty) + (r21 * tz));
    DT_f32 itz = -((r02 * tx) + (r12 * ty) + (r22 * tz));

    return (MATH_Mat4){.membs = {r00, r01, r02, 0.0f,

                                 r10, r11, r12, 0.0f,

                                 r20, r21, r22, 0.0f,

                                 // Inverse translation
                                 itx, ity, itz, 1.0f}};
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4AffineOrthonormalize(MATH_Mat4 a) {
    MATH_Vec3 c0 = {.x = a.membs[0], .y = a.membs[1], .z = a.membs[2]};
    MATH_Vec3 c1 = {.x = a.membs[4], .y = a.membs[5], .z = a.membs[6]};
    MATH_Vec3 c2 = {.x = a.membs[8], .y = a.membs[9], .z = a.membs[10]};

    // Gram-Schmidt
    c0 = MATH_Vec3Normalize(c0);
    DT_f32 proj10 = MATH_Vec3Dot(c1, c0);
    c1.x -= c0.x * proj10;
    c1.y -= c0.y * proj10;
    c1.z -= c0.z * proj10;
    c1 = MATH_Vec3Normalize(c1);

    DT_f32 proj20 = MATH_Vec3Dot(c2, c0);
    c2.x -= c0.x * proj20;
    c2.y -= c0.y * proj20;
    c2.z -= c0.z * proj20;

    DT_f32 proj21 = MATH_Vec3Dot(c2, c1);
    c2.x -= c1.x * proj21;
    c2.y -= c1.y * proj21;
    c2.z -= c1.z * proj21;
    c2 = MATH_Vec3Normalize(c2);

    a.membs[0] = c0.x;
    a.membs[1] = c0.y;
    a.membs[2] = c0.z;
    a.membs[3] = 0.0f;

    a.membs[4] = c1.x;
    a.membs[5] = c1.y;
    a.membs[6] = c1.z;
    a.membs[7] = 0.0f;

    a.membs[8] = c2.x;
    a.membs[9] = c2.y;
    a.membs[10] = c2.z;
    a.membs[11] = 0.0f;

    // Restore affine bottom row
    a.membs[15] = 1.0f;

    return a;
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL
MATH_Mat4AffineOrthonormalizeSafe(MATH_Mat4 a, MATH_Mat4 fallback) {
    if (MATH_IsZeroF32(MATH_Vec3LenSq(
            (MATH_Vec3){.x = a.membs[0], .y = a.membs[1], .z = a.membs[2]})) ||

        MATH_IsZeroF32(MATH_Vec3LenSq(
            (MATH_Vec3){.x = a.membs[4], .y = a.membs[5], .z = a.membs[6]})) ||

        MATH_IsZeroF32(MATH_Vec3LenSq(
            (MATH_Vec3){.x = a.membs[8], .y = a.membs[9], .z = a.membs[10]}))) {
        return fallback;
    }

    return MATH_Mat4AffineOrthonormalize(a);
}

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

PRP_FN_API MATH_Mat3 PRP_FN_CALL MATH_Mat4ExtractRotation(MATH_Mat4 a) {
    MATH_Vec3 scale = MATH_Mat4ExtractScale(a);
    MATH_Mat3 rslt = {0};

    if (!MATH_IsZeroF32(scale.x)) {
        rslt.membs[0] = a.membs[0] / scale.x;
        rslt.membs[1] = a.membs[1] / scale.x;
        rslt.membs[2] = a.membs[2] / scale.x;
    }
    if (!MATH_IsZeroF32(scale.y)) {
        rslt.membs[3] = a.membs[4] / scale.y;
        rslt.membs[4] = a.membs[5] / scale.y;
        rslt.membs[5] = a.membs[6] / scale.y;
    }
    if (!MATH_IsZeroF32(scale.z)) {
        rslt.membs[6] = a.membs[8] / scale.z;
        rslt.membs[7] = a.membs[9] / scale.z;
        rslt.membs[8] = a.membs[10] / scale.z;
    }

    return rslt;
}

PRP_FN_API MATH_Vec3 PRP_FN_CALL MATH_Mat4ExtractScale(MATH_Mat4 a) {
    MATH_Vec3 c0 = {.x = a.membs[0], .y = a.membs[1], .z = a.membs[2]};
    MATH_Vec3 c1 = {.x = a.membs[4], .y = a.membs[5], .z = a.membs[6]};
    MATH_Vec3 c2 = {.x = a.membs[8], .y = a.membs[9], .z = a.membs[10]};

    return (MATH_Vec3){
        .x = MATH_Vec3Len(c0),
        .y = MATH_Vec3Len(c1),
        .z = MATH_Vec3Len(c2),
    };
}

PRP_FN_API MATH_Mat4 PRP_FN_CALL MATH_Mat4NormBasis(MATH_Mat4 a) {
    MATH_Vec3 s = MATH_Mat4ExtractScale(a);

    if (!MATH_IsZeroF32(s.x)) {
        a.membs[0] /= s.x;
        a.membs[1] /= s.x;
        a.membs[2] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.membs[4] /= s.y;
        a.membs[5] /= s.y;
        a.membs[6] /= s.y;
    }
    if (!MATH_IsZeroF32(s.z)) {
        a.membs[8] /= s.z;
        a.membs[9] /= s.z;
        a.membs[10] /= s.z;
    }

    return a;
}

// /* ----  SPECIAL OPS  ---- */

// static inline MATH_Mat4 MATH_Mat4LookAt(MATH_Vec3 eye, MATH_Vec3 target,
//                                         MATH_Vec3 up) {
//     MATH_Vec3 f = MATH_Vec3Normalize(MATH_Vec3SubComps(target, eye));
//     MATH_Vec3 r = MATH_Vec3Normalize(MATH_Vec3Cross(f, up));
//     MATH_Vec3 u = MATH_Vec3Cross(r, f);

//     MATH_Mat4 m = MATH_Mat4CreateIdentity();

//     m.membs[0] = r.x;
//     m.membs[1] = r.y;
//     m.membs[2] = r.z;
//     m.membs[4] = u.x;
//     m.membs[5] = u.y;
//     m.membs[6] = u.z;
//     m.membs[8] = -f.x;
//     m.membs[9] = -f.y;
//     m.membs[10] = -f.z;

//     m.membs[12] = -MATH_Vec3Dot(r, eye);
//     m.membs[13] = -MATH_Vec3Dot(u, eye);
//     m.membs[14] = MATH_Vec3Dot(f, eye);

//     return m;
// }

// static inline MATH_Mat4 MATH_Mat4Perspective(DT_f32 fov, DT_f32 aspect,
//                                              DT_f32 near, DT_f32 far) {
//     DT_f32 f = 1.0f / MATH_TanF32(fov * 0.5f);

//     MATH_Mat4 m = {0};

//     m.membs[0] = f / aspect;
//     m.membs[5] = f;

//     m.membs[10] = (far + near) / (near - far);
//     m.membs[11] = -1.0f;

//     m.membs[14] = (2.0f * far * near) / (near - far);

//     return m;
// }

// static inline MATH_Mat4 MATH_Mat4Ortho(DT_f32 left, DT_f32 right, DT_f32
// bottom,
//                                        DT_f32 top, DT_f32 near, DT_f32
//                                        far) {
//     MATH_Mat4 m = {0};

//     m.membs[0] = 2.0f / (right - left);
//     m.membs[5] = 2.0f / (top - bottom);
//     m.membs[10] = -2.0f / (far - near);

//     m.membs[12] = -(right + left) / (right - left);
//     m.membs[13] = -(top + bottom) / (top - bottom);
//     m.membs[14] = -(far + near) / (far - near);

//     m.membs[15] = 1.0f;

//     return m;
// }

// static inline MATH_Mat4 MATH_Mat4Frustum(DT_f32 left, DT_f32 right,
//                                          DT_f32 bottom, DT_f32 top,
//                                          DT_f32 near, DT_f32 far) {
//     MATH_Mat4 m = {0};

//     m.membs[0] = (2.0f * near) / (right - left);
//     m.membs[5] = (2.0f * near) / (top - bottom);

//     m.membs[8] = (right + left) / (right - left);
//     m.membs[9] = (top + bottom) / (top - bottom);

//     m.membs[10] = -(far + near) / (far - near);
//     m.membs[11] = -1.0f;

//     m.membs[14] = -(2.0f * far * near) / (far - near);

//     return m;
// }

// static inline MATH_Mat3 MATH_Mat4ToNormalMatrix(MATH_Mat4 m) {
//     // Extract upper-left 3x3
//     DT_f32 a00 = m.membs[0], a01 = m.membs[4], a02 = m.membs[8];
//     DT_f32 a10 = m.membs[1], a11 = m.membs[5], a12 = m.membs[9];
//     DT_f32 a20 = m.membs[2], a21 = m.membs[6], a22 = m.membs[10];

//     // Compute determinant
//     DT_f32 det = a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 *
//     a20) +
//                  a02 * (a10 * a21 - a11 * a20);

//     if (MATH_IsZeroF32(det)) {
//         return MATH_Mat3CreateIdentity(); // or fallback
//     }

//     DT_f32 inv_det = 1.0f / det;

//     MATH_Mat3 r;

//     // Inverse (cofactor method)
//     r.membs[0] = (a11 * a22 - a12 * a21) * inv_det;
//     r.membs[1] = -(a10 * a22 - a12 * a20) * inv_det;
//     r.membs[2] = (a10 * a21 - a11 * a20) * inv_det;

//     r.membs[3] = -(a01 * a22 - a02 * a21) * inv_det;
//     r.membs[4] = (a00 * a22 - a02 * a20) * inv_det;
//     r.membs[5] = -(a00 * a21 - a01 * a20) * inv_det;

//     r.membs[6] = (a01 * a12 - a02 * a11) * inv_det;
//     r.membs[7] = -(a00 * a12 - a02 * a10) * inv_det;
//     r.membs[8] = (a00 * a11 - a01 * a10) * inv_det;

//     // Transpose
//     MATH_Mat3 t;
//     t.membs[0] = r.membs[0];
//     t.membs[1] = r.membs[3];
//     t.membs[2] = r.membs[6];

//     t.membs[3] = r.membs[1];
//     t.membs[4] = r.membs[4];
//     t.membs[5] = r.membs[7];

//     t.membs[6] = r.membs[2];
//     t.membs[7] = r.membs[5];
//     t.membs[8] = r.membs[8];

//     return t;
// }

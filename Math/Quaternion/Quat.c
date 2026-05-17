#include "Quat.h"

/* ----  CONSTRUCTORS ---- */

PRP_FN_API MATH_Quat PRP_FN_API MATH_QuatCreateAxis(MATH_Vec3 axis,
                                                    DT_f32 rad) {
    DT_f32 half = rad * 0.5f;
    DT_f32 s = MATH_SinF32(half);
    DT_f32 c = MATH_CosF32(half);
    axis = MATH_Vec3Normalize(axis);

    return (MATH_Quat){
        .x = axis.x * s,
        .y = axis.y * s,
        .z = axis.z * s,
        .w = c,
    };
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatCreateAxisSafe(MATH_Vec3 axis,
                                                         DT_f32 rad,
                                                         MATH_Quat fallback) {
    if (MATH_IsZeroF32(MATH_Vec3LenSq(axis))) {
        return fallback;
    }

    return MATH_QuatCreateAxis(axis, rad);
}

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerXYZ(MATH_EulerAngle angles) {
    DT_f32 hx = angles.x_rad * 0.5f;
    DT_f32 hy = angles.y_rad * 0.5f;
    DT_f32 hz = angles.z_rad * 0.5f;
    DT_f32 cx = MATH_CosF32(hx);
    DT_f32 sx = MATH_SinF32(hx);
    DT_f32 cy = MATH_CosF32(hy);
    DT_f32 sy = MATH_SinF32(hy);
    DT_f32 cz = MATH_CosF32(hz);
    DT_f32 sz = MATH_SinF32(hz);
    MATH_Quat quat;

    quat.x = (sx * cy * cz) + (cx * sy * sz);
    quat.y = (cx * sy * cz) - (sx * cy * sz);
    quat.z = (cx * cy * sz) + (sx * sy * cz);
    quat.w = (cx * cy * cz) - (sx * sy * sz);

    return quat;
}

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerZXY(MATH_EulerAngle angles) {
    DT_f32 hx = angles.x_rad * 0.5f;
    DT_f32 hy = angles.y_rad * 0.5f;
    DT_f32 hz = angles.z_rad * 0.5f;
    DT_f32 cx = MATH_CosF32(hx);
    DT_f32 sx = MATH_SinF32(hx);
    DT_f32 cy = MATH_CosF32(hy);
    DT_f32 sy = MATH_SinF32(hy);
    DT_f32 cz = MATH_CosF32(hz);
    DT_f32 sz = MATH_SinF32(hz);
    MATH_Quat quat;

    quat.x = (sx * cy * cz) - (cx * sy * sz);
    quat.y = (cx * sy * cz) + (sx * cy * sz);
    quat.z = (cx * cy * sz) + (sx * sy * cz);
    quat.w = (cx * cy * cz) - (sx * sy * sz);

    return quat;
}

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerYZX(MATH_EulerAngle angles) {
    DT_f32 hx = angles.x_rad * 0.5f;
    DT_f32 hy = angles.y_rad * 0.5f;
    DT_f32 hz = angles.z_rad * 0.5f;
    DT_f32 cx = MATH_CosF32(hx);
    DT_f32 sx = MATH_SinF32(hx);
    DT_f32 cy = MATH_CosF32(hy);
    DT_f32 sy = MATH_SinF32(hy);
    DT_f32 cz = MATH_CosF32(hz);
    DT_f32 sz = MATH_SinF32(hz);
    MATH_Quat quat;

    quat.x = (sx * cy * cz) + (cx * sy * sz);
    quat.y = (cx * sy * cz) + (sx * cy * sz);
    quat.z = (cx * cy * sz) - (sx * sy * cz);
    quat.w = (cx * cy * cz) - (sx * sy * sz);

    return quat;
}

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerYXZ(MATH_EulerAngle angles) {
    DT_f32 hx = angles.x_rad * 0.5f;
    DT_f32 hy = angles.y_rad * 0.5f;
    DT_f32 hz = angles.z_rad * 0.5f;
    DT_f32 cx = MATH_CosF32(hx);
    DT_f32 sx = MATH_SinF32(hx);
    DT_f32 cy = MATH_CosF32(hy);
    DT_f32 sy = MATH_SinF32(hy);
    DT_f32 cz = MATH_CosF32(hz);
    DT_f32 sz = MATH_SinF32(hz);
    MATH_Quat quat;

    quat.x = (sx * cy * cz) + (cx * sy * sz);
    quat.y = (cx * sy * cz) - (sx * cy * sz);
    quat.z = (cx * cy * sz) - (sx * sy * cz);
    quat.w = (cx * cy * cz) + (sx * sy * sz);

    return quat;
}

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerZYX(MATH_EulerAngle angles) {
    DT_f32 hx = angles.x_rad * 0.5f;
    DT_f32 hy = angles.y_rad * 0.5f;
    DT_f32 hz = angles.z_rad * 0.5f;
    DT_f32 cx = MATH_CosF32(hx);
    DT_f32 sx = MATH_SinF32(hx);
    DT_f32 cy = MATH_CosF32(hy);
    DT_f32 sy = MATH_SinF32(hy);
    DT_f32 cz = MATH_CosF32(hz);
    DT_f32 sz = MATH_SinF32(hz);
    MATH_Quat quat;

    quat.x = (sx * cy * cz) - (cx * sy * sz);
    quat.y = (cx * sy * cz) + (sx * cy * sz);
    quat.z = (cx * cy * sz) - (sx * sy * cz);
    quat.w = (cx * cy * cz) + (sx * sy * sz);

    return quat;
}

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerXZY(MATH_EulerAngle angles) {
    DT_f32 hx = angles.x_rad * 0.5f;
    DT_f32 hy = angles.y_rad * 0.5f;
    DT_f32 hz = angles.z_rad * 0.5f;
    DT_f32 cx = MATH_CosF32(hx);
    DT_f32 sx = MATH_SinF32(hx);
    DT_f32 cy = MATH_CosF32(hy);
    DT_f32 sy = MATH_SinF32(hy);
    DT_f32 cz = MATH_CosF32(hz);
    DT_f32 sz = MATH_SinF32(hz);
    MATH_Quat quat;

    quat.x = (sx * cy * cz) - (cx * sy * sz);
    quat.y = (cx * sy * cz) - (sx * cy * sz);
    quat.z = (cx * cy * sz) + (sx * sy * cz);
    quat.w = (cx * cy * cz) + (sx * sy * sz);

    return quat;
}

/* ----  BASIC OPS ---- */

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatMul(MATH_Quat a, MATH_Quat b) {
    MATH_Quat quat;

    quat.x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
    quat.y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
    quat.z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
    quat.w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);

    return quat;
}

/* ----  BASIC ALGEBRA  ---- */

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatExp(MATH_Quat x) {
    DT_f32 exp_w = MATH_ExpF32(x.w);
    DT_f32 v_len = MATH_SqrtF32((x.x * x.x) + (x.y * x.y) + (x.z * x.z));
    DT_f32 v_const;
    /*
     * Document this behavior since lim(x - > 0) (sin(x)/x) = 1
     */
    if (MATH_IsZeroF32(v_len)) {
        v_const = exp_w;
    } else {
        v_const = (MATH_SinF32(v_len) * exp_w) / v_len;
    }

    x.x *= v_const;
    x.y *= v_const;
    x.z *= v_const;
    x.w = MATH_CosF32(v_len) * exp_w;

    return x;
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatLog(MATH_Quat x) {
    DT_f32 v_len_sq = (x.x * x.x) + (x.y * x.y) + (x.z * x.z);
    DT_f32 q_len = MATH_SqrtF32(v_len_sq + (x.w * x.w));

    // Assuming q_len is != 0 which means x.w != 0.
    DT_f32 acos_term = MATH_ACosF32(MATH_ClampF32(x.w / q_len, -1, 1));
    DT_f32 v_const;
    if (MATH_IsZeroF32(v_len_sq)) {
        v_const = 1.0f;
    } else {
        v_const = acos_term / MATH_SqrtF32(v_len_sq);
    }

    x.x *= v_const;
    x.y *= v_const;
    x.z *= v_const;
    x.w = MATH_LogF32(q_len);

    return x;
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatLogSafe(MATH_Quat x,
                                                  MATH_Quat fallback) {
    DT_f32 q_len_sq = (x.x * x.x) + (x.y * x.y) + (x.z * x.z) + (x.w * x.w);
    if (MATH_IsZeroF32(q_len_sq)) {
        return fallback;
    }

    return MATH_QuatLog(x);
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatPow(MATH_Quat x, DT_f32 y) {
    return MATH_QuatExp(MATH_QuatScalarMul(MATH_QuatLog(x), y));
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatPowSafe(MATH_Quat x, DT_f32 y,
                                                  MATH_Quat fallback) {
    DT_f32 q_len_sq = (x.x * x.x) + (x.y * x.y) + (x.z * x.z) + (x.w * x.w);
    if (MATH_IsZeroF32(q_len_sq)) {
        return fallback;
    }

    return MATH_QuatPow(x, y);
}

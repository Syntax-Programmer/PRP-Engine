#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes/Typedefs.h"
#include "Diagnostics/Assert.h"
#include "Math/EulerAngle.h"
#include "Math/Vector/Vec3.h"
#include "Utils/Defs.h"

typedef struct {
    DT_f32 x;
    DT_f32 y;
    DT_f32 z;
    DT_f32 w;
} MATH_Quat;

/* ----  CONSTRUCTORS ---- */

static inline MATH_Quat MATH_QuatCreateVec3Scalar(MATH_Vec3 v, DT_f32 w) {
    return (MATH_Quat){.x = v.x, .y = v.y, .z = v.z, .w = w};
}

static inline MATH_Quat MATH_QuatCreateIdentity(DT_void) {
    return (MATH_Quat){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
}

static inline MATH_Quat MATH_QuatCreateScalar(DT_f32 w) {
    return (MATH_Quat){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = w};
}

static inline MATH_Quat MATH_QuatCreateAxisX(DT_f32 rad) {
    DT_f32 half = rad * 0.5f;
    DT_f32 s = MATH_SinF32(half);
    DT_f32 c = MATH_CosF32(half);

    return (MATH_Quat){.x = s, .y = 0.0f, .z = 0.0f, .w = c};
}

static inline MATH_Quat MATH_QuatCreateAxisY(DT_f32 rad) {
    DT_f32 half = rad * 0.5f;
    DT_f32 s = MATH_SinF32(half);
    DT_f32 c = MATH_CosF32(half);

    return (MATH_Quat){.x = 0.0f, .y = s, .z = 0.0f, .w = c};
}

static inline MATH_Quat MATH_QuatCreateAxisZ(DT_f32 rad) {
    DT_f32 half = rad * 0.5f;
    DT_f32 s = MATH_SinF32(half);
    DT_f32 c = MATH_CosF32(half);

    return (MATH_Quat){.x = 0.0f, .y = 0.0f, .z = s, .w = c};
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatCreateAxis(MATH_Vec3 axis,
                                                     DT_f32 rad);
PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatCreateAxisSafe(MATH_Vec3 axis,
                                                         DT_f32 rad,
                                                         MATH_Quat fallback);

PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerXYZ(MATH_EulerAngle angles);
PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerZXY(MATH_EulerAngle angles);
PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerYZX(MATH_EulerAngle angles);
PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerYXZ(MATH_EulerAngle angles);
PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerZYX(MATH_EulerAngle angles);
PRP_FN_API MATH_Quat PRP_FN_CALL
MATH_QuatCreateEulerXZY(MATH_EulerAngle angles);

static inline MATH_Quat MATH_QuatCreateEuler(MATH_EulerAngle angles,
                                             MATH_EulerAngleOrder order) {
    switch (order) {
    case MATH_EULER_ANGLE_ORDER_XYZ:
        return MATH_QuatCreateEulerXYZ(angles);
    case MATH_EULER_ANGLE_ORDER_ZXY:
        return MATH_QuatCreateEulerZXY(angles);
    case MATH_EULER_ANGLE_ORDER_YZX:
        return MATH_QuatCreateEulerYZX(angles);
    case MATH_EULER_ANGLE_ORDER_YXZ:
        return MATH_QuatCreateEulerYXZ(angles);
    case MATH_EULER_ANGLE_ORDER_ZYX:
        return MATH_QuatCreateEulerZYX(angles);
    case MATH_EULER_ANGLE_ORDER_XZY:
        return MATH_QuatCreateEulerXZY(angles);
    default:
        DIAG_ASSERT(0 && "Invalid MATH_EulerOrder provided.");
        // Identity matrix.
        return MATH_QuatCreateIdentity();
    }
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline DT_bool MATH_QuatEq(MATH_Quat a, MATH_Quat b) {
    return (DT_bool)(a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

static inline DT_bool MATH_QuatAlmostEq(MATH_Quat a, MATH_Quat b) {
    return (DT_bool)(MATH_AlmostEqF32(a.x, b.x) && MATH_AlmostEqF32(a.y, b.y) &&
                     MATH_AlmostEqF32(a.z, b.z) && MATH_AlmostEqF32(a.w, b.w));
}

static inline DT_bool MATH_QuatIsZero(MATH_Quat a) {
    return (DT_bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y) &&
                     MATH_IsZeroF32(a.z) && MATH_IsZeroF32(a.w));
}

static inline DT_bool MATH_QuatIsNaN(MATH_Quat a) {
    return (DT_bool)(MATH_IsNaNF32(a.x) || MATH_IsNaNF32(a.y) ||
                     MATH_IsNaNF32(a.z) || MATH_IsNaNF32(a.w));
}

static inline DT_bool MATH_QuatIsInf(MATH_Quat a) {
    return (DT_bool)(MATH_IsInfF32(a.x) || MATH_IsInfF32(a.y) ||
                     MATH_IsInfF32(a.z) || MATH_IsInfF32(a.w));
}

static inline DT_bool MATH_QuatIsIdentity(MATH_Quat a) {
    return (DT_bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y) &&
                     MATH_IsZeroF32(a.z) && MATH_AlmostEqF32(a.w, 1.0f));
}

static inline DT_bool MATH_QuatRotationEq(MATH_Quat a, MATH_Quat b) {
    return MATH_AlmostEqF32(
        MATH_AbsF32((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w)),
        1.0f);
}

/* ----  BASIC OPS ---- */

static inline MATH_Quat MATH_QuatNeg(MATH_Quat a) {
    return (MATH_Quat){.x = -a.x, .y = -a.y, .z = -a.z, .w = -a.w};
}

static inline MATH_Quat MATH_QuatConjugate(MATH_Quat a) {
    return (MATH_Quat){.x = -a.x, .y = -a.y, .z = -a.z, .w = a.w};
}

#define QUAT_INTERNAL_DEFINE_ELEMS_OP(op_name, op)                             \
    static inline MATH_Quat MATH_QuatElems##op_name(MATH_Quat a,               \
                                                    MATH_Quat b) {             \
        return (MATH_Quat){                                                    \
            .x = (a.x)op(b.x),                                                 \
            .y = (a.y)op(b.y),                                                 \
            .z = (a.z)op(b.z),                                                 \
            .w = (a.w)op(b.w),                                                 \
        };                                                                     \
    }

QUAT_INTERNAL_DEFINE_ELEMS_OP(Add, +);
QUAT_INTERNAL_DEFINE_ELEMS_OP(Sub, -);
QUAT_INTERNAL_DEFINE_ELEMS_OP(Mul, *);
QUAT_INTERNAL_DEFINE_ELEMS_OP(Div, /);

#undef QUAT_INTERNAL_DEFINE_ELEMS_OP

static inline MATH_Quat MATH_QuatElemsDivSafe(MATH_Quat a, MATH_Quat b,
                                              DT_f32 fallback) {
    return (MATH_Quat){
        .x = MATH_SafeDivF32(a.x, b.x, fallback),
        .y = MATH_SafeDivF32(a.y, b.y, fallback),
        .z = MATH_SafeDivF32(a.z, b.z, fallback),
        .w = MATH_SafeDivF32(a.w, b.w, fallback),
    };
}

#define QUAT_INTERNAL_DEFINE_SCALAR_OP(op_name, op)                            \
    static inline MATH_Quat MATH_QuatScalar##op_name(MATH_Quat a, DT_f32 s) {  \
        return (MATH_Quat){                                                    \
            .x = (a.x)op(s),                                                   \
            .y = (a.y)op(s),                                                   \
            .z = (a.z)op(s),                                                   \
            .w = (a.w)op(s),                                                   \
        };                                                                     \
    }

QUAT_INTERNAL_DEFINE_SCALAR_OP(Add, +);
QUAT_INTERNAL_DEFINE_SCALAR_OP(Sub, -);
QUAT_INTERNAL_DEFINE_SCALAR_OP(Mul, *);
QUAT_INTERNAL_DEFINE_SCALAR_OP(Div, /);

#undef QUAT_INTERNAL_DEFINE_SCALAR_OP

static inline MATH_Quat MATH_QuatScalarDivSafe(MATH_Quat a, DT_f32 s,
                                               MATH_Quat fallback) {
    if (MATH_IsZeroF32(s)) {
        return fallback;
    }

    return MATH_QuatScalarDiv(a, s);
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatMul(MATH_Quat a, MATH_Quat b);

/* ----  BASIC ALGEBRA  ---- */

static inline DT_f32 MATH_QuatDot(MATH_Quat a, MATH_Quat b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

static inline DT_f32 MATH_QuatLen(MATH_Quat a) {
    return MATH_SqrtF32(MATH_QuatDot(a, a));
}

static inline DT_f32 MATH_QuatLenSq(MATH_Quat a) { return MATH_QuatDot(a, a); }

static inline MATH_Quat MATH_QuatNormalize(MATH_Quat a) {
    DT_f32 inv_len = 1.0f / MATH_QuatLen(a);

    return MATH_QuatScalarMul(a, inv_len);
}

static inline MATH_Quat MATH_QuatNormalizeSafe(MATH_Quat a,
                                               MATH_Quat fallback) {
    DT_f32 len = MATH_QuatLen(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }

    return MATH_QuatScalarMul(a, 1.0f / len);
}

static inline DT_bool MATH_QuatIsNormalized(MATH_Quat a) {
    return (DT_bool)(MATH_AlmostEqF32(MATH_QuatLenSq(a), 1.0f));
}

static inline MATH_Quat MATH_QuatInv(MATH_Quat a) {
    DT_f32 len_sq = MATH_QuatLenSq(a);

    return (MATH_AlmostEqF32(len_sq, 1.0f))
               ? MATH_QuatConjugate(a)
               : MATH_QuatScalarMul(MATH_QuatConjugate(a), 1.0f / len_sq);
}

PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatExp(MATH_Quat x);
PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatLog(MATH_Quat x);
PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatLogSafe(MATH_Quat x,
                                                  MATH_Quat fallback);
PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatPow(MATH_Quat x, DT_f32 y);
PRP_FN_API MATH_Quat PRP_FN_CALL MATH_QuatPowSafe(MATH_Quat x, DT_f32 y,
                                                  MATH_Quat fallback);

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

static inline MATH_Vec3 MATH_QuatRight(MATH_Quat q) {
    DT_f32 yy = q.y * q.y;
    DT_f32 zz = q.z * q.z;
    DT_f32 xy = q.x * q.y;
    DT_f32 xz = q.x * q.z;
    DT_f32 wy = q.w * q.y;
    DT_f32 wz = q.w * q.z;

    return (MATH_Vec3){
        .x = 1.0f - (2.0f * (yy + zz)),
        .y = 2.0f * (xy + wz),
        .z = 2.0f * (xz - wy),
    };
}

static inline DT_f32 MATH_QuatRightLen(MATH_Quat q) {
    DT_f32 yy = q.y * q.y;
    DT_f32 zz = q.z * q.z;
    DT_f32 xy = q.x * q.y;
    DT_f32 xz = q.x * q.z;
    DT_f32 wy = q.w * q.y;
    DT_f32 wz = q.w * q.z;

    DT_f32 x = 1.0f - (2.0f * (yy + zz));
    DT_f32 y = 2.0f * (xy + wz);
    DT_f32 z = 2.0f * (xz - wy);

    return MATH_SqrtF32((x * x) + (y * y) + (z * z));
}

static inline DT_f32 MATH_QuatRightLenSq(MATH_Quat q) {
    DT_f32 yy = q.y * q.y;
    DT_f32 zz = q.z * q.z;
    DT_f32 xy = q.x * q.y;
    DT_f32 xz = q.x * q.z;
    DT_f32 wy = q.w * q.y;
    DT_f32 wz = q.w * q.z;

    DT_f32 x = 1.0f - (2.0f * (yy + zz));
    DT_f32 y = 2.0f * (xy + wz);
    DT_f32 z = 2.0f * (xz - wy);

    return (x * x) + (y * y) + (z * z);
}

static inline MATH_Vec3 MATH_QuatUp(MATH_Quat q) {
    DT_f32 xx = q.x * q.x;
    DT_f32 zz = q.z * q.z;
    DT_f32 xy = q.x * q.y;
    DT_f32 yz = q.y * q.z;
    DT_f32 wx = q.w * q.x;
    DT_f32 wz = q.w * q.z;

    return (MATH_Vec3){
        .x = 2.0f * (xy - wz),
        .y = 1.0f - (2.0f * (xx + zz)),
        .z = 2.0f * (yz + wx),
    };
}

static inline DT_f32 MATH_QuatUpLen(MATH_Quat q) {
    DT_f32 xx = q.x * q.x;
    DT_f32 zz = q.z * q.z;
    DT_f32 xy = q.x * q.y;
    DT_f32 yz = q.y * q.z;
    DT_f32 wx = q.w * q.x;
    DT_f32 wz = q.w * q.z;

    DT_f32 x = 2.0f * (xy - wz);
    DT_f32 y = 1.0f - (2.0f * (xx + zz));
    DT_f32 z = 2.0f * (yz + wx);

    return MATH_SqrtF32((x * x) + (y * y) + (z * z));
}

static inline DT_f32 MATH_QuatUpLenSq(MATH_Quat q) {
    DT_f32 xx = q.x * q.x;
    DT_f32 zz = q.z * q.z;
    DT_f32 xy = q.x * q.y;
    DT_f32 yz = q.y * q.z;
    DT_f32 wx = q.w * q.x;
    DT_f32 wz = q.w * q.z;

    DT_f32 x = 2.0f * (xy - wz);
    DT_f32 y = 1.0f - (2.0f * (xx + zz));
    DT_f32 z = 2.0f * (yz + wx);

    return (x * x) + (y * y) + (z * z);
}

static inline MATH_Vec3 MATH_QuatForward(MATH_Quat q) {
    DT_f32 xx = q.x * q.x;
    DT_f32 yy = q.y * q.y;
    DT_f32 xz = q.x * q.z;
    DT_f32 yz = q.y * q.z;
    DT_f32 wx = q.w * q.x;
    DT_f32 wy = q.w * q.y;

    return (MATH_Vec3){
        .x = 2.0f * (xz + wy),
        .y = 2.0f * (yz - wx),
        .z = 1.0f - (2.0f * (xx + yy)),
    };
}

static inline DT_f32 MATH_QuatForwardLen(MATH_Quat q) {
    DT_f32 xx = q.x * q.x;
    DT_f32 yy = q.y * q.y;
    DT_f32 xz = q.x * q.z;
    DT_f32 yz = q.y * q.z;
    DT_f32 wx = q.w * q.x;
    DT_f32 wy = q.w * q.y;

    DT_f32 x = 2.0f * (xz + wy);
    DT_f32 y = 2.0f * (yz - wx);
    DT_f32 z = 1.0f - (2.0f * (xx + yy));

    return MATH_SqrtF32((x * x) + (y * y) + (z * z));
}

static inline DT_f32 MATH_QuatForwardLenSq(MATH_Quat q) {
    DT_f32 xx = q.x * q.x;
    DT_f32 yy = q.y * q.y;
    DT_f32 xz = q.x * q.z;
    DT_f32 yz = q.y * q.z;
    DT_f32 wx = q.w * q.x;
    DT_f32 wy = q.w * q.y;

    DT_f32 x = 2.0f * (xz + wy);
    DT_f32 y = 2.0f * (yz - wx);
    DT_f32 z = 1.0f - (2.0f * (xx + yy));

    return (x * x) + (y * y) + (z * z);
}

// QuatToAxisAngle

// QuatToEuler

// QuatFromMat3
// QuatFromMat4

// QuatToMat3
// QuatToMat4

// QuatRotateVec3
// QuatLookRotation
// QuatRotateAroundAxis(q, axis, angle)

// DualQuaternion
// SwingTwist
/*
 But SLERP/NLERP should internally use:

 if(dot < 0)
     b = -b;

 Otherwise interpolation may take the long rotational arc.

 You may want an explicit helper:

 QuatEnsureShortestPath(a, b)

 Optional but useful.
 */
// QuatNLerp
// QuatSLerp
// QuatSquad

#ifdef __cplusplus
}
#endif

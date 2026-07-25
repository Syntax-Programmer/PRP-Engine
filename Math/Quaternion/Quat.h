#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Types.h"
#include "Diagnostics/Assert.h"
#include "Math/EulerAngle.h"
#include "Math/Vector/Vec3.h"

typedef struct {
    PRP_F32 x;
    PRP_F32 y;
    PRP_F32 z;
    PRP_F32 w;
} MATH_Quat;

/* ----  CONSTRUCTORS ---- */

static inline MATH_Quat MATH_QuatCreateVec3Scalar(MATH_Vec3 v, PRP_F32 w) {
    return (MATH_Quat){.x = v.x, .y = v.y, .z = v.z, .w = w};
}

static inline MATH_Quat MATH_QuatCreateIdentity(void) {
    return (MATH_Quat){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
}

static inline MATH_Quat MATH_QuatCreateScalar(PRP_F32 w) {
    return (MATH_Quat){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = w};
}

static inline MATH_Quat MATH_QuatCreateAxisX(PRP_F32 rad) {
    PRP_F32 half = rad * 0.5f;
    PRP_F32 s = MATH_SinF32(half);
    PRP_F32 c = MATH_CosF32(half);

    return (MATH_Quat){.x = s, .y = 0.0f, .z = 0.0f, .w = c};
}

static inline MATH_Quat MATH_QuatCreateAxisY(PRP_F32 rad) {
    PRP_F32 half = rad * 0.5f;
    PRP_F32 s = MATH_SinF32(half);
    PRP_F32 c = MATH_CosF32(half);

    return (MATH_Quat){.x = 0.0f, .y = s, .z = 0.0f, .w = c};
}

static inline MATH_Quat MATH_QuatCreateAxisZ(PRP_F32 rad) {
    PRP_F32 half = rad * 0.5f;
    PRP_F32 s = MATH_SinF32(half);
    PRP_F32 c = MATH_CosF32(half);

    return (MATH_Quat){.x = 0.0f, .y = 0.0f, .z = s, .w = c};
}

PRP_API MATH_Quat PRP_CALL MATH_QuatCreateAxis(MATH_Vec3 axis, PRP_F32 rad);
PRP_API MATH_Quat PRP_CALL MATH_QuatCreateAxisSafe(MATH_Vec3 axis,
                                                      PRP_F32 rad,
                                                      MATH_Quat fallback);

PRP_API MATH_Quat PRP_CALL MATH_QuatCreateEulerXYZ(MATH_EulerAngle angles);
PRP_API MATH_Quat PRP_CALL MATH_QuatCreateEulerZXY(MATH_EulerAngle angles);
PRP_API MATH_Quat PRP_CALL MATH_QuatCreateEulerYZX(MATH_EulerAngle angles);
PRP_API MATH_Quat PRP_CALL MATH_QuatCreateEulerYXZ(MATH_EulerAngle angles);
PRP_API MATH_Quat PRP_CALL MATH_QuatCreateEulerZYX(MATH_EulerAngle angles);
PRP_API MATH_Quat PRP_CALL MATH_QuatCreateEulerXZY(MATH_EulerAngle angles);

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

static inline PRP_Bool MATH_QuatEq(MATH_Quat a, MATH_Quat b) {
    return (PRP_Bool)(a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

static inline PRP_Bool MATH_QuatAlmostEq(MATH_Quat a, MATH_Quat b) {
    return (PRP_Bool)(MATH_AlmostEqF32(a.x, b.x) &&
                      MATH_AlmostEqF32(a.y, b.y) &&
                      MATH_AlmostEqF32(a.z, b.z) && MATH_AlmostEqF32(a.w, b.w));
}

static inline PRP_Bool MATH_QuatIsZero(MATH_Quat a) {
    return (PRP_Bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y) &&
                      MATH_IsZeroF32(a.z) && MATH_IsZeroF32(a.w));
}

static inline PRP_Bool MATH_QuatIsNaN(MATH_Quat a) {
    return (PRP_Bool)(MATH_IsNaNF32(a.x) || MATH_IsNaNF32(a.y) ||
                      MATH_IsNaNF32(a.z) || MATH_IsNaNF32(a.w));
}

static inline PRP_Bool MATH_QuatIsInf(MATH_Quat a) {
    return (PRP_Bool)(MATH_IsInfF32(a.x) || MATH_IsInfF32(a.y) ||
                      MATH_IsInfF32(a.z) || MATH_IsInfF32(a.w));
}

static inline PRP_Bool MATH_QuatIsIdentity(MATH_Quat a) {
    return (PRP_Bool)(MATH_IsZeroF32(a.x) && MATH_IsZeroF32(a.y) &&
                      MATH_IsZeroF32(a.z) && MATH_AlmostEqF32(a.w, 1.0f));
}

static inline PRP_Bool MATH_QuatRotationEq(MATH_Quat a, MATH_Quat b) {
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
                                              PRP_F32 fallback) {
    return (MATH_Quat){
        .x = MATH_SafeDivF32(a.x, b.x, fallback),
        .y = MATH_SafeDivF32(a.y, b.y, fallback),
        .z = MATH_SafeDivF32(a.z, b.z, fallback),
        .w = MATH_SafeDivF32(a.w, b.w, fallback),
    };
}

#define QUAT_INTERNAL_DEFINE_SCALAR_OP(op_name, op)                            \
    static inline MATH_Quat MATH_QuatScalar##op_name(MATH_Quat a, PRP_F32 s) { \
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

static inline MATH_Quat MATH_QuatScalarDivSafe(MATH_Quat a, PRP_F32 s,
                                               MATH_Quat fallback) {
    if (MATH_IsZeroF32(s)) {
        return fallback;
    }

    return MATH_QuatScalarDiv(a, s);
}

PRP_API MATH_Quat PRP_CALL MATH_QuatMul(MATH_Quat a, MATH_Quat b);

/* ----  BASIC ALGEBRA  ---- */

static inline PRP_F32 MATH_QuatDot(MATH_Quat a, MATH_Quat b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

static inline PRP_F32 MATH_QuatLen(MATH_Quat a) {
    return MATH_SqrtF32(MATH_QuatDot(a, a));
}

static inline PRP_F32 MATH_QuatLenSq(MATH_Quat a) { return MATH_QuatDot(a, a); }

static inline MATH_Quat MATH_QuatNormalize(MATH_Quat a) {
    PRP_F32 inv_len = 1.0f / MATH_QuatLen(a);

    return MATH_QuatScalarMul(a, inv_len);
}

static inline MATH_Quat MATH_QuatNormalizeSafe(MATH_Quat a,
                                               MATH_Quat fallback) {
    PRP_F32 len = MATH_QuatLen(a);
    if (MATH_IsZeroF32(len)) {
        return fallback;
    }

    return MATH_QuatScalarMul(a, 1.0f / len);
}

static inline PRP_Bool MATH_QuatIsNormalized(MATH_Quat a) {
    return (PRP_Bool)(MATH_AlmostEqF32(MATH_QuatLenSq(a), 1.0f));
}

static inline MATH_Quat MATH_QuatInv(MATH_Quat a) {
    PRP_F32 len_sq = MATH_QuatLenSq(a);

    return (MATH_AlmostEqF32(len_sq, 1.0f))
               ? MATH_QuatConjugate(a)
               : MATH_QuatScalarMul(MATH_QuatConjugate(a), 1.0f / len_sq);
}

PRP_API MATH_Quat PRP_CALL MATH_QuatExp(MATH_Quat x);
PRP_API MATH_Quat PRP_CALL MATH_QuatLog(MATH_Quat x);
PRP_API MATH_Quat PRP_CALL MATH_QuatLogSafe(MATH_Quat x, MATH_Quat fallback);
PRP_API MATH_Quat PRP_CALL MATH_QuatPow(MATH_Quat x, PRP_F32 y);
PRP_API MATH_Quat PRP_CALL MATH_QuatPowSafe(MATH_Quat x, PRP_F32 y,
                                               MATH_Quat fallback);

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

static inline MATH_Vec3 MATH_QuatRight(MATH_Quat q) {
    PRP_F32 yy = q.y * q.y;
    PRP_F32 zz = q.z * q.z;
    PRP_F32 xy = q.x * q.y;
    PRP_F32 xz = q.x * q.z;
    PRP_F32 wy = q.w * q.y;
    PRP_F32 wz = q.w * q.z;

    return (MATH_Vec3){
        .x = 1.0f - (2.0f * (yy + zz)),
        .y = 2.0f * (xy + wz),
        .z = 2.0f * (xz - wy),
    };
}

static inline PRP_F32 MATH_QuatRightLen(MATH_Quat q) {
    PRP_F32 yy = q.y * q.y;
    PRP_F32 zz = q.z * q.z;
    PRP_F32 xy = q.x * q.y;
    PRP_F32 xz = q.x * q.z;
    PRP_F32 wy = q.w * q.y;
    PRP_F32 wz = q.w * q.z;

    PRP_F32 x = 1.0f - (2.0f * (yy + zz));
    PRP_F32 y = 2.0f * (xy + wz);
    PRP_F32 z = 2.0f * (xz - wy);

    return MATH_SqrtF32((x * x) + (y * y) + (z * z));
}

static inline PRP_F32 MATH_QuatRightLenSq(MATH_Quat q) {
    PRP_F32 yy = q.y * q.y;
    PRP_F32 zz = q.z * q.z;
    PRP_F32 xy = q.x * q.y;
    PRP_F32 xz = q.x * q.z;
    PRP_F32 wy = q.w * q.y;
    PRP_F32 wz = q.w * q.z;

    PRP_F32 x = 1.0f - (2.0f * (yy + zz));
    PRP_F32 y = 2.0f * (xy + wz);
    PRP_F32 z = 2.0f * (xz - wy);

    return (x * x) + (y * y) + (z * z);
}

static inline MATH_Vec3 MATH_QuatUp(MATH_Quat q) {
    PRP_F32 xx = q.x * q.x;
    PRP_F32 zz = q.z * q.z;
    PRP_F32 xy = q.x * q.y;
    PRP_F32 yz = q.y * q.z;
    PRP_F32 wx = q.w * q.x;
    PRP_F32 wz = q.w * q.z;

    return (MATH_Vec3){
        .x = 2.0f * (xy - wz),
        .y = 1.0f - (2.0f * (xx + zz)),
        .z = 2.0f * (yz + wx),
    };
}

static inline PRP_F32 MATH_QuatUpLen(MATH_Quat q) {
    PRP_F32 xx = q.x * q.x;
    PRP_F32 zz = q.z * q.z;
    PRP_F32 xy = q.x * q.y;
    PRP_F32 yz = q.y * q.z;
    PRP_F32 wx = q.w * q.x;
    PRP_F32 wz = q.w * q.z;

    PRP_F32 x = 2.0f * (xy - wz);
    PRP_F32 y = 1.0f - (2.0f * (xx + zz));
    PRP_F32 z = 2.0f * (yz + wx);

    return MATH_SqrtF32((x * x) + (y * y) + (z * z));
}

static inline PRP_F32 MATH_QuatUpLenSq(MATH_Quat q) {
    PRP_F32 xx = q.x * q.x;
    PRP_F32 zz = q.z * q.z;
    PRP_F32 xy = q.x * q.y;
    PRP_F32 yz = q.y * q.z;
    PRP_F32 wx = q.w * q.x;
    PRP_F32 wz = q.w * q.z;

    PRP_F32 x = 2.0f * (xy - wz);
    PRP_F32 y = 1.0f - (2.0f * (xx + zz));
    PRP_F32 z = 2.0f * (yz + wx);

    return (x * x) + (y * y) + (z * z);
}

static inline MATH_Vec3 MATH_QuatForward(MATH_Quat q) {
    PRP_F32 xx = q.x * q.x;
    PRP_F32 yy = q.y * q.y;
    PRP_F32 xz = q.x * q.z;
    PRP_F32 yz = q.y * q.z;
    PRP_F32 wx = q.w * q.x;
    PRP_F32 wy = q.w * q.y;

    return (MATH_Vec3){
        .x = 2.0f * (xz + wy),
        .y = 2.0f * (yz - wx),
        .z = 1.0f - (2.0f * (xx + yy)),
    };
}

static inline PRP_F32 MATH_QuatForwardLen(MATH_Quat q) {
    PRP_F32 xx = q.x * q.x;
    PRP_F32 yy = q.y * q.y;
    PRP_F32 xz = q.x * q.z;
    PRP_F32 yz = q.y * q.z;
    PRP_F32 wx = q.w * q.x;
    PRP_F32 wy = q.w * q.y;

    PRP_F32 x = 2.0f * (xz + wy);
    PRP_F32 y = 2.0f * (yz - wx);
    PRP_F32 z = 1.0f - (2.0f * (xx + yy));

    return MATH_SqrtF32((x * x) + (y * y) + (z * z));
}

static inline PRP_F32 MATH_QuatForwardLenSq(MATH_Quat q) {
    PRP_F32 xx = q.x * q.x;
    PRP_F32 yy = q.y * q.y;
    PRP_F32 xz = q.x * q.z;
    PRP_F32 yz = q.y * q.z;
    PRP_F32 wx = q.w * q.x;
    PRP_F32 wy = q.w * q.y;

    PRP_F32 x = 2.0f * (xz + wy);
    PRP_F32 y = 2.0f * (yz - wx);
    PRP_F32 z = 1.0f - (2.0f * (xx + yy));

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

#pragma once

#include <cmath>
#ifdef __cplusplus
extern "C" {
#endif

#include "../../Diagnostics/Assert.h"
#include "../Vector/Vec3.h"
#include "../Vector/Vec4.h"
#include "Mat3.h"

#define MATH_MAT4_SIZE (4)
#define MATH_MAT4_ELEM_COUNT (16)

// Its a column major matrix.
typedef struct {
    DT_f32 m[MATH_MAT4_ELEM_COUNT];
} MATH_Mat4;

/* ----  CONSTRUCTORS  ---- */

static inline MATH_Mat4 MATH_Mat4CreateZero(DT_void) { return (MATH_Mat4){0}; }

static inline MATH_Mat4 MATH_Mat4CreateIdentity(DT_void) {
    return (MATH_Mat4){.m = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateDiagScalar(DT_f32 s) {
    return (MATH_Mat4){.m = {s, 0.0f, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f,
                             0.0f, s, 0.0f, 0.0f, 0.0f, 0.0f, s}};
}

static inline MATH_Mat4 MATH_Mat4CreateFillScalar(DT_f32 s) {
    return (MATH_Mat4){.m = {s, s, s, s, s, s, s, s, s, s, s, s, s, s, s, s}};
}

static inline MATH_Mat4 MATH_Mat4CreateFromCols(MATH_Vec4 c1, MATH_Vec4 c2,
                                                MATH_Vec4 c3, MATH_Vec4 c4) {
    return (MATH_Mat4){.m = {c1.x, c1.y, c1.z, c1.w, c2.x, c2.y, c2.z, c2.w,
                             c3.x, c3.y, c3.z, c3.w, c4.x, c4.y, c4.z, c4.w}};
}

static inline MATH_Mat4 MATH_Mat4CreateFromRows(MATH_Vec4 r1, MATH_Vec4 r2,
                                                MATH_Vec4 r3, MATH_Vec4 r4) {
    return (MATH_Mat4){.m = {r1.x, r2.x, r3.x, r4.x, r1.y, r2.y, r3.y, r4.y,
                             r1.z, r2.z, r3.z, r4.z, r1.w, r2.w, r3.w, r4.w}};
}

static inline MATH_Mat4 MATH_Mat4CreateTranslation(MATH_Vec4 pos) {
    return (MATH_Mat4){.m = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f, pos.x, pos.y, pos.z,
                             1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateRotationAxis(MATH_Vec3 axis,
                                                    DT_f32 angle) {
    axis = MATH_Vec3Normalize(axis);
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);
    DT_f32 t = 1.0f - c;

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = t * axis.x * axis.x + c;
    m.m[1] = t * axis.x * axis.y + s * axis.z;
    m.m[2] = t * axis.x * axis.z - s * axis.y;

    m.m[4] = t * axis.x * axis.y - s * axis.z;
    m.m[5] = t * axis.y * axis.y + c;
    m.m[6] = t * axis.y * axis.z + s * axis.x;

    m.m[8] = t * axis.x * axis.z + s * axis.y;
    m.m[9] = t * axis.y * axis.z - s * axis.x;
    m.m[10] = t * axis.z * axis.z + c;

    return m;
}

static inline MATH_Mat4 MATH_Mat4CreateRotationAxisX(DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Mat4){.m = {1.0f, 0.0f, 0.0f, 0.0f,

                             0.0f, c, s, 0.0f,

                             0.0f, -s, c, 0.0f,

                             0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateRotationAxisY(DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Mat4){.m = {c, 0.0f, -s, 0.0f,

                             0.0f, 1.0f, 0.0f, 0.0f,

                             s, 0.0f, c, 0.0f,

                             0.0f, 0.0f, 0.0f, 1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateRotationAxisZ(DT_f32 angle) {
    DT_f32 c = MATH_CosF32(angle);
    DT_f32 s = MATH_SinF32(angle);

    return (MATH_Mat4){.m = {c, s, 0.0f, 0.0f,

                             -s, c, 0.0f, 0.0f,

                             0.0f, 0.0f, 1.0f, 0.0f,

                             0.0f, 0.0f, 0.0f, 1.0f}};
}

typedef enum {
    MATH_EULER_XYZ,
    MATH_EULER_ZXY,
    MATH_EULER_YZX,
    MATH_EULER_YXZ,
    MATH_EULER_ZYX,
    MATH_EULER_XZY,
} MATH_Mat4EulerRotOrder;

static inline MATH_Mat4 MATH_Mat4CreateRotationEulerXYZ(MATH_Vec3 angles) {
    DT_f32 cx = MATH_CosF32(angles.x), sx = MATH_SinF32(angles.x);
    DT_f32 cy = MATH_CosF32(angles.y), sy = MATH_SinF32(angles.y);
    DT_f32 cz = MATH_CosF32(angles.z), sz = MATH_SinF32(angles.z);

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = cy * cz;
    m.m[1] = cy * sz;
    m.m[2] = -sy;

    m.m[4] = (cz * sx * sy) - (cx * sz);
    m.m[5] = (cx * cz) + (sx * sy * sz);
    m.m[6] = cy * sx;

    m.m[8] = (sx * sz) + (cx * cz * sy);
    m.m[9] = (cx * sy * sz) - (cz * sx);
    m.m[10] = cx * cy;

    return m;
}

static inline MATH_Mat4 MATH_Mat4CreateRotationEulerZXY(MATH_Vec3 angles) {
    DT_f32 cx = MATH_CosF32(angles.x), sx = MATH_SinF32(angles.x);
    DT_f32 cy = MATH_CosF32(angles.y), sy = MATH_SinF32(angles.y);
    DT_f32 cz = MATH_CosF32(angles.z), sz = MATH_SinF32(angles.z);

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = (cx * cz) + (sx * sy * sz);
    m.m[1] = cx * sz;
    m.m[2] = -(sy * cz) + (sx * cy * sz);

    m.m[4] = (cz * sx * sy) - (cx * sz);
    m.m[5] = cx * cz;
    m.m[6] = (sy * sz) + (sx * cy * cz);

    m.m[8] = cx * sy;
    m.m[9] = -sx;
    m.m[10] = cx * cy;

    return m;
}

static inline MATH_Mat4 MATH_Mat4CreateRotationEulerYZX(MATH_Vec3 angles) {
    DT_f32 cx = MATH_CosF32(angles.x), sx = MATH_SinF32(angles.x);
    DT_f32 cy = MATH_CosF32(angles.y), sy = MATH_SinF32(angles.y);
    DT_f32 cz = MATH_CosF32(angles.z), sz = MATH_SinF32(angles.z);

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = cy * cz;
    m.m[1] = (cx * sz * cy) + (sx * sy);
    m.m[2] = (sx * sz * cy) - (cx * sy);

    m.m[4] = -sz;
    m.m[5] = cx * cz;
    m.m[6] = sx * cz;

    m.m[8] = cz * sy;
    m.m[9] = (cx * sz * sy) - (sx * cy);
    m.m[10] = (sx * sz * sy) + (cx * cy);

    return m;
}

static inline MATH_Mat4 MATH_Mat4CreateRotationEulerYXZ(MATH_Vec3 angles) {
    DT_f32 cx = MATH_CosF32(angles.x), sx = MATH_SinF32(angles.x);
    DT_f32 cy = MATH_CosF32(angles.y), sy = MATH_SinF32(angles.y);
    DT_f32 cz = MATH_CosF32(angles.z), sz = MATH_SinF32(angles.z);

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = (cy * cz) + (sx * sy * sz);
    m.m[1] = cx * sz;
    m.m[2] = (cy * sx * sz) - (cz * sy);

    m.m[4] = (cz * sx * sy) - (cy * sz);
    m.m[5] = cx * cz;
    m.m[6] = (cy * cz * sx) + (sy * sz);

    m.m[8] = cx * sy;
    m.m[9] = -sx;
    m.m[10] = cx * cy;

    return m;
}

static inline MATH_Mat4 MATH_Mat4CreateRotationEulerZYX(MATH_Vec3 angles) {
    DT_f32 cx = MATH_CosF32(angles.x), sx = MATH_SinF32(angles.x);
    DT_f32 cy = MATH_CosF32(angles.y), sy = MATH_SinF32(angles.y);
    DT_f32 cz = MATH_CosF32(angles.z), sz = MATH_SinF32(angles.z);

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = (cy * cz);
    m.m[1] = (cx * sz) + (sx * sy * cz);
    m.m[2] = (sx * sz) - (cx * sy * cz);

    m.m[4] = -(cy * sz);
    m.m[5] = (cx * cz) - (sx * sy * sz);
    m.m[6] = (sx * cz) + (cx * sy * sz);

    m.m[8] = sy;
    m.m[9] = -(sx * cy);
    m.m[10] = cx * cy;

    return m;
}

static inline MATH_Mat4 MATH_Mat4CreateRotationEulerXZY(MATH_Vec3 angles) {
    DT_f32 cx = MATH_CosF32(angles.x), sx = MATH_SinF32(angles.x);
    DT_f32 cy = MATH_CosF32(angles.y), sy = MATH_SinF32(angles.y);
    DT_f32 cz = MATH_CosF32(angles.z), sz = MATH_SinF32(angles.z);

    MATH_Mat4 m = MATH_Mat4CreateIdentity();
    m.m[0] = cy * cz;
    m.m[1] = sz;
    m.m[2] = -(cz * sy);

    m.m[4] = (sx * sy) - (cx * cy * sz);
    m.m[5] = cx * cz;
    m.m[6] = (cy * sx) + (cx * sy * sz);

    m.m[8] = (cx * sy) + (cy * sx * sz);
    m.m[9] = -(cz * sx);
    m.m[10] = (cx * cy) - (sx * sy * sz);

    return m;
}

static inline MATH_Mat4
MATH_Mat4CreateRotationEuler(MATH_Vec3 angles, MATH_Mat4EulerRotOrder order) {
    switch (order) {
    case MATH_EULER_XYZ:
        return MATH_Mat4CreateRotationEulerXYZ(angles);
    case MATH_EULER_ZXY:
        return MATH_Mat4CreateRotationEulerZXY(angles);
    case MATH_EULER_YZX:
        return MATH_Mat4CreateRotationEulerYZX(angles);
    case MATH_EULER_YXZ:
        return MATH_Mat4CreateRotationEulerYXZ(angles);
    case MATH_EULER_ZYX:
        return MATH_Mat4CreateRotationEulerZYX(angles);
    case MATH_EULER_XZY:
        return MATH_Mat4CreateRotationEulerXZY(angles);
    default:
        DIAG_ASSERT(0 && "Invalid MATH_EulerOrder provided.");
        return MATH_Mat4CreateIdentity();
    }
}

static inline MATH_Mat4 MATH_Mat4CreateScale(MATH_Vec3 scale) {
    return (MATH_Mat4){.m = {scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f,
                             0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f,
                             1.0f}};
}

static inline MATH_Mat4 MATH_Mat4CreateTRSAxis(MATH_Vec3 pos,
                                               MATH_Vec3 rot_axis,
                                               DT_f32 rot_rad,
                                               MATH_Vec3 scale) {
    MATH_Mat4 m = MATH_Mat4CreateRotationAxis(rot_axis, rot_rad);
    // applying scale on it.
    m.m[0] *= scale.x;
    m.m[1] *= scale.x;
    m.m[2] *= scale.x;
    m.m[4] *= scale.y;
    m.m[5] *= scale.y;
    m.m[6] *= scale.y;
    m.m[8] *= scale.z;
    m.m[9] *= scale.z;
    m.m[10] *= scale.z;

    // Adding traslation.
    m.m[12] = pos.x;
    m.m[13] = pos.y;
    m.m[14] = pos.z;

    return m;
}

static inline MATH_Mat4
MATH_Mat4CreateTRSEuler(MATH_Vec3 pos, MATH_Vec3 rot_angles,
                        MATH_Mat4EulerRotOrder rot_order, MATH_Vec3 scale) {
    MATH_Mat4 m = MATH_Mat4CreateRotationEuler(rot_angles, rot_order);
    // applying scale on it.
    m.m[0] *= scale.x;
    m.m[1] *= scale.x;
    m.m[2] *= scale.x;
    m.m[4] *= scale.y;
    m.m[5] *= scale.y;
    m.m[6] *= scale.y;
    m.m[8] *= scale.z;
    m.m[9] *= scale.z;
    m.m[10] *= scale.z;

    // Adding traslation.
    m.m[12] = pos.x;
    m.m[13] = pos.y;
    m.m[14] = pos.z;

    return m;
}

/* ----  COMPARE FUNCTIONS  ---- */

static inline MATH_Mat4 MATH_Mat4MinElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.m[0] = MATH_MinF32(a.m[0], b.m[0]);
    a.m[1] = MATH_MinF32(a.m[1], b.m[1]);
    a.m[2] = MATH_MinF32(a.m[2], b.m[2]);
    a.m[3] = MATH_MinF32(a.m[3], b.m[3]);
    a.m[4] = MATH_MinF32(a.m[4], b.m[4]);
    a.m[5] = MATH_MinF32(a.m[5], b.m[5]);
    a.m[6] = MATH_MinF32(a.m[6], b.m[6]);
    a.m[7] = MATH_MinF32(a.m[7], b.m[7]);
    a.m[8] = MATH_MinF32(a.m[8], b.m[8]);
    a.m[0] = MATH_MinF32(a.m[8], b.m[9]);
    a.m[1] = MATH_MinF32(a.m[10], b.m[10]);
    a.m[2] = MATH_MinF32(a.m[11], b.m[11]);
    a.m[3] = MATH_MinF32(a.m[12], b.m[12]);
    a.m[4] = MATH_MinF32(a.m[13], b.m[13]);
    a.m[5] = MATH_MinF32(a.m[14], b.m[14]);
    a.m[6] = MATH_MinF32(a.m[15], b.m[15]);

    return a;
}

static inline MATH_Mat4 MATH_Max4MaxElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.m[0] = MATH_MaxF32(a.m[0], b.m[0]);
    a.m[1] = MATH_MaxF32(a.m[1], b.m[1]);
    a.m[2] = MATH_MaxF32(a.m[2], b.m[2]);
    a.m[3] = MATH_MaxF32(a.m[3], b.m[3]);
    a.m[4] = MATH_MaxF32(a.m[4], b.m[4]);
    a.m[5] = MATH_MaxF32(a.m[5], b.m[5]);
    a.m[6] = MATH_MaxF32(a.m[6], b.m[6]);
    a.m[7] = MATH_MaxF32(a.m[7], b.m[7]);
    a.m[8] = MATH_MaxF32(a.m[8], b.m[8]);
    a.m[0] = MATH_MaxF32(a.m[8], b.m[9]);
    a.m[1] = MATH_MaxF32(a.m[10], b.m[10]);
    a.m[2] = MATH_MaxF32(a.m[11], b.m[11]);
    a.m[3] = MATH_MaxF32(a.m[12], b.m[12]);
    a.m[4] = MATH_MaxF32(a.m[13], b.m[13]);
    a.m[5] = MATH_MaxF32(a.m[14], b.m[14]);
    a.m[6] = MATH_MaxF32(a.m[15], b.m[15]);

    return a;
}

static inline DT_f32 MATH_Mat4Min(MATH_Mat4 a) {
    DT_f32 min1 = MATH_MinF32(a.m[0], a.m[1]);
    DT_f32 min2 = MATH_MinF32(a.m[2], a.m[3]);
    DT_f32 min3 = MATH_MinF32(a.m[4], a.m[5]);
    DT_f32 min4 = MATH_MinF32(a.m[6], a.m[7]);
    DT_f32 min5 = MATH_MinF32(a.m[8], a.m[9]);
    DT_f32 min6 = MATH_MinF32(a.m[10], a.m[11]);
    DT_f32 min7 = MATH_MinF32(a.m[12], a.m[13]);
    DT_f32 min8 = MATH_MinF32(a.m[14], a.m[15]);

    DT_f32 min9 = MATH_MinF32(min1, min2);
    DT_f32 min10 = MATH_MinF32(min3, min4);
    DT_f32 min11 = MATH_MinF32(min5, min6);
    DT_f32 min12 = MATH_MinF32(min7, min8);

    DT_f32 min13 = MATH_MinF32(min9, min10);
    DT_f32 min14 = MATH_MinF32(min11, min12);

    return MATH_MinF32(min13, min14);
}

static inline DT_f32 MATH_Mat4Max(MATH_Mat4 a) {
    DT_f32 min1 = MATH_MaxF32(a.m[0], a.m[1]);
    DT_f32 min2 = MATH_MaxF32(a.m[2], a.m[3]);
    DT_f32 min3 = MATH_MaxF32(a.m[4], a.m[5]);
    DT_f32 min4 = MATH_MaxF32(a.m[6], a.m[7]);
    DT_f32 min5 = MATH_MaxF32(a.m[8], a.m[9]);
    DT_f32 min6 = MATH_MaxF32(a.m[10], a.m[11]);
    DT_f32 min7 = MATH_MaxF32(a.m[12], a.m[13]);
    DT_f32 min8 = MATH_MaxF32(a.m[14], a.m[15]);

    DT_f32 min9 = MATH_MaxF32(min1, min2);
    DT_f32 min10 = MATH_MaxF32(min3, min4);
    DT_f32 min11 = MATH_MaxF32(min5, min6);
    DT_f32 min12 = MATH_MaxF32(min7, min8);

    DT_f32 min13 = MATH_MaxF32(min9, min10);
    DT_f32 min14 = MATH_MaxF32(min11, min12);

    return MATH_MaxF32(min13, min14);
}

static inline DT_bool MATH_Mat4Eq(MATH_Mat4 a, MATH_Mat4 b) {
    return (DT_bool)((a.m[0] == b.m[0]) && (a.m[1] == b.m[1]) &&
                     (a.m[2] == b.m[2]) && (a.m[3] == b.m[3]) &&
                     (a.m[4] == b.m[4]) && (a.m[5] == b.m[5]) &&
                     (a.m[6] == b.m[6]) && (a.m[7] == b.m[7]) &&
                     (a.m[8] == b.m[8]) && (a.m[9] == b.m[9]) &&
                     (a.m[10] == b.m[10]) && (a.m[11] == b.m[11]) &&
                     (a.m[12] == b.m[12]) && (a.m[12] == b.m[14]) &&
                     (a.m[14] == b.m[14]) && (a.m[15] == b.m[15]));
}

static inline DT_bool MATH_Mat4AlmostEq(MATH_Mat4 a, MATH_Mat4 b) {
    return (DT_bool)(MATH_AlmostEqF32(a.m[0], b.m[0]) &&
                     MATH_AlmostEqF32(a.m[1], b.m[1]) &&
                     MATH_AlmostEqF32(a.m[2], b.m[2]) &&
                     MATH_AlmostEqF32(a.m[3], b.m[3]) &&
                     MATH_AlmostEqF32(a.m[4], b.m[4]) &&
                     MATH_AlmostEqF32(a.m[5], b.m[5]) &&
                     MATH_AlmostEqF32(a.m[6], b.m[6]) &&
                     MATH_AlmostEqF32(a.m[7], b.m[7]) &&
                     MATH_AlmostEqF32(a.m[8], b.m[8]) &&
                     MATH_AlmostEqF32(a.m[9], b.m[9]) &&
                     MATH_AlmostEqF32(a.m[10], b.m[10]) &&
                     MATH_AlmostEqF32(a.m[11], b.m[11]) &&
                     MATH_AlmostEqF32(a.m[12], b.m[12]) &&
                     MATH_AlmostEqF32(a.m[13], b.m[13]) &&
                     MATH_AlmostEqF32(a.m[14], b.m[14]) &&
                     MATH_AlmostEqF32(a.m[15], b.m[15]));
}

static inline DT_bool MATH_Mat4IsZero(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[0]) && MATH_IsZeroF32(a.m[1]) &&
                     MATH_IsZeroF32(a.m[2]) && MATH_IsZeroF32(a.m[3]) &&
                     MATH_IsZeroF32(a.m[4]) && MATH_IsZeroF32(a.m[5]) &&
                     MATH_IsZeroF32(a.m[6]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_IsZeroF32(a.m[8]) && MATH_IsZeroF32(a.m[9]) &&
                     MATH_IsZeroF32(a.m[10]) && MATH_IsZeroF32(a.m[11]) &&
                     MATH_IsZeroF32(a.m[12]) && MATH_IsZeroF32(a.m[13]) &&
                     MATH_IsZeroF32(a.m[14]) && MATH_IsZeroF32(a.m[15]));
}

static inline DT_bool MATH_Mat4IsNaN(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsNaNF32(a.m[0]) || MATH_IsNaNF32(a.m[1]) ||
                     MATH_IsNaNF32(a.m[2]) || MATH_IsNaNF32(a.m[3]) ||
                     MATH_IsNaNF32(a.m[4]) || MATH_IsNaNF32(a.m[5]) ||
                     MATH_IsNaNF32(a.m[6]) || MATH_IsNaNF32(a.m[7]) ||
                     MATH_IsNaNF32(a.m[8]) || MATH_IsNaNF32(a.m[9]) ||
                     MATH_IsNaNF32(a.m[10]) || MATH_IsNaNF32(a.m[11]) ||
                     MATH_IsNaNF32(a.m[12]) || MATH_IsNaNF32(a.m[13]) ||
                     MATH_IsNaNF32(a.m[14]) || MATH_IsNaNF32(a.m[15]));
}

static inline DT_bool MATH_Mat4IsInf(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsInfF32(a.m[0]) || MATH_IsInfF32(a.m[1]) ||
                     MATH_IsInfF32(a.m[2]) || MATH_IsInfF32(a.m[3]) ||
                     MATH_IsInfF32(a.m[4]) || MATH_IsInfF32(a.m[5]) ||
                     MATH_IsInfF32(a.m[6]) || MATH_IsInfF32(a.m[7]) ||
                     MATH_IsInfF32(a.m[8]) || MATH_IsInfF32(a.m[9]) ||
                     MATH_IsInfF32(a.m[10]) || MATH_IsInfF32(a.m[11]) ||
                     MATH_IsInfF32(a.m[12]) || MATH_IsInfF32(a.m[13]) ||
                     MATH_IsInfF32(a.m[14]) || MATH_IsInfF32(a.m[15]));
}

static inline DT_bool MATH_Mat4IsOrthonormal(MATH_Mat4 a) {
    MATH_Vec4 c0 = {.x = a.m[0], .y = a.m[1], .z = a.m[2], .w = a.m[3]};
    MATH_Vec4 c1 = {.x = a.m[4], .y = a.m[5], .z = a.m[6], .w = a.m[7]};
    MATH_Vec4 c2 = {.x = a.m[8], .y = a.m[9], .z = a.m[10], .w = a.m[11]};
    MATH_Vec4 c3 = {.x = a.m[12], .y = a.m[13], .z = a.m[14], .w = a.m[15]};

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

static inline DT_bool MATH_Mat4IsAffine(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[3]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_IsZeroF32(a.m[11]) &&
                     MATH_AlmostEqF32(a.m[15], 1.0f));
}

static inline DT_bool MATH_Mat4IsAffineOrthonormal(MATH_Mat4 a) {
    MATH_Vec3 c0 = {.x = a.m[0], .y = a.m[1], .z = a.m[2]};
    MATH_Vec3 c1 = {.x = a.m[4], .y = a.m[5], .z = a.m[6]};
    MATH_Vec3 c2 = {.x = a.m[8], .y = a.m[9], .z = a.m[10]};

    DT_f32 d01 = MATH_Vec3Dot(c0, c1);
    DT_f32 d02 = MATH_Vec3Dot(c0, c2);
    DT_f32 d12 = MATH_Vec3Dot(c1, c2);

    DT_f32 l0 = MATH_Vec3LenSq(c0);
    DT_f32 l1 = MATH_Vec3LenSq(c1);
    DT_f32 l2 = MATH_Vec3LenSq(c2);

    return (DT_bool)(MATH_IsZeroF32(a.m[3]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_IsZeroF32(a.m[11]) &&
                     MATH_AlmostEqF32(a.m[15], 1.0f) &&
                     MATH_IsZeroF32(d01) && MATH_IsZeroF32(d02) &&
                     MATH_IsZeroF32(d12) &&
                     MATH_AlmostEqF32(l0, 1.0f) && MATH_AlmostEqF32(l1, 1.0f) &&
                     MATH_AlmostEqF32(l2, 1.0f));
}

static inline DT_bool MATH_Mat4IsSymmetric(MATH_Mat4 a) {
    return (DT_bool)(MATH_AlmostEqF32(a.m[1], a.m[4]) &&
                     MATH_AlmostEqF32(a.m[2], a.m[8]) &&
                     MATH_AlmostEqF32(a.m[3], a.m[12]) &&
                     MATH_AlmostEqF32(a.m[6], a.m[9]) &&
                     MATH_AlmostEqF32(a.m[7], a.m[13]) &&
                     MATH_AlmostEqF32(a.m[11], a.m[14]));
}

static inline DT_bool MATH_Mat4IsDiagonal(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[1]) && MATH_IsZeroF32(a.m[2]) &&
                     MATH_IsZeroF32(a.m[3]) && MATH_IsZeroF32(a.m[4]) &&
                     MATH_IsZeroF32(a.m[6]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_IsZeroF32(a.m[8]) && MATH_IsZeroF32(a.m[9]) &&
                     MATH_IsZeroF32(a.m[11]) && MATH_IsZeroF32(a.m[12]) &&
                     MATH_IsZeroF32(a.m[13]) && MATH_IsZeroF32(a.m[14]));
}

static inline DT_bool MATH_Mat4IsIdentity(MATH_Mat4 a) {
    return (DT_bool)(MATH_IsZeroF32(a.m[1]) && MATH_IsZeroF32(a.m[2]) &&
                     MATH_IsZeroF32(a.m[3]) && MATH_IsZeroF32(a.m[4]) &&
                     MATH_IsZeroF32(a.m[6]) && MATH_IsZeroF32(a.m[7]) &&
                     MATH_IsZeroF32(a.m[8]) && MATH_IsZeroF32(a.m[9]) &&
                     MATH_IsZeroF32(a.m[11]) && MATH_IsZeroF32(a.m[12]) &&
                     MATH_IsZeroF32(a.m[13]) && MATH_IsZeroF32(a.m[14]) &&
                     MATH_AlmostEqF32(a.m[0], 1.0f) &&
                     MATH_AlmostEqF32(a.m[5], 1.0f) &&
                     MATH_AlmostEqF32(a.m[10], 1.0f) &&
                     MATH_AlmostEqF32(a.m[15], 1.0f));
}

/* ----  BASIC OPS  ---- */

static inline MATH_Mat4 MATH_Mat4Abs(MATH_Mat4 a) {
    a.m[0] = MATH_AbsF32(a.m[0]);
    a.m[1] = MATH_AbsF32(a.m[1]);
    a.m[2] = MATH_AbsF32(a.m[2]);
    a.m[3] = MATH_AbsF32(a.m[3]);
    a.m[4] = MATH_AbsF32(a.m[4]);
    a.m[5] = MATH_AbsF32(a.m[5]);
    a.m[6] = MATH_AbsF32(a.m[6]);
    a.m[7] = MATH_AbsF32(a.m[7]);
    a.m[8] = MATH_AbsF32(a.m[8]);
    a.m[9] = MATH_AbsF32(a.m[9]);
    a.m[10] = MATH_AbsF32(a.m[10]);
    a.m[11] = MATH_AbsF32(a.m[11]);
    a.m[12] = MATH_AbsF32(a.m[12]);
    a.m[13] = MATH_AbsF32(a.m[13]);
    a.m[14] = MATH_AbsF32(a.m[14]);
    a.m[15] = MATH_AbsF32(a.m[15]);

    return a;
}

static inline MATH_Mat4 MATH_Mat4Sign(MATH_Mat4 a) {
    a.m[0] = MATH_SignF32(a.m[0]);
    a.m[1] = MATH_SignF32(a.m[1]);
    a.m[2] = MATH_SignF32(a.m[2]);
    a.m[3] = MATH_SignF32(a.m[3]);
    a.m[4] = MATH_SignF32(a.m[4]);
    a.m[5] = MATH_SignF32(a.m[5]);
    a.m[6] = MATH_SignF32(a.m[6]);
    a.m[7] = MATH_SignF32(a.m[7]);
    a.m[8] = MATH_SignF32(a.m[8]);
    a.m[9] = MATH_SignF32(a.m[9]);
    a.m[10] = MATH_SignF32(a.m[10]);
    a.m[11] = MATH_SignF32(a.m[11]);
    a.m[12] = MATH_SignF32(a.m[12]);
    a.m[13] = MATH_SignF32(a.m[13]);
    a.m[14] = MATH_SignF32(a.m[14]);
    a.m[15] = MATH_SignF32(a.m[15]);

    return a;
}

static inline MATH_Mat4 MATH_Mat4AddElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.m[0] += b.m[0];
    a.m[1] += b.m[1];
    a.m[2] += b.m[2];
    a.m[3] += b.m[3];
    a.m[4] += b.m[4];
    a.m[5] += b.m[5];
    a.m[6] += b.m[6];
    a.m[7] += b.m[7];
    a.m[8] += b.m[8];
    a.m[9] += b.m[9];
    a.m[10] += b.m[10];
    a.m[11] += b.m[11];
    a.m[12] += b.m[12];
    a.m[13] += b.m[13];
    a.m[14] += b.m[14];
    a.m[15] += b.m[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4SubElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.m[0] -= b.m[0];
    a.m[1] -= b.m[1];
    a.m[2] -= b.m[2];
    a.m[3] -= b.m[3];
    a.m[4] -= b.m[4];
    a.m[5] -= b.m[5];
    a.m[6] -= b.m[6];
    a.m[7] -= b.m[7];
    a.m[8] -= b.m[8];
    a.m[9] -= b.m[9];
    a.m[10] -= b.m[10];
    a.m[11] -= b.m[11];
    a.m[12] -= b.m[12];
    a.m[13] -= b.m[13];
    a.m[14] -= b.m[14];
    a.m[15] -= b.m[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4MulElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.m[0] *= b.m[0];
    a.m[1] *= b.m[1];
    a.m[2] *= b.m[2];
    a.m[3] *= b.m[3];
    a.m[4] *= b.m[4];
    a.m[5] *= b.m[5];
    a.m[6] *= b.m[6];
    a.m[7] *= b.m[7];
    a.m[8] *= b.m[8];
    a.m[9] *= b.m[9];
    a.m[10] *= b.m[10];
    a.m[11] *= b.m[11];
    a.m[12] *= b.m[12];
    a.m[13] *= b.m[13];
    a.m[14] *= b.m[14];
    a.m[15] *= b.m[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivElems(MATH_Mat4 a, MATH_Mat4 b) {
    a.m[0] /= b.m[0];
    a.m[1] /= b.m[1];
    a.m[2] /= b.m[2];
    a.m[3] /= b.m[3];
    a.m[4] /= b.m[4];
    a.m[5] /= b.m[5];
    a.m[6] /= b.m[6];
    a.m[7] /= b.m[7];
    a.m[8] /= b.m[8];
    a.m[9] /= b.m[9];
    a.m[10] /= b.m[10];
    a.m[11] /= b.m[11];
    a.m[12] /= b.m[12];
    a.m[13] /= b.m[13];
    a.m[14] /= b.m[14];
    a.m[15] /= b.m[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivElemsSafe(MATH_Mat4 a, MATH_Mat4 b,
                                              DT_f32 fallback) {
    a.m[0] = MATH_SafeDivF32(a.m[0], b.m[0], fallback);
    a.m[1] = MATH_SafeDivF32(a.m[1], b.m[1], fallback);
    a.m[2] = MATH_SafeDivF32(a.m[2], b.m[2], fallback);
    a.m[3] = MATH_SafeDivF32(a.m[3], b.m[3], fallback);
    a.m[4] = MATH_SafeDivF32(a.m[4], b.m[4], fallback);
    a.m[5] = MATH_SafeDivF32(a.m[5], b.m[5], fallback);
    a.m[6] = MATH_SafeDivF32(a.m[6], b.m[6], fallback);
    a.m[7] = MATH_SafeDivF32(a.m[7], b.m[7], fallback);
    a.m[8] = MATH_SafeDivF32(a.m[8], b.m[8], fallback);
    a.m[9] = MATH_SafeDivF32(a.m[9], b.m[9], fallback);
    a.m[10] = MATH_SafeDivF32(a.m[10], b.m[10], fallback);
    a.m[11] = MATH_SafeDivF32(a.m[11], b.m[11], fallback);
    a.m[12] = MATH_SafeDivF32(a.m[12], b.m[12], fallback);
    a.m[13] = MATH_SafeDivF32(a.m[13], b.m[13], fallback);
    a.m[14] = MATH_SafeDivF32(a.m[14], b.m[14], fallback);
    a.m[15] = MATH_SafeDivF32(a.m[15], b.m[15], fallback);

    return a;
}

static inline MATH_Mat4 MATH_Mat4AddScalar(MATH_Mat4 a, DT_f32 s) {
    a.m[0] += s;
    a.m[1] += s;
    a.m[2] += s;
    a.m[3] += s;
    a.m[4] += s;
    a.m[5] += s;
    a.m[6] += s;
    a.m[7] += s;
    a.m[8] += s;
    a.m[9] += s;
    a.m[10] += s;
    a.m[11] += s;
    a.m[12] += s;
    a.m[13] += s;
    a.m[14] += s;
    a.m[15] += s;

    return a;
}

static inline MATH_Mat4 MATH_Mat4SubScalar(MATH_Mat4 a, DT_f32 s) {
    a.m[0] -= s;
    a.m[1] -= s;
    a.m[2] -= s;
    a.m[3] -= s;
    a.m[4] -= s;
    a.m[5] -= s;
    a.m[6] -= s;
    a.m[7] -= s;
    a.m[8] -= s;
    a.m[9] -= s;
    a.m[10] -= s;
    a.m[11] -= s;
    a.m[12] -= s;
    a.m[13] -= s;
    a.m[14] -= s;
    a.m[15] -= s;

    return a;
}

static inline MATH_Mat4 MATH_Mat4MulScalar(MATH_Mat4 a, DT_f32 s) {
    a.m[0] *= s;
    a.m[1] *= s;
    a.m[2] *= s;
    a.m[3] *= s;
    a.m[4] *= s;
    a.m[5] *= s;
    a.m[6] *= s;
    a.m[7] *= s;
    a.m[8] *= s;
    a.m[9] *= s;
    a.m[10] *= s;
    a.m[11] *= s;
    a.m[12] *= s;
    a.m[13] *= s;
    a.m[14] *= s;
    a.m[15] *= s;

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivScalar(MATH_Mat4 a, DT_f32 s) {
    DT_f32 inv_scalar = 1.0f / s;

    a.m[0] *= inv_scalar;
    a.m[1] *= inv_scalar;
    a.m[2] *= inv_scalar;
    a.m[3] *= inv_scalar;
    a.m[4] *= inv_scalar;
    a.m[5] *= inv_scalar;
    a.m[6] *= inv_scalar;
    a.m[7] *= inv_scalar;
    a.m[8] *= inv_scalar;
    a.m[9] *= inv_scalar;
    a.m[10] *= inv_scalar;
    a.m[11] *= inv_scalar;
    a.m[12] *= inv_scalar;
    a.m[13] *= inv_scalar;
    a.m[14] *= inv_scalar;
    a.m[15] *= inv_scalar;

    return a;
}

static inline MATH_Mat4 MATH_Mat4DivScalarSafe(MATH_Mat4 a, DT_f32 s,
                                               DT_f32 fallback) {
    if (MATH_IsZeroF32(s)) {
        return MATH_Mat4CreateFillScalar(fallback);
    }

    return MATH_Mat4DivScalar(a, s);
}

static inline MATH_Mat4 MATH_Mat4Neg(MATH_Mat4 a) {
    a.m[0] = -a.m[0];
    a.m[1] = -a.m[1];
    a.m[2] = -a.m[2];
    a.m[3] = -a.m[3];
    a.m[4] = -a.m[4];
    a.m[5] = -a.m[5];
    a.m[6] = -a.m[6];
    a.m[7] = -a.m[7];
    a.m[8] = -a.m[8];
    a.m[9] = -a.m[9];
    a.m[10] = -a.m[10];
    a.m[11] = -a.m[11];
    a.m[12] = -a.m[12];
    a.m[13] = -a.m[13];
    a.m[14] = -a.m[14];
    a.m[15] = -a.m[15];

    return a;
}

static inline MATH_Mat4 MATH_Mat4Mul(MATH_Mat4 a, MATH_Mat4 b) {
    DT_f32 a00 = a.m[0], a10 = a.m[1], a20 = a.m[2], a30 = a.m[3];
    DT_f32 a01 = a.m[4], a11 = a.m[5], a21 = a.m[6], a31 = a.m[7];
    DT_f32 a02 = a.m[8], a12 = a.m[9], a22 = a.m[10], a32 = a.m[11];
    DT_f32 a03 = a.m[12], a13 = a.m[13], a23 = a.m[14], a33 = a.m[15];

    DT_f32 b00 = b.m[0], b10 = b.m[1], b20 = b.m[2], b30 = b.m[3];
    DT_f32 b01 = b.m[4], b11 = b.m[5], b21 = b.m[6], b31 = b.m[7];
    DT_f32 b02 = b.m[8], b12 = b.m[9], b22 = b.m[10], b32 = b.m[11];
    DT_f32 b03 = b.m[12], b13 = b.m[13], b23 = b.m[14], b33 = b.m[15];

    a.m[0] = (a00 * b00) + (a01 * b10) + (a02 * b20) + (a03 * b30);
    a.m[1] = (a10 * b00) + (a11 * b10) + (a12 * b20) + (a13 * b30);
    a.m[2] = (a20 * b00) + (a21 * b10) + (a22 * b20) + (a23 * b30);
    a.m[3] = (a30 * b00) + (a31 * b10) + (a32 * b20) + (a33 * b30);

    a.m[4] = (a00 * b01) + (a01 * b11) + (a02 * b21) + (a03 * b31);
    a.m[5] = (a10 * b01) + (a11 * b11) + (a12 * b21) + (a13 * b31);
    a.m[6] = (a20 * b01) + (a21 * b11) + (a22 * b21) + (a23 * b31);
    a.m[7] = (a30 * b01) + (a31 * b11) + (a32 * b21) + (a33 * b31);

    a.m[8] = (a00 * b02) + (a01 * b12) + (a02 * b22) + (a03 * b32);
    a.m[9] = (a10 * b02) + (a11 * b12) + (a12 * b22) + (a13 * b32);
    a.m[10] = (a20 * b02) + (a21 * b12) + (a22 * b22) + (a23 * b32);
    a.m[11] = (a30 * b02) + (a31 * b12) + (a32 * b22) + (a33 * b32);

    a.m[12] = (a00 * b03) + (a01 * b13) + (a02 * b23) + (a03 * b33);
    a.m[13] = (a10 * b03) + (a11 * b13) + (a12 * b23) + (a13 * b33);
    a.m[14] = (a20 * b03) + (a21 * b13) + (a22 * b23) + (a23 * b33);
    a.m[15] = (a30 * b03) + (a31 * b13) + (a32 * b23) + (a33 * b33);

    return a;
}

static inline MATH_Mat4 MATH_Mat4Outer(MATH_Vec4 u, MATH_Vec4 v) {

    return (MATH_Mat4){.m = {(u.x * v.x), (u.y * v.x), (u.z * v.x), (u.w * v.x),
                             (u.x * v.y), (u.y * v.y), (u.z * v.y), (u.w * v.y),
                             (u.x * v.z), (u.y * v.z), (u.z * v.z), (u.w * v.y),
                             (u.x * v.w), (u.y * v.w), (u.z * v.w),
                             (u.w * v.w)}};
}

static inline MATH_Vec4 MATH_Mat4MulVec4(MATH_Mat4 a, MATH_Vec4 v) {
    return (MATH_Vec4){
        .x = (a.m[0] * v.x) + (a.m[4] * v.y) + (a.m[8] * v.z) + (a.m[12] * v.w),
        .y = (a.m[1] * v.x) + (a.m[5] * v.y) + (a.m[9] * v.z) + (a.m[13] * v.w),
        .z =
            (a.m[2] * v.x) + (a.m[6] * v.y) + (a.m[10] * v.z) + (a.m[14] * v.w),
        .w =
            (a.m[3] * v.x) + (a.m[7] * v.y) + (a.m[11] * v.z) + (a.m[15] * v.w),
    };
}

static inline MATH_Vec3 MATH_Mat4MulVec3Projective(MATH_Mat4 a, MATH_Vec3 v) {
    MATH_Vec4 mul = {
        .x = (a.m[0] * v.x) + (a.m[4] * v.y) + (a.m[8] * v.z) + (a.m[12]),
        .y = (a.m[1] * v.x) + (a.m[5] * v.y) + (a.m[9] * v.z) + (a.m[13]),
        .z = (a.m[2] * v.x) + (a.m[6] * v.y) + (a.m[10] * v.z) + (a.m[14]),
        .w = (a.m[3] * v.x) + (a.m[7] * v.y) + (a.m[11] * v.z) + (a.m[15]),
    };
    DT_f32 inv_w = 1 / mul.w;

    return (MATH_Vec3){
        .x = mul.x * inv_w, .y = mul.y * inv_w, .z = mul.z * inv_w};
}

static inline MATH_Vec3 MATH_Mat4MulVec3ProjectiveSafe(MATH_Mat4 a, MATH_Vec3 v,
                                                       MATH_Vec3 fallback) {
    MATH_Vec4 mul = {
        .x = (a.m[0] * v.x) + (a.m[4] * v.y) + (a.m[8] * v.z) + (a.m[12]),
        .y = (a.m[1] * v.x) + (a.m[5] * v.y) + (a.m[9] * v.z) + (a.m[13]),
        .z = (a.m[2] * v.x) + (a.m[6] * v.y) + (a.m[10] * v.z) + (a.m[14]),
        .w = (a.m[3] * v.x) + (a.m[7] * v.y) + (a.m[11] * v.z) + (a.m[15]),
    };
    DT_f32 inv_w = 1 / mul.w;
    if (MATH_IsZeroF32(inv_w)) {
        return fallback;
    }

    return (MATH_Vec3){
        .x = mul.x * inv_w, .y = mul.y * inv_w, .z = mul.z * inv_w};
}

static inline MATH_Vec3 MATH_Mat4MulVec3Dir(MATH_Mat4 a, MATH_Vec3 v) {
    return (MATH_Vec3){
        .x = (a.m[0] * v.x) + (a.m[4] * v.y) + (a.m[8] * v.z),
        .y = (a.m[1] * v.x) + (a.m[5] * v.y) + (a.m[9] * v.z),
        .z = (a.m[2] * v.x) + (a.m[6] * v.y) + (a.m[10] * v.z),
    };
}

static inline MATH_Vec3 MATH_Mat4MulVec3Affine(MATH_Mat4 a, MATH_Vec3 v) {
    return (MATH_Vec3){
        .x = (a.m[0] * v.x) + (a.m[4] * v.y) + (a.m[8] * v.z) + (a.m[12]),
        .y = (a.m[1] * v.x) + (a.m[5] * v.y) + (a.m[9] * v.z) + (a.m[13]),
        .z = (a.m[2] * v.x) + (a.m[6] * v.y) + (a.m[10] * v.z) + (a.m[14]),
    };
}

static inline DT_f32 MATH_Mat4Trace(MATH_Mat4 a) {
    return a.m[0] + a.m[5] + a.m[10] + a.m[15];
}

/* ----  ACCESSORS  ---- */

static inline MATH_Vec4 MATH_Mat4GetRow(MATH_Mat4 a, DT_size row) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);

    return (MATH_Vec4){
        .x = a.m[0 * MATH_MAT4_SIZE + row],
        .y = a.m[1 * MATH_MAT4_SIZE + row],
        .z = a.m[2 * MATH_MAT4_SIZE + row],
        .w = a.m[3 * MATH_MAT4_SIZE + row],
    };
}

static inline MATH_Vec4 MATH_Mat4GetCol(MATH_Mat4 a, DT_size col) {
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    return (MATH_Vec4){
        .x = a.m[col * MATH_MAT4_SIZE + 0],
        .y = a.m[col * MATH_MAT4_SIZE + 1],
        .z = a.m[col * MATH_MAT4_SIZE + 2],
        .w = a.m[col * MATH_MAT4_SIZE + 3],
    };
}

static inline MATH_Mat4 MATH_Mat4SetRow(MATH_Mat4 a, DT_size row, MATH_Vec4 v) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);

    a.m[0 * MATH_MAT4_SIZE + row] = v.x;
    a.m[1 * MATH_MAT4_SIZE + row] = v.y;
    a.m[2 * MATH_MAT4_SIZE + row] = v.z;
    a.m[3 * MATH_MAT4_SIZE + row] = v.w;

    return a;
}

static inline MATH_Mat4 MATH_Mat4SetCol(MATH_Mat4 a, DT_size col, MATH_Vec4 v) {
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    a.m[col * MATH_MAT4_SIZE + 0] = v.x;
    a.m[col * MATH_MAT4_SIZE + 1] = v.y;
    a.m[col * MATH_MAT4_SIZE + 2] = v.z;
    a.m[col * MATH_MAT4_SIZE + 3] = v.w;

    return a;
}

static inline DT_f32 MATH_Mat4GetAt(MATH_Mat4 a, DT_size row, DT_size col) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    return a.m[col * MATH_MAT4_SIZE + row];
}

static inline MATH_Mat4 MATH_Mat4SetAt(MATH_Mat4 a, DT_size row, DT_size col,
                                       DT_f32 val) {
    DIAG_ASSERT(row < MATH_MAT4_SIZE);
    DIAG_ASSERT(col < MATH_MAT4_SIZE);

    a.m[col * MATH_MAT4_SIZE + row] = val;

    return a;
}

/* ----  BASIC ALGEBRA  ---- */

static inline MATH_Mat4 MATH_Mat4Transpose(MATH_Mat4 a) {
    MATH_Mat4 rslt;

    rslt.m[0 * MATH_MAT4_SIZE + 0] = a.m[0 * MATH_MAT4_SIZE + 0];
    rslt.m[0 * MATH_MAT4_SIZE + 1] = a.m[1 * MATH_MAT4_SIZE + 0];
    rslt.m[0 * MATH_MAT4_SIZE + 2] = a.m[2 * MATH_MAT4_SIZE + 0];
    rslt.m[0 * MATH_MAT4_SIZE + 3] = a.m[3 * MATH_MAT4_SIZE + 0];
    rslt.m[1 * MATH_MAT4_SIZE + 0] = a.m[0 * MATH_MAT4_SIZE + 1];
    rslt.m[1 * MATH_MAT4_SIZE + 1] = a.m[1 * MATH_MAT4_SIZE + 1];
    rslt.m[1 * MATH_MAT4_SIZE + 2] = a.m[2 * MATH_MAT4_SIZE + 1];
    rslt.m[1 * MATH_MAT4_SIZE + 3] = a.m[3 * MATH_MAT4_SIZE + 1];
    rslt.m[2 * MATH_MAT4_SIZE + 0] = a.m[0 * MATH_MAT4_SIZE + 2];
    rslt.m[2 * MATH_MAT4_SIZE + 1] = a.m[1 * MATH_MAT4_SIZE + 2];
    rslt.m[2 * MATH_MAT4_SIZE + 2] = a.m[2 * MATH_MAT4_SIZE + 2];
    rslt.m[2 * MATH_MAT4_SIZE + 3] = a.m[3 * MATH_MAT4_SIZE + 2];
    rslt.m[3 * MATH_MAT4_SIZE + 0] = a.m[0 * MATH_MAT4_SIZE + 3];
    rslt.m[3 * MATH_MAT4_SIZE + 1] = a.m[1 * MATH_MAT4_SIZE + 3];
    rslt.m[3 * MATH_MAT4_SIZE + 2] = a.m[2 * MATH_MAT4_SIZE + 3];
    rslt.m[3 * MATH_MAT4_SIZE + 3] = a.m[3 * MATH_MAT4_SIZE + 3];

    return rslt;
}

static inline DT_f32 MATH_Mat4Det(MATH_Mat4 a) {
    /**
     * Pre expanded optimized expression of the mat4 determinant, rather
     than
     * the naive way.
     *
     * This is also faster than LU decomposition method of 4x4 matrix.
     *
     * This also assumes that the matrix is a column major matrix.
     *
     * Determinant via Laplace (cofactor) expansion, algebraically
     optimized
     * using 2×2 minors
     */
    const DT_f32 *m = a.m;

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

static inline DT_bool MATH_Mat4IsFlipped(MATH_Mat4 a) {
    return (DT_bool)(MATH_Mat4Det(a) < 0.0f);
}

static inline MATH_Mat4 MATH_Mat4Inv(MATH_Mat4 a, MATH_Mat4 det_zero_fallback) {
    /**
     * Adjugate (cofactor) method for matrix inversion, in an
     optimized/unrolled
     * form
     */
    const DT_f32 *m = a.m;
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

    r.m[0] = (m[5] * c5 - m[6] * c4 + m[7] * c3) * inv_det;
    r.m[1] = (-m[1] * c5 + m[2] * c4 - m[3] * c3) * inv_det;
    r.m[2] = (m[13] * s5 - m[14] * s4 + m[15] * s3) * inv_det;
    r.m[3] = (-m[9] * s5 + m[10] * s4 - m[11] * s3) * inv_det;

    r.m[4] = (-m[4] * c5 + m[6] * c2 - m[7] * c1) * inv_det;
    r.m[5] = (m[0] * c5 - m[2] * c2 + m[3] * c1) * inv_det;
    r.m[6] = (-m[12] * s5 + m[14] * s2 - m[15] * s1) * inv_det;
    r.m[7] = (m[8] * s5 - m[10] * s2 + m[11] * s1) * inv_det;

    r.m[8] = (m[4] * c4 - m[5] * c2 + m[7] * c0) * inv_det;
    r.m[9] = (-m[0] * c4 + m[1] * c2 - m[3] * c0) * inv_det;
    r.m[10] = (m[12] * s4 - m[13] * s2 + m[15] * s0) * inv_det;
    r.m[11] = (-m[8] * s4 + m[9] * s2 - m[11] * s0) * inv_det;

    r.m[12] = (-m[4] * c3 + m[5] * c1 - m[6] * c0) * inv_det;
    r.m[13] = (m[0] * c3 - m[1] * c1 + m[2] * c0) * inv_det;
    r.m[14] = (-m[12] * s3 + m[13] * s1 - m[14] * s0) * inv_det;
    r.m[15] = (m[8] * s3 - m[9] * s1 + m[10] * s0) * inv_det;

    return r;
}

static inline MATH_Mat4 MATH_Mat4InvAffine(MATH_Mat4 a,
                                           MATH_Mat4 det_zero_fallback) {
    // Extract 3x3 linear part
    DT_f32 a00 = a.m[0], a01 = a.m[4], a02 = a.m[8];
    DT_f32 a10 = a.m[1], a11 = a.m[5], a12 = a.m[9];
    DT_f32 a20 = a.m[2], a21 = a.m[6], a22 = a.m[10];

    // Compute inverse of 3x3 (same pattern as your optimized det)
    DT_f32 det = a00 * (a11 * a22 - a12 * a21) - a01 * (a10 * a22 - a12 * a20) +
                 a02 * (a10 * a21 - a11 * a20);
    if (MATH_IsZeroF32(det)) {
        return det_zero_fallback;
    }
    DT_f32 inv_det = 1.0f / det;

    // Inverse 3x3
    a.m[0] = (a11 * a22 - a12 * a21) * inv_det;
    a.m[1] = -(a10 * a22 - a12 * a20) * inv_det;
    a.m[2] = (a10 * a21 - a11 * a20) * inv_det;
    a.m[3] = 0.0f;

    a.m[4] = -(a01 * a22 - a02 * a21) * inv_det;
    a.m[5] = (a00 * a22 - a02 * a20) * inv_det;
    a.m[6] = -(a00 * a21 - a01 * a20) * inv_det;
    a.m[7] = 0.0f;

    a.m[8] = (a01 * a12 - a02 * a11) * inv_det;
    a.m[9] = -(a00 * a12 - a02 * a10) * inv_det;
    a.m[10] = (a00 * a11 - a01 * a10) * inv_det;
    a.m[11] = 0.0f;

    // Translation
    DT_f32 tx = a.m[12];
    DT_f32 ty = a.m[13];
    DT_f32 tz = a.m[14];
    a.m[12] = -(a.m[0] * tx + a.m[4] * ty + a.m[8] * tz);
    a.m[13] = -(a.m[1] * tx + a.m[5] * ty + a.m[9] * tz);
    a.m[14] = -(a.m[2] * tx + a.m[6] * ty + a.m[10] * tz);
    a.m[15] = 1.0f;

    return a;
}

static inline MATH_Vec3 MATH_Mat4ExtractTranslation(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.m[12],
        .y = a.m[13],
        .z = a.m[14],
    };
}

static inline MATH_Vec3 MATH_Mat4ExtractScale(MATH_Mat4 a) {
    MATH_Vec3 c0 = {.x = a.m[0], .y = a.m[1], .z = a.m[2]};
    MATH_Vec3 c1 = {.x = a.m[4], .y = a.m[5], .z = a.m[6]};
    MATH_Vec3 c2 = {.x = a.m[8], .y = a.m[9], .z = a.m[10]};

    return (MATH_Vec3){
        .x = MATH_Vec3Len(c0),
        .y = MATH_Vec3Len(c1),
        .z = MATH_Vec3Len(c2),
    };
}

static inline MATH_Mat3 MATH_Mat4ExtractRotation(MATH_Mat4 a) {
    MATH_Vec3 scale = MATH_Mat4ExtractScale(a);
    MATH_Mat3 rslt = {0};

    if (!MATH_IsZeroF32(scale.x)) {
        rslt.m[0] = a.m[0] / scale.x;
        rslt.m[1] = a.m[1] / scale.x;
        rslt.m[2] = a.m[2] / scale.x;
    }
    if (!MATH_IsZeroF32(scale.y)) {
        rslt.m[3] = a.m[4] / scale.y;
        rslt.m[4] = a.m[5] / scale.y;
        rslt.m[5] = a.m[6] / scale.y;
    }
    if (!MATH_IsZeroF32(scale.z)) {
        rslt.m[6] = a.m[8] / scale.z;
        rslt.m[7] = a.m[9] / scale.z;
        rslt.m[8] = a.m[10] / scale.z;
    }

    return rslt;
}

static inline MATH_Mat4 MATH_Mat4NormBasis(MATH_Mat4 a) {
    MATH_Vec3 s = MATH_Mat4ExtractScale(a);

    if (!MATH_IsZeroF32(s.x)) {
        a.m[0] /= s.x;
        a.m[1] /= s.x;
        a.m[2] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.m[4] /= s.y;
        a.m[5] /= s.y;
        a.m[6] /= s.y;
    }
    if (!MATH_IsZeroF32(s.z)) {
        a.m[8] /= s.z;
        a.m[9] /= s.z;
        a.m[10] /= s.z;
    }

    return a;
}

static inline DT_f32 MATH_Mat4FrobeniusNorm(MATH_Mat4 a) {
    return MATH_SqrtF32(
        (a.m[0] * a.m[0]) + (a.m[1] * a.m[1]) + (a.m[2] * a.m[2]) +
        (a.m[3] * a.m[3]) + (a.m[4] * a.m[4]) + (a.m[5] * a.m[5]) +
        (a.m[6] * a.m[6]) + (a.m[7] * a.m[7]) + (a.m[8] * a.m[8]) +
        (a.m[9] * a.m[9]) + (a.m[10] * a.m[10]) + (a.m[11] * a.m[11]) +
        (a.m[12] * a.m[12]) + (a.m[13] * a.m[13]) + (a.m[14] * a.m[14]) +
        (a.m[15] * a.m[15]));
}

static inline DT_f32 MATH_Mat4FrobeniusNormSq(MATH_Mat4 a) {
    return (a.m[0] * a.m[0]) + (a.m[1] * a.m[1]) + (a.m[2] * a.m[2]) +
           (a.m[3] * a.m[3]) + (a.m[4] * a.m[4]) + (a.m[5] * a.m[5]) +
           (a.m[6] * a.m[6]) + (a.m[7] * a.m[7]) + (a.m[8] * a.m[8]) +
           (a.m[9] * a.m[9]) + (a.m[10] * a.m[10]) + (a.m[11] * a.m[11]) +
           (a.m[12] * a.m[12]) + (a.m[13] * a.m[13]) + (a.m[14] * a.m[14]) +
           (a.m[15] * a.m[15]);
}

static inline MATH_Vec3 MATH_Mat4GetRight(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.m[0],
        .y = a.m[1],
        .z = a.m[2],
    };
}

static inline MATH_Vec3 MATH_Mat4GetUp(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.m[4],
        .y = a.m[5],
        .z = a.m[6],
    };
}

static inline MATH_Vec3 MATH_Mat4GetForward(MATH_Mat4 a) {
    return (MATH_Vec3){
        .x = a.m[8],
        .y = a.m[9],
        .z = a.m[10],
    };
}

// /* ----  SPECIAL OPS  ---- */

// static inline MATH_Mat4 MATH_Mat4LookAt(MATH_Vec3 eye, MATH_Vec3 target,
//                                         MATH_Vec3 up) {
//     MATH_Vec3 f = MATH_Vec3Normalize(MATH_Vec3SubComps(target, eye));
//     MATH_Vec3 r = MATH_Vec3Normalize(MATH_Vec3Cross(f, up));
//     MATH_Vec3 u = MATH_Vec3Cross(r, f);

//     MATH_Mat4 m = MATH_Mat4CreateIdentity();

//     m.m[0] = r.x;
//     m.m[1] = r.y;
//     m.m[2] = r.z;
//     m.m[4] = u.x;
//     m.m[5] = u.y;
//     m.m[6] = u.z;
//     m.m[8] = -f.x;
//     m.m[9] = -f.y;
//     m.m[10] = -f.z;

//     m.m[12] = -MATH_Vec3Dot(r, eye);
//     m.m[13] = -MATH_Vec3Dot(u, eye);
//     m.m[14] = MATH_Vec3Dot(f, eye);

//     return m;
// }

// static inline MATH_Mat4 MATH_Mat4Perspective(DT_f32 fov, DT_f32 aspect,
//                                              DT_f32 near, DT_f32 far) {
//     DT_f32 f = 1.0f / MATH_TanF32(fov * 0.5f);

//     MATH_Mat4 m = {0};

//     m.m[0] = f / aspect;
//     m.m[5] = f;

//     m.m[10] = (far + near) / (near - far);
//     m.m[11] = -1.0f;

//     m.m[14] = (2.0f * far * near) / (near - far);

//     return m;
// }

// static inline MATH_Mat4 MATH_Mat4Ortho(DT_f32 left, DT_f32 right, DT_f32
// bottom,
//                                        DT_f32 top, DT_f32 near, DT_f32
//                                        far) {
//     MATH_Mat4 m = {0};

//     m.m[0] = 2.0f / (right - left);
//     m.m[5] = 2.0f / (top - bottom);
//     m.m[10] = -2.0f / (far - near);

//     m.m[12] = -(right + left) / (right - left);
//     m.m[13] = -(top + bottom) / (top - bottom);
//     m.m[14] = -(far + near) / (far - near);

//     m.m[15] = 1.0f;

//     return m;
// }

// static inline MATH_Mat4 MATH_Mat4Frustum(DT_f32 left, DT_f32 right,
//                                          DT_f32 bottom, DT_f32 top,
//                                          DT_f32 near, DT_f32 far) {
//     MATH_Mat4 m = {0};

//     m.m[0] = (2.0f * near) / (right - left);
//     m.m[5] = (2.0f * near) / (top - bottom);

//     m.m[8] = (right + left) / (right - left);
//     m.m[9] = (top + bottom) / (top - bottom);

//     m.m[10] = -(far + near) / (far - near);
//     m.m[11] = -1.0f;

//     m.m[14] = -(2.0f * far * near) / (far - near);

//     return m;
// }

// static inline MATH_Mat3 MATH_Mat4ToNormalMatrix(MATH_Mat4 m) {
//     // Extract upper-left 3x3
//     DT_f32 a00 = m.m[0], a01 = m.m[4], a02 = m.m[8];
//     DT_f32 a10 = m.m[1], a11 = m.m[5], a12 = m.m[9];
//     DT_f32 a20 = m.m[2], a21 = m.m[6], a22 = m.m[10];

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
//     r.m[0] = (a11 * a22 - a12 * a21) * inv_det;
//     r.m[1] = -(a10 * a22 - a12 * a20) * inv_det;
//     r.m[2] = (a10 * a21 - a11 * a20) * inv_det;

//     r.m[3] = -(a01 * a22 - a02 * a21) * inv_det;
//     r.m[4] = (a00 * a22 - a02 * a20) * inv_det;
//     r.m[5] = -(a00 * a21 - a01 * a20) * inv_det;

//     r.m[6] = (a01 * a12 - a02 * a11) * inv_det;
//     r.m[7] = -(a00 * a12 - a02 * a10) * inv_det;
//     r.m[8] = (a00 * a11 - a01 * a10) * inv_det;

//     // Transpose
//     MATH_Mat3 t;
//     t.m[0] = r.m[0];
//     t.m[1] = r.m[3];
//     t.m[2] = r.m[6];

//     t.m[3] = r.m[1];
//     t.m[4] = r.m[4];
//     t.m[5] = r.m[7];

//     t.m[6] = r.m[2];
//     t.m[7] = r.m[5];
//     t.m[8] = r.m[8];

//     return t;
// }

#ifdef __cplusplus
}
#endif

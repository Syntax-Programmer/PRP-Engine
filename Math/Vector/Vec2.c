#include "Vec2.h"
#include "../../Diagnostics/Assert.h"
#include <math.h>

#define MATH_EPSILON_F32 1e-6f

/* ----  COVERSIONS ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2FromTheta(DT_f32 theta) {
    return (MATH_Vec2){.x = cosf(theta), .y = sinf(theta)};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2FromPolar(DT_f32 r, DT_f32 theta) {
    return (MATH_Vec2){.x = cosf(theta) * r, .y = sinf(theta) * r};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2FromArray(const DT_f32 arr[2]) {
    DIAG_GUARD(arr != DT_null, (MATH_Vec2){0});

    return (MATH_Vec2){.x = arr[0], .y = arr[1]};
}

PRP_FN_API PRP_Result PRP_FN_CALL MATH_Vec2ToArray(MATH_Vec2 v, DT_f32 out[2]) {
    DIAG_GUARD(out != DT_null, PRP_ERR_INV_ARG);

    out[0] = v.x;
    out[1] = v.y;

    return PRP_OK;
}

PRP_FN_API PRP_Result PRP_FN_CALL MATH_Vec2ToPolar(MATH_Vec2 v, DT_f32 *pR,
                                                   DT_f32 *pTheta) {
    DIAG_GUARD(pR != DT_null, PRP_ERR_INV_ARG);
    DIAG_GUARD(pTheta != DT_null, PRP_ERR_INV_ARG);

    *pR = sqrtf((v.x * v.x) + (v.y * v.y));
    *pTheta = atan2f(v.y, v.x);

    return PRP_OK;
}

/* ----  VEC-VEC MATH ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Add(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x + b.x, .y = a.y + b.y};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Sub(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x - b.x, .y = a.y - b.y};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Mul(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x * b.x, .y = a.y * b.y};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Div(MATH_Vec2 a, MATH_Vec2 b) {
    return (MATH_Vec2){.x = a.x / b.x, .y = a.y / b.y};
}

/* ----  VEC-SCALAR MATH ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2AddScalar(MATH_Vec2 v, DT_f32 s) {
    return (MATH_Vec2){.x = v.x + s, .y = v.y + s};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2SubScalar(MATH_Vec2 v, DT_f32 s) {
    return (MATH_Vec2){.x = v.x - s, .y = v.y - s};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2MulScalar(MATH_Vec2 v, DT_f32 s) {
    return (MATH_Vec2){.x = v.x * s, .y = v.y * s};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2DivScalar(MATH_Vec2 v, DT_f32 s) {
    return (MATH_Vec2){.x = v.x / s, .y = v.y / s};
}

/* ----  COMPARISION/VALIDATION ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2Equal(MATH_Vec2 a, MATH_Vec2 b) {
    return (a.x == b.x && a.y == b.y);
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2EqualEps(MATH_Vec2 a, MATH_Vec2 b,
                                                 DT_f32 eps) {
    const DT_f32 dx = a.x - b.x;
    const DT_f32 dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (eps * eps);
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsZero(MATH_Vec2 v) {
    return (v.x == 0 && v.y == 0);
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsFinite(MATH_Vec2 v) {
    return isfinite(v.x) && isfinite(v.y);
}

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsNaN(MATH_Vec2 v) {
    return isnan(v.x) || isnan(v.y);
}

/* ----  LENGTH / DISTANCE ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Length(MATH_Vec2 v) {
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2LengthSq(MATH_Vec2 v) {
    return (v.x * v.x) + (v.y * v.y);
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Distance(MATH_Vec2 a, MATH_Vec2 b) {
    DT_f32 dx = a.x - b.x;
    DT_f32 dy = a.y - b.y;

    return sqrtf((dx * dx) + (dy * dy));
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2DistanceSq(MATH_Vec2 a, MATH_Vec2 b) {
    DT_f32 dx = a.x - b.x;
    DT_f32 dy = a.y - b.y;

    return (dx * dx) + (dy * dy);
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2ManhattanLength(MATH_Vec2 v) {
    return fabsf(v.x) + fabsf(v.y);
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2ChebyshevLength(MATH_Vec2 v) {
    DT_f32 ax = fabsf(v.x);
    DT_f32 ay = fabsf(v.y);

    return PRP_MAX(ax, ay);
}

/* ----  NORMALIZATION / DIRECTION ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Normalize(MATH_Vec2 v) {
    DT_f32 len = MATH_Vec2Length(v);

    return (MATH_Vec2){.x = v.x / len, .y = v.y / len};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2NormalizeSafe(MATH_Vec2 v) {
    DT_f32 len_sq = MATH_Vec2LengthSq(v);

    if (len_sq <= MATH_EPSILON_F32 * MATH_EPSILON_F32) {
        return (MATH_Vec2){0, 0};
    }

    DT_f32 inv_len = 1.0f / sqrtf(len_sq);
    return (MATH_Vec2){v.x * inv_len, v.y * inv_len};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Direction(MATH_Vec2 from,
                                                    MATH_Vec2 to) {
    return MATH_Vec2NormalizeSafe((MATH_Vec2){to.x - from.x, to.y - from.y});
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2SetLength(MATH_Vec2 v, DT_f32 len) {
    DT_f32 curr_len = MATH_Vec2Length(v);
    DT_f32 scale = len / curr_len;

    return (MATH_Vec2){.x = v.x * scale, .y = v.y * scale};
}

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2ClampLength(MATH_Vec2 v, DT_f32 min,
                                                      DT_f32 max) {
    DT_f32 len_sq = MATH_Vec2LengthSq(v);

    if (len_sq > max * max) {
        return MATH_Vec2SetLength(v, max);
    }
    if (len_sq < min * min) {
        return MATH_Vec2SetLength(v, min);
    }

    return v;
}

/* ----  DOT / CROSS / ANGLES ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Dot(MATH_Vec2 a, MATH_Vec2 b) {
    return (a.x * b.x) + (a.y * b.y);
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Cross(MATH_Vec2 a, MATH_Vec2 b) {
    return (a.x * b.y) - (a.y * b.x);
}

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Angle(MATH_Vec2 a, MATH_Vec2 b);
// DT_f32 dot = MATH_Vec2Dot(a, b);
// DT_f32 denom = sqrtf(MATH_Vec2LengthSq(a) * MATH_Vec2LengthSq(b));

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2SignedAngle(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2AngleOf(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Perp(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2PerpCW(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2PerpCCW(MATH_Vec2 v);

/* ----  PROJECTION / REFLECTION ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Project(MATH_Vec2 v, MATH_Vec2 onto);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Reject(MATH_Vec2 v, MATH_Vec2 from);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Reflect(MATH_Vec2 v,
                                                  MATH_Vec2 normal);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Slide(MATH_Vec2 v, MATH_Vec2 normal);

/* ----  INTERPOLATION / MOTION ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Lerp(MATH_Vec2 a, MATH_Vec2 b,
                                               DT_f32 t);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Nlerp(MATH_Vec2 a, MATH_Vec2 b,
                                                DT_f32 t);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Slerp(MATH_Vec2 a, MATH_Vec2 b,
                                                DT_f32 t);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2SmoothStep(MATH_Vec2 a, MATH_Vec2 b,
                                                     DT_f32 t);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2MoveTowards(MATH_Vec2 a, MATH_Vec2 b,
                                                      DT_f32 max_dist);

/* ----  ROTATION ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Rotate(MATH_Vec2 v, DT_f32 radians);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2RotateDeg(MATH_Vec2 v,
                                                    DT_f32 degrees);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2RotateAbout(MATH_Vec2 v,
                                                      MATH_Vec2 center,
                                                      DT_f32 radians);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Align(MATH_Vec2 v, MATH_Vec2 target);

/* ----  COMPONENT UTILITIES ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Min(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Max(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Abs(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Sign(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Floor(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Ceil(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Round(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Frac(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Clamp(MATH_Vec2 v, MATH_Vec2 min,
                                                MATH_Vec2 max);

/* ----  GEOMETRY ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2ClosestPointOnLine(MATH_Vec2 p,
                                                             MATH_Vec2 a,
                                                             MATH_Vec2 b);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2ClosestPointOnSegment(MATH_Vec2 p,
                                                                MATH_Vec2 a,
                                                                MATH_Vec2 b);

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2DistanceToLine(MATH_Vec2 p, MATH_Vec2 a,
                                                      MATH_Vec2 b);

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2DistanceToSegment(MATH_Vec2 p,
                                                         MATH_Vec2 a,
                                                         MATH_Vec2 b);

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsLeftOf(MATH_Vec2 a, MATH_Vec2 b,
                                                 MATH_Vec2 p);

/* ----  RANDOM / SAMPLING ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Random(void);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2RandomRange(MATH_Vec2 min,
                                                      MATH_Vec2 max);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2RandomUnit(void);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2RandomInCircle(DT_f32 radius);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2RandomOnCircle(DT_f32 radius);

/* ----  ADVANCED ---- */

PRP_FN_API DT_void PRP_FN_CALL MATH_Vec2Barycentric(MATH_Vec2 p, MATH_Vec2 a,
                                                    MATH_Vec2 b, MATH_Vec2 c,
                                                    DT_f32 *u, DT_f32 *v,
                                                    DT_f32 *w);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Refraction(MATH_Vec2 v,
                                                     MATH_Vec2 normal,
                                                     DT_f32 eta);

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Curvature(MATH_Vec2 a, MATH_Vec2 b,
                                                 MATH_Vec2 c);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Bezier(MATH_Vec2 p0, MATH_Vec2 p1,
                                                 MATH_Vec2 p2, DT_f32 t);

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2CatmullRom(MATH_Vec2 p0, MATH_Vec2 p1,
                                                     MATH_Vec2 p2, MATH_Vec2 p3,
                                                     DT_f32 t);

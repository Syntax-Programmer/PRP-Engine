#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../Data-Types/Typedefs.h"
#include "../../Utils/Defs.h"

typedef struct {
    DT_f32 x, y;
} MATH_Vec2;

/* ----  CONSTS ---- */

#define MATH_Vec2(x, y) ((MATH_Vec2){.x = (x), .y = (y)})
#define MATH_Vec2Zero ((MATH_Vec2){.x = 0, .y = 0})
#define MATH_Vec2One ((MATH_Vec2){.x = 1, .y = 1})
#define MATH_Vec2UnitX ((MATH_Vec2){.x = 1, .y = 0})
#define MATH_Vec2UnitY ((MATH_Vec2){.x = 0, .y = 1})
#define MATH_Vec2FromScalar(s) ((MATH_Vec2){.x = (s), .y = (s)})
#define MATH_Vec2Cpy(v) ((MATH_Vec2){.x = (v).x, .y = (v).y})

/* ----  COVERSIONS ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2FromTheta(DT_f32 theta);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2FromPolar(DT_f32 r, DT_f32 theta);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2FromArray(const DT_f32 arr[2]);
PRP_FN_API PRP_Result PRP_FN_CALL MATH_Vec2ToArray(MATH_Vec2 v, DT_f32 out[2]);
PRP_FN_API PRP_Result PRP_FN_CALL MATH_Vec2ToPolar(MATH_Vec2 v, DT_f32 *r,
                                                   DT_f32 *theta);

/* ----  VEC-VEC MATH ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Add(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Sub(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Mul(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Div(MATH_Vec2 a, MATH_Vec2 b);

/* ----  VEC-SCALAR MATH ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2AddScalar(MATH_Vec2 v, DT_f32 s);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2SubScalar(MATH_Vec2 v, DT_f32 s);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2MulScalar(MATH_Vec2 v, DT_f32 s);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2DivScalar(MATH_Vec2 v, DT_f32 s);

/* ----  COMPARISION/VALIDATION ---- */

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2Equal(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2EqualEps(MATH_Vec2 a, MATH_Vec2 b,
                                                 DT_f32 eps);

PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsZero(MATH_Vec2 v);
PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsFinite(MATH_Vec2 v);
PRP_FN_API DT_bool PRP_FN_CALL MATH_Vec2IsNaN(MATH_Vec2 v);

/* ----  LENGTH / DISTANCE ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Length(MATH_Vec2 v);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2LengthSq(MATH_Vec2 v);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Distance(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2DistanceSq(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2ManhattanLength(MATH_Vec2 v);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2ChebyshevLength(MATH_Vec2 v);

/* ----  NORMALIZATION / DIRECTION ---- */

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Normalize(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2NormalizeSafe(MATH_Vec2 v);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2Direction(MATH_Vec2 from,
                                                    MATH_Vec2 to);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2SetLength(MATH_Vec2 v, DT_f32 len);
PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Vec2ClampLength(MATH_Vec2 v, DT_f32 min,
                                                      DT_f32 max);

/* ----  DOT / CROSS / ANGLES ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Dot(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Cross(MATH_Vec2 a, MATH_Vec2 b);
PRP_FN_API DT_f32 PRP_FN_CALL MATH_Vec2Angle(MATH_Vec2 a, MATH_Vec2 b);
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

#ifdef __cplusplus
}
#endif

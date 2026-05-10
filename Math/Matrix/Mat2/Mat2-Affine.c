#include "Mat2-Affine.h"

/* ----  ALGEBRAIC EXTRACTIONS  ---- */

PRP_FN_API DT_f32 PRP_FN_CALL MATH_Mat2ExtractRotation(MATH_Mat2 a) {
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

PRP_FN_API MATH_Vec2 PRP_FN_CALL MATH_Mat2ExtractScale(MATH_Mat2 a) {
    MATH_Vec2 c0 = {.x = a.membs[0], .y = a.membs[1]};
    MATH_Vec2 c1 = {.x = a.membs[2], .y = a.membs[3]};

    return (MATH_Vec2){
        .x = MATH_Vec2Len(c0),
        .y = MATH_Vec2Len(c1),
    };
}

PRP_FN_API MATH_Mat2 PRP_FN_CALL MATH_Mat2NormBasis(MATH_Mat2 a) {
    MATH_Vec2 s = MATH_Mat2ExtractScale(a);
    if (!MATH_IsZeroF32(s.x)) {
        a.membs[0] /= s.x;
        a.membs[1] /= s.x;
    }
    if (!MATH_IsZeroF32(s.y)) {
        a.membs[2] /= s.y;
        a.membs[3] /= s.y;
    }

    return a;
}

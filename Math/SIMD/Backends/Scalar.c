#include "../Dispatch.h"

static inline MATH_SimdF32 SimdMake(DT_f32 x, DT_f32 y, DT_f32 z, DT_f32 w);

static inline MATH_SimdF32 SimdMake(DT_f32 x, DT_f32 y, DT_f32 z, DT_f32 w) {
    MATH_SimdF32 v;
    v.vec[0] = x;
    v.vec[1] = y;
    v.vec[2] = z;
    v.vec[3] = w;

    return v;
}

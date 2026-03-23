#include "../Diagnostics/Assert.h"
#include "Internals.h"

DT_DSId WorldCreate(DT_void) {
    ASSERT_CTX_INVARIANT_EXPR;

    World data = {0};
    data.layouts = DT_ArrCreateUnchecked(sizeof(Layout), DT_ARR_DEFAULT_CAP);
    data.system_caches =
        DT_ArrCreateUnchecked(sizeof(SystemCache), DT_ARR_DEFAULT_CAP);
    if (!data.layouts || !data.system_caches) {
        SET_LAST_ERR_CODE(PRP_ERR_OOM);
        goto free_internals;
    }

    DT_DSId world = DT_DSArrAddUnchecked(g_ctx->worlds, &data);
    if (world == DT_DS_INVALID_ID) {
        PRP_Result code = DT_DSArrGetLastErrCode();
        if (code == PRP_ERR_RES_EXHAUSTED || code == PRP_ERR_OOM) {
            SET_LAST_ERR_CODE(PRP_ERR_OOM);
            goto free_internals;
        } else if (code != PRP_OK) {
            SET_LAST_ERR_CODE(PRP_ERR_INTERNAL);
            goto free_internals;
        }
    }

    return world;

free_internals:
    if (data.layouts) {
        DT_ArrDeleteUnchecked(&data.layouts);
    }
    if (data.system_caches) {
        DT_ArrDeleteUnchecked(&data.system_caches);
    }

    return DT_DS_INVALID_ID;
}

DT_void WorldDelete(DT_DSId *pWorld_id) {
    ASSERT_CTX_INVARIANT_EXPR;

    DT_DSArrDelElemUnchecked(g_ctx->worlds, pWorld_id);
}

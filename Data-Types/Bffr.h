#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../Data-Types/Typedefs.h"
#include "../Utils/Defs.h"

typedef struct _Bffr DT_Bffr;

PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreate(DT_size memb_size, DT_size cap);
PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrCreateDefault(DT_size memb_size);
PRP_FN_API DT_Bffr *PRP_FN_CALL DT_BffrClone(DT_Bffr *bffr);

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrDelete(DT_Bffr **pBffr);

PRP_FN_API DT_void *PRP_FN_CALL DT_BffrRaw(DT_Bffr *bffr, DT_size *pCap);
PRP_FN_API DT_size PRP_FN_CALL DT_BffrCap(DT_Bffr *bffr);
PRP_FN_API DT_size PRP_FN_CALL DT_BffrMembSize(DT_Bffr *bffr);

PRP_FN_API DT_void *PRP_FN_CALL DT_BffrGet(DT_Bffr *bffr, DT_size i);
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrSet(DT_Bffr *bffr, DT_size i,
                                             DT_void *data);

PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrCmp(DT_Bffr *bffr1, DT_Bffr *bffr2,
                                             DT_bool *pRslt);
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrExtend(DT_Bffr *bffr1, DT_Bffr *bffr2);
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrClear(DT_Bffr *bffr);
PRP_FN_API PRP_FnCode PRP_FN_CALL DT_BffrChangeSize(DT_Bffr *bffr,
                                                    DT_size new_cap);

#ifdef __cplusplus
}
#endif

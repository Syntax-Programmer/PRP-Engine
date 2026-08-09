#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

typedef PRP_Result (*PRP_ThreadFunc)(void *pData);

typedef enum PRP_ThreadPriority {
    PRP_THREAD_PRIORITY_LVL_0 = 0,
    PRP_THREAD_PRIORITY_LVL_1,
    PRP_THREAD_PRIORITY_LVL_2,
    PRP_THREAD_PRIORITY_LVL_3,
    PRP_THREAD_PRIORITY_LVL_4,
    PRP_THREAD_PRIORITY_LVL_5,
    PRP_THREAD_PRIORITY_LVL_6,
} PRP_ThreadPriority;

#define PRP_THREAD_PRIORITY_MIN (PRP_THREAD_PRIORITY_LVL_0)
#define PRP_THREAD_PRIORITY_MAX (PRP_THREAD_PRIORITY_LVL_6)
#define PRP_THREAD_PRIORITY_DEFAULT (PRP_THREAD_PRIORITY_LVL_3)

typedef struct PRP_ThreadCreateInfo {
    PRP_ThreadFunc pThread_func;
    void *pUser_data;

    const PRP_Char8 *pName;
    PRP_Size name_len;

    PRP_ThreadPriority priority;
    /*
     * Thread affinity also come here.
     */
} PRP_ThreadCreateInfo;

// TODO:---------Change this to CONT_DSId when we use it.
typedef PRP_Size PRP_ThreadId;

PRP_API PRP_Result PRP_CALL PRP_ThreadCreate(
    const PRP_ThreadCreateInfo *pThread_create_info, PRP_ThreadId *pThread_id);
PRP_API PRP_Result PRP_CALL PRP_ThreadJoin(PRP_ThreadId thread_id,
                                           PRP_Result *pRslt);
PRP_API PRP_Result PRP_CALL PRP_ThreadDetach(PRP_ThreadId thread_id);

PRP_API PRP_Result PRP_CALL PRP_ThreadGetCurrentId(PRP_ThreadId *pThread_id);
PRP_API PRP_Result PRP_CALL PRP_ThreadIsCurrent(PRP_ThreadId thread_id,
                                                PRP_Bool *pRslt);

PRP_API PRP_Result PRP_CALL PRP_ThreadSetName(PRP_ThreadId thread_id,
                                              const PRP_Char8 *pName,
                                              PRP_Size len);
PRP_API PRP_Result PRP_CALL PRP_ThreadSetPriority(PRP_ThreadId thread_id,
                                                  PRP_ThreadPriority priority);
PRP_API PRP_Result PRP_CALL PRP_ThreadSetAffinity(PRP_ThreadId thread_id, );

PRP_API PRP_Result PRP_CALL PRP_ThreadIsRunning(PRP_ThreadId thread_id,
                                                PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL PRP_ThreadIsJoinable(PRP_ThreadId thread_id,
                                                 PRP_Bool *pRslt);

#ifdef __cplusplus
}
#endif

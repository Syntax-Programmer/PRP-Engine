#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Core/Defs.h"

typedef PRP_Result (*PRP_ThrdThreadFunc)(void *pData);

typedef enum PRP_ThrdhreadPriority {
    PRP_THRD_THREAD_PRIORITY_LVL_0 = 0,
    PRP_THRD_THREAD_PRIORITY_LVL_1,
    PRP_THRD_THREAD_PRIORITY_LVL_2,
    PRP_THRD_THREAD_PRIORITY_LVL_3,
    PRP_THRD_THREAD_PRIORITY_LVL_4,
    PRP_THRD_THREAD_PRIORITY_LVL_5,
    PRP_THRD_THREAD_PRIORITY_LVL_6,
} PRP_ThrdThreadPriority;

#define PRP_THRD_THREAD_PRIORITY_MIN (PRP_THRD_THREAD_PRIORITY_LVL_0)
#define PRP_THRD_THREAD_PRIORITY_MAX (PRP_THRD_THREAD_PRIORITY_LVL_6)
#define PRP_THRD_THREAD_PRIORITY_DEFAULT (PRP_THRD_THREAD_PRIORITY_LVL_3)

typedef struct {
    PRP_Size word_c;
    PRP_Size cpu_c;
    PRP_U64 pWords[];
} PRP_ThrdCpuAffinity;

typedef struct PRP_ThrdThreadCreateInfo {
    PRP_ThrdThreadFunc pThread_func;
    void *pUser_data;

    const PRP_Char8 *pName;
    PRP_Size name_len;

    PRP_ThrdThreadPriority priority;
    PRP_ThrdCpuAffinity affinity;
} PRP_ThrdThreadCreateInfo;

// TODO:---------Change this to CONT_DSId when we use it.
typedef PRP_Size PRP_ThrdThreadId;

PRP_API PRP_Result PRP_CALL
PRP_ThreadCreate(const PRP_ThrdThreadCreateInfo *pThread_create_info,
                 PRP_ThrdThreadId *pThread_id);
PRP_API PRP_Result PRP_CALL PRP_ThreadJoin(PRP_ThrdThreadId thread_id,
                                           PRP_Result *pRslt);
PRP_API PRP_Result PRP_CALL PRP_ThreadDetach(PRP_ThrdThreadId thread_id);

PRP_API PRP_Result PRP_CALL
PRP_ThreadGetCurrentId(PRP_ThrdThreadId *pThread_id);
PRP_API PRP_Result PRP_CALL PRP_ThreadIsCurrent(PRP_ThrdThreadId thread_id,
                                                PRP_Bool *pRslt);

PRP_API PRP_Result PRP_CALL PRP_ThreadSetName(PRP_ThrdThreadId thread_id,
                                              const PRP_Char8 *pName,
                                              PRP_Size len);
PRP_API PRP_Result PRP_CALL PRP_ThreadSetPriority(
    PRP_ThrdThreadId thread_id, PRP_ThrdThreadPriority priority);
PRP_API PRP_Result PRP_CALL PRP_ThreadSetAffinity(PRP_ThrdThreadId thread_id,
                                                  PRP_ThrdCpuAffinity affinity);

PRP_API PRP_Result PRP_CALL PRP_ThreadIsRunning(PRP_ThrdThreadId thread_id,
                                                PRP_Bool *pRslt);
PRP_API PRP_Result PRP_CALL PRP_ThreadIsJoinable(PRP_ThrdThreadId thread_id,
                                                 PRP_Bool *pRslt);

#ifdef __cplusplus
}
#endif

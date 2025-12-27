#include "Shared-Internals.h"

CORE_Id QueryCreate(CORE_Id exclude_b_set_id, CORE_Id include_b_set_id);
PRP_FnCode QueryDelete(CORE_Id **pQuery_id);

PRP_FnCode QueryDelCb(DT_void *query);
PRP_FnCode QueryCascadeLayoutCreate(CORE_Id layout_id);
PRP_FnCode QueryCascadeLayoutDelete(CORE_Id layout_id);

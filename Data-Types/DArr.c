#include "DArr.h"
#include "../Utils/Logger.h"
#include <string.h>

struct _DArr {
  PRP_size cap;
  PRP_size len;
  PRP_size memb_size;
  PRP_u8 *mem;
};

#define ARRAY_VALIDITY_CHECK(arr, ret)                                         \
  do {                                                                         \
    if (!arr) {                                                                \
      PRP_LOG_FN_INV_ARG_ERROR(arr);                                           \
      return ret;                                                              \
    }                                                                          \
  } while (0)

#define DEFAULT_NEW_CAP(cap) ((cap) * 2)

static PRP_FnCode DArrChangeSize(PRP_DArr *arr, PRP_size new_cap);

static PRP_FnCode DArrChangeSize(PRP_DArr *arr, PRP_size new_cap) {
  if (arr->cap == new_cap) {
    return PRP_FN_SUCCESS;
  }

  PRP_u8 *mem = realloc(arr->mem, new_cap);
  if (!mem) {
    PRP_LOG_FN_MALLOC_ERROR(mem);
    return PRP_FN_MALLOC_ERROR;
  }

  arr->mem = mem;
  arr->cap = new_cap;

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_DArr *PRP_FN_CALL PRP_DArrCreate(PRP_size memb_size,
                                                PRP_size cap) {
  if (!memb_size) {
    PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                    "PRP_DArray can't be made with memb_size=0.");
    return PRP_null;
  }
  if (!cap) {
    PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                    "PRP_DArray can't be made with cap=0.");
    return PRP_null;
  }

  PRP_DArr *arr = malloc(sizeof(PRP_DArr));
  if (arr) {
    PRP_LOG_FN_MALLOC_ERROR(arr);
    return PRP_null;
  }
  arr->mem = malloc(memb_size * cap);
  if (arr->mem) {
    free(arr);
    PRP_LOG_FN_MALLOC_ERROR(arr->mem);
    return PRP_null;
  }
  arr->memb_size = memb_size;
  arr->cap = cap;
  arr->len = 0;

  return arr;
}

PRP_FN_API PRP_DArr *PRP_FN_CALL PRP_DArrCreateDefault(PRP_size memb_size) {
  return PRP_DArrCreate(memb_size, 16);
}

PRP_FN_API PRP_DArr *PRP_FN_CALL PRP_DArrClone(PRP_DArr *arr) {
  ARRAY_VALIDITY_CHECK(arr, PRP_null);

  PRP_DArr *cpy = PRP_DArrCreate(arr->memb_size, arr->cap);
  if (!cpy) {
    PRP_LOG_FN_MALLOC_ERROR(cpy);
    return PRP_null;
  }

  cpy->len = arr->len;
  memcpy(cpy->mem, arr->mem, arr->memb_size * arr->len);

  return cpy;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrDelete(PRP_DArr **pArr) {
  if (!pArr) {
    PRP_LOG_FN_INV_ARG_ERROR(pArr);
    return PRP_FN_INV_ARG_ERROR;
  }
  PRP_DArr *arr = *pArr;
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);

  if (arr->mem) {
    free(arr->mem);
    arr->mem = PRP_null;
  }
  arr->cap = arr->len = arr->memb_size = 0;
  free(arr);
  *pArr = PRP_null;

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_void *PRP_FN_CALL PRP_DArrRaw(PRP_DArr *arr, PRP_size *pLen) {
  ARRAY_VALIDITY_CHECK(arr, PRP_null);
  if (!pLen) {
    PRP_LOG_FN_INV_ARG_ERROR(pLen);
    return PRP_null;
  }

  *pLen = arr->len;

  return arr->mem;
}

PRP_FN_API PRP_size PRP_FN_CALL PRP_DArrLen(PRP_DArr *arr) {
  ARRAY_VALIDITY_CHECK(arr, PRP_INVALID_SIZE);

  return arr->len;
}

PRP_FN_API PRP_size PRP_FN_CALL PRP_DArrCap(PRP_DArr *arr) {
  ARRAY_VALIDITY_CHECK(arr, PRP_INVALID_SIZE);

  return arr->cap;
}

PRP_FN_API PRP_void *PRP_FN_CALL PRP_DArrGet(PRP_DArr *arr, PRP_size i) {
  ARRAY_VALIDITY_CHECK(arr, PRP_null);
  if (i >= arr->len) {
    PRP_LOG_FN_CODE(
        PRP_FN_OOB_ERROR,
        "Tried accessing the array index: %zu, of an array with len: %zu", i,
        arr->len);
    return PRP_null;
  }

  return arr->mem + (i * arr->memb_size);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrSet(PRP_DArr *arr, PRP_size i,
                                              PRP_void *data) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);
  if (!data) {
    PRP_LOG_FN_INV_ARG_ERROR(data);
  }
  if (i >= arr->len) {
    PRP_LOG_FN_CODE(
        PRP_FN_OOB_ERROR,
        "Tried accessing the array index: %zu, of an array with len: %zu", i,
        arr->len);
    return PRP_FN_OOB_ERROR;
  }

  memcpy(arr->mem + (i * arr->memb_size), data, arr->memb_size);

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrPush(PRP_DArr *arr, PRP_void *data) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);
  if (!data) {
    PRP_LOG_FN_INV_ARG_ERROR(data);
  }

  if (arr->len == arr->cap &&
      DArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap)) != PRP_FN_SUCCESS) {
    PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                    "Cannot push more elements to the array. Array is full.");
    return PRP_FN_RES_EXHAUSTED_ERROR;
  }
  memcpy(arr->mem + ((arr->len++) * arr->memb_size), data, arr->memb_size);

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrReserve(PRP_DArr *arr,
                                                  PRP_size count) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);
  if (!count) {
    PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                    "Cannot reserve 0 members in PRP_DArr.");
    return PRP_FN_INV_ARG_ERROR;
  }

  if (arr->cap - arr->len >= count) {
    return PRP_FN_SUCCESS;
  }

  return DArrChangeSize(arr, arr->len + count);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrInsert(PRP_DArr *arr, PRP_void *data,
                                                 PRP_size i) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);
  if (!data) {
    PRP_LOG_FN_INV_ARG_ERROR(data);
  }
  if (i >= arr->len) {
    PRP_LOG_FN_CODE(
        PRP_FN_OOB_ERROR,
        "Tried accessing the array index: %zu, of an array with len: %zu", i,
        arr->len);
    return PRP_FN_OOB_ERROR;
  }

  if (arr->len == arr->cap &&
      DArrChangeSize(arr, DEFAULT_NEW_CAP(arr->cap)) != PRP_FN_SUCCESS) {
    PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                    "Cannot insert more elements to the array. Array is full.");
    return PRP_FN_RES_EXHAUSTED_ERROR;
  }
  memmove(arr->mem + ((i + 1) * arr->memb_size),
          arr->mem + (i * arr->memb_size), (arr->len - i) * arr->memb_size);
  memcpy(arr->mem + (i * arr->memb_size), data, arr->memb_size);
  arr->len++;

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrPop(PRP_DArr *arr, PRP_void *dest) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);

  if (!arr->len) {
    PRP_LOG_FN_CODE(PRP_FN_RES_EXHAUSTED_ERROR,
                    "No more elements to pop from array.");
    return PRP_FN_RES_EXHAUSTED_ERROR;
  }
  arr->len--;
  if (dest) {
    memcpy(dest, arr->mem + (arr->len * arr->memb_size), arr->memb_size);
  }

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrRemove(PRP_DArr *arr, PRP_void *dest,
                                                 PRP_size i) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);
  if (i >= arr->len) {
    PRP_LOG_FN_CODE(
        PRP_FN_OOB_ERROR,
        "Tried accessing the array index: %zu, of an array with len: %zu", i,
        arr->len);
    return PRP_FN_OOB_ERROR;
  }

  if (dest) {
    memcpy(dest, arr->mem + (i * arr->memb_size), arr->memb_size);
  }
  memmove(arr->mem + (i * arr->memb_size),
          arr->mem + ((i + 1) * arr->memb_size),
          (arr->len - i - 1) * arr->memb_size);
  arr->len--;

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_bool PRP_FN_CALL PRP_DArrCmp(PRP_DArr *arr1, PRP_DArr *arr2) {
  ARRAY_VALIDITY_CHECK(arr1, PRP_false);
  ARRAY_VALIDITY_CHECK(arr2, PRP_false);

  if (arr1->len != arr2->len || arr1->memb_size != arr2->memb_size) {
    return PRP_false;
  }

  return memcmp(arr1->mem, arr2->mem, arr1->len * arr1->memb_size) == 0;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrExtend(PRP_DArr *arr1,
                                                 PRP_DArr *arr2) {
  ARRAY_VALIDITY_CHECK(arr1, PRP_FN_INV_ARG_ERROR);
  ARRAY_VALIDITY_CHECK(arr2, PRP_FN_INV_ARG_ERROR);
  if (arr1->memb_size != arr2->memb_size) {
    PRP_LOG_FN_CODE(PRP_FN_INV_ARG_ERROR,
                    "Cannot join two arrays with different memb_sizes.");
    return PRP_FN_INV_ARG_ERROR;
  }

  PRP_size new_cap = arr1->len + arr2->len;
  PRP_u8 *mem = realloc(arr1->mem, new_cap * arr1->memb_size);
  if (mem) {
    PRP_LOG_FN_MALLOC_ERROR(mem);
    return PRP_FN_MALLOC_ERROR;
  }
  memcpy(mem + (arr1->len * arr1->memb_size), arr2->mem,
         arr2->len * arr2->memb_size);
  arr1->mem = mem;
  arr1->len = arr1->cap = new_cap;

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrReset(PRP_DArr *arr) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);

  arr->len = 0;

  return PRP_FN_SUCCESS;
}

PRP_FN_API PRP_FnCode PRP_FN_CALL PRP_DArrShrinkFit(PRP_DArr *arr) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);

  // The ternary op makes sure there is space of at-least one elem.
  return DArrChangeSize(arr, (arr->len) ? arr->len : 1);
}

PRP_FN_API PRP_FnCode PRP_FN_CALL
PRP_DArrForEach(PRP_DArr *arr, PRP_FnCode (*cb)(PRP_void *val)) {
  ARRAY_VALIDITY_CHECK(arr, PRP_FN_INV_ARG_ERROR);
  if (!cb) {
    PRP_LOG_FN_INV_ARG_ERROR(cb);
    return PRP_FN_INV_ARG_ERROR;
  }

  PRP_u8 *mem = arr->mem;
  for (PRP_size i = 0; i < arr->len; i++) {
    if (cb(mem) != PRP_FN_SUCCESS) {
      /*
       * We don't care why the foreach was called to be terminated. There was no
       * error from our side so even after termination it is still a success.
       */
      return PRP_FN_SUCCESS;
    }
    mem += arr->memb_size;
  }

  return PRP_FN_SUCCESS;
}

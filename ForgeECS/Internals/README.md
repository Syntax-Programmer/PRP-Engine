## Assumtion assumed across all three sub modules:

Any valid id produced/provided can be used to directly index into their
respective array.
i.e., an FECS_CompId can be directly indexed into g_ctx->pComp_sizes. Or
an FECS_Layout can be directly indexed into the pLayout array of it's respective world.

FECS_LayoutId and FECS_SystemInstanceId are world depended and the same layout/system_instance can have different ids if they are present in different worlds.

## -- IMPORTANT --

To update ChunkCap for the system do it across four places.

ONLY PERMITTED CHUNK CAPS:

- 8
- 16
- 32
- 64

To change chunk cap:

- Change CHUNK_CAP macro to required permitted chunk cap.

- Change the data typedef underlying type int of **WorldInternals.FECS_ChunkFreeSlotType** and **Typedefs.FECS_SystemExecOccupancyMask** DT_u(x), where x is the required permitted chunk cap.

- Change the below macros inside Layout.c file so that
  #define ENTITY_SLOT_MASK ((DT_size)x)
  #define ENTITY_SLOT_BITS (y)
  x = required permitted chunk cap - 1;
  y = log2(required permitted chunk cap);

- DIAG_STATIC_ASSERT(CHUNK_CAP == sizeof(DT_u(x)) * 8,
  "free_slot bit width must match CHUNK_CAP");
  Update this macro inside WorldInternals.h to:
  x = required permitted chunk cap - 1;

#ifndef XMEMMGR_H
#define XMEMMGR_H

#include <types.h>

struct xMemPool;
typedef void(*xMemPoolInitCallback)(xMemPool*, void*);

struct xMemPool
{
    void* FreeList;
    U16 NextOffset;
    U16 Flags;
    void* UsedList;
    xMemPoolInitCallback InitCB;
    void* Buffer;
    U16 Size;
    U16 NumRealloc;
    U32 Total;
};

typedef struct xMemArea_tag
{
    U32 addr;
    U32 size;
    U32 flags;
} xMemArea;

typedef struct xMemInfo_tag
{
    xMemArea system;
    xMemArea stack;
    xMemArea DRAM;
    xMemArea SRAM;
} xMemInfo;

#define XMEMMAXSTACK 12

enum { XMEM_MAX_HEAPS = 3 };

typedef void(*xMemBaseNotifyFunc)();

typedef struct xHeapState_tag
{
    U32 curr;
    U16 blk_ct;
    U16 pad;
    U32 used;
    U32 wasted;
} xHeapState;

typedef struct xMemBlock_tag
{
    U32 addr;
    U32 size;
    S32 align;
} xMemBlock;

typedef struct xMemHeap_tag
{
    U32 flags;
    U32 hard_base;
    U32 size;
    S16 opp_heap[2];
    xHeapState state[XMEMMAXSTACK];
    U16 state_idx;
    U16 max_blks;
    xMemBlock* blk;
    xMemBlock* lastblk;
} xMemHeap;

extern xMemInfo gMemInfo;
extern U32 gActiveHeap;

#define xMEMALLOC(size, align) xMemAlloc(gActiveHeap, (size), (align))
#define xMEMPUSHTEMP(amt) xMemPushTemp((amt))
#define xMEMPOPTEMP(pointer) xMemPopTemp((pointer))

#define xMemGrowAllocSize(size) xMemGrowAlloc(gActiveHeap, size)
#define xMemAllocSize(size) xMemAlloc(gActiveHeap, size, 0)
#define xMemAllocSizeAlign(size, align) xMemAlloc(gActiveHeap, size, align)

void xMemDebug_SoakLog(const char*);
void xMemInit();
void xMemExit();
void xMemInitHeap(xMemHeap* heap, U32 base, U32 size, U32 flags);

S32 xMemPushBase(U32 heapID);
S32 xMemPushBase();
S32 xMemPopBase(U32 heapID, S32 depth);
S32 xMemPopBase(S32 depth);
S32 xMemGetBase(U32 heapID);
void xMemRegisterBaseNotifyFunc(xMemBaseNotifyFunc func);
S32 xMemGetBase();
void xMemPoolSetup(xMemPool* pool, void* buffer, U32 nextOffset, U32 flags, xMemPoolInitCallback initCB, U32 size, U32 count, U32 numRealloc);
void* xMemPoolAlloc(xMemPool* pool);
void xMemPoolFree(xMemPool* pool, void* data);
void* xMemGrowAlloc(U32 heapID, U32 size);
void* xMemAlloc(U32 heapID, U32 size, S32 align);
void* xMemPushTemp(U32 size);
void xMemPopTemp(void* addr);

#endif

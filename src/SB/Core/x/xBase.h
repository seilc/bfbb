#ifndef XBASE_H
#define XBASE_H

#include "xLinkAsset.h"
#include "xserializer.h"

// Size: 0x8
struct xBaseAsset
{
    U32 id;
    U8 baseType;
    U8 linkCount;
    U16 baseFlags;
};

struct xBase;

typedef S32 (*xBaseEventCB)(xBase*, xBase*, U32, const F32*, xBase*);

// Size: 0x10
struct xBase
{
    U32 id;
    U8 baseType; // see en_ZBASETYPE in zBase.h
    U8 linkCount;
    U16 baseFlags;
    xLinkAsset* link;
    xBaseEventCB eventFunc; // 0xC
};

// xBase::baseFlags
#define k_XBASE_IS_ENABLED ((U16)(1 << 0))
#define k_XBASE_IS_VALID ((U16)(1 << 2))
#define k_XBASE_RECEIVES_SHADOWS ((U16)(1 << 4))

void xBaseInit(xBase* xb, xBaseAsset* asset);
void xBaseSetup(xBase* xb);
void xBaseSave(xBase* ent, xSerial* s);
void xBaseLoad(xBase* ent, xSerial* s);
void xBaseReset(xBase* xb, xBaseAsset* asset);
U32 xBaseIsValid(xBase* xb);
inline void xBaseValidate(xBase* xb)
{
    xb->baseFlags |= k_XBASE_IS_VALID;
}

inline bool xBaseIsEnabled(const xBase* xb)
{
    return (xb->baseFlags & k_XBASE_IS_ENABLED);
}

inline void xBaseDisable(xBase* xb)
{
    xb->baseFlags &= ~k_XBASE_IS_ENABLED;
}

inline void xBaseEnable(xBase* xb)
{
    xb->baseFlags |= k_XBASE_IS_ENABLED;
}

#endif

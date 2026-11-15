#ifndef XJSP_H
#define XJSP_H

#include "xClumpColl.h"

#include <rwcore.h>
#include <rpworld.h>

struct xJSPNodeInfo
{
    S32 originalMatIndex;
    S32 nodeFlags;
};

struct xJSPHeader
{
    char idtag[4];
    U32 version;
    U32 jspNodeCount;
    RpClump* clump;
    xClumpCollBSPTree* colltree;
    xJSPNodeInfo* jspNodeList;
};

void xJSP_MultiStreamRead(void* data, U32 size, xJSPHeader** jsp);
void xJSP_Destroy(xJSPHeader* jsp);

#endif

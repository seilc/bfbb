#include "xCommon.h"
#include "iDraw.h"

#include <dolphin.h>

#include <stddef.h>

void iDrawSetFBMSK(U32 abgr)
{
    size_t tmp, hi;

    hi = abgr >> 24;

    if (hi == 0)
    {
        GXSetAlphaUpdate(GX_TRUE);
    }
    else if (hi == 255)
    {
        GXSetAlphaUpdate(GX_FALSE);
    }

    tmp = abgr & 0x00FFFFFF;

    if (tmp == 0)
    {
        GXSetColorUpdate(GX_TRUE);
    }
    else
    {
        GXSetColorUpdate(GX_FALSE);
    }
}

#if PAL
extern GXRenderModeObj* _RwDlRenderMode;

void iDrawSetDisplayOffset(F32 x, F32 y)
{
    F32 nPosX = x * SCREEN_WIDTH + 40.0f;
    F32 nPosY = y * SCREEN_HEIGHT + 23.0f;

    nPosX = CLAMP(nPosX, 0.0f, 80.0f);
    nPosY = CLAMP(nPosY, 0.0f, 551.0f);

    _RwDlRenderMode->viXOrigin = nPosX;
    _RwDlRenderMode->viYOrigin = nPosY;

    VIConfigure(_RwDlRenderMode);
}
#endif

void iDrawBegin()
{
    return;
}

void iDrawEnd()
{
    return;
}

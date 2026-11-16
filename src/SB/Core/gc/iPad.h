#ifndef IPAD_H
#define IPAD_H

#include <types.h>

typedef struct _tagxPad xPad;
typedef struct _tagxRumble xRumble;

typedef struct _tagiPad
{
    S32 port;
} iPad;

S32 iPadInit();
xPad* iPadEnable(xPad* pad, S16 port);
S32 iPadConvStick(float value);
S32 iPadUpdate(xPad* pad, U32* on);
S32 iPadConvFromGCN(U32 a, U32 b, U32 c);
void iPadRumbleFx(xPad* p, xRumble* r, F32 time_passed);
void iPadStopRumble(xPad* pad);
void iPadStopRumble();
void iPadStartRumble(xPad* pad, xRumble* rumble);
void iPadKill();

#endif

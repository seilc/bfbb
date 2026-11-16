#ifndef XPAD_H
#define XPAD_H

#include "iPad.h"
#include "xMath2.h"

#define k_XPAD_MAX 4
#define k_XPAD_BUTTON_MAX 22
#define k_XPAD_ANALOG_MAX 2

typedef enum _tagRumbleType
{
    eRumble_Off,
    eRumble_Hi,
    eRumble_VeryLightHi,
    eRumble_VeryLight,
    eRumble_LightHi,
    eRumble_Light,
    eRumble_MediumHi,
    eRumble_Medium,
    eRumble_HeavyHi,
    eRumble_Heavy,
    eRumble_VeryHeavyHi,
    eRumble_VeryHeavy,
    eRumble_Total,
    eRumbleForceU32 = FORCEENUMSIZEINT
} RumbleType;

typedef struct _tagxRumble
{
    RumbleType type;
    F32 seconds;
    xRumble* next;
    S16 active;
    U16 fxflags;
} xRumble;

typedef struct _tagPadAnalog
{
    S8 x;
    S8 y;
} PadAnalog;

typedef enum _tagPadState
{
    ePad_Disabled,
    ePad_DisabledError,
    ePad_Enabled,
    ePad_Missing,
    ePad_Total
} PadState;

typedef struct _tagxPad
{
    struct analog_data
    {
        xVec2 offset;
        xVec2 dir;
        F32 mag;
        F32 ang;
    };

    U8 value[k_XPAD_BUTTON_MAX];
    U8 last_value[k_XPAD_BUTTON_MAX];
    U32 on;
    U32 pressed;
    U32 released;
    PadAnalog analog1;
    PadAnalog analog2;
    PadState state;
    U32 flags;
    xRumble rumble_head;
    S16 port;
    S16 slot;
    iPad context;
    F32 al2d_timer;
    F32 ar2d_timer;
    F32 d_timer;
    F32 up_tmr[k_XPAD_BUTTON_MAX];
    F32 down_tmr[k_XPAD_BUTTON_MAX];
    analog_data analog[k_XPAD_ANALOG_MAX];
} xPad;

// xPad::on
#define XPAD_BUTTON_START 0x1
#define XPAD_BUTTON_SELECT 0x2
#define XPAD_BUTTON_UP 0x10
#define XPAD_BUTTON_RIGHT 0x20
#define XPAD_BUTTON_DOWN 0x40
#define XPAD_BUTTON_LEFT 0x80
#define XPAD_BUTTON_L1 0x100
#define XPAD_BUTTON_L2 0x200
#define XPAD_BUTTON_R1 0x1000
#define XPAD_BUTTON_R2 0x2000
#define XPAD_BUTTON_X 0x10000
#define XPAD_BUTTON_O 0x20000
#define XPAD_BUTTON_SQUARE 0x40000
#define XPAD_BUTTON_TRIANGLE 0x80000
#define XPAD_BUTTON_Z 0x100000

extern xPad mPad[k_XPAD_MAX];
extern xPad* gDebugPad;
extern xPad* gPlayerPad;

S32 xPadInit();
xPad* xPadEnable(S32 idx);
void xPadRumbleEnable(S32 idx, S32 enable);
S32 xPadUpdate(S32 idx, F32 time_passed);
void xPadNormalizeAnalog(xPad& pad, S32 inner_zone, S32 outer_zone);
void xPadKill();
xRumble* xPadGetRumbleSlot();
void xPadDestroyRumbleChain(xPad* pad);
void xPadDestroyRumbleChain(S32 idx);
S32 xPadAddRumble(S32 idx, RumbleType type, F32 time, S32 replace, U32 fxflags);
void xPadAnalogIsDigital(S32 idx, S32 enable);

inline F32 normalize_analog(S32 v, S32 v_min, S32 v_max, S32 dead_center, S32 dead_min, S32 dead_max)
{
    if (v <= dead_center) {
        if (v >= dead_min) return 0.0f;
        else if (v <= v_min) return -1.0f;
        else return (F32)(v - dead_min) / (dead_min - v_min);
    } else {
        if (v <= dead_max) return 0.0f;
        else if (v >= v_max) return 1.0f;
        else return (F32)(v - dead_max) / (v_max - dead_max);
    }
}

#endif

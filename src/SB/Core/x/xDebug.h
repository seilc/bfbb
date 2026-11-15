#ifndef XDEBUG_H
#define XDEBUG_H

#include "xString.h"

#include <types.h>

extern U32 gFrameCount;

struct uint_data
{
    U32 value_def;
    U32 value_min;
    U32 value_max;
};

struct float_data
{
    F32 value_def;
    F32 value_min;
    F32 value_max;
};

struct bool_data
{
    U8 value_def;
};

struct select_data
{
    U32 value_def;
    U32 labels_size;
    char** labels;
    void* values;
};

struct flag_data
{
    U32 value_def;
    U32 mask;
};

struct raw_data
{
    U8 pad[16];
};

struct int_data
{
    S32 value_def;
    S32 value_min;
    S32 value_max;
};

struct tweak_callback;
struct tweak_info
{
    substr name;
    void* value;
    tweak_callback* cb;
    void* context;
    U8 type;
    U8 value_size;
    U16 flags;
    union
    {
        int_data int_context;
        uint_data uint_context;
        float_data float_context;
        bool_data bool_context;
        select_data select_context;
        flag_data flag_context;
        raw_data all_context;
    };
};

typedef void(*xDebugModeCallback)();

struct xSB;

void xSBRender(xSB* sb); // UNUSED
void xprintf(const char* fmt, ...);
S32 xDebugModeAdd(const char* debugModeName, xDebugModeCallback func);
void xDebugInit();
void xDebugUpdate();
void xDebugExit();
void xDebugTimestampScreen();

inline void xDebugRemoveTweak(const char*)
{
}

inline void xDebugAddTweak(const char*, F32*, F32, F32, const tweak_callback*, void*, U32)
{
}

inline void xDebugAddTweak(const char*, S16*, S16, S16, const tweak_callback*, void*, U32)
{
}

inline void xDebugAddTweak(const char*, U8*, U8, U8, const tweak_callback*, void*, U32)
{
}

inline void xDebugAddTweak(const char*, const char*, const tweak_callback*, void*, U32)
{
}

inline void xDebugAddFlagTweak(const char*, U32*, U32, const tweak_callback*, void*, U32)
{
}

inline void xDebugAddSelectTweak(const char*, U32*, const char**, const U32*, U32,
                                 const tweak_callback*, void*, U32)
{
}

#define xTWEAK(name, v, vmin, vmax, cb, context, flags) xDebugAddTweak(name, v, vmin, vmax, cb, context, flags)

#endif

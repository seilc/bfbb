#ifndef XHUDASSET_H
#define XHUDASSET_H

#include "xHud.h"

namespace xhud {

struct model_asset : asset
{
    U32 model;

    static const char* type_name() { return "hud:model"; }
};

struct text_asset : asset
{
    U32 text_box;
    U32 text;

    static const char* type_name() { return "hud:text"; }
};

struct meter_asset : asset
{
    F32 start_value;
    F32 min_value;
    F32 max_value;
    F32 increment_time;
    F32 decrement_time;
    struct
    {
        U32 start_increment;
        U32 increment;
        U32 start_decrement;
        U32 decrement;
    } sound;

    static const char* type_name() { return "hud:meter"; }
};

struct font_meter_asset : meter_asset
{
    font_context font;
    U8 counter_mode;
    U8 pad1;
    U8 pad2;
    U8 pad3;

    static const char* type_name() { return "hud:meter:font"; }
};

}

#endif

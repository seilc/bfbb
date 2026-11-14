#ifndef XHUDASSET_H
#define XHUDASSET_H

#include "xHud.h"

namespace xhud {

struct model_asset : asset
{
    U32 model;

    static const char* type_name() { return "hud:model"; }
};

}

#endif

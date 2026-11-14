#ifndef ICOLOR_H
#define ICOLOR_H

#include "xHud.h"

#include <types.h>

struct iColor_tag
{
    U8 r;
    U8 g;
    U8 b;
    U8 a;

    // Conversion constructor instead?
    iColor_tag& operator=(const xhud::color32u& rhs)
    {
        this->r = rhs.r;
        this->g = rhs.g;
        this->b = rhs.b;
        this->a = rhs.a;
        return *this;
    };
};

#endif

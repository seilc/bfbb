#ifndef XHUDMODEL_H
#define XHUDMODEL_H

#include "xHud.h"

struct xAnimTable;

xAnimTable* XHUD_AnimTable_Idle();

namespace xhud {

struct model_asset;

class model_widget : public widget
{
public:
    struct widget_chunk;

    U32 mid;
    xModelInstance* model;

    static void load(xBase& data, xDynAsset& asset, size_t);

    model_widget(const model_asset& a);

    void destruct();
    virtual void destroy();
    virtual U32 type() const;
    virtual bool is(U32 id) const;
    virtual void update(F32 dt);
    virtual void render();
};

struct model_widget::widget_chunk : xBase
{
    model_widget w;
};

}

#endif

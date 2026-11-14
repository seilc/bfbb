#include "xCommon.h"
#include "xHudModel.h"

#include "xHudAsset.h"

#include <new>

xAnimTable* XHUD_AnimTable_Idle()
{
    xAnimTable* table = xAnimTableNew("XHUD_AnimTable_Idle", NULL, 0);
    F32 anspd = 1.0f;

    xAnimTableNewState(table, "Idle01", 0x10, 1, anspd, NULL, NULL, 0.0f, NULL, NULL, xAnimDefaultBeforeEnter, NULL,
                       NULL);

    return table;
}

namespace xhud {

void model_widget::load(xBase& data, xDynAsset& asset, size_t)
{
    init_base(data, asset, sizeof(widget_chunk));

    widget_chunk& wc = (widget_chunk&)data;
    new (&wc.w) model_widget((model_asset&)asset);
}

model_widget::model_widget(const model_asset& a)
    : widget(a)
    , mid(a.model)
    , model(0)
{
    model = xhud::load_model(mid);
}

void model_widget::destruct()
{
    widget::destruct();
}

void model_widget::destroy()
{
    destruct();
}

U32 model_widget::type() const
{
    static U32 myid = xStrHash(model_asset::type_name());
    return myid;
}

bool model_widget::is(U32 id) const
{
    return id == model_widget::type() || widget::is(id);
}

void model_widget::update(F32 dt)
{
    updater(dt);

    if (!visible() || rc.a <= 0.5f/255.0f) return;

    xModelUpdate(model, dt);
    xModelEval(model);
}

void model_widget::render()
{
    if (rc.a <= 0.0f)
        return;

    xhud::render_model(*model, rc);
}

}

#include "xCommon.h"
#include "xHudText.h"

#include "xHudAsset.h"
#include "zTextBox.h"
#include "zScene.h"
#include "xTextAsset.h"
#include "xstransvc.h"

#include <string.h>
#include <new>

namespace xhud {

void text_widget::load(xBase& data, xDynAsset& asset, size_t)
{
    init_base(data, asset, sizeof(widget_chunk));

    widget_chunk& wc = (widget_chunk&)data;
    new (&wc.w) text_widget((text_asset&)asset);
}

text_widget::text_widget(const text_asset& a)
    : widget(a)
{
    text[0] = '\0';
}

void text_widget::destruct() { widget::destruct(); }

void text_widget::setup()
{
    presetup();

    const text_asset& ta = *(text_asset*)a;
    ztextbox* ztb = (ztextbox*)zSceneFindObject(ta.text_box);

    if (ztb == 0) {
        tb = xtextbox::create();
        return;
    }

    ztb->refresh();
    tb = ztb->tb;
    ztb->get_text(text, sizeof(text));

    if (ta.text != 0) {
        xTextAsset* t = (xTextAsset*)xSTFindAsset(ta.text, NULL);

        if (t) {
            char* s = (char*)((U8*)t + sizeof(xTextAsset));
            U32 len = strlen(s);

            if (len >= sizeof(text)) {
                len = sizeof(text) - 1;
            }

            memcpy(text, s, len);
            text[len] = '\0';
        }
    }

    rc.r = tb.font.color.r / 255.0f;
    rc.g = tb.font.color.g / 255.0f;
    rc.b = tb.font.color.b / 255.0f;
}

void text_widget::destroy()
{
    destruct();
}

U32 text_widget::type() const
{
    static U32 myid = xStrHash(text_asset::type_name());
    return myid;
}

bool text_widget::is(U32 id) const
{
    return id == text_widget::type() || widget::is(id);
}

void text_widget::update(F32 dt)
{
    updater(dt);

    tb.set_text(text);

    tb.bounds.x = rc.loc.x;
    tb.bounds.y = rc.loc.y;
    tb.bounds.w = rc.size.x;
    tb.bounds.h = rc.size.y;

    tb.font.clip = tb.bounds;

    tb.font.color.r = (U8)(255.0f * rc.r + 0.5f);
    tb.font.color.g = (U8)(255.0f * rc.g + 0.5f);
    tb.font.color.b = (U8)(255.0f * rc.b + 0.5f);
    tb.font.color.a = (U8)(255.0f * rc.a + 0.5f);
}

void text_widget::render()
{
    F32 x = tb.bounds.x;
    F32 y = tb.bounds.y;
    U8 r = tb.font.color.r;
    U8 g = tb.font.color.g;
    U8 b = tb.font.color.b;
    U8 a = tb.font.color.a;
    F32 floatA = 200.0f * a / 255.0f;
    U8 newA;

    if (floatA > 255.0f) {
        newA = 255;
    } else if (floatA < 0.0f) {
        newA = 0;
    } else {
        newA = (U8)floatA;
    }

    tb.bounds.x += 0.0075f;
    tb.bounds.y += 0.0075f;
    tb.font.color.r = 0;
    tb.font.color.g = 70;
    tb.font.color.b = 85;
    tb.font.color.a = newA;
    tb.render(true);

    tb.bounds.x = x;
    tb.bounds.y = y;
    tb.font.color.r = r;
    tb.font.color.g = g;
    tb.font.color.b = b;
    tb.font.color.a = a;
    tb.render(true);
}

}

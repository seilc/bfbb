#include "xCommon.h"
#include "xCM.h"

#include "xDebug.h"
#include "xDebugTweak.h"
#include "xEvent.h"
#include "xFont.h"
#include "xShadow.h"
#include "xstransvc.h"

const U32 CM_ALIGN_CENTER = 0;
const U32 CM_ALIGN_LEFT = 1;
const U32 CM_ALIGN_RIGHT = 2;
const U32 CM_ALIGN_INNER = 3;
const U32 CM_ALIGN_TEXTURE = 4;

struct sxy
{
    F32 x;
    F32 y;
};

struct fade
{
    F32 start;
    F32 end;
};

struct xCreditsData
{
    U32 dummy;
};

struct xCMheader
{
    U32 magic;
    U32 version;
    U32 crdID;
    U32 state;
    F32 total_time;
    U32 total_size;
};

struct xCMcredits
{
    U32 credits_size;
    F32 len;
    U32 flags;
    sxy in;
    sxy out;
    F32 scroll_rate;
    F32 lifetime;
    fade fin;
    fade fout;
    U32 num_presets;
};

struct xCMtextbox
{
    U32 font;
    iColor_tag color;
    sxy char_size;
    sxy char_spacing;
    sxy box;
};

struct xCMtexture
{
    U32 assetID;
    iColor_tag color;
    F32 x;
    F32 y;
    F32 w;
    F32 h;
    RwTexture* texture;
    U32 pad;
};

struct xCMpreset
{
    U16 num;
    U16 align;
    F32 delay;
    F32 innerspace;
    xCMtextbox box[2];
};

struct xCMhunk
{
    U32 hunk_size;
    U32 preset;
    F32 t0;
    F32 t1;
    const char* text1;
    const char* text2;
};

struct xColorUnpack
{
    U8 r, g, b, a;
};

static void xCMprep(xCreditsData* data)
{
    xCMheader* hdr = (xCMheader*)data;
    if (hdr->magic != 0xBEEEEEEF)
        return;

    xCMhunk* hp;
    char* dp;
    xCMcredits* cp;
    xCMpreset* pp;

    dp = (char*)(hdr + 1);
    while (dp - (char*)data < hdr->total_size) {
        cp = (xCMcredits*)dp;
        pp = (xCMpreset*)(cp + 1);
        hp = (xCMhunk*)(pp + cp->num_presets);

        while ((char*)hp - (char*)cp < cp->credits_size) {
            xCMpreset* preset = &pp[hp->preset];
            switch (preset->align) {
            case CM_ALIGN_CENTER:
            case CM_ALIGN_LEFT:
            case CM_ALIGN_RIGHT:
            case CM_ALIGN_INNER:
                if (hdr->state == 0) {
                    if (hp->text1)
                        hp->text1 = (const char*)((U32)hp->text1 - (U32)data);
                    if (hp->text2)
                        hp->text2 = (const char*)((U32)hp->text2 - (U32)data);
                } else {
                    if (hp->text1)
                        hp->text1 = (const char*)((U32)hp->text1 + (U32)data);
                    if (hp->text2)
                        hp->text2 = (const char*)((U32)hp->text2 + (U32)data);
                }
                break;
            }

            hp = (xCMhunk*)((char*)hp + hp->hunk_size);
        }

        dp = (char*)hp;
    }

    hdr->state = (hdr->state == 0) ? 1 : 0;
}

static iColor_tag xCMcolor_scale(iColor_tag color, F32 scale)
{
    iColor_tag tmp = color;
    xColorUnpack tmp2 = *(xColorUnpack*)&tmp;

    F32 r = (F32)tmp2.r;
    F32 g = (F32)tmp2.g;
    F32 b = (F32)tmp2.b;
    F32 a = (F32)tmp2.a;

    a *= scale;

    xColorInit(&tmp, (U8)r, (U8)g, (U8)b, (U8)a);

    return tmp;
}

static const char* preset_names[] = {
    "CM_ALIGN_CENTER",
    "CM_ALIGN_LEFT",
    "CM_ALIGN_RIGHT",
    "CM_ALIGN_INNER",
    "CM_ALIGN_TEXTURE"
};

static U32 xCMrender(F32 time, xCreditsData* data)
{
    xCMhunk* hp;

    xCMheader* hdr = (xCMheader*)data;

    if (!hdr || hdr->magic != 0xBEEEEEEF)
        return FALSE;

    char* dp;
    xCMcredits* cp;
    xCMpreset* pp;

    if (hdr->state == 1) {
        xCMprep(data);
    }

    xprintf("credits time %6.2f\n", time);

    dp = (char*)(hdr + 1);
    while (dp - (char*)data < hdr->total_size) {
        cp = (xCMcredits*)dp;
        pp = (xCMpreset*)(cp + 1);
        hp = (xCMhunk*)(pp + cp->num_presets);

        while ((char*)hp - (char*)cp < cp->credits_size) {
            if (time >= hp->t0 && time <= hp->t1) {
                F32 a = (time - hp->t0) / (hp->t1 - hp->t0);
                xCMpreset* preset = &pp[hp->preset];

                F32 nx, ny;
                ny = a * (cp->out.y - cp->in.y) + cp->in.y;

                F32 x0, x1, y0, y1, ca;
                if (a < cp->fin.start || a > cp->fout.end)
                    ca = 0.0f;
                else if (a >= cp->fin.start && a <= cp->fin.end)
                    ca = (a - cp->fin.start) / (cp->fin.end - cp->fin.start);
                else if (a >= cp->fout.start && a <= cp->fout.end)
                    ca = (cp->fout.end - a) / (cp->fout.end - cp->fout.start);
                else
                    ca = 1.0f;

                switch (preset->align) {
                case CM_ALIGN_TEXTURE: {
                    xCMtexture* tex = (xCMtexture*)&preset->box[0];
                    if (!tex->texture)
                        tex->texture = (RwTexture*)xSTFindAsset(tex->assetID, NULL);
                    if (tex->texture) {
                        RwRaster* raster = RwTextureGetRaster(tex->texture);
                        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
                    }

                    nx = tex->x;
                    ny = tex->y;
                    x0 = nx * SCREEN_WIDTH;
                    y0 = ny * SCREEN_HEIGHT;
                    x1 = (nx + tex->w) * SCREEN_WIDTH;
                    y1 = (ny + tex->h) * SCREEN_HEIGHT;

                    Im2DRenderQuad(x0, y0, x1, y1, 0.0f, 1000000.0f, 0.5f);

                    xprintf("tex %6.2f,%6.2f - %6.2f,%6.2f\n", x0, y0, x1, y1);
                    break;
                }
                case CM_ALIGN_CENTER: {
                    nx = 0.5f * (1.0f - preset->box[0].box.x);
                    x0 = nx * SCREEN_WIDTH;
                    y0 = ny * SCREEN_HEIGHT;
                    x1 = (nx + preset->box[0].box.x) * SCREEN_WIDTH;
                    y1 = (ny + preset->box[0].box.y) * SCREEN_HEIGHT;

                    iColor_tag color = xCMcolor_scale(preset->box[0].color, ca);

                    basic_rect<F32> bounds = { nx, ny, preset->box[0].box.x, preset->box[0].box.y };

                    xtextbox tb = xtextbox::create(
                        xfont::create(
                            preset->box[0].font,
                            NSCREENX(preset->box[0].char_size.x),
                            NSCREENY(preset->box[0].char_size.y),
                            0.0f,
                            color,
                            screen_bounds),
                        bounds,
                        0x2,
                        0.0f,
                        0.0f,
                        0.0f,
                        0.0f);

                    tb.set_text(hp->text1);
                    tb.render(true);
                    break;
                }
                case CM_ALIGN_LEFT:
                case CM_ALIGN_RIGHT:
                case CM_ALIGN_INNER: {
                    nx = 0.5f * (1.0f - preset->box[0].box.x - preset->box[1].box.x - preset->innerspace);
                    x0 = nx * SCREEN_WIDTH;
                    y0 = ny * SCREEN_HEIGHT;
                    x1 = (nx + preset->box[0].box.x) * SCREEN_WIDTH;
                    y1 = (ny + preset->box[0].box.y) * SCREEN_HEIGHT;

                    U32 alignL = 0, alignR = 0;
                    if (preset->align == CM_ALIGN_LEFT) {
                        alignL = alignR = 0x0;
                    } else if (preset->align == CM_ALIGN_RIGHT) {
                        alignL = alignR = 0x1;
                    } else {
                        alignL = 0x1;
                        alignR = 0x0;
                    }

                    {
                        iColor_tag colorL = xCMcolor_scale(preset->box[0].color, ca);

                        basic_rect<F32> bounds = { nx, ny, preset->box[0].box.x, preset->box[0].box.y };

                        xtextbox tb = xtextbox::create(
                            xfont::create(
                                preset->box[0].font,
                                NSCREENX(preset->box[0].char_size.x),
                                NSCREENY(preset->box[0].char_size.y),
                                0.0f,
                                colorL,
                                screen_bounds),
                            bounds,
                            alignL,
                            0.0f,
                            0.0f,
                            0.0f,
                            0.0f);

                        tb.set_text(hp->text1);
                        tb.render(true);
                    }

                    nx += preset->box[0].box.x + preset->innerspace;

                    {
                        iColor_tag colorR = xCMcolor_scale(preset->box[1].color, ca);

                        basic_rect<F32> bounds = { nx, ny, preset->box[1].box.x, preset->box[1].box.y };

                        xtextbox tb = xtextbox::create(
                            xfont::create(
                                preset->box[0].font,
                                NSCREENX(preset->box[1].char_size.x),
                                NSCREENY(preset->box[1].char_size.y),
                                0.0f,
                                colorR,
                                screen_bounds),
                            bounds,
                            alignR,
                            0.0f,
                            0.0f,
                            0.0f,
                            0.0f);

                        tb.set_text(hp->text2);
                        tb.render(true);
                    }
                    break;
                }
                }
            }

            hp = (xCMhunk*)((char*)hp + hp->hunk_size);
        }

        dp = (char*)hp;
    }

    return time >= 0.0f && time <= hdr->total_time;
}

F32 dtscale = 1.0f;

static F32 credits_time = 10000.0f;
static xCreditsData* credits_data = NULL;

void xCMupdate(F32 dt)
{
    if (!credits_data) {
        return;
    }

    credits_time += dt * dtscale;

    xCMheader* hdr = (xCMheader*)credits_data;
    if (credits_time >= hdr->total_time) {
        xCMstop();
    }
}

void xCMrender()
{
    xCMrender(credits_time, credits_data);
}

static U32 credits_parentID = 0;

void xCMstart(xCreditsData* data, F32, xBase* parent)
{
    credits_data = data;

    if (!credits_data) {
        return;
    }

    credits_time = 0.0f;

    if (parent)
        credits_parentID = parent->id;
}

void xCMstop()
{
    credits_time = 10000.0f;
    credits_data = NULL;

    if (credits_parentID)
        zEntEvent(credits_parentID, eEventCreditsEnded);
}

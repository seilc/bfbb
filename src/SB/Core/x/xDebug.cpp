#include "xCommon.h"
#include "xDebug.h"

#include "xColor.h"
#include "xFont.h"

struct xSB
{
    char* buf;
    U32 max;
    char* cur;
    char* disp;
};

static const basic_rect<F32> screen_bounds = { 0, 0, 1, 1 };

U32 gFrameCount;

// UNUSED: needed to match (code from Rat GC)
void xSBRender(xSB* sb)
{
    if (!RwCameraGetCurrentCamera()) {
        return;
    }

    static xtextbox tb = xtextbox::create(
        xfont::create(2, NSCREENX(12.0f), NSCREENY(12.0f), NSCREENX(2.0f), g_WHITE, screen_bounds),
        screen_bounds, 0, NSCREENY(2.0f), 0.0f, 0.0f, 0.0f);
    tb.font.width = NSCREENX(12.0f);
    tb.font.height = NSCREENY(12.0f);
    tb.bounds = screen_bounds;
    tb.bounds.contract(NSCREENX(28.0f), NSCREENY(80.0f), NSCREENX(28.0f), NSCREENY(0.0f));
    S32 lineCount = 1;
    for (S32 i = 0; sb->disp[i] != '\0'; i++) {
        if (sb->disp[i] == '\n') {
            lineCount++;
            if (lineCount >= 32) {
                sb->disp[i] = '\0';
                break;
            }
        }
    }
    tb.set_text(sb->disp);
    tb.render(false);
}

void xprintf(const char*, ...)
{
}

S32 xDebugModeAdd(const char*, xDebugModeCallback)
{
    return -1;
}

void xDebugInit()
{
}

void xDebugUpdate()
{
}

void xDebugExit()
{
}

void xDebugTimestampScreen()
{
}

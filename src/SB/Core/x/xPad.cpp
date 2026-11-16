#include "xCommon.h"
#include "xPad.h"

#include "zMenu.h"
#include "zGameExtras.h"

#include <string.h>

U32 zScene_ScreenAdjustMode();
S32 zGameIsPaused();

#define k_XPAD_RUMBLE_MAX 32

static xRumble mRumbleList[k_XPAD_RUMBLE_MAX];
xPad mPad[k_XPAD_MAX];
xPad* gDebugPad;
xPad* gPlayerPad;

S32 xPadInit()
{
    memset(mPad, 0, sizeof(mPad));
    memset(mRumbleList, 0, sizeof(mRumbleList));

    if (!iPadInit()) {
        return 0;
    }

    gPlayerPad = &mPad[0];

    return 1;
}

xPad* xPadEnable(S32 idx)
{
    xPad* p = &mPad[idx];

    if (p->state != ePad_Disabled) {
        return p;
    }

    if (idx != 0) {
        return p;
    }

    p = iPadEnable(p, idx);
    xPadRumbleEnable(idx, TRUE);

    return p;
}

void xPadRumbleEnable(S32 idx, S32 enable)
{
    xPad* p = &mPad[idx];

    if (p->state != ePad_Enabled) {
        enable = FALSE;
    }

    if (enable) {
        if (p->flags & 0x4)
            p->flags |= 0x8;
    } else {
        if (p->flags & 0x8) {
            p->flags ^= 0x8;
            xPadDestroyRumbleChain(p);
        }
    }
}

S32 xPadUpdate(S32 idx, F32 time_passed)
{
    U32 new_on;

    if (idx != 0) {
        return 0;
    }

    if (zScene_ScreenAdjustMode()) {
        xPadAnalogIsDigital(idx, FALSE);
    }
    else if (zMenuRunning() || zGameIsPaused() || zGame_HackIsGallery()) {
        xPadAnalogIsDigital(idx, TRUE);
    } else {
        xPadAnalogIsDigital(idx, FALSE);
    }

    xPad* p = &mPad[idx];

    if ((p->flags & 0x4) && (p->flags & 0x8)) {
        xRumble* r = p->rumble_head.next;
        while (r) {
            r->seconds -= time_passed;
            if (r->seconds <= 0.0f) {
                r->active = 0;
                if (r->next == NULL) {
                    p->rumble_head.next = NULL;
                    r = NULL;
                    iPadStopRumble(p);
                } else {
                    p->rumble_head.next = r->next;
                    r = p->rumble_head.next;
                    iPadStartRumble(p, r);
                }
            } else {
                iPadRumbleFx(p, r, time_passed);
                break;
            }
        }
    }

    S32 ret = iPadUpdate(p, &new_on);
    if (!ret) {
        return 1;
    }

    if (p->flags & 0x10) {
        if (p->flags & 0x1) {
            U32 fake_dpad = 0;
            if (p->analog1.x >= 50) {
                fake_dpad |= 0x20;
            }
            else if (p->analog1.x <= -50) {
                fake_dpad |= 0x80;
            }
            if (p->analog1.y >= 50) {
                fake_dpad |= 0x40;
            }
            else if (p->analog1.y <= -50) {
                fake_dpad |= 0x10;
            }
            if (fake_dpad == 0) {
                p->al2d_timer = 0.0f;
            } else {
                p->al2d_timer -= time_passed;
                if (p->al2d_timer <= 0.0f) {
                    new_on |= fake_dpad;
                    p->al2d_timer = 0.35f;
                }
            }
        }
        if (p->flags & 0x2) {
            if (p->analog2.x > -50 && p->analog2.x < 50 && p->analog2.y > -50 && p->analog2.y < 50) {
                p->ar2d_timer = 0.0f;
            } else {
                p->ar2d_timer -= time_passed;
                if (p->ar2d_timer <= 0.0f) {
                    p->ar2d_timer = 0.35f;
                    if (p->analog2.x >= 50) {
                        new_on |= 0x20;
                    }
                    else if (p->analog2.x <= -50) {
                        new_on |= 0x80;
                    }
                    if (p->analog2.y >= 50) {
                        new_on |= 0x40;
                    }
                    else if (p->analog2.y <= -50) {
                        new_on |= 0x10;
                    }
                }
            }
        }
    }

    p->pressed = new_on & ~p->on;
    p->released = p->on & ~new_on;
    p->on = new_on;

    for (S32 i = 0; i < k_XPAD_BUTTON_MAX; i++) {
        if (p->pressed & (1<<i)) {
            p->down_tmr[i] = 0.0f;
        } else if (p->released & (1<<i)) {
            p->up_tmr[i] = 0.0f;
        }
        if (p->on & (1<<i)) {
            p->down_tmr[i] += time_passed;
        } else {
            p->up_tmr[i] += time_passed;
        }
    }

    if (p->flags & 0x10) {
        if (!(p->on & 0x10) && !(p->on & 0x40) && !(p->on & 0x80) && !(p->on & 0x20)) {
            p->d_timer = 0.0f;
        } else {
            p->d_timer -= time_passed;
            if (p->d_timer <= 0.0f) {
                p->d_timer = 0.35f;
                if (p->on & 0x10) {
                    p->pressed |= 0x10;
                }
                else if (p->on & 0x40) {
                    p->pressed |= 0x40;
                }
                if (p->on & 0x80) {
                    p->pressed |= 0x80;
                }
                else if (p->on & 0x20) {
                    p->pressed |= 0x20;
                }
            }
        }
    }

    return 1;
}

void xPadNormalizeAnalog(xPad& pad, S32 inner_zone, S32 outer_zone)
{
    const PadAnalog* src = &pad.analog1;
    for (S32 i = 0; i < k_XPAD_ANALOG_MAX; i++) {
        pad.analog[i].offset.x = normalize_analog(src[i].x, -outer_zone, outer_zone, 0, -inner_zone, inner_zone);
        pad.analog[i].offset.y = normalize_analog(src[i].y, -outer_zone, outer_zone, 0, -inner_zone, inner_zone);
        if (pad.analog[i].offset.x == 0.0f && pad.analog[i].offset.y == 0.0f) {
            pad.analog[i].mag = 0.0f;
            pad.analog[i].dir.assign(1.0f, 0.0f);
            pad.analog[i].ang = 0.0f;
        } else {
            pad.analog[i].mag = pad.analog[i].offset.length();
            pad.analog[i].dir = pad.analog[i].offset;
            pad.analog[i].dir /= pad.analog[i].mag;
            pad.analog[i].ang = xatan2(pad.analog[i].dir.y, pad.analog[i].dir.x);
        }
    }
}

void xPadKill()
{
    iPadKill();
}

xRumble* xPadGetRumbleSlot()
{
    for (S32 i = 0; i < k_XPAD_RUMBLE_MAX; i++) {
        if (!mRumbleList[i].active) {
            xRumble* r = &mRumbleList[i];
            memset(r, 0, sizeof(xRumble));
            return r;
        }
    }

    return NULL;
}

void xPadDestroyRumbleChain(xPad* pad)
{
    iPadStopRumble(pad);

    xRumble* r = pad->rumble_head.next;
    while (r) {
        memset(r, 0, sizeof(xRumble));
        // BUG: next is already cleared, so only the first is freed
        r = r->next;
    }

    pad->rumble_head.next = NULL;
}

void xPadDestroyRumbleChain(S32 idx)
{
    xPadDestroyRumbleChain(&mPad[idx]);
}

S32 xPadAddRumble(S32 idx, RumbleType type, F32 time, S32 replace, U32 fxflags)
{
    S32 appended;
    xPad* pad = &mPad[idx];

    if (!(pad->flags & 0x4)) {
        return 0;
    }

    if (!(pad->flags & 0x8)) {
        return 0;
    }

    appended = 1;

    if (replace) {
        xPadDestroyRumbleChain(pad);
        appended = 0;
    }

    xRumble* r = &pad->rumble_head;
    xRumble* last_r = r;
    if (r->next == NULL) {
        appended = 0;
    }

    while (r) {
        last_r = r;
        r = r->next;
    }

    r = xPadGetRumbleSlot();
    if (!r) {
        return 0;
    }

    r->type = type;
    r->seconds = time;
    r->active = 1;
    r->fxflags = fxflags;
    last_r->next = r;
    last_r->next->next = NULL;

    if (!appended) {
        iPadStartRumble(pad, r);
    }

    return 1;
}

void xPadAnalogIsDigital(S32 idx, S32 enable)
{
    if (idx != 0) {
        return;
    }

    xPad* pad = &mPad[idx];

    if (enable)
        pad->flags |= 0x10;
    else
        pad->flags &= ~0x10;

    if (pad->al2d_timer >= 0.35f)
        pad->al2d_timer = 0.35f;

    if (pad->ar2d_timer >= 0.35f)
        pad->ar2d_timer = 0.35f;

    if (pad->d_timer >= 0.35f)
        pad->d_timer = 0.35f;
}

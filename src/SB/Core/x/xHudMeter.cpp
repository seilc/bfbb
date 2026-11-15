#include "xCommon.h"
#include "xHudMeter.h"

#include "xHudAsset.h"
#include "xMath.h"
#include "xSnd.h"

#include <stdio.h>

namespace xhud {

namespace {

void add_global_tweaks()
{
}

}

void meter_widget::set_value(F32 v)
{
    F32 dvalue = v - value;
    F32 sign;

    if (dvalue < -0.01f) {
        if (res.sound.start_decrement) {
            xSndPlay(res.sound.start_decrement, 1.0f, 0.0f, 0x80, 0, 0, SND_CAT_GAME, 0.0f);
        }
        if (res.decrement_time < 0.01f) {
            set_value_immediate(v);
            return;
        }
        sign = -1.0f;
    } else if (dvalue > 0.01f) {
        if (res.sound.start_increment) {
            xSndPlay(res.sound.start_increment, 1.0f, 0.0f, 0x80, 0, 0, SND_CAT_GAME, 0.0f);
        }
        if (res.increment_time < 0.01f) {
            set_value_immediate(v);
            return;
        }
        sign = 1.0f;
    } else {
        set_value_immediate(v);
        return;
    }

    end_value = v;

    if (res.decrement_time == 0.0f) {
        printf("decrement time = 0 -- ass saved!\n");
    }

    // BUG: decrement_time for increases too
    value_vel = sign / MAX(res.decrement_time, 0.00001f);
    value_accel = 50.0f * sign;
    pitch = 0.0f;

    if (xsqrt(2.0f * dvalue / value_accel) > 2.0f) {
        value_accel = 2.0f * dvalue / 4.0f;
    }
}

void meter_widget::set_value_immediate(F32 v)
{
    value = v;
    end_value = v;
    value_vel = 0.0f;
}

meter_widget::meter_widget(const meter_asset& a)
    : widget(a)
    , res(a)
    , value(a.start_value)
    , min_value(a.min_value)
    , max_value(a.max_value)
    , end_value(a.start_value)
    , value_vel(0.0f)
    , ping_delay(10.0f)
    , pings()
{
    add_global_tweaks();
}

void meter_widget::destruct() { widget::destruct(); }

U32 meter_widget::type() const
{
    static U32 myid = xStrHash(meter_asset::type_name());
    return myid;
}

bool meter_widget::is(U32 id) const
{
    return id == meter_widget::type() || widget::is(id);
}

void meter_widget::updater(F32 dt)
{
    widget::updater(dt);

    ping_delay += dt;
    pitch += dt;

    if (value_vel == 0.0f) return;

    F32 old_value = value;

    value += 0.5f * value_accel * dt * dt + value_vel * dt;
    value_vel += value_accel * dt;

    if ((value_vel < 0.0f) ? 1 : 0) {
        if (value <= end_value) {
            value = end_value;
            value_vel = 0.0f;
        }
        F32 pitch = -4.0f * this->pitch;
        pitch = range_limit(pitch, -10.0f, 6.5f);
        F32 min_ping_time = xpow(0.5f, (1.0f / 12.0f) * pitch);
        min_ping_time = 0.05f * min_ping_time;
        if ((S32)value != (S32)old_value && res.sound.decrement && ping_delay > min_ping_time) {
            ping_delay = 0.0f;
            pings.play(res.sound.decrement, 1.0f, pitch, 0x80, 0, 0, SND_CAT_GAME);
        }
    } else {
        if (value >= end_value) {
            value = end_value;
            value_vel = 0;
        }
        F32 pitch = 2.0f * this->pitch;
        pitch = range_limit(pitch, -10.0f, 6.5f);
        F32 min_ping_time = xpow(0.5f, (1.0f / 12.0f) * pitch);
        min_ping_time = 0.05f * min_ping_time;
        if ((S32)value != (S32)old_value && res.sound.increment && ping_delay > min_ping_time) {
            ping_delay = 0.0f;
            pings.play(res.sound.increment, 1.0f, pitch, 0x80, 0, 0, SND_CAT_GAME);
        }
    }
}

}

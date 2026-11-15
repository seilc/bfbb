#include "xCommon.h"
#include "xClimate.h"

#include "xEnv.h"
#include "xGlobals.h"
#include "xMath.h"
#include "xMath3.h"
#include "xParEmitter.h"
#include "xString.h"
#include "zParEmitter.h"
#include "zParPTank.h"

#include <string.h>

static Climate* sClimate;
F32 snow_life = 3.0f;
static const xVec3 snow_vel = { 0.0f, -2.0f, 0.0f };
static const xVec3 snow_dvel = { 0.1f, 0.1f, 0.1f };

static void xClimateVecFromAngle(F32 angle, xVec3* vec)
{
    xMat3x3 mat;
    xMat3x3Identity(&mat);
    xMat3x3Euler(&mat, PI * angle / 180.0f, 0.0f, 0.0f);
    xVec3Init(vec, 0.0f, 0.0f, 1.0f);
    xMat3x3LMulVec(vec, &mat, vec);
}

void xClimateInit(Climate* climate)
{
    climate->rain.strength = 0.0f;
    climate->rain.rain_emitter = zParEmitterFind(xStrHash("PAREMIT_RAIN"));
    climate->rain.rain_emitter->emit_flags &= (U8)~0x1;
    climate->rain.snow_emitter = zParEmitterFind(xStrHash("PAREMIT_SNOW"));
    climate->rain.snow_emitter->emit_flags &= (U8)~0x1;
}

void xClimateInitAsset(Climate* climate, xEnvAsset* easset)
{
    Wind* w = &climate->wind;
    sClimate = climate;
    w->strength = 0.0f;
    xClimateVecFromAngle(w->angle, &w->dir);

    if (easset->climateFlags == 0) {
        w->strength = 0.0f;
        climate->rain.strength = 0.0f;
    } else if (easset->climateFlags & 0x1) {
        climate->rain.rain = TRUE;
        climate->rain.strength = 0.5f * (easset->climateStrengthMax - easset->climateStrengthMin);
        climate->rain.strength += easset->climateStrengthMin;
    } else if (easset->climateFlags & 0x2) {
        climate->rain.rain = FALSE;
        climate->rain.strength = 0.5f * (easset->climateStrengthMax - easset->climateStrengthMin);
        climate->rain.strength += easset->climateStrengthMin;
    }
}

void xClimateSetSnow(F32 stre)
{
    sClimate->rain.rain = FALSE;
    sClimate->rain.strength = stre;
}

void xClimateSetRain(F32 stre)
{
    sClimate->rain.rain = TRUE;
    sClimate->rain.strength = stre;
}

static void GetPosBigDogWhattupFool(xVec3* pos)
{
    xMat4x3* mat = &xglobals->camera.mat;
    pos->x = 10.0f * mat->at.x + mat->pos.x;
    pos->y = 10.0f * mat->at.y + mat->pos.y;
    pos->z = 10.0f * mat->at.z + mat->pos.z;
}

static void UpdateRain(Climate* climate, F32 seconds)
{
    Rain* r = &climate->rain;
    xParEmitterCustomSettings info;
    memset(&info, 0, sizeof(info));
    info.custom_flags = 0x100;

    if (r->rain) {
        S32 total_rain_drops = 25.0f * r->strength;
        for (S32 i = 0; i < total_rain_drops; i++) {
            GetPosBigDogWhattupFool(&info.pos);
            info.pos.x += 25.0f * xurand() - 12.5f;
            info.pos.y += 8.0f;
            info.pos.z += 25.0f * xurand() - 12.5f;
            xParEmitterEmitCustom(r->rain_emitter, seconds, &info);
        }
    } else {
        S32 total_snow_flakes = 25.0f * r->strength;
        info.custom_flags |= 0x202;
        xVec3 player_pos;
        GetPosBigDogWhattupFool(&player_pos);

        if (gPTankDisable) {
            for (S32 i = 0; i < total_snow_flakes; i++) {
                info.pos = player_pos;
                info.pos.x += 45.0f * xurand() - 22.5f;
                info.pos.z += 45.0f * xurand() - 22.5f;
                // BUG: x times z, not the squared distance
                F32 perc = (info.pos.x - player_pos.x) * (info.pos.z - player_pos.z) / 506.25f;
                perc = 1.0f - perc;
                info.vel.x = snow_dvel.x * xurand() + snow_vel.x;
                info.vel.y = snow_dvel.y * xurand() + snow_vel.y;
                info.vel.z = snow_dvel.z * xurand() + snow_vel.z;
                info.pos.y += 4.0f * perc + 4.0f;
                info.life.val[0] = snow_life * perc + snow_life;
                xParEmitterEmitCustom(r->snow_emitter, seconds, &info);
            }
        } else {
            S32 num = 0.1f * total_snow_flakes;
            if (num > 0) {
                xVec3* pos = (xVec3*)xMEMPUSHTEMP(num * 2 * sizeof(xVec3));
                xVec3* vel = pos + num;
                if (pos) {
                    xVec3* pp = pos;
                    xVec3* vp = vel;
                    for (S32 i = 0; i < num; i++, pp++, vp++) {
                        *pp = player_pos;
                        pp->x += 45.0f * xurand() - 22.5f;
                        pp->z += 45.0f * xurand() - 22.5f;
                        F32 xx = pp->x - player_pos.x;
                        F32 zz = pp->z - player_pos.z;
                        F32 perc = (xx * xx + zz * zz) / 506.25f;
                        perc = 1.0f - perc;
                        pp->y += 4.0f * perc + 4.0f;
                        vp->x = snow_dvel.x * xurand() + snow_vel.x;
                        vp->y = snow_dvel.y * xurand() + snow_vel.y;
                        vp->z = snow_dvel.z * xurand() + snow_vel.z;
                    }
                    zParPTankSpawnSnow(pos, vel, num);
                    xMEMPOPTEMP(pos);
                }
            }
        }
    }
}

static void UpdateWind(Climate*, F32)
{
}

void xClimateUpdate(Climate* climate, F32 seconds)
{
    UpdateRain(climate, seconds);
    UpdateWind(climate, seconds);
}

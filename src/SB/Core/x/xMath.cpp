#include "xCommon.h"
#include "xMath.h"

#include "decomp.h"

#include <stdlib.h>

static S32 xmath_inited;
static S32 xmath_exited;
static U32 rndseed;

void xMathInit()
{
    if (xmath_inited) {
        return;
    }

    xmath_inited = TRUE;
    rndseed = 0;
}

void xMathExit()
{
    if (xmath_exited) {
        return;
    }

    xmath_exited = TRUE;
}

F32 xatof(const char* x)
{
    return atof(x);
}

DECOMP_FORCEFLOAT(HUGE)
DECOMP_FORCEFLOAT(0.5f)
DECOMP_FORCEFLOAT(1.0f)
DECOMP_FORCEFLOAT(2.0f)
DECOMP_FORCEFLOAT(0.0f)
DECOMP_FORCEACTIVE(xsqrt)

void xsrand(U32 seed)
{
    rndseed = seed;
}

U32 xrand()
{
    rndseed = rndseed * 1103515245 + 12345;
    return rndseed;
}

F32 xurand()
{
    static const F32 scale = 1.0f / 4294967296.0f;
    return scale * xrand();
}

U32 xMathSolveQuadratic(F32 a, F32 b, F32 c, F32* x1, F32* x2)
{
    F32 d, dx, p;

    if (a == 0.0f) {
        if (b == 0.0f) {
            return 0;
        }

        *x1 = -c / b;
        return 1;
    }

    d = b * b - 4.0f * a * c;
    if (d < 0.0f) {
        return 0;
    }

    p = 1.0f / (2.0f * a);
    *x1 = -b * p;
    if (d == 0.0f) {
        return 1;
    }

    *x2 = *x1;
    dx = p * xsqrt(d);
    if (a > 0.0f) {
        *x1 -= dx;
        *x2 += dx;
    } else {
        *x1 += dx;
        *x2 -= dx;
    }

    return 2;
}

U32 xMathSolveCubic(F32 a, F32 b, F32 c, F32 d, F32* x1, F32* x2, F32* x3)
{
    F32 arecip, fA, fB, fOffset, fDiscr, fHalfB, fTemp, fDist, fAngle, fCos, fSin;

    if (a == 0.0f) {
        return xMathSolveQuadratic(b, c, d, x1, x2);
    }

    if (a != 1.0f) {
        arecip = 1.0f / a;
        b *= arecip;
        c *= arecip;
        d *= arecip;
    }

    fA = (1.0f / 3.0f) * (3.0f * c - b * b);
    fB = (1.0f / 27.0f) * (2.0f * (b * b * b) - 9.0f * b * c + 27.0f * d);
    fOffset = (1.0f / 3.0f) * b;
    fDiscr = 0.25f * (fB * fB) + (1.0f / 27.0f) * (fA * fA * fA);
    fHalfB = 0.5f * fB;

    if (xabs(fDiscr) < 0.000001f) {
        fDiscr = 0.0f;
    }

    if (fDiscr > 0.0f) {
        fTemp = xsqrt(fDiscr);

        if (-fHalfB + fTemp >= 0.0f) {
            *x1 = xpow(-fHalfB + fTemp, 1.0f / 3.0f);
        } else {
            *x1 = -xpow(-(-fHalfB + fTemp), 1.0f / 3.0f);
        }

        if (-fHalfB - fTemp >= 0.0f) {
            *x1 += xpow(-fHalfB - fTemp, 1.0f / 3.0f);
        } else {
            *x1 -= xpow(-(-fHalfB - fTemp), 1.0f / 3.0f);
        }

        *x1 -= fOffset;
        return 1;
    } else if (fDiscr < 0.0f) {
        fDist = xsqrt(-(1.0f / 3.0f) * fA);
        fAngle = (1.0f / 3.0f) * xatan2(xsqrt(-fDiscr), -fHalfB);
        fCos = icos(fAngle);
        fSin = isin(fAngle);
        *x1 = 2.0f * fDist * fCos - fOffset;
        *x2 = -fDist * (fCos + 1.7320508f * fSin) - fOffset;
        *x3 = -fDist * (fCos - 1.7320508f * fSin) - fOffset;
        return 3;
    } else {
        fTemp = (fHalfB >= 0.0f) ? -xpow(fHalfB, 1.0f / 3.0f) : xpow(-fHalfB, 1.0f / 3.0f);

        *x1 = 2.0f * fTemp - fOffset;
        *x2 = -fTemp - fOffset;
        return 2;
    }
}

DECOMP_FORCEFLOAT(PI)
DECOMP_FORCEFLOAT(-1.0f)

F32 xAngleClamp(F32 a)
{
    F32 b = xfmod(a, 2.0f * PI);
    if (b < 0.0f) {
        b += 2.0f * PI;
    }

    return b;
}

F32 xAngleClampFast(F32 a)
{
    if (a < 0.0f) {
        a += 2.0f * PI;
    } else if (a >= 2.0f * PI) {
        a -= 2.0f * PI;
    }

    return a;
}

F32 xDangleClamp(F32 a)
{
    F32 b = xfmod(a, 2.0f * PI);
    if (b >= PI) {
        b -= 2.0f * PI;
    } else if (b < -PI) {
        b += 2.0f * PI;
    }

    return b;
}

void xAccelMove(F32& x, F32& v, F32 a, F32 dt, F32 endx, F32 maxv)
{
    F32 offset = endx - x;
    F32 t1 = (xabs(v) < 0.001f || !xsamesign(offset, v)) ? HUGE : offset / v;
    F32 t2 = xabs(v / a);

    if (t1 < t2) a *= -1.0f;
    if (offset < 0.0f) a *= -1.0f;

    const F32 oldv = v;
    F32 dv, newv, adx;
    dv = a * dt;
    newv = v + dv;

    if (xabs(newv) <= maxv) { v = newv; adx = 0.5f * dv * dt; }
    else if (xabs(v) <= maxv) {
        v = range_limit(newv, -maxv, maxv);
        if (oldv != v) {
            F32 diff = v - oldv;
            adx = 0.5f * diff * diff / a;
        } else {
            adx = 0.0f;
        }
    }
    else if (!xsamesign(newv, dv)) { v = newv; adx = 0.5f * dv * dt; }
    else adx = 0.0f;

    F32 dx = oldv * dt + adx;

    if (t1 > t2) {
        if (xsamesign(dx, offset) && xabs(dx) > xabs(offset)) {
            dx = offset;
            v = 0.0f;
        }
    } else if (!xsamesign(dx, offset)) {
        dx = offset;
        v = 0.0f;
    }

    x += dx;
}

F32 xAccelMoveTime(F32 dx, F32 a, F32, F32 maxv)
{
    F32 time;
    F32 atime = maxv / a;
    F32 adist = atime * (0.5f * a * atime);

    dx *= 0.5f;
    if (adist < dx) {
        time = xsqrt(2.0f * dx / a);
    } else {
        time = atime + (dx - adist) / maxv;
    }

    return 2.0f * time;
}

void xAccelMove(F32& x, F32& v, F32 a, F32 dt, F32 maxv)
{
    if (xabs(v) > xabs(maxv)) {
        xSIGNMATCH(a, v);
        a = -a;
    }

    xSIGNMATCH(maxv, a);

    F32 diff = maxv - v;
    F32 dv = a * dt;
    if (xabs(diff) < xabs(dv)) {
        x += v * dt + 0.5f * diff * diff / a;
        v = maxv;
    } else {
        x += 0.5f * a * dt * dt + v * dt;
        v += dv;
    }
}

void xAccelStop(F32& x, F32& v, F32 a, F32 dt)
{
    if (xfeq0(v)) {
        return;
    }

    xSIGNMATCH(a, v);
    a = -a;

    F32 oldv = v;
    v += a * dt;
    if (xsamesign(oldv, v)) {
        x += 0.5f * dt * dt * a + oldv * dt;
    } else {
        if (!xfeq0(a)) {
            x -= 0.5f * oldv * oldv / a;
        }

        v = 0.0f;
    }
}

F32 xFuncPiece_Eval(xFuncPiece* func, F32 param, xFuncPiece** iterator)
{
    F32 result;

    while (func && func->end < param - EPSILON) func = func->next;

    if (func) {
        result = func->coef[func->order];
        for (S32 i = func->order - 1; i >= 0; i--) {
            result = result * param + func->coef[i];
        }

        if (iterator) *iterator = func;
    } else {
        result = 0.0f;
        if (iterator) *iterator = NULL;
    }

    return result;
}

void xFuncPiece_EndPoints(xFuncPiece* func, F32 pi, F32 pf, F32 fi, F32 ff)
{
    F32 fdiff = ff - fi;
    func->end = pf - pi;
    F32 xfinv = 1.0f / func->end;
    func->order = 1;
    func->coef[0] = fi;
    func->coef[1] = fdiff * xfinv;
    xFuncPiece_ShiftPiece(func, func, -pi);
}

void xFuncPiece_ShiftPiece(xFuncPiece* shift, xFuncPiece* func, F32 newZero)
{
    S32 i, j;
    xFuncPiece temp;

    for (i = 0; i < func->order; i++) temp.coef[i] = 0.0f;

    temp.coef[func->order] = func->coef[func->order];
    for (i = func->order - 1; i >= 0; i--) {
        for (j = i; j < func->order; j++) {
            temp.coef[j] += newZero * temp.coef[j + 1];
        }

        temp.coef[i] += func->coef[i];
    }

    shift->order = func->order;
    for (i = 0; i <= shift->order; i++) {
        shift->coef[i] = temp.coef[i];
    }

    shift->end = func->end - newZero;
}

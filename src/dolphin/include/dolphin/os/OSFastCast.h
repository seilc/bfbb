#ifndef _DOLPHIN_OSFASTCAST
#define _DOLPHIN_OSFASTCAST

#ifdef __cplusplus
extern "C" {
#endif

#include "dolphin/types.h"

#define OS_GQR_F32 0x0000
#define OS_GQR_U8 0x0004
#define OS_GQR_U16 0x0005
#define OS_GQR_S8 0x0006
#define OS_GQR_S16 0x0007

#define OS_FASTCAST_U8 2
#define OS_FASTCAST_U16 3
#define OS_FASTCAST_S8 4
#define OS_FASTCAST_S16 5

static inline void OSInitFastCast(void)
{
#ifdef __MWERKS__
    asm
    {
        li      r3, OS_GQR_U8
        oris    r3, r3, OS_GQR_U8
        mtspr   GQR2, r3

        li      r3, OS_GQR_U16
        oris    r3, r3, OS_GQR_U16
        mtspr   GQR3, r3

        li      r3, OS_GQR_S8
        oris    r3, r3, OS_GQR_S8
        mtspr   GQR4, r3

        li      r3, OS_GQR_S16
        oris    r3, r3, OS_GQR_S16
        mtspr   GQR5, r3
    }
#else

#endif
}

static inline f32 __OSu8tof32(register u8* in)
{
    register f32 r;
    asm
    {
        psq_l r, 0(in), 0x1, OS_FASTCAST_U8
    }
    return r;
}

static inline void OSu8tof32(u8* in, volatile f32* out)
{
    *out = __OSu8tof32(in);
}

static inline f32 __OSu16tof32(register u16* in)
{
    register f32 r;
    asm
    {
        psq_l r, 0(in), 0x1, OS_FASTCAST_U16
    }
    return r;
}

static inline void OSu16tof32(u16* in, volatile f32* out)
{
    *out = __OSu16tof32(in);
}

static inline f32 __OSs8tof32(register s8* in)
{
    register f32 r;
    asm
    {
        psq_l r, 0(in), 0x1, OS_FASTCAST_S8
    }
    return r;
}

static inline void OSs8tof32(s8* in, volatile f32* out)
{
    *out = __OSs8tof32(in);
}

static inline f32 __OSs16tof32(register s16* in)
{
    register f32 r;
    asm
    {
        psq_l r, 0(in), 0x1, OS_FASTCAST_S16
    }
    return r;
}

static inline void OSs16tof32(s16* in, volatile f32* out)
{
    *out = __OSs16tof32(in);
}

static inline u8 __OSf32tou8(register f32 inF)
{
    register u8 out;
    u32 tmp;
    register u32* tmpPtr = &tmp;
    asm
    {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_U8
        lbz out, 0(tmpPtr)
    }
    return out;
}

static inline void OSf32tou8(f32* in, volatile u8* out)
{
    *out = __OSf32tou8(*in);
}

static inline u16 __OSf32tou16(register f32 inF)
{
    register u16 out;
    u32 tmp;
    register u32* tmpPtr = &tmp;
    asm
    {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_U16
        lhz out, 0(tmpPtr)
    }
    return out;
}

static inline void OSf32tou16(f32* in, volatile u16* out)
{
    *out = __OSf32tou16(*in);
}

static inline s8 __OSf32tos8(register f32 inF)
{
    register s8 out;
    u32 tmp;
    register u32* tmpPtr = &tmp;
    asm
    {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_S8
        lbz out, 0(tmpPtr)
        extsb out, out
    }
    return out;
}

static inline void OSf32tos8(f32* in, volatile s8* out)
{
    *out = __OSf32tos8(*in);
}

static inline s16 __OSf32tos16(register f32 inF)
{
    register s16 out;
    u32 tmp;
    register u32* tmpPtr = &tmp;
    asm
    {
        psq_st inF, 0(tmpPtr), 0x1, OS_FASTCAST_S16
        lha out, 0(tmpPtr)
    }
    return out;
}

static inline void OSf32tos16(f32* in, volatile s16* out)
{
    *out = __OSf32tos16(*in);
}

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN_OSFASTCAST

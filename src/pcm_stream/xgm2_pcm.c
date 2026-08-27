#include <genesis.h>
#include "xgm2_pcm.h"

// Uncomment to enable GensKmod debug logging
// #define DEBUG_LOG

// Fast custom bus accesses save about 4 scanlines
// Comment out to use default SGDK bus access functions
#define XGM2_PCM_FAST_BUS_ACCESS

// ===========================
// PRIVATE
// ===========================

extern void XGM2_PCM_mixIntoRingBuffer(s8 *pcm, vu8 *ringbuf);
extern void XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(s8 *pcmBuf, vu8 *ringBuf,
                                                                  u16 *pcmPos, u8 ringPosWrite,
                                                                  u8 ringPosStop);

#ifdef XGM2_PCM_FAST_BUS_ACCESS

u16 intsPrev;

extern u16 XGM2_PCM_SYS_disableInts_fast_noStack();
extern void XGM2_PCM_SYS_enableInts_fast_noStack(u16 intsPrev);

static inline void XGM2_PCM_Z80_getAndRequestBus_fast()
{
    // take bus and end reset
    *((vu16 *) Z80_HALT_PORT) = 0x0100;
    *((vu16 *) Z80_RESET_PORT) = 0x0100;

    // wait for bus taken
    while (*((vu16 *) Z80_HALT_PORT) & 0x0100)
        ;
}

static inline void XGM2_PCM_Z80_releaseBus_fast()
{
    *((vu16 *) Z80_HALT_PORT) = 0x0000;
}

static inline void enterBus()
{
    intsPrev = XGM2_PCM_SYS_disableInts_fast_noStack();
    XGM2_PCM_Z80_getAndRequestBus_fast();
}

static inline void exitBus()
{
    XGM2_PCM_Z80_releaseBus_fast();
    XGM2_PCM_SYS_enableInts_fast_noStack(intsPrev);
}

#else

static inline void enterBus()
{
    SYS_disableInts();
    Z80_getAndRequestBus(true);
}

static inline void exitBus()
{
    Z80_releaseBus();
    SYS_enableInts();
}

#endif // XGM2_PCM_FAST_BUS_ACCESS

// ===========================
// PUBLIC
// ===========================

u8 XGM2_PCM_peek_ringbuf_writepos()
{
    enterBus();

    u8 pos = *XGM2_PCM_RINGBUF_WRITEPOS_VAR;

    exitBus();

    return pos;
}

void XGM2_PCM_activate()
{
    enterBus();

    *XGM2_DAC_ENABLE = 0x80;
    *XGM2_DAC_ENABLED_CNT = 0x04;

    exitBus();
}

void XGM2_PCM_mix_into_ringbuf(void *pcmSource512, u16 *bufPos, u8 *ringbufPos)
{
    u8 ringWritePosPrev = *ringbufPos;

    enterBus();

    *XGM2_DAC_ENABLE = 0x80;
    *XGM2_DAC_ENABLED_CNT = 0x04;

    // Get current write pos
    vu8 ringWritePos = *XGM2_PCM_RINGBUF_WRITEPOS_VAR;
    if (ringWritePos != ringWritePosPrev) {

        /**
         * TODO: Read if any PCM sample plays are currently active.
         *       If not we can overwrite instead of mix in.
         *       We can only skip overflow protection if this and previous
         *       check both found no playing samples.
         */

#ifdef DEBUG_LOG
        KLog_U2("Mixing from ", pcmSource512 + posAt, " to ",
                XGM2_PCM_RINGBUF_ADDR + ringWritePosPrev, ", stop at ",
                XGM2_PCM_RINGBUF_ADDR + ringWritePos);
#endif

        XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(
            pcmSource512, XGM2_PCM_RINGBUF_ADDR, bufPos, ringWritePosPrev, ringWritePos);
    }

    exitBus();

    // Update output variables
    *ringbufPos = ringWritePos;
}

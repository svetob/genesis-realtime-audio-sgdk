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
extern void XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(s8 *pcmBuf,
                                                                  XGM2PCMMixerStatus *status);
extern void XGM2_PCM_overwriteRingBuffer_ASM(s8 *pcmBuf, XGM2PCMMixerStatus *status);

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

u8 XGM2_PCM_peek_pcm_channel_status()
{
    enterBus();

    vu8 status = *((vu8 *) Z80_DRV_STATUS);
    u8 pcmPlaying = status & XGM2_PCM_PLAYING_MASK;

    exitBus();

    return pcmPlaying;
}

void XGM2_PCM_activate()
{
    enterBus();

    *XGM2_DAC_ENABLE = 0x80;
    *XGM2_DAC_ENABLED_CNT = 0x04;

    exitBus();
}

void XGM2_PCM_mix_into_ringbuf(void *pcmSource512, XGM2PCMMixerStatus *mixerStatus)
{
    enterBus();

    *XGM2_DAC_ENABLE = 0x80;
    *XGM2_DAC_ENABLED_CNT = 0x04;

    // Get driver status
    vu8 xgm2Status = *((vu8 *) Z80_DRV_STATUS);

    // Check if PCM ring buffer is empty or has samples
    u8 pcmPlaying = xgm2Status & XGM2_PCM_PLAYING_MASK;
    bool shouldProtect = pcmPlaying || mixerStatus->pcmWasPlaying;

    // Get current write pos
    vu8 ringWritePos = *XGM2_PCM_RINGBUF_WRITEPOS_VAR;
    if (ringWritePos != mixerStatus->ringPosPrev) {

#ifdef DEBUG_LOG
        KLog_U2("Mixing from ", pcmSource512 + posAt, " to ",
                XGM2_PCM_RINGBUF_ADDR + ringWritePosPrev, ", stop at ",
                XGM2_PCM_RINGBUF_ADDR + ringWritePos);
#endif
        if (shouldProtect) {
            // PCM ring buffer may have samples - mix into it with overflow protection
            XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(pcmSource512, mixerStatus);
        } else {
            // PCM ring buffer is clean - just overwrite it
            // XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(pcmSource512, mixerStatus);
            XGM2_PCM_overwriteRingBuffer_ASM(pcmSource512, mixerStatus);
        }
    }

    mixerStatus->pcmWasPlaying = pcmPlaying;

    exitBus();
}

#include <genesis.h>
#include "xgm2_pcm.h"

// #define DEBUG_LOG

u16 intsPrev;
extern u16 XGM2_PCM_SYS_disableInts_fast_noStack();
extern void XGM2_PCM_SYS_enableInts_fast_noStack(u16 intsPrev);
extern void XGM2_PCM_mixIntoRingBuffer(s8 *pcm, vu8 *ringbuf);

static inline void ZZ80_getAndRequestBus()
{
    // take bus and end reset
    *((vu16 *) Z80_HALT_PORT) = 0x0100;
    *((vu16 *) Z80_RESET_PORT) = 0x0100;

    // wait for bus taken
    while (*((vu16 *) Z80_HALT_PORT) & 0x0100)
        ;
}

static inline void ZZ80_releaseBus()
{
    *((u16 *) Z80_HALT_PORT) = 0x0000;
}

static inline void enterBus()
{
    intsPrev = XGM2_PCM_SYS_disableInts_fast_noStack();
    ZZ80_getAndRequestBus(true);
}

static inline void exitBus()
{
    ZZ80_releaseBus();
    XGM2_PCM_SYS_enableInts_fast_noStack(intsPrev);
}

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

void XGM2_PCM_mix_into_ringbuf(void *pcmSource512, u16 *pos, u8 *ringbufPosPrev)
{
    u8 writePosPrev = *ringbufPosPrev;
    u16 posAt = *pos;

    enterBus();

    *XGM2_DAC_ENABLE = 0x80;
    *XGM2_DAC_ENABLED_CNT = 0x04;

    // Get current write pos
    vu8 writePos = *XGM2_PCM_RINGBUF_WRITEPOS_VAR;

    // While not up to speed on write pos:
    while (writePos != writePosPrev) {

        vu8 *write = (vu8 *) (XGM2_PCM_RINGBUF_ADDR + writePosPrev);
        s8 *read = (s8 *) (pcmSource512 + posAt);

#ifdef DEBUG_LOG
        KLog_U2("Mix from ", read, " to ", write);
#endif

        XGM2_PCM_mixIntoRingBuffer(read, write);

        writePosPrev += 0x40;
        posAt = (posAt + 0x40) & 0x01FF;
    }

    exitBus();

    // Update output variables
    *pos = posAt;
    *ringbufPosPrev = writePos;
}

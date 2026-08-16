#include <genesis.h>
#include "xgm2_interface.h"

// #define DEBUG_LOG

extern void XGM2_PCM_mixIntoRingBuffer(s8 *pcm, u8 *ringbuf);

u8 XGM2_PCM_peek_ringbuf_writepos()
{
    SYS_disableInts();
    Z80_getAndRequestBus(true);

    u8 pos = *XGM2_PCM_RINGBUF_WRITEPOS_VAR;

    Z80_releaseBus();
    SYS_enableInts();

    return pos;
}

void XGM2_PCM_mix_into_ringbuf(void *pcmSource512, u16 *pos, u8 *ringbufPosPrev)
{
    u8 writePosPrev = *ringbufPosPrev;
    u16 posAt = *pos;

    // Request bus
    SYS_disableInts();
    Z80_getAndRequestBus(true);

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

    // Release bus
    Z80_releaseBus();
    SYS_enableInts();

    // Update output variables
    *pos = posAt;
    *ringbufPosPrev = writePos;
}

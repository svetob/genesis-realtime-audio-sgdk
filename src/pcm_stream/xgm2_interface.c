#include <genesis.h>
#include "xgm2_interface.h"

// #define DEBUG_LOG

void peek_XGM2_channel(SoundPCMChannel channel, u32 *addr, u16 *len, u8 *isPlaying)
{
    // Find Z80 addresses to read from
    u8 *p_vars = XGM2_PCM_VARS + (channel * (XGM2_PCM_VARS_CHANNEL_OFFSET));
    vu8 *p_status = (vu8 *) Z80_DRV_STATUS;

    // Request bus and read values
    SYS_disableInts();
    Z80_getAndRequestBus(true);

    // References for address, len offsets: drv_xgm2.s80, xgm2.c
    u8 a0 = *p_vars++;
    u8 a1 = *p_vars++;
    u8 a2 = *p_vars++;
    u8 l0 = *p_vars++;
    u8 l1 = *p_vars;

    u8 status = *p_status;

    Z80_releaseBus();
    SYS_enableInts();

    // Build and return results
    *addr = (a2 << 16) + (a1 << 8) + a0;
    *len = (l1 << 8) + l0;
    // TODO: Also check address location to confirm it is within stream boundaries
    *isPlaying = status & (1 << channel);
}

u8 peek_XGM2_ringbuf_writepos()
{
    // Request bus and read values
    SYS_disableInts();
    Z80_getAndRequestBus(true);

    u8 pos = *XGM2_PCM_RINGBUF_WRITEPOS_VAR;

    Z80_releaseBus();
    SYS_enableInts();

    return pos;
}

void mix_into_PCM_ring_buffer(void *pcmSource512, u16 *pos, u8 *ringbufPosPrev)
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
        u8 *read = (u8 *) (pcmSource512 + posAt);

#ifdef DEBUG_LOG
        KLog_U2("Mix from ", read, " to ", write);
#endif

        u8 i = 64;
        while (i--) {
            // Update sample
            u8 samp = *read++;
            u8 buf = *write;
            u8 out = samp + buf;
            *write++ = out;

#ifdef DEBUG_LOG
            KLog_U3("Mixed in ", samp, " buf ", buf, " out ", out);
#endif
        }

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

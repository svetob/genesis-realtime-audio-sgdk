#include <genesis.h>
#include "util.h"

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

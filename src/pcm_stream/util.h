#ifndef __PCM_STREAM_UTIL_H
#define __PCM_STREAM_UTIL_H

#include <genesis.h>

// Addresses to internal Z80 vars    TODO: Use existing constants instead
#define XGM2_PCM_VARS                (void *) (Z80_RAM + 0x0110 + 0xD0)
#define XGM2_PCM_VARS_CHANNEL_OFFSET 0x08

#define XGM2_PCM_CHUNK_SIZE          64

/**
 * \brief
 *      Reads current state of XGM2 PCM channel via Z80 bus.
 */
void peek_XGM2_channel(SoundPCMChannel channel, u32 *addr, u16 *len, u8 *isPlaying);

#endif

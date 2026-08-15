#ifndef __PCM_STREAM_UTIL_H
#define __PCM_STREAM_UTIL_H

#include <genesis.h>

// Addresses to internal Z80 vars    TODO: Use existing constants instead
#define XGM2_VARS                     Z80_RAM + 0x0110

#define XGM2_PCM_VARS                 (void *) (Z80_RAM + XGM2_VARS + 0xD0)
#define XGM2_PCM_VARS_CHANNEL_OFFSET  0x08

#define XGM2_PCM_CHUNK_SIZE           64

#define XGM2_PCM_RINGBUF_ADDR         (void *) (Z80_RAM + 0x1900)
#define XGM2_PCM_RINGBUF_WRITEPOS_VAR (u8 *) (XGM2_VARS + 0xE8)

#define XGM2_DAC_ENABLE               (u8 *) (XGM2_VARS + 0x59)
#define XGM2_DAC_ENABLED_CNT          (u8 *) (XGM2_VARS + 0xE9)

/**
 * \brief
 *      Reads current state of XGM2 PCM channel via Z80 bus.
 */
void peek_XGM2_channel(SoundPCMChannel channel, u32 *addr, u16 *len, u8 *isPlaying);

/**
 * \brief
 *      Reads current write position of XGM2 PCM ring buffer.
 */
u8 peek_XGM2_ringbuf_writepos();

/**
 * \brief
 *      Writes chunks from source buffer onto the PCM ring buffer.
 *
 * \param pcmSource512
 *      PCM stream source pointer
 *
 * \param pos
 *      Current position in buffer, will be updated.
 *
 * \param ringbufPosPrev
 *      Previous ring buffer position, will be updated.
 */
void mix_into_PCM_ring_buffer(void *pcmSource512, u16 *pos, u8 *ringbufPosPrev);

#endif

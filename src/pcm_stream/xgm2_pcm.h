#ifndef __XGM2_PCM_H
#define __XGM2_PCM_H

#include <genesis.h>

// Addresses to internal Z80 vars    TODO: Use existing constants instead
#define XGM2_VARS                     Z80_RAM + 0x0110

#define XGM2_PCM_VARS                 (void *) (Z80_RAM + XGM2_VARS + 0xD0)
#define XGM2_PCM_VARS_CHANNEL_OFFSET  0x08

#define XGM2_PCM_CHUNK_SIZE           64

#define XGM2_PCM_RINGBUF_ADDR         (void *) (Z80_RAM + 0x1900)
#define XGM2_PCM_RINGBUF_WRITEPOS_VAR (vu8 *) (XGM2_VARS + 0xE8)

#define XGM2_DAC_ENABLE               (vu8 *) (XGM2_VARS + 0x59)
#define XGM2_DAC_ENABLED_CNT          (vu8 *) (XGM2_VARS + 0xE9)

/**
 * \brief
 *      Reads current write position of XGM2 PCM ring buffer.
 */
u8 XGM2_PCM_peek_ringbuf_writepos();

/**
 * \brief
 *      Forces activation of XGM2 PCM output to DAC controller.
 */
void XGM2_PCM_activate();

/**
 * \brief
 *      Renders 64-byte chunks from PCM source onto all new unplayed 64-byte chunks in the XGM2 PCM
 * ring buffer.
 *
 * \param pcmSource512
 *      PCM stream source pointer
 *
 * \param pos
 *      Current position in buffer, will be updated.
 *
 * \param ringbufPos
 *      Previous ring buffer position, will be updated.
 */
void XGM2_PCM_mix_into_ringbuf(void *pcmSource512, u16 *pos, u8 *ringbufPos);

#endif

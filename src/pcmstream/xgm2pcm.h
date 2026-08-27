#ifndef __XGM2PCM_H
#define __XGM2PCM_H

#include <genesis.h>

// Addresses to internal Z80 vars    TODO: Use existing constants instead
#define XGM2_VARS                    Z80_RAM + 0x0110

#define XGM2PCM_VARS                 (void *) (Z80_RAM + XGM2_VARS + 0xD0)
#define XGM2PCM_VARS_CHANNEL_OFFSET  0x08

#define XGM2PCM_CHUNK_SIZE           64

#define XGM2PCM_RINGBUF_ADDR         (void *) (Z80_RAM + 0x1900)
#define XGM2PCM_RINGBUF_WRITEPOS_VAR (vu8 *) (XGM2_VARS + 0xE8)

#define XGM2_DAC_ENABLE              (vu8 *) (XGM2_VARS + 0x59)
#define XGM2_DAC_ENABLED_CNT         (vu8 *) (XGM2_VARS + 0xE9)

#define XGM2PCM_PLAYING_MASK \
    (SOUND_PCM_CH1_MSK | SOUND_PCM_CH2_MSK | SOUND_PCM_CH3_MSK | SOUND_PCM_CH4_MSK)

typedef struct {
    u16 bufferPos;
    u8 ringPosPrev;
    bool pcmWasPlaying;
} XGM2PCMMixerStatus;

/**
 * \brief
 *      Reads current write position of XGM2 PCM ring buffer.
 */
u8 XGM2PCM_peek_ringbuf_writepos();

/**
 * \brief
 *      Returns current playing status of PCM channels 1-4.
 */
u8 XGM2PCM_peek_pcm_channel_status();

/**
 * \brief
 *      Forces activation of XGM2 PCM output to DAC controller.
 */
void XGM2PCM_activate();

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
void XGM2PCM_mix_into_ringbuf(void *pcmSource512, XGM2PCMMixerStatus *status);

#endif

#ifndef _REVERB_H
#define _REVERB_H

#include <genesis.h>

typedef enum {
    REVERB_BUFFER_SIZE_1024 = 1024,
    REVERB_BUFFER_SIZE_2048 = 2048,
    REVERB_BUFFER_SIZE_4096 = 4096,
    REVERB_BUFFER_SIZE_8192 = 8192,
} AFXReverbBufferSize;

/**
 * \brief
 *      8-bit Reverb Audio Effect data
 */
typedef struct {
    s8 *delayLine;
    u16 size;
    u16 pos;

    u16 delay1;
    u16 delay2;
    u16 delay3;
    u16 delay4;
} AFXReverb;

/**
 * \brief
 *      Create a new 8-bit Reverb Audio Effect with default delays.
 *
 * \param bufferSize
 *      Buffer size, in samples. Must be a power of two and >= 1024.
 *
 */
AFXReverb *AFX_reverb_create(AFXReverbBufferSize bufferSize);

/**
 * \brief
 *      Free reverb from memory.
 */
void AFX_reverb_free(AFXReverb *afx);

/**
 * \brief
 *      Reset reverb.
 */
void AFX_reverb_reset(AFXReverb *afx);

/**
 * \brief
 *      Process samples through Reverb Audio Effect.
 *
 * \param samples
 *      Pointer to signed 8-bit PCM samples
 *
 * \param len
 *      Nr of samples to process. Must be multiple of 64.
 *
 * \param afx
 *      8-bit Reverb Audio Effect
 */
void AFX_reverb_process(s8 *samples, u16 len, AFXReverb *afx);

#endif

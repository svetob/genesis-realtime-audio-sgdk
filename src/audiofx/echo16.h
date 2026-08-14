#ifndef __ECHO8_H
#define __ECHO8_H

#include <genesis.h>

/**
 * \brief
 *      8-bit Echo Audio Effect data
 */
typedef struct {
    s16 *delayLine;
    u16 size;
    u16 pos;
} AFX16Echo;

/**
 * \brief
 *      Create a new 16-bit Echo Audio Effect.
 *
 * \param delay
 *      Delay, in samples. Must be multiple of 256.
 */
AFX16Echo *AFX16_echo_create(u16 delay);

/**
 * \brief
 *      Free effect from memory.
 */
void AFX16_echo_free(AFX16Echo *afx);

/**
 * \brief
 *      Process 256 samples through Echo Audio Effect.
 *
 * \param samples
 *      Pointer to 256 signed 16-bit PCM samples
 *
 * \param afx
 *      16-bit Echo Audio Effect
 */
void AFX16_echo_process256(s16 *samples, AFX16Echo *afx);

#endif

#ifndef __ECHO_H
#define __ECHO_H

#include <genesis.h>

/**
 * \brief
 *      8-bit Echo Audio Effect data
 */
typedef struct {
    s8 *delayLine;
    u16 size;
    u16 pos;
} AFX8Echo;

/**
 * \brief
 *      Create a new 8-bit Echo Audio Effect.
 *
 * \param delay
 *      Delay, in samples. Must be multiple of 256.
 */
AFX8Echo *AFX8_echo_create(u16 delay);

/**
 * \brief
 *      Free effect from memory.
 */
void AFX8_echo_free(AFX8Echo *afx);

/**
 * \brief
 *      Process 256 samples through Echo Audio Effect.
 *
 * \param samples
 *      Pointer to 256 signed 8-bit PCM samples
 *
 * \param afx
 *      8-bit Echo Audio Effect
 */
void AFX8_echo_process256(s8 *samples, AFX8Echo *afx);

#endif

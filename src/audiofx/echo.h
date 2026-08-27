#ifndef __ECHO8_H
#define __ECHO8_H

#include <genesis.h>

/**
 * \brief
 *      8-bit Echo Audio Effect data
 */
typedef struct {
    s8 *delayLine;
    u16 size;
    u16 delay;
    u16 pos;
} AFXEcho;

/**
 * \brief
 *      Create a new 8-bit Echo Audio Effect.
 *
 * \param bufferSize
 *      Buffer size, in samples. Must be multiple of 256.
 *
 * \param delay
 *      Delay, in samples. Must be multiple of 256 and not
 *      greater than bufferSize.
 */
AFXEcho *AFX_echo_create(u16 bufferSize, u16 delay);

/**
 * \brief
 *      Change echo parameters.
 *
 * \param delay
 *      Delay, in samples. Must be multiple of 256 and not
 *      greater than bufferSize.
 */
void AFX_echo_update(AFXEcho *afx, u16 delay);

/**
 * \brief
 *      Free effect from memory.
 */
void AFX_echo_free(AFXEcho *afx);

/**
 * \brief
 *      Reset echo.
 */
void AFX_echo_reset(AFXEcho *afx);

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
void AFX_echo_process(s8 *samples, u16 len, AFXEcho *afx);

#endif

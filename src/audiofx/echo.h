#ifndef __ECHO8_H
#define __ECHO8_H

#include <genesis.h>

typedef enum {
    ECHO_BUFFER_SIZE_1024 = 1024,
    ECHO_BUFFER_SIZE_2048 = 2048,
    ECHO_BUFFER_SIZE_4096 = 4096,
    ECHO_BUFFER_SIZE_8192 = 8192,
} AFXEchoBufferSize;

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
 *      Buffer size, in samples.
 *      Must be a power of two and >= 1024.
 *
 * \param delay
 *      Delay, in samples.
 *      Must be multiple of 4 and not greater than bufferSize.
 */
AFXEcho *AFX_echo_create(AFXEchoBufferSize bufferSize, u16 delay);

/**
 * \brief
 *      Change echo parameters.
 *
 * \param delay
 *      Delay, in samples.
 *      Must be multiple of 4 and not greater than bufferSize.
 */
void AFX_echo_update(AFXEcho *afx, u16 delay);

/**
 * \brief
 *      Free echo from memory.
 */
void AFX_echo_free(AFXEcho *afx);

/**
 * \brief
 *      Reset echo.
 */
void AFX_echo_reset(AFXEcho *afx);

/**
 * \brief
 *      Process samples through Echo Audio Effect.
 *
 * \param samples
 *      Pointer to signed 8-bit PCM samples.
 *
 * \param len
 *      Nr of samples to process. Must be multiple of 64.
 *
 * \param afx
 *      8-bit Echo Audio Effect
 */
void AFX_echo_process(s8 *samples, u16 len, AFXEcho *afx);

#endif

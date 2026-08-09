#ifndef __ECHO_H
#define __ECHO_H

#include <genesis.h>

/**
 * \brief
 *      8-bit Echo processor
 */
typedef struct {
    s8* delayLine;
    u16 size;
    u16 pos;
} AFX_Echo;

AFX_Echo* AFX_echo_create(u16 delay);
void AFX_echo_free(AFX_Echo* afx);
void AFX_echo_process(s8* samples, u16 size, AFX_Echo* afx);

#endif
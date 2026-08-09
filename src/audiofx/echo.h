#ifndef __ECHO_H
#define __ECHO_H

#include <genesis.h>

typedef struct {
    u8* delayLine;
    u16 size;
    u16 pos;
} AFX_Echo;

AFX_Echo* AFX_echo_create(u16 delay);
void AFX_echo_free(AFX_Echo* afx);
void AFX_echo_process(u8* samples, u16 size, AFX_Echo* afx);

#endif
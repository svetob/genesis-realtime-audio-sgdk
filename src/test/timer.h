#ifndef _HEADER_TIMER_H
#define _HEADER_TIMER_H

#include <genesis.h>

extern u16 scanlines_avg;
extern u16 frame_ctr;

void scanlineTimerWait();
void scanlineTimerStart();
u16 scanlineTimerStop();
void scanlineTimerNextFrame();

#endif
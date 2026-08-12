
#ifndef __INST_SAW_H
#define __INST_SAW_H

#include <genesis.h>

typedef struct {
    s8 wave;
    s8 spd;
} InstrSaw;

InstrSaw *INST_saw_create(u8 spd);
void INST_saw_free(InstrSaw *saw);
void INST_saw_play(s8 *stream, u16 len, void *data);

#endif

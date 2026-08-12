#include <genesis.h>
#include "saw.h"

InstrSaw *INST_saw_create(u8 spd)
{
    InstrSaw *saw = MEM_alloc(sizeof(InstrSaw));
    saw->spd = spd;
    saw->wave = -63;
    return saw;
}

void INST_saw_free(InstrSaw *saw)
{
    MEM_free(saw);
}

void INST_saw_play(s8 *stream, u16 len, void *data)
{
    InstrSaw *saw = data;
    s8 wave = saw->wave;
    s8 spd = saw->spd;

    while (len--) {
        *stream++ += wave;
        wave += spd;
        if (wave >= 32) {
            wave -= 64;
        }
    }

    saw->wave = wave;
}
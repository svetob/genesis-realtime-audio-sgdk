#include <genesis.h>
#include "echo.h"

AFX_Echo* AFX_echo_create(u16 delay) {
    void* buf = MEM_alloc(delay);
    memset(buf, 0, delay);
    
    AFX_Echo* afx = (AFX_Echo*) MEM_alloc(sizeof(AFX_Echo));
    afx->delayLine = buf;
    afx->size = delay;
    afx->pos = 0;

    return afx;
}

void AFX_echo_free(AFX_Echo* afx) {
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX_echo_process(u8* samples, u16 size, AFX_Echo* afx) {
    u8* line = afx->delayLine;
    u16 pos = afx->pos;
    u16 delay = afx->size;

    for (u16 i = 0; i < size; i++) {
        u8 lineSample = line[pos];
        u8 sample = samples[i];
        u8 out = sample + (lineSample >> 1);
        
        samples[i] = out;
        line[pos++] = out;

        //KLog_U3("Smpl", sample, "Line", lineSample, "Out", out);

        if (pos > delay) {
            pos = 0;
        }
    }

    afx->pos = pos;
}

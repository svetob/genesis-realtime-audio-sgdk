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

void AFX_echo_process(s8* samples, u16 size, AFX_Echo* afx) {
    s8* line = afx->delayLine;
    u16 pos = afx->pos;
    u16 delay = afx->size;

    while (size--) {
        s8 lineSample = line[pos];
        s8 sample = *samples;
        s8 out = lineSample == -1 ? sample : sample + (lineSample >> 1);
        
        *samples++ = out;
        line[pos++] = out;

        //KLog_U2("Samp ", sample, ", Out", out);

        if (pos >= delay) {
            pos = 0;
        }
    }

    afx->pos = pos;
}


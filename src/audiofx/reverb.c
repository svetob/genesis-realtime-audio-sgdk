#include <genesis.h>
#include "reverb.h"

AFXReverb *AFX_reverb_create(AFXReverbBufferSize bufferSize)
{
    void *buf = MEM_alloc(bufferSize);
    memset(buf, 0, bufferSize);

    AFXReverb *afx = (AFXReverb *) MEM_alloc(sizeof(AFXReverb));
    afx->delayLine = buf;
    afx->size = bufferSize;
    afx->pos = 0;

    afx->delay1 = 2800 & 0xFFFC; // Must be multiple of 4
    afx->delay2 = 3200 & 0xFFFC; // Must be multiple of 4
    afx->delay3 = 3600 & 0xFFFC; // Must be multiple of 4
    afx->delay4 = 3900 & 0xFFFC; // Must be multiple of 4

    return afx;
}

void AFX_reverb_free(AFXReverb *afx)
{
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX_reverb_reset(AFXReverb *afx)
{
    memsetU32(afx->delayLine, 0, afx->size >> 2);
    afx->pos = 0;
}

extern void AFX_reverb_process(s8 *samples, u16 len, AFXReverb *afx);

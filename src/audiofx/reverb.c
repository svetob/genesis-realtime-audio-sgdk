#include <genesis.h>
#include "reverb.h"

#define TO_DELAY_VALUE(x) (x << 2) // Must be multiple of 4

AFXReverb *AFX_reverb_create(AFXReverbBufferSize bufferSize)
{
    void *buf = MEM_alloc(bufferSize);
    memset(buf, 0, bufferSize);

    AFXReverb *afx = (AFXReverb *) MEM_alloc(sizeof(AFXReverb));
    afx->delayLine = buf;
    afx->size = bufferSize;
    afx->pos = 0;

    afx->delay1 = TO_DELAY_VALUE(97);
    afx->delay2 = TO_DELAY_VALUE(211);
    afx->delay3 = TO_DELAY_VALUE(421);
    afx->delay4 = TO_DELAY_VALUE(1021);

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

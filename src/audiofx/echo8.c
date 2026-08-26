#include <genesis.h>
#include "echo8.h"

extern void AFX8_echo_process64_ASM(s8 *samples, u16 size, s8 *delay_line, u16 pos, u16 len);

AFX8Echo *AFX8_echo_create(u16 bufferSize, u16 delay)
{
    delay = delay & 0xFF00;           // Must be multiple of 256
    bufferSize = bufferSize & 0xFF00; // Must be multiple of 256

    void *buf = MEM_alloc(bufferSize);
    memset(buf, 0, bufferSize);

    AFX8Echo *afx = (AFX8Echo *) MEM_alloc(sizeof(AFX8Echo));
    afx->delayLine = buf;
    afx->size = bufferSize;
    afx->delay = delay;
    afx->pos = 0;

    return afx;
}

void AFX8_echo_free(AFX8Echo *afx)
{
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX8_echo_update(AFX8Echo *afx, u16 delay)
{
    if (delay != afx->delay) {
        delay = delay & 0xFF00; // Must be multiple of 256
        if (delay > afx->size) {
            delay = afx->size;
        }
        afx->delay = delay;

        while (afx->pos > delay) {
            afx->pos -= delay;
        }
    }
}

void AFX8_echo_process(s8 *samples, u16 len, AFX8Echo *afx)
{
    AFX8_echo_process64_ASM(samples, len, afx->delayLine, afx->pos, afx->delay);

    afx->pos += len;
    if (afx->pos >= afx->delay) {
        afx->pos -= afx->delay;
    }
}

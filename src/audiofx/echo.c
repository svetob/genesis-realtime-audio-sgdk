#include <genesis.h>
#include "echo.h"

extern void AFX_echo_process64_ASM(s8 *samples, u16 size, s8 *delay_line, u16 pos, u16 len);

AFXEcho *AFX_echo_create(u16 bufferSize, u16 delay)
{
    delay = delay & 0xFF00;           // Must be multiple of 256
    bufferSize = bufferSize & 0xFF00; // Must be multiple of 256

    void *buf = MEM_alloc(bufferSize);
    memset(buf, 0, bufferSize);

    AFXEcho *afx = (AFXEcho *) MEM_alloc(sizeof(AFXEcho));
    afx->delayLine = buf;
    afx->size = bufferSize;
    afx->delay = delay;
    afx->pos = 0;

    return afx;
}

void AFX_echo_free(AFXEcho *afx)
{
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX_echo_reset(AFXEcho *afx)
{
    memset(afx->delayLine, 0, afx->size);
    afx->pos = 0;
}

void AFX_echo_update(AFXEcho *afx, u16 delay)
{
    if (delay != afx->delay) {
        delay = delay & 0xFF00; // Must be multiple of 256
        if (delay > afx->size) {
            delay = afx->size;
        }
        afx->delay = delay;

        while (afx->pos >= delay) {
            afx->pos -= delay;
        }
    }
}

void AFX_echo_process(s8 *samples, u16 len, AFXEcho *afx)
{
    AFX_echo_process64_ASM(samples, len, afx->delayLine, afx->pos, afx->delay);

    afx->pos += len;
    if (afx->pos >= afx->delay) {
        afx->pos -= afx->delay;
    }
}

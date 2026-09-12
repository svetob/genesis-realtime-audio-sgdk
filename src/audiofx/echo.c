#include <genesis.h>
#include "echo.h"

extern void AFX_echo_process64_ASM(s8 *samples, u16 size, s8 *delay_line, u16 pos, u16 len,
                                   u16 delay);

AFXEcho *AFX_echo_create(AFXEchoBufferSize bufferSize, u16 delay)
{
    delay = delay & 0xFFFC; // Must be multiple of 4

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
    memsetU32(afx->delayLine, 0, afx->size >> 2);
    afx->pos = 0;
}

void AFX_echo_update(AFXEcho *afx, u16 delay)
{
    afx->delay = delay & 0xFFFC; // Must be multiple of 4
}

extern void AFX_echo_process(s8 *samples, u16 len, AFXEcho *afx);

#include <genesis.h>
#include "echo.h"

extern void AFX_echo_process256_ASM(s8 *samples, u16 size, s8 *delay_line, u16 pos, u16 len);

AFX8Echo *AFX8_echo_create(u16 delay)
{
    delay = delay & 0xFF00; // Delay must be multiple of 256
    void *buf = MEM_alloc(delay);
    memset(buf, 0, delay);

    AFX8Echo *afx = (AFX8Echo *) MEM_alloc(sizeof(AFX8Echo));
    afx->delayLine = buf;
    afx->size = delay;
    afx->pos = 0;

    return afx;
}

void AFX8_echo_free(AFX8Echo *afx)
{
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX8_echo_process256(s8 *samples, AFX8Echo *afx)
{
    AFX_echo_process256_ASM(samples, 256, afx->delayLine, afx->pos, afx->size);

    afx->pos += 256;
    if (afx->pos >= afx->size) {
        afx->pos -= afx->size;
    }
}

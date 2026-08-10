#include <genesis.h>
#include "echo.h"

extern void AFX_echo_process256_ASM(s8 *samples, u16 size, s8 *delay_line, u16 pos, u16 len);

AFX8_Echo *AFX8_echo_create(u16 delay)
{
    delay = delay & 0xFF00; // Delay must be multiple of 256
    void *buf = MEM_alloc(delay);
    memset(buf, 0, delay);

    AFX8_Echo *afx = (AFX8_Echo *) MEM_alloc(sizeof(AFX8_Echo));
    afx->delayLine = buf;
    afx->size = delay;
    afx->pos = 0;

    return afx;
}

void AFX8_echo_free(AFX8_Echo *afx)
{
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX8_echo_process(s8 *samples, u16 size, AFX8_Echo *afx)
{
    AFX_echo_process256_ASM(samples, size, afx->delayLine, afx->pos, afx->size);

    afx->pos += size;
    if (afx->pos >= afx->size) {
        afx->pos -= afx->size;
    }
}

#include <genesis.h>
#include "echo16.h"

extern void AFX16_echo_process256_ASM(s16 *samples, u16 size, s16 *delay_line, u16 pos, u16 len);

AFX16Echo *AFX16_echo_create(u16 delay)
{
    delay = delay & 0xFF00; // Delay must be multiple of 256
    u16 size = delay << 1;
    void *buf = MEM_alloc(size);
    memset(buf, 0, size);

    AFX16Echo *afx = (AFX16Echo *) MEM_alloc(sizeof(AFX16Echo));
    afx->delayLine = buf;
    afx->size = delay;
    afx->pos = 0;

    return afx;
}

void AFX16_echo_free(AFX16Echo *afx)
{
    MEM_free(afx->delayLine);
    MEM_free(afx);
}

void AFX16_echo_process256(s16 *samples, AFX16Echo *afx)
{
    AFX16_echo_process256_ASM(samples, 256, afx->delayLine, afx->pos, afx->size);

    afx->pos += 256;
    if (afx->pos >= afx->size) {
        afx->pos -= afx->size;
    }
}

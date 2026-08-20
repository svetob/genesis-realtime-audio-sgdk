#include <genesis.h>
#include "log.h"

static char logbuf[40];

void logU8H(u8 val, u16 x, u16 y)
{
    intToHex(val, logbuf, 2);
    VDP_drawText(logbuf, x, y);
}

void logU16H(u16 val, u16 x, u16 y)
{
    intToHex(val, logbuf, 4);
    VDP_drawText(logbuf, x, y);
}

void logU32(u32 val, u16 x, u16 y, u16 minsize)
{
    intToStr(val, logbuf, minsize);
    VDP_drawText(logbuf, x, y);
}

void logS16H(s16 val, u16 x, u16 y)
{
    intToHex(val, logbuf, 4);
    VDP_drawText(logbuf, x, y);
}

void logS16(s16 val, u16 x, u16 y, u16 minsize)
{
    intToStr(val, logbuf, minsize);
    VDP_drawText(logbuf, x, y);
}

void logNamedU32H(const char *name, u32 val, u16 x, u16 y)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToHex(val, logbuf, 8);
    VDP_drawText(logbuf, x + strlen(name) + 2, y);
}

void logNamedU32(const char *name, u32 val, u16 x, u16 y, u16 minsize)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToStr(val, logbuf, minsize);
    VDP_drawText(logbuf, x + strlen(name) + 2, y);
}

void logNamedU16H(const char *name, u16 val, u16 x, u16 y)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToHex(val, logbuf, 4);
    VDP_drawText(logbuf, x + strlen(name) + 2, y);
}

void logNamedU16(const char *name, u16 val, u16 x, u16 y, u16 minsize)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToStr(val, logbuf, minsize);
    VDP_drawText(logbuf, x + strlen(name) + 2, y);
}

void logNamedU8H(const char *name, u8 val, u16 x, u16 y)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToHex(val, logbuf, 2);
    VDP_drawText(logbuf, x + strlen(name) + 2, y);
}

void logNamedPtr(const char *name, void *ptr, u16 x, u16 y)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToHex((u32) ptr, logbuf, 8);
    VDP_drawText(logbuf, x + strlen(name) + 2, y);
}

void logNamedPtrU16(const char *name, void *ptr, u16 x, u16 y)
{
    VDP_drawText(name, x, y);
    VDP_drawText(":", x + strlen(name), y);
    intToHex(((u32) ptr) & 0x0000FFFF, logbuf, 4);
    VDP_drawText(logbuf, x + strlen(name) + 1, y);
}

void logNamedArrayU8H(const char *name, u8 *arr, u16 n, u8 maxLine, u16 y)
{
    VDP_drawText("[", 1, y);
    VDP_drawText(name, 2, y);
    VDP_drawText("]:", 2 + strlen(name), y);

    u16 dx = 2;
    u16 dy = y + 1;
    VDP_drawText("[", 1, dy);
    for (u16 i = 0; i < n; i++) {
        if (dx >= 36 || dx >= (2 + maxLine * 3)) {
            dx = 2;
            dy++;
        }

        intToHex(arr[i], logbuf, 2);
        VDP_drawText(logbuf, dx, dy);
        VDP_drawText(",", dx + 2, dy);
        dx += 3;
    }
    VDP_drawText("]", dx - 1, dy);
}

void logNamedArrayU16H(const char *name, u16 *arr, u16 n, u8 maxLine, u16 y)
{
    VDP_drawText("[", 1, y);
    VDP_drawText(name, 2, y);
    VDP_drawText("]:", 2 + strlen(name), y);

    u16 dx = 2;
    u16 dy = y + 1;
    VDP_drawText("[", 1, dy);
    for (u16 i = 0; i < n; i++) {
        if (dx >= 36 || dx >= (2 + maxLine * 5)) {
            dx = 2;
            dy++;
        }

        intToHex(arr[i], logbuf, 4);
        VDP_drawText(logbuf, dx, dy);
        VDP_drawText(",", dx + 4, dy);
        dx += 5;
    }
    VDP_drawText("]", dx - 1, dy);
}

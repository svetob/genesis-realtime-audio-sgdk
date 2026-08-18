#include <genesis.h>

#include "log.h"

u16 startLine;

void scanlineTimerStart()
{
    startLine = GET_VCOUNTER;
}

u16 scanlineTimerStop()
{
    u16 endLine = GET_VCOUNTER;
    u16 scanLines = (endLine >= startLine) ? (endLine - startLine) : (262 - startLine + endLine);
    return scanLines;
}

#include <genesis.h>

#include "log.h"
#include "timer.h"

// #define DEBUG_LOG

static u16 scanlines[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static u8 scanlines_pos = 0;
static u16 startLine;

u16 scanlines_avg = 0;
u16 frame_ctr = 0;

#ifdef DEBUG_LOG
static u16 sampleI = 0;
#endif

/**
 * Reading the VCOUNTER properly turns out to require a PhD.
 *
 * Its behaviour and overflow point depends on:
 * - PAL vs NTSC,
 * - 224px (V28) vs 240px (V30) vertical screen resolution, and
 * - interlaced vs non-interlaced mode.
 *
 * To keep things simple here, we just assume:
 * V28 NTSC non-interlaced.
 *
 * Sources:
 * Genesis_Software_Manual.pdf
 * timer.c (SGDK)
 * https://plutiedev.com/mirror/kabuto-hardware-notes#hv-counter
 * http://www.gamepilgrimage.com/sites/default/files/SystemSpecs/Genesis/genvdp.txt
 */

#define VC_OVERFLOW_LOWER   0xE5
#define VC_OVERFLOW_UPPER   0xEA
#define IN_OVERFLOW_ZONE(x) (x >= VC_OVERFLOW_LOWER && x <= VC_OVERFLOW_UPPER)

static inline u16 calcVCounterInterval(u16 start, u16 end)
{
    if (end == start) {
        return 0;
    }
    if (end < start) {
        if (IN_OVERFLOW_ZONE(start) && IN_OVERFLOW_ZONE(end)) {
            end += (VC_OVERFLOW_UPPER - VC_OVERFLOW_LOWER + 1);
            return end - start;
        } else {
            return (end + 262) - start;
        }
    } else {
        return end - start;
    }
}

// Avoids measurement issues by waiting for VCounter to pass VBlank and overflow section
void scanlineTimerWait()
{
    while (GET_VCOUNTER > 200 && GET_VCOUNTER <= VC_OVERFLOW_UPPER) {
    }
}

void scanlineTimerStart()
{
    startLine = GET_VCOUNTER;
}

u16 scanlineTimerStop()
{
    u16 endLine = GET_VCOUNTER;

    u16 countedScanlines = calcVCounterInterval(startLine, endLine);
    scanlines[scanlines_pos] += countedScanlines;

#ifdef DEBUG_LOG
    logNamedU16("S", startLine, 1, 4 + sampleI++, 3);
    logNamedU16("E", endLine, 1, 4 + sampleI++, 3);
    logNamedU16("C", countedScanlines, 1, 4 + sampleI++, 3);
#endif

    return countedScanlines;
}

void scanlineTimerNextFrame()
{
    // Increment frame counter
    frame_ctr += 1;

    // Calculate scanline average
    scanlines_avg = 0;

    scanlines_avg += scanlines[0];
    scanlines_avg += scanlines[1];
    scanlines_avg += scanlines[2];
    scanlines_avg += scanlines[3];
    scanlines_avg += scanlines[4];
    scanlines_avg += scanlines[5];
    scanlines_avg += scanlines[6];
    scanlines_avg += scanlines[7];
    scanlines_avg += scanlines[8];
    scanlines_avg += scanlines[9];
    scanlines_avg += scanlines[10];
    scanlines_avg += scanlines[11];
    scanlines_avg += scanlines[12];
    scanlines_avg += scanlines[13];
    scanlines_avg += scanlines[14];
    scanlines_avg += scanlines[15];

    scanlines_avg = scanlines_avg >> 4;

    // Increment scanline array position
    scanlines_pos = (scanlines_pos + 1) & 0x0F;
    scanlines[scanlines_pos] = 0;

#ifdef DEBUG_LOG
    sampleI = 0;
#endif
}
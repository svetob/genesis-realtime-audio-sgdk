#ifndef __LP8_H
#define __LP8_H

#include <genesis.h>

#define multfix(x, y) ((s16) ((((s32) (x)) * ((s32) (y))) >> 8))

/**
 * References:
 *
 * Resonant filter algorithm:
 * https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html
 * https://github.com/bdejong/musicdsp/blob/master/source/Filters/29-resonant-filter.rst
 *
 * Replacing mults with shifts: (n*x) -> (n>>a + n>>b)
 * https://people.ece.cornell.edu/land/courses/ece4760/Math/DigitalFiltersVersion2.pdf
 *
 * Low-level fixed point ASM implementation:
 * https://github.com/MeeBlip/meeblip-synth/blob/master/meeblip-se.asm
 *
 */

typedef struct {
    u32 f;
    s32 q;
    s32 fb;

    void *mul_table_f_dec;
    void *mul_table_fb_int;
    void *mul_table_fb_dec;

    s8 buf0;
    s8 buf1;
} AFX8FilterLP;

AFX8FilterLP *AFX8_filter_lp_create(u32 cutoffFreq, s32 q);
AFX8FilterLP *AFX8_filter_lp_update(AFX8FilterLP *filter, u32 cutoffFreq, s32 q);
void AFX8_filter_lp_free(AFX8FilterLP *filter);
void AFX8_filter_lp_process(s8 *samples, u16 len, AFX8FilterLP *filter);

#endif
#include <genesis.h>
#include "pcm_stream.h"
#include "filterlp.h"
#include "../pcmstream/const.h"
#include "../test/log.h"

/**
 * 8-bit LP Filter implementations.
 *
 * Three filter types are available: 1POLE, 2POLE, 2POLE_RESONANT.
 *
 * The 2POLE_RESONANT implementation is based on:
 * https://www.musicdsp.org/en/latest/Filters/29-resonant-filter.html
 *
 * ```
 *      //set feedback amount given f and q between 0 and 1
 *      fb = q + q/(1.0 - f);
 *
 *      //for each sample...
 *      buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
 *      buf1 = buf1 + f * (buf0 - buf1);
 *      out = buf1;
 * ```
 *
 * The 2POLE implementation removes the feedback step from the above,
 * and ignores the Q parameter:
 *
 * ```
 *      //for each sample...
 *      buf0 = buf0 + f * (in - buf0);
 *      buf1 = buf1 + f * (buf0 - buf1);
 *      out = buf1;
 * ```
 *
 * The 1POLE implementation further removes the second chained filter:
 *
 * ```
 *      //for each sample...
 *      buf0 = buf0 + f * (in - buf0);
 *      out = buf0;
 * ```
 *
 * While the 2POLE_RESONANT filter sounds better under ideal conditions,
 * it also requires more cpu time, and is vulnerable to overflow, self
 * oscillation, and leaving behind audible noise floors.
 *
 * For the most easy to use filter with good sound and performance, the
 * 2POLE filter is the recommended one. The 1POLE filter can be used if
 * you are tight on CPU resources.
 *
 */

// #define DEBUG_LOG
// #define DEBUG_LOG_TRACE

// ===========================
// PRIVATE
// ===========================

extern void AFX_filter_lp_1pole_process64_ASM(s8 *samples, u16 len, void *mult_table_f_dec,
                                              s8 *buf0);

extern void AFX_filter_lp_2pole_process16_ASM(s8 *samples, u16 len, void *mult_table_f_dec,
                                              s8 *buf0, s8 *buf1);

extern void AFX_filter_lp_2pole_resonant_process16_ASM(s8 *samples, u16 len, void *mult_table_f_dec,
                                                       void *mult_table_fb_int,
                                                       void *mult_table_fb_dec, s8 *buf0, s8 *buf1);

static inline u32 calculateFeedback(u32 f, s32 q)
{
    /*
    //set feedback amount given f and q between 0 and 1
    fb = q + q/(1.0 - f);

    //for each sample...
    buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
    buf1 = buf1 + f * (buf0 - buf1);
    out = buf1;

    // Example:
    sample rate = 13300hz
    nyqvist freq = 6650hz

    f = 800hz
    q = 0.7

    gives:

    f = (800 / 6650) * 65536 = (800 << 16) / 6650 = 7884
    q = 0.7 * 65536 = 45875
    fb = 45875 + (45875 * 65536) / (65536 - 7884) = 45875 + 3006464000 / 57652 = 45875 + 52148 =
    98023
    */

    u32 divisor = (65536 - f);
    if (divisor < 100) {
        divisor = 100;
    }
    u32 fb = q + ((q << 16) / divisor);

#ifdef DEBUG_LOG
    KLog_U3("Feedback is ", fb, ", f ", f, ", q ", q);
    logNamedU32("F ", f, 1, 1, 1);
    logNamedU32("Q ", q, 1, 2, 1);
    logNamedU32("DV", divisor, 1, 3, 1);
    logNamedU32("FB", fb, 1, 4, 1);
#endif

    return fb;
}

// ===========================
// PUBLIC
// ===========================

AFXFilterLP *AFX_filter_lp_create(FilterLPType type, u32 cutoffFreq, s32 q)
{
    AFXFilterLP *filter = (AFXFilterLP *) MEM_alloc(sizeof(AFXFilterLP));
    filter->type = type;
    filter->buf0 = 0;
    filter->buf1 = 0;

    AFX_filter_lp_update(filter, cutoffFreq, q);

    return filter;
}

void AFX_filter_lp_setType(AFXFilterLP *filter, FilterLPType type)
{
    filter->type = type;
}

void AFX_filter_lp_update(AFXFilterLP *filter, u32 cutoffFreq, s32 q)
{
    if (cutoffFreq < 20) {
        cutoffFreq = 20;
    }
    if (cutoffFreq > ((PCM_PLAYBACK_RATE / 2) - 100)) {
        cutoffFreq = (PCM_PLAYBACK_RATE / 2) - 100;
    }

    /**
     * TODO: FIX INCORRECT RESPONSE CURVE
     *
     * Although the filter works, this does not give the correct linear response curve.
     * The correct algoritm for computing f from freq and samplerate is in comment thread
     * from 2006-09-12 by peter schoffhauzer:
     *
     * f = 2.0*sin(pi*freq/samplerate);
     *
     * This should go into perhaps a 1024-byte LUT. The input cutoffFreq should then be in
     * range [0-1023] representing a linear curve where 0 is 40hz and 1023 is samplerate/2.
     * We could also allow toggling between exponential and linear response curve here with
     * two different LUTs.
     */
    u32 f = (cutoffFreq << 16) / (PCM_PLAYBACK_RATE / 2);

    filter->f = f;
    filter->mul_table_f_dec = (void *) mult_s8_dec + (filter->f & 0x0000FF00);

    if (filter->type == FILTER_LP_2POLE_RESONANT) {
        filter->q = q;
        filter->fb = calculateFeedback(f, q);

        filter->mul_table_fb_int = (void *) mult_s8_int + ((filter->fb & 0x00FF0000) >> 8);
        filter->mul_table_fb_dec = (void *) mult_s8_dec + (filter->fb & 0x0000FF00);
    }

#ifdef DEBUG_LOG
    logNamedU32("cut", cutoffFreq, 15, 1, 1);
    logNamedPtr("mult int", mult_s8_int, 15, 2);
    logNamedPtr("mult dec", mult_s8_dec, 15, 3);
    logNamedU32H("f index dec", filter->f & 0x0000FF00, 15, 4);
    logNamedU32H("b index inc", ((filter->fb & 0x00FF0000) >> 8), 15, 5);
    logNamedU32H("b index dec", filter->fb & 0x0000FF00, 15, 6);
    logNamedPtr("mult f  dec", filter->mul_table_f_dec, 15, 7);
    logNamedPtr("mult fb int", filter->mul_table_fb_int, 15, 8);
    logNamedPtr("mult fb dec", filter->mul_table_fb_dec, 15, 9);
#endif
}

void AFX_filter_lp_free(AFXFilterLP *filter)
{
    MEM_free(filter);
}

void AFX_filter_lp_process(s8 *samples, u16 len, AFXFilterLP *filter)
{
    if (filter->type == FILTER_LP_1POLE) {
        AFX_filter_lp_1pole_process64_ASM(samples, len, filter->mul_table_f_dec, &(filter->buf0));
    }
    if (filter->type == FILTER_LP_2POLE) {
        AFX_filter_lp_2pole_process16_ASM(samples, len, filter->mul_table_f_dec, &(filter->buf0),
                                          &(filter->buf1));
    }
    if (filter->type == FILTER_LP_2POLE_RESONANT) {
        AFX_filter_lp_2pole_resonant_process16_ASM(
            samples, len, filter->mul_table_f_dec, filter->mul_table_fb_int,
            filter->mul_table_fb_dec, &(filter->buf0), &(filter->buf1));
    }
}

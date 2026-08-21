#include <genesis.h>
#include "pcm_stream.h"
#include "filter_lp8.h"
#include "../pcm_stream/const.h"
#include "../test/log.h"

#define DEBUG_LOG
// #define DEBUG_LOG_TRACE

extern void AFX8_filter_lp_process256_ASM(s8 *samples, u16 len, void *mult_table_f_dec,
                                          void *mult_table_fb_int, void *mult_table_fb_dec,
                                          s8 *buf0, s8 *buf1);

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
fb = 45875 + (45875 * 65536) / (65536 - 7884) = 45875 + 3006464000 / 57652 = 45875 + 52148 = 98023
*/

u32 calculateFeedback(u32 f, s32 q)
{
    u32 divisor = (65536 - f);
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

AFX8FilterLP *AFX8_filter_lp_create(u32 cutoffFreq, s32 q)
{
    if (cutoffFreq < 20) {
        cutoffFreq = 20;
    }
    if (cutoffFreq > (PCM_PLAYBACK_RATE / 2)) {
        cutoffFreq = (PCM_PLAYBACK_RATE / 2);
    }

    u32 f = (cutoffFreq << 16) / (PCM_PLAYBACK_RATE / 2);

    AFX8FilterLP *filter = (AFX8FilterLP *) MEM_alloc(sizeof(AFX8FilterLP));
    filter->f = f;
    filter->q = q;
    filter->fb = calculateFeedback(f, q);
    filter->buf0 = 0;
    filter->buf1 = 0;

    filter->mul_table_f_dec = mult_s8_dec + (filter->f & 0x0000FF00);

    filter->mul_table_fb_int = mult_s8_int + ((filter->fb & 0x00FF0000) >> 8);
    filter->mul_table_fb_dec = mult_s8_dec + (filter->fb & 0x0000FF00);

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

    return filter;
}

void AFX8_filter_lp_free(AFX8FilterLP *filter)
{
    MEM_free(filter);
}

void AFX8_filter_lp_process(s8 *samples, u16 len, AFX8FilterLP *filter)
{
    AFX8_filter_lp_process256_ASM(samples, len, filter->mul_table_f_dec, filter->mul_table_fb_int,
                                  filter->mul_table_fb_dec, &(filter->buf0), &(filter->buf1));
}

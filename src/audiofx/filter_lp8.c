#include <genesis.h>
#include "filter_lp8.h"
#include "../pcm_stream/const.h"
#include "../test/log.h"

#define DEBUG_LOG
// #define DEBUG_LOG_TRACE

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

u32 calculateFeedback(s32 f, s32 q)
{
    s32 divisor = (65536 - f);
    s32 fb = q + ((q << 16) / divisor);

#ifdef DEBUG_LOG
    KLog_U3("Feedback is ", fb, ", f ", f, ", q ", q);
    logNamedU32("F ", f, 20, 1, 1);
    logNamedU32("Q ", q, 20, 2, 1);
    logNamedU32("DV", divisor, 20, 3, 1);
    logNamedU32("FB", fb, 20, 4, 1);
#endif

    return fb;
}

AFX8FilterLP *AFX8_filter_lp_create(s32 cutoffFreq, s32 q)
{
    if (cutoffFreq < 20) {
        cutoffFreq = 20;
    }
    if (cutoffFreq > (PCM_PLAYBACK_RATE / 2)) {
        cutoffFreq = (PCM_PLAYBACK_RATE / 2);
    }

    logNamedU32("cut", cutoffFreq, 20, 0, 1);
    s32 f = (cutoffFreq << 16) / (PCM_PLAYBACK_RATE / 2);

    AFX8FilterLP *filter = (AFX8FilterLP *) MEM_alloc(sizeof(AFX8FilterLP));
    filter->f = f;
    filter->q = q;
    filter->fb = calculateFeedback(f, q);
    filter->buf0 = 0;
    filter->buf1 = 0;

    return filter;
}

void AFX8_filter_lp_free(AFX8FilterLP *filter)
{
    MEM_free(filter);
}

void AFX8_filter_lp_process(s8 *samples, AFX8FilterLP *filter)
{
    u16 i = 256;

    s8 buf0 = filter->buf0;
    s8 buf1 = filter->buf0;

    s32 f = filter->f;
    s32 fb = filter->fb;

#ifdef DEBUG_LOG_TRACE
    KLog_U3("F ", f, ", FB ", fb, ", Q ", filter->q);
#endif

    while (i--) {
        s8 in = *samples;

        // buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
        s8 buf0fb = (s8) ((fb * (s32) (buf0 - buf1)) >> 16);
        s8 buf0f = (s8) ((f * ((in - buf0) + buf0fb)) >> 16);
        buf0 = buf0 - buf0f;

        // buf1 = buf1 + f * (buf0 - buf1);
        s8 buf1f = (s8) ((f * (buf0 - buf1)) >> 16);
        buf1 = buf1 + buf1f;

#ifdef DEBUG_LOG_TRACE
        KLog_U4("Out is ", buf1, ", buf0 ", buf0, ", buf0f ", buf0f, ", buf0fb ", buf0fb);
        KLog_U1("In was ", in);
#endif

        *samples++ = buf1;
    }
}

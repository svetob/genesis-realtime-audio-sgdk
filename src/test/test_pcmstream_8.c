#include <genesis.h>
#include "resources.h"
#include "../audiofx/echo8.h"
#include "../audiofx/filter_lp8.h"
#include "../pcm_stream/pcm_stream8.h"

#include "log.h"
#include "timer.h"

#define PCM_PLAYBACK_RATE  13300
#define ECHO_DELAY_SAMPLES 4096

static PCMStream8 *pcm_stream = NULL;
static AFX8Echo *afx_echo = NULL;
static AFX8FilterLP *filter_lp = NULL;

static void streamProcessingCallback(s8 *stream, u16 len, void *data)
{
    // AFX8_echo_process((s8 *) stream, len, afx_echo);
    AFX8_filter_lp_process((s8 *) stream, len, filter_lp);
}

static void playStream()
{
    if (pcm_stream == NULL) {
        pcm_stream = PCM_STREAM_create(SOUND_PCM_CH3);
        PCM_STREAM_setProcessingCallback(streamProcessingCallback, NULL, pcm_stream);
    } else {
        PCM_STREAM_stop(pcm_stream);
        PCM_STREAM_reset(pcm_stream);
    }

    if (afx_echo == NULL) {
        afx_echo = AFX8_echo_create(ECHO_DELAY_SAMPLES);
    }

    if (filter_lp == NULL) {
        // filter_lp = AFX8_filter_lp_create(2000, 45875);
        filter_lp = AFX8_filter_lp_create(2000, 40000);
    }

    PCM_STREAM_start(pcm_stream);
}

static inline void updateStream(bool renderNext)
{
    scanlineTimerStart();
    PCM_STREAM_update(pcm_stream, renderNext);
    scanlineTimerStop();
}

static void resetStream()
{
    PCM_STREAM_stop(pcm_stream);
    PCM_STREAM_reset(pcm_stream);

    AFX8_echo_free(afx_echo);
    AFX8_echo_create(ECHO_DELAY_SAMPLES);

    PCM_STREAM_start(pcm_stream);
}

static void playSound()
{
    // PCM_STREAM_playSound((u8 *) wav_snare_rim, sizeof(wav_snare_rim), pcm_stream);
    PCM_STREAM_playSound((u8 *) wav_saw_sweep, sizeof(wav_saw_sweep), pcm_stream);
}

static void handleInput(u16 joy, u16 changed, u16 state)
{
    if (changed & state & BUTTON_A) {
        playSound();
    }

    if (changed & state & BUTTON_B) {
        resetStream();
    }
}

void testPCMStream8()
{
    VDP_drawText("8-Bit Echo demo!", 10, 13);

    JOY_setEventHandler(handleInput);
    Z80_loadDriver(Z80_DRIVER_XGM2, true);

    SYS_showFrameLoad(false);

    XGM2_play(vgm_test);

    playStream();

    while (true) {
        SYS_doVBlankProcess();

        if (pcm_stream != NULL) {
            updateStream(true);

            while (GET_VCOUNTER < 70) {
            }

            updateStream(false);

            while (GET_VCOUNTER < 140) {
            }

            updateStream(false);
        }

        scanlineTimerNextFrame();
        logNamedU16("FRAMES", frame_ctr, 1, 26, 4);
        logNamedU16("SCANLINES USED (AVG)", scanlines_avg, 1, 27, 3);
    }
}

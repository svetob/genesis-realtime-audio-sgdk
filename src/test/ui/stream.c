#include <genesis.h>
#include "resources.h"
#include "ui.h"

#include <audiofx/echo8.h>
#include <audiofx/filter_lp8.h>
#include <pcm_stream/pcm_stream8.h>

#include <test/log.h>
#include <test/timer.h>

// ===========================
// PARAMS
// ===========================

bool param_filter_enabled = true;
FilterLPType param_filter_type = FILTER_LP_2_POLE_RESONANT;
u16 param_filter_freq = 4000;
u16 param_filter_q = 40000;
bool filter_params_updated = false;

bool param_echo_enabled = true;
u16 param_echo_delay = 4096;
u8 param_echo_feedback = 220;
bool echo_params_updated = false;

// ===========================
// PRIVATE
// ===========================

#define PCM_PLAYBACK_RATE 13300
#define ECHO_BUFFER_SIZE  4096

static PCMStream8 *pcm_stream = NULL;
static AFX8Echo *afx_echo = NULL;
static AFX8FilterLP *afx_filter_lp = NULL;

static bool vgm_is_playing = false;

static void streamProcessingCallback(s8 *stream, u16 len, void *data)
{
    if (param_filter_enabled) {
        AFX8_filter_lp_process((s8 *) stream, len, afx_filter_lp);
    }
    if (param_echo_enabled) {
        AFX8_echo_process((s8 *) stream, len, afx_echo);
    }
}

// ===========================
// PUBLIC
// ===========================

void startStream()
{
    if (pcm_stream == NULL) {
        pcm_stream = PCM_STREAM_create(SOUND_PCM_CH3);
        PCM_STREAM_setProcessingCallback(streamProcessingCallback, NULL, pcm_stream);
    } else {
        PCM_STREAM_stop(pcm_stream);
        PCM_STREAM_reset(pcm_stream);
    }

    if (afx_echo == NULL) {
        afx_echo = AFX8_echo_create(ECHO_BUFFER_SIZE, param_echo_delay);
    }

    if (afx_filter_lp == NULL) {
        afx_filter_lp = AFX8_filter_lp_create(FILTER_LP_2_POLE_RESONANT, 2000, 45875);
    }

    PCM_STREAM_start(pcm_stream);
}

void updateParams()
{
    scanlineTimerStart();

    if (filter_params_updated) {
        AFX8_filter_lp_setType(afx_filter_lp, param_filter_type);
        AFX8_filter_lp_update(afx_filter_lp, param_filter_freq, param_filter_q);
        filter_params_updated = false;
    }
    if (echo_params_updated) {
        AFX8_echo_update(afx_echo, param_echo_delay);
        echo_params_updated = false;
    }
    scanlineTimerStop();
}

void updateStream(bool renderNext)
{
    scanlineTimerStart();
    PCM_STREAM_update(pcm_stream, renderNext);
    scanlineTimerStop();
}

void resetStream()
{
    PCM_STREAM_stop(pcm_stream);
    PCM_STREAM_reset(pcm_stream);

    AFX8_echo_free(afx_echo);
    afx_echo = AFX8_echo_create(ECHO_BUFFER_SIZE, param_echo_delay);

    AFX8_filter_lp_free(afx_filter_lp);
    afx_filter_lp = AFX8_filter_lp_create(param_filter_type, 2000, 45875);

    PCM_STREAM_start(pcm_stream);
}

void playSoundSweep()
{
    PCM_STREAM_playSound((u8 *) wav_saw_sweep, sizeof(wav_saw_sweep), pcm_stream);
}

void playSoundSnare()
{
    PCM_STREAM_playSound((u8 *) wav_snare_rim, sizeof(wav_snare_rim), pcm_stream);
}

void toggleVGM()
{
    if (vgm_is_playing) {
        XGM2_stop();
        vgm_is_playing = false;
    } else {
        XGM2_play(vgm_test);
        vgm_is_playing = true;
    }
}

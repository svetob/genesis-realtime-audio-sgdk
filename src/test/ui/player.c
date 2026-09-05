#include <genesis.h>
#include "resources.h"
#include "ui.h"

#include <audiofx/drive.h>
#include <audiofx/echo.h>
#include <audiofx/filterlp.h>
#include <pcmstream/pcmstream.h>

#include <test/log.h>
#include <test/timer.h>

// ===========================
// PARAMS
// ===========================

bool param_filter_enabled = false;
FilterLPType param_filter_type = FILTER_LP_2POLE_RESONANT;
u16 param_filter_freq = 4000;
u16 param_filter_q = 40000;
bool filter_params_updated = false;

bool param_echo_enabled = false;
u16 param_echo_delay = 4096;
u8 param_echo_feedback = 220;
bool echo_params_updated = false;

// ===========================
// PRIVATE
// ===========================

#define PCM_PLAYBACK_RATE 13300
#define ECHO_BUFFER_SIZE  4096

PCMStream *pcm_stream = NULL;
static AFXEcho *afx_echo = NULL;
static AFXFilterLP *afx_filter_lp = NULL;
static AFXDrive *afx_drive = NULL;

static bool vgm_is_playing = false;

static void streamProcessingCallback(s8 *stream, u16 len, void *data)
{
    if (param_filter_enabled) {
        AFX_filter_lp_process(stream, len, afx_filter_lp);
    }
    if (param_echo_enabled) {
        AFX_echo_process(stream, len, afx_echo);
    }
    AFX_drive_process(stream, len, afx_drive);
}

// ===========================
// PUBLIC
// ===========================

void startStream()
{
    if (pcm_stream == NULL) {
        pcm_stream = PCMSTREAM_create(SOUND_PCM_CH3);
        PCMSTREAM_setProcessingCallback(streamProcessingCallback, NULL, pcm_stream);
    } else {
        PCMSTREAM_stop(pcm_stream);
        PCMSTREAM_reset(pcm_stream);
    }

    if (afx_echo == NULL) {
        afx_echo = AFX_echo_create(ECHO_BUFFER_SIZE, param_echo_delay);
    }

    if (afx_filter_lp == NULL) {
        afx_filter_lp =
            AFX_filter_lp_create(FILTER_LP_2POLE_RESONANT, param_filter_freq, param_filter_q);
    }

    if (afx_drive == NULL) {
        afx_drive = AFX_drive_create(DRIVE_DIGITAL_CLIP, 4);
    }

    PCMSTREAM_start(pcm_stream);
}

void updateParams()
{
    scanlineTimerStart();

    if (filter_params_updated) {
        AFX_filter_lp_setType(afx_filter_lp, param_filter_type);
        AFX_filter_lp_update(afx_filter_lp, param_filter_freq, param_filter_q);
        filter_params_updated = false;
    }
    if (echo_params_updated) {
        AFX_echo_update(afx_echo, param_echo_delay);
        AFX_echo_reset(afx_echo);
        echo_params_updated = false;
    }
    scanlineTimerStop();
}

void updateStream(bool renderNext)
{
    scanlineTimerStart();
    PCMSTREAM_update(pcm_stream, renderNext);
    scanlineTimerStop();
}

void resetStream()
{
    PCMSTREAM_stop(pcm_stream);
    PCMSTREAM_reset(pcm_stream);

    AFX_echo_free(afx_echo);
    afx_echo = AFX_echo_create(ECHO_BUFFER_SIZE, param_echo_delay);

    AFX_filter_lp_free(afx_filter_lp);
    afx_filter_lp = AFX_filter_lp_create(param_filter_type, param_filter_freq, param_filter_q);

    PCMSTREAM_start(pcm_stream);
}

void playSoundSweep()
{
    PCMSTREAM_playSound((u8 *) wav_saw_sweep, sizeof(wav_saw_sweep), pcm_stream);
}

void playSoundSnare()
{
    PCMSTREAM_playSound((u8 *) wav_snare_rim, sizeof(wav_snare_rim), pcm_stream);
}

void playTestSoundMax()
{
    PCMSTREAM_playSound((u8 *) wav_test_max, sizeof(wav_test_max), pcm_stream);
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

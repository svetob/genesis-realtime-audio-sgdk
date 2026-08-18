#include <genesis.h>
#include "resources.h"
#include "../inst/saw.h"
#include "../audiofx/echo8.h"
#include "../pcm_stream/pcm_stream8.h"

#include "log.h"
#include "timer.h"

#define PCM_PLAYBACK_RATE  13300
#define ECHO_DELAY_SAMPLES 4096

static PCMStream8 *pcm_stream = NULL;
static AFX8Echo *afx_echo = NULL;
// static InstrSaw *inst_saw = NULL;

static u16 scanlines[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static u8 scanlines_pos = 0;

static void streamProcessingCallback(s8 *stream, u16 len, void *data)
{
    AFX8_echo_process256((s8 *) stream, afx_echo);
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

    // if (inst_saw == NULL) {
    //     inst_saw = INST_saw_create(1);
    //     PCM_STREAM_setInstrumentCallback(INST_saw_play, inst_saw, pcm_stream);
    // }

    PCM_STREAM_start(pcm_stream);
}

static void updateStream()
{
    scanlineTimerStart();
    PCM_STREAM_update(pcm_stream);
    scanlines[scanlines_pos] += scanlineTimerStop();
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
    PCM_STREAM_playSound((u8 *) wav_snare_rim, sizeof(wav_snare_rim), pcm_stream);
    // PCM_STREAM_setInstrumentCallback(NULL, NULL, pcm_stream);
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
    VDP_drawText("HInt used: ", 1, 30);

    JOY_setEventHandler(handleInput);
    Z80_loadDriver(Z80_DRIVER_XGM2, true);

    SYS_showFrameLoad(false);

    XGM2_play(vgm_test);

    playStream();

    while (true) {
        SYS_doVBlankProcess();

        scanlines[scanlines_pos] = 0;

        if (pcm_stream != NULL) {
            updateStream();

            vu16 i = 300;
            while (i--) {
            }

            updateStream();

            i = 300;
            while (i--) {
            }

            updateStream();

            i = 300;
            while (i--) {
            }

            updateStream();
        }

        u16 scanlines_avg = 0;
        for (u8 i = 0; i < 16; i++) {
            scanlines_avg += scanlines[i];
        }
        scanlines_avg = scanlines_avg >> 4;

        logNamedU16("SCANLINES USED (AVG)", scanlines_avg, 1, 20, 3);

        scanlines_pos = (scanlines_pos + 1) & 0x0F;
    }
}

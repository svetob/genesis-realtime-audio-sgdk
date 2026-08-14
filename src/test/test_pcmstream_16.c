#include <genesis.h>
#include "resources.h"
#include "../inst/saw.h"
#include "../audiofx/echo16.h"
#include "../pcm_stream/pcm_stream16.h"

#define PCM_PLAYBACK_RATE  13300
#define ECHO_DELAY_SAMPLES 4096

static PCMStream16 *pcm_stream = NULL;
static AFX16Echo *afx_echo = NULL;

static void streamProcessingCallback(s16 *stream, u16 len, void *data)
{
    AFX16_echo_process256((s16 *) stream, afx_echo);
}

static void playStream()
{
    if (pcm_stream == NULL) {
        pcm_stream = PCM_STREAM16_create(SOUND_PCM_CH3);
        PCM_STREAM16_setProcessingCallback(streamProcessingCallback, NULL, pcm_stream);
    } else {
        PCM_STREAM16_stop(pcm_stream);
        PCM_STREAM16_reset(pcm_stream);
    }

    if (afx_echo == NULL) {
        afx_echo = AFX16_echo_create(ECHO_DELAY_SAMPLES);
    }

    PCM_STREAM16_start(pcm_stream);
}

static void updateStream()
{
    PCM_STREAM16_update(pcm_stream);
}

static void resetStream()
{
    PCM_STREAM16_stop(pcm_stream);
    PCM_STREAM16_reset(pcm_stream);

    AFX16_echo_free(afx_echo);
    AFX16_echo_create(ECHO_DELAY_SAMPLES);

    PCM_STREAM16_start(pcm_stream);
}

static void playSound()
{
    PCM_STREAM16_playSound((u8 *) wav_snare_rim, sizeof(wav_snare_rim), pcm_stream);
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

void testPCMStream16()
{
    VDP_drawText("16-Bit Echo demo!", 10, 13);

    JOY_setEventHandler(handleInput);
    Z80_loadDriver(Z80_DRIVER_XGM2, true);

    SYS_showFrameLoad(false);

    playStream();

    while (true) {
        SYS_doVBlankProcess();

        if (pcm_stream != NULL) {
            updateStream();
        }
    }
}

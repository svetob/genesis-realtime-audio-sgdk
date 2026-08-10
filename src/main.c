#include <genesis.h>
#include "resources.h"
#include "audiofx/echo.h"
#include "pcm_stream/pcm_stream.h"

#define PCM_PLAYBACK_RATE  13300

#define ECHO_DELAY_SAMPLES 4096

SoundPCMStream *pcm_stream = NULL;
AFX8_Echo *afx_echo = NULL;

static void streamProcessingCallback(s8 *stream, u16 len, void *data)
{
    AFX8_echo_process((s8 *) stream, len, afx_echo);
}

static void playStream()
{
    if (pcm_stream == NULL) {
        pcm_stream = PCM_STREAM_create(SOUND_PCM_CH3, streamProcessingCallback, NULL);
    } else {
        PCM_STREAM_stop(pcm_stream);
        PCM_STREAM_reset(pcm_stream);
    }

    if (afx_echo == NULL) {
        afx_echo = AFX_echo_create(ECHO_DELAY_SAMPLES);
    }

    PCM_STREAM_start(pcm_stream);
}

static void updateStream()
{
    PCM_STREAM_update(pcm_stream);
}

static void resetStream()
{
    PCM_STREAM_stop(pcm_stream);
    PCM_STREAM_reset(pcm_stream);

    AFX_echo_free(afx_echo);
    AFX_echo_create(ECHO_DELAY_SAMPLES);

    PCM_STREAM_start(pcm_stream);
}

static void playSound()
{
    PCM_STREAM_playSound((u8 *) wav_snare_rim, sizeof(wav_snare_rim), pcm_stream);
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

int main(bool hard)
{
    VDP_drawText("8-Bit Echo demo!", 10, 13);

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

    return 0;
}

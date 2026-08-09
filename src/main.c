#include <genesis.h>
#include "resources.h"
#include "audiofx/echo.h"
#include "pcm_stream.h"

#define PCM_PLAYBACK_RATE 13300

#define ECHO_DELAY_SAMPLES (int) (PCM_PLAYBACK_RATE * 0.3)

const int SOUND_BUFFER_SIZE = 16384;
void* pcm_stream = NULL;
void* afx_echo = NULL;

static void playStream() {
    if (pcm_stream == NULL) {
        pcm_stream = PCM_STREAM_create(SOUND_BUFFER_SIZE);
    } else {
        PCM_STREAM_stop(pcm_stream);
        PCM_STREAM_reset(pcm_stream);
    }

    if (afx_echo == NULL) {
        afx_echo = AFX_echo_create(ECHO_DELAY_SAMPLES);
    }

    
}

static void updateStream() {

}

// TODO: Convert to stream
// static void playSoundBuffer() {
//     if (pcm_stream == NULL) {
//         pcm_stream = PCM_(SOUND_BUFFER_SIZE);
//     }

//     memset(sound_buffer, 0, SOUND_BUFFER_SIZE);
//     memcpy(sound_buffer, wav_as_rim, sizeof(wav_as_rim));
    
//     // Apply echo
//     AFX_Echo* echo = AFX_echo_create(2000);
//     AFX_echo_process(sound_buffer, SOUND_BUFFER_SIZE, echo);
//     AFX_echo_free(echo);

//     // Play
//     XGM2_playPCMEx((u8*) sound_buffer, SOUND_BUFFER_SIZE, SOUND_PCM_CH3, 15, false, true);
// }

static void playSound() {
    XGM2_playPCMEx(wav_as_rim, sizeof(wav_as_rim), SOUND_PCM_CH3, 15, false, false);
}

static void handleInput(u16 joy, u16 changed, u16 state) {
    if (changed & state & BUTTON_A) {
        playSound();
    }

    if (changed & state & BUTTON_B) {
        playSoundBuffer();
    }
}

int main(bool hard)
{
    VDP_drawText("Hello World!", 10, 13);
    KLog_U1("Hello World!", 1234);

    // Set up keyboard listener
    JOY_setEventHandler(handleInput);
    Z80_loadDriver(Z80_DRIVER_XGM2, true);

    u32 addr;
    u16 len;
    u8 isPlaying;

    while (true) {
        SYS_doVBlankProcess();

        XGM2_API_peek_channel(SOUND_PCM_CH3, &addr, &len, &isPlaying);
        KLog_U3("Addr: ", addr, ", Len: ", len, ", Playing: ", isPlaying);

        // TODO: Update stream and act on status
    }

    return 0;
}

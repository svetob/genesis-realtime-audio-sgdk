#ifndef __XGM2API_H
#define __XGM2API_H

#include <genesis.h>

void XGM2_API_peek_channel(SoundPCMChannel channel, u32* addr, u16* len, u8* isPlaying);

#define PCM_STREAM_SIZE 1024
#define PCM_STREAM_BUFFER_SIZE 512

/**
 *  \brief
 *      Current playback status of stream
 */
typedef enum
{
    PCM_STREAM_STATUS_STOPPED = 0, // Stream is stopped
    PCM_STREAM_STATUS_BUFFER0 = 1, // First buffer of stream is playing
    PCM_STREAM_STATUS_BUFFER1 = 2, // Second buffer of stream is playing
} SoundPCMStreamStatus;

/**
 *  \brief
 *      Action to take on stream this frame
 */
typedef enum
{
    PCM_STREAM_ACTION_NONE = 0, // Do nothing
    PCM_STREAM_ACTION_STOP = 1, // Stop and release stream
    PCM_STREAM_ACTION_PROCESS_BUFFER_0 = 2, // Apply audio processing to first buffer
    PCM_STREAM_ACTION_PROCESS_BUFFER_1 = 3, // Apply audio processing to second buffer
} SoundPCMStreamAction;

/**
 *  \brief
 *      Sound PCM Stream data
 */
typedef struct
{
    void* buffer;
    SoundPCMChannel channel;
    SoundPCMStreamStatus status;

    void* pcm_sound;
    int pcm_remain;
} SoundPCMStream;

/**
 * \brief
 *      PCM Stream audio processing callback.<br>
 *      This callback is where you should apply your audio processing onto the stream.<br>
 *      The callback is called after stream has had playing PCM sounds rendered onto it.<br>
 *      If no sounds are playing, the stream is cleared (with zeroes).
 * 
 * \param data
 *      Data for the audio processor.<br>
 *      Should be e.g. a struct with needed config and data for audio processing.
 * 
 * 
 */
typedef void SoundPCMStreamProcessingCallback(void* data);


SoundPCMStream* PCM_STREAM_create(SoundPCMChannel channel, bool clear);
void PCM_STREAM_reset(SoundPCMStream* stream);
void PCM_STREAM_free(SoundPCMStream* stream);

void PCM_STREAM_start(SoundPCMStream* stream);
void PCM_STREAM_isPlaying(SoundPCMStream* stream);
void PCM_STREAM_stop(SoundPCMStream* stream);

void PCM_STREAM_playSound(u8* pcm);

SoundPCMStreamAction PCM_STREAM_updateAndGetAction(SoundPCMStream* stream);
void* PCM_STREAM_getBuffer0(SoundPCMStream* stream);
void* PCM_STREAM_getBuffer1(SoundPCMStream* stream);

#endif
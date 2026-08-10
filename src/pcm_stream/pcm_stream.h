#ifndef __XGM2API_H
#define __XGM2API_H

#include <genesis.h>

/**
 *  \brief
 *      Current playback status of stream
 */
typedef enum {
    PCM_STREAM_STATUS_STOPPED = 0, // Stream is stopped
    PCM_STREAM_STATUS_BUFFER0 = 1, // First buffer of stream is playing
    PCM_STREAM_STATUS_BUFFER1 = 2, // Second buffer of stream is playing
} SoundPCMStreamStatus;

/**
 * \brief
 *      PCM Stream audio processing callback.<br>
 *      This callback is where you should apply your audio processing onto the stream.<br>
 *      The callback is called after stream has had playing PCM sounds rendered onto it.<br>
 *      If no sounds are playing, the stream is cleared (with zeroes).
 *
 * \param stream
 *      The PCM stream to apply audio proccessing onto.
 *
 * \param len
 *      Number of samples to process.
 *
 * \param data
 *      Data for the audio processor.<br>
 *      Should be e.g. a struct with needed config and data for audio processing.
 */
typedef void SoundPCMStreamProcessingCallback(s8 *stream, u16 len, void *data);

/**
 *  \brief
 *      Sound PCM Stream data struct
 */
typedef struct {
    void *buffer;
    SoundPCMChannel channel;
    SoundPCMStreamStatus status;

    SoundPCMStreamProcessingCallback *cb;
    void *cb_data;

    void *pcm_sound;
    u16 pcm_remain;
} SoundPCMStream;

SoundPCMStream *PCM_STREAM_create(SoundPCMChannel channel,
                                  SoundPCMStreamProcessingCallback *callback, void *callbackData);
void PCM_STREAM_reset(SoundPCMStream *stream);
void PCM_STREAM_free(SoundPCMStream *stream);

void PCM_STREAM_update(SoundPCMStream *stream);

void PCM_STREAM_start(SoundPCMStream *stream);
void PCM_STREAM_isPlaying(SoundPCMStream *stream);
void PCM_STREAM_stop(SoundPCMStream *stream);

void PCM_STREAM_playSound(u8 *pcm, u16 len, SoundPCMStream *stream);

#endif

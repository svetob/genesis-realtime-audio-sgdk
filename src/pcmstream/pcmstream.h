#ifndef __PCM_STREAM8_H
#define __PCM_STREAM8_H

#include <genesis.h>

#include "const.h"
#include "xgm2pcm.h"

/**
 *  \brief
 *      Current playback status of stream
 */
typedef enum {
    PCM_STREAM_STATUS_STOPPED = 0,         // Stream is stopped
    PCM_STREAM_STATUS_PLAYING = 1,         // Stream has just started playing
    PCM_STREAM_STATUS_PLAYING_BUFFER0 = 2, // First buffer of stream is playing
    PCM_STREAM_STATUS_PLAYING_BUFFER1 = 3, // Second buffer of stream is playing
} PCMStreamStatus;

/**
 * \brief
 *      PCM Stream instrument callback.<br>
 *      This callback is where you should play any software instruments and render their
 *      output ontp the stream.<br>
 *      The callback is called before stream has had playing PCM sounds rendered onto it.<br>
 *      The stream is cleared with zeroes before this callback.
 *
 * \param stream
 *      The PCM stream to apply instrument output onto.
 *
 * \param len
 *      Number of samples to process. Always equal to PCM_STREAM_BUFFER_SIZE.
 *
 * \param data
 *      Data for the instrument.<br>
 *      Should be e.g. a struct with needed config and data for audio processing.
 */
typedef void PCMStreamInstrumentCallback(s8 *stream, u16 len, void *data);

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
 *      Number of samples to process. Always equal to PCM_STREAM_BUFFER_SIZE.
 *
 * \param data
 *      Data for the audio processor.<br>
 *      Should be e.g. a struct with needed config and data for audio processing.
 */
typedef void PCMStreamProcessingCallback(s8 *stream, u16 len, void *data);

/**
 *  \brief
 *      Sound PCM Stream data struct
 */
typedef struct {
    void *buffer;

    XGM2PCMMixerStatus mixer;

    u16 bufferPosPrev;
    bool isPlaying; // Current playback status

    // AudioFX callback
    PCMStreamProcessingCallback *afx_cb;
    void *afx_cb_data;

    // PCM sound playback
    void *pcm_sound;
    u16 pcm_remain;

    // Instrument playback
    PCMStreamInstrumentCallback *inst_cb;
    void *inst_cb_data;
} PCMStream;

PCMStream *PCM_STREAM_create(SoundPCMChannel channel);
void PCM_STREAM_reset(PCMStream *stream);
void PCM_STREAM_free(PCMStream *stream);

void PCM_STREAM_update(PCMStream *stream, bool renderNext);

void PCM_STREAM_start(PCMStream *stream);
void PCM_STREAM_isPlaying(PCMStream *stream);
void PCM_STREAM_stop(PCMStream *stream);

void PCM_STREAM_playSound(u8 *pcm, u16 len, PCMStream *stream);
void PCM_STREAM_setInstrumentCallback(PCMStreamInstrumentCallback *callback, void *callbackData,
                                      PCMStream *stream);
void PCM_STREAM_setProcessingCallback(PCMStreamProcessingCallback *callback, void *callbackData,
                                      PCMStream *stream);

#endif

#ifndef __PCM_STREAM16_H
#define __PCM_STREAM16_H

#include <genesis.h>
#include "const.h"

/**
 *  \brief
 *      Current playback status of stream
 */
typedef enum {
    PCM_STREAM_STATUS_STOPPED = 0, // Stream is stopped
    // TODO Add to stream8, vital fix
    PCM_STREAM_STATUS_PLAYING_INIT = 1,    // Stream has just started playing
    PCM_STREAM_STATUS_PLAYING_BUFFER0 = 2, // First buffer of stream is playing
    PCM_STREAM_STATUS_PLAYING_BUFFER1 = 3, // Second buffer of stream is playing
} PCMStream16Status;

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
typedef void PCMStream16InstrumentCallback(s8 *stream, u16 len, void *data);

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
typedef void PCMStream16ProcessingCallback(s16 *stream, u16 len, void *data);

/**
 *  \brief
 *      Sound PCM Stream data struct
 */
typedef struct {
    void *render; // Render buffer;
    void *buffer; // Output buffer
    u16 bufferPos;
    PCMStream16Status status; // Current playback status
    u8 ringbufPos;            // Previous ring buffer write position

    // AudioFX callback
    PCMStream16ProcessingCallback *afx_cb;
    void *afx_cb_data;

    // PCM sound playback
    void *pcm_sound;
    s16 pcm_remain;

    // Instrument playback
    PCMStream16InstrumentCallback *inst_cb;
    void *inst_cb_data;
} PCMStream16;

PCMStream16 *PCM_STREAM16_create(SoundPCMChannel channel);
void PCM_STREAM16_reset(PCMStream16 *stream);
void PCM_STREAM16_free(PCMStream16 *stream);

void PCM_STREAM16_update(PCMStream16 *stream);

void PCM_STREAM16_start(PCMStream16 *stream);
void PCM_STREAM16_isPlaying(PCMStream16 *stream);
void PCM_STREAM16_stop(PCMStream16 *stream);

void PCM_STREAM16_playSound(u8 *pcm, u16 len, PCMStream16 *stream);
void PCM_STREAM16_setInstrumentCallback(PCMStream16InstrumentCallback *callback, void *callbackData,
                                        PCMStream16 *stream);
void PCM_STREAM16_setProcessingCallback(PCMStream16ProcessingCallback *callback, void *callbackData,
                                        PCMStream16 *stream);

#endif

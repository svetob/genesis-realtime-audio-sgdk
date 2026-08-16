#include <genesis.h>
#include "resources.h"
#include "pcm_stream8.h"
#include "xgm2_pcm.h"

// #define DEBUG_LOG

extern void PCM_STREAM8_mixAndClip256_ASM(s8 *in, s8 *out);

// ===========================
// PRIVATE
// ===========================

static inline void *getBuffer0(PCMStream8 *stream)
{
    return stream->buffer;
}

static inline void *getBuffer1(PCMStream8 *stream)
{
    return stream->buffer + PCM_STREAM8_BUFFER_SIZE;
}

static inline void doInstrumentCallback(void *buf, PCMStream8 *stream)
{
    if (stream->inst_cb != NULL) {
        stream->inst_cb(buf, PCM_STREAM8_BUFFER_SIZE, stream->inst_cb_data);
    }
}

static inline void doProcessingCallback(void *buf, PCMStream8 *stream)
{
    if (stream->afx_cb != NULL) {
        stream->afx_cb(buf, PCM_STREAM8_BUFFER_SIZE, stream->afx_cb_data);
    }
}

static inline void renderSoundsToStream(PCMStream8 *stream)
{
    if (stream->pcm_sound != NULL) {
#ifdef DEBUG_LOG
        KLog_U3("Playing sound stream16 - from 8bit PCM at ", stream->pcm_sound, ", to 16bit buf ",
                stream->render, ", len remaining ", stream->pcm_remain);
#endif

        PCM_STREAM8_mixAndClip256_ASM(stream->pcm_sound, stream->buffer);

        stream->pcm_remain -= PCM_STREAM_BUFFERLEN_SAMPLES;
        if (stream->pcm_remain <= 0) {
            stream->pcm_sound = NULL;
        } else {
            stream->pcm_sound += PCM_STREAM_BUFFERLEN_SAMPLES;
        }
    }
}

/**
 * \brief
 *      Initial rendering of next stream buffer.<br>
 *      Renders currently playing sounds and instruments, then
 *      applies the processing callback.
 */
static void renderStreamBuffer(u8 *buf, PCMStream8 *stream)
{
#ifdef DEBUG_LOG
    KLog_U2("Clearing at ", buf, ", len ", len);
#endif
    memset(buf, 0, PCM_STREAM8_BUFFER_SIZE);

    renderSoundsToStream(stream);
    doInstrumentCallback(buf, stream);
    doProcessingCallback(buf, stream);
}

// ===========================
// PUBLIC
// ===========================

PCMStream8 *PCM_STREAM_create(SoundPCMChannel channel)
{
    void *buf = MEM_alloc(PCM_STREAM8_SIZE);
    memsetU32(buf, 0, PCM_STREAM8_SIZE >> 2);

    PCMStream8 *stream = MEM_alloc(sizeof(PCMStream8));
    stream->buffer = buf;
    stream->bufferPos = 0;
    stream->ringbufPosPrev = XGM2_PCM_peek_ringbuf_writepos();

    stream->status = PCM_STREAM_STATUS_STOPPED;

    stream->inst_cb = NULL;
    stream->inst_cb_data = NULL;

    stream->afx_cb = NULL;
    stream->afx_cb_data = NULL;

    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;

    return stream;
}

void PCM_STREAM_reset(PCMStream8 *stream)
{
    memsetU32(stream->buffer, 0, PCM_STREAM8_SIZE >> 2);

    stream->bufferPos = 0;
    stream->status = PCM_STREAM_STATUS_STOPPED;
    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;
}

void PCM_STREAM_free(PCMStream8 *stream)
{
    MEM_free(stream->buffer);
    MEM_free(stream);
}

void PCM_STREAM_start(PCMStream8 *stream)
{
    stream->status = PCM_STREAM_STATUS_PLAYING_INIT;
    renderStreamBuffer(getBuffer0(stream), stream);
    XGM2_PCM_activate();
}

void PCM_STREAM_stop(PCMStream8 *stream)
{
    stream->status = PCM_STREAM_STATUS_STOPPED;
}

void PCM_STREAM_update(PCMStream8 *stream)
{
    if (stream->status == PCM_STREAM_STATUS_STOPPED) {
        return;
    }

    XGM2_PCM_mix_into_ringbuf(stream->buffer, &(stream->bufferPos), &(stream->ringbufPosPrev));

    PCMStream8Status statusPrev = stream->status;
    bool isBuffer0 = stream->bufferPos < 256;

#ifdef DEBUG_LOG
    KLog_U4("buf ", stream->buffer, ", status ", stream->status, ", statusPrev", statusPrev,
            ", isBuffer0 ", isBuffer0);
    KLog_U2("bufferPos ", stream->bufferPos, ", ringbufPosPrev ", stream->ringbufPosPrev);
#endif

    if (isBuffer0) {
        stream->status = PCM_STREAM_STATUS_PLAYING_BUFFER0;
        if (statusPrev == PCM_STREAM_STATUS_PLAYING_BUFFER1 ||
            statusPrev == PCM_STREAM_STATUS_PLAYING_INIT) {
#ifdef DEBUG_LOG
            KLog("Render Buf1");
#endif
            renderStreamBuffer(getBuffer1(stream), stream);
        }

    } else {
        stream->status = PCM_STREAM_STATUS_PLAYING_BUFFER1;
        if (statusPrev == PCM_STREAM_STATUS_PLAYING_BUFFER0) {
#ifdef DEBUG_LOG
            KLog("Render Buf0");
#endif
            renderStreamBuffer(getBuffer0(stream), stream);
        }
    }
}

void PCM_STREAM_playSound(u8 *pcm, u16 len, PCMStream8 *stream)
{
    stream->pcm_sound = pcm;
    stream->pcm_remain = len;
}

void PCM_STREAM_setInstrumentCallback(PCMStreamInstrumentCallback *callback, void *callbackData,
                                      PCMStream8 *stream)
{
    stream->inst_cb = callback;
    stream->inst_cb_data = callbackData;
}

void PCM_STREAM_setProcessingCallback(PCMStream8ProcessingCallback *callback, void *callbackData,
                                      PCMStream8 *stream)
{
    stream->afx_cb = callback;
    stream->afx_cb_data = callbackData;
}

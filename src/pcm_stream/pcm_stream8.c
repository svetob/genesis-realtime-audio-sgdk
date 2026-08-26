#include <genesis.h>
#include "resources.h"
#include "pcm_stream8.h"
#include "xgm2_pcm.h"

// #define DEBUG_LOG

extern void PCM_STREAM8_clear64_ASM(void *buf, u16 len);
extern void PCM_STREAM8_mixAndClip64_ASM(s8 *in, s8 *out, u16 len);

// ===========================
// PRIVATE
// ===========================

static inline void doInstrumentCallback(void *buf, u16 len, PCMStream8 *stream)
{
    if (stream->inst_cb != NULL) {
        stream->inst_cb(buf, len, stream->inst_cb_data);
    }
}

static inline void doProcessingCallback(void *buf, u16 len, PCMStream8 *stream)
{
    if (stream->afx_cb != NULL) {
        stream->afx_cb(buf, len, stream->afx_cb_data);
    }
}

static inline void renderSoundsToStream(void *buf, u16 len, PCMStream8 *stream)
{
    if (stream->pcm_sound != NULL) {
#ifdef DEBUG_LOG
        KLog_U3("Playing sound stream16 - from 8bit PCM at ", (u32) stream->pcm_sound,
                ", to 8bit buf ", (u32) buf, ", len remaining ", stream->pcm_remain);
#endif
        u16 renderLen = stream->pcm_remain > len ? len : stream->pcm_remain;

        PCM_STREAM8_mixAndClip64_ASM(stream->pcm_sound, buf, renderLen);

        stream->pcm_remain -= renderLen;
        if (stream->pcm_remain <= 0) {
            stream->pcm_sound = NULL;
        } else {
            stream->pcm_sound += renderLen;
        }
    }
}

/**
 * \brief
 *      Initial rendering of next stream buffer.<br>
 *      Renders currently playing sounds and instruments, then
 *      applies the processing callback.
 */
static inline void renderStreamBuffer(u8 *buf, u16 len, PCMStream8 *stream)
{
#ifdef DEBUG_LOG
    KLog_U2("Clearing at ", (u32) buf, ", len ", len);
#endif
    PCM_STREAM8_clear64_ASM((void *) buf, len);

    renderSoundsToStream(buf, len, stream);
    doInstrumentCallback(buf, len, stream);
    doProcessingCallback(buf, len, stream);
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
    stream->bufferPosPrev = 0;
    stream->ringbufPos = XGM2_PCM_peek_ringbuf_writepos();

    stream->isPlaying = false;

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
    stream->bufferPosPrev = 0;
    stream->isPlaying = false;
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
    stream->isPlaying = true;
    stream->bufferPos = 0;
    stream->bufferPosPrev = 0;
    renderStreamBuffer(stream->buffer, PCM_STREAM8_SIZE, stream);

    XGM2_PCM_activate();
    stream->ringbufPos = XGM2_PCM_peek_ringbuf_writepos();
}

void PCM_STREAM_stop(PCMStream8 *stream)
{
    stream->isPlaying = false;
}

void PCM_STREAM_update(PCMStream8 *stream, bool render)
{
    if (stream->isPlaying == false) {
        return;
    }

#ifdef DEBUG_LOG
    KLog("Updating PCM Stream");
#endif

    XGM2_PCM_mix_into_ringbuf(stream->buffer, &(stream->bufferPos), &(stream->ringbufPos));

    if (render) {
        if (stream->bufferPos < stream->bufferPosPrev) {
            u16 renderLen = PCM_STREAM8_SIZE - stream->bufferPosPrev;
#ifdef DEBUG_LOG
            KLog_U4("Rendering to buf ", (u32) stream->buffer, ", bufferPos ", stream->bufferPos,
                    ", bufferPosPrev ", stream->bufferPosPrev, ", renderLen ", renderLen);
#endif
            renderStreamBuffer(stream->buffer + stream->bufferPosPrev, renderLen, stream);
            stream->bufferPosPrev = 0;
        }

        if (stream->bufferPos > stream->bufferPosPrev) {
            u16 renderLen = stream->bufferPos - stream->bufferPosPrev;
#ifdef DEBUG_LOG
            KLog_U4("Rendering to buf ", (u32) stream->buffer, ", bufferPos ", stream->bufferPos,
                    ", bufferPosPrev ", stream->bufferPosPrev, ", renderLen ", renderLen);
#endif
            renderStreamBuffer(stream->buffer + stream->bufferPosPrev, renderLen, stream);
            stream->bufferPosPrev = stream->bufferPos;
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

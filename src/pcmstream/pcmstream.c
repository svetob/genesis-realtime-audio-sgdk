#include <genesis.h>
#include "resources.h"
#include "pcmstream.h"
#include "xgm2pcm.h"

// #define DEBUG_LOG

extern void PCMSTREAM_clear64_ASM(void *buf, u16 len);
extern void PCMSTREAM_mixAndClip64_ASM(s8 *in, s8 *out, u16 len);

// ===========================
// PRIVATE
// ===========================

static inline void doInstrumentCallback(void *buf, u16 len, PCMStream *stream)
{
    if (stream->inst_cb != NULL) {
        stream->inst_cb(buf, len, stream->inst_cb_data);
    }
}

static inline void doProcessingCallback(void *buf, u16 len, PCMStream *stream)
{
    if (stream->afx_cb != NULL) {
        stream->afx_cb(buf, len, stream->afx_cb_data);
    }
}

static inline void renderSoundsToStream(void *buf, u16 len, PCMStream *stream)
{
    if (stream->pcm_sound != NULL) {
#ifdef DEBUG_LOG
        KLog_U3("Playing 8bit PCM at ", (u32) stream->pcm_sound, ", to 8bit buf ", (u32) buf,
                ", len remaining ", stream->pcm_remain);
#endif
        u16 renderLen = stream->pcm_remain > len ? len : stream->pcm_remain;

        PCMSTREAM_mixAndClip64_ASM(stream->pcm_sound, buf, renderLen);

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
static inline void renderStreamBuffer(u8 *buf, u16 len, PCMStream *stream)
{
#ifdef DEBUG_LOG
    KLog_U2("Clearing at ", (u32) buf, ", len ", len);
#endif
    PCMSTREAM_clear64_ASM((void *) buf, len);

    renderSoundsToStream(buf, len, stream);
    doInstrumentCallback(buf, len, stream);
    doProcessingCallback(buf, len, stream);
}

// ===========================
// PUBLIC
// ===========================

PCMStream *PCMSTREAM_create(SoundPCMChannel channel)
{
    void *buf = MEM_alloc(PCMSTREAM_SIZE);
    memsetU32(buf, 0, PCMSTREAM_SIZE >> 2);

    PCMStream *stream = MEM_alloc(sizeof(PCMStream));
    stream->buffer = buf;
    stream->mixer.bufferPos = 0;
    stream->mixer.ringPosPrev = XGM2PCM_peek_ringbuf_writepos();
    stream->mixer.pcmWasPlaying = XGM2PCM_peek_ringbuf_writepos();

    stream->isPlaying = false;

    stream->inst_cb = NULL;
    stream->inst_cb_data = NULL;

    stream->afx_cb = NULL;
    stream->afx_cb_data = NULL;

    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;

    return stream;
}

void PCMSTREAM_reset(PCMStream *stream)
{
    memsetU32(stream->buffer, 0, PCMSTREAM_SIZE >> 2);

    stream->mixer.bufferPos = 0;
    stream->mixer.ringPosPrev = XGM2PCM_peek_ringbuf_writepos();
    stream->mixer.pcmWasPlaying = XGM2PCM_peek_ringbuf_writepos();

    stream->bufferPosPrev = 0;
    stream->isPlaying = false;
    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;
}

void PCMSTREAM_free(PCMStream *stream)
{
    MEM_free(stream->buffer);
    MEM_free(stream);
}

void PCMSTREAM_start(PCMStream *stream)
{
    stream->isPlaying = true;
    stream->mixer.bufferPos = 0;
    stream->bufferPosPrev = 0;
    renderStreamBuffer(stream->buffer, PCMSTREAM_SIZE, stream);

    XGM2PCM_activate();
    stream->mixer.ringPosPrev = XGM2PCM_peek_ringbuf_writepos();
}

void PCMSTREAM_stop(PCMStream *stream)
{
    stream->isPlaying = false;
}

void PCMSTREAM_update(PCMStream *stream, bool render)
{
    if (stream->isPlaying == false) {
        return;
    }

#ifdef DEBUG_LOG
    KLog("Updating PCM Stream");
#endif

    XGM2PCM_mix_into_ringbuf(stream->buffer, &(stream->mixer));

    if (render) {
        if (stream->mixer.bufferPos < stream->bufferPosPrev) {
            u16 renderLen = PCMSTREAM_SIZE - stream->bufferPosPrev;
#ifdef DEBUG_LOG
            KLog_U4("Rendering to buf ", (u32) stream->buffer, ", bufferPos ", stream->bufferPos,
                    ", bufferPosPrev ", stream->bufferPosPrev, ", renderLen ", renderLen);
#endif
            renderStreamBuffer(stream->buffer + stream->bufferPosPrev, renderLen, stream);
            stream->bufferPosPrev = 0;
        }

        if (stream->mixer.bufferPos > stream->bufferPosPrev) {
            u16 renderLen = stream->mixer.bufferPos - stream->bufferPosPrev;
#ifdef DEBUG_LOG
            KLog_U4("Rendering to buf ", (u32) stream->buffer, ", bufferPos ", stream->bufferPos,
                    ", bufferPosPrev ", stream->bufferPosPrev, ", renderLen ", renderLen);
#endif
            renderStreamBuffer(stream->buffer + stream->bufferPosPrev, renderLen, stream);
            stream->bufferPosPrev = stream->mixer.bufferPos;
        }
    }
}

void PCMSTREAM_playSound(u8 *pcm, u16 len, PCMStream *stream)
{
    stream->pcm_sound = pcm;
    stream->pcm_remain = len;
}

void PCMSTREAM_setInstrumentCallback(PCMStreamInstrumentCallback *callback, void *callbackData,
                                     PCMStream *stream)
{
    stream->inst_cb = callback;
    stream->inst_cb_data = callbackData;
}

void PCMSTREAM_setProcessingCallback(PCMStreamProcessingCallback *callback, void *callbackData,
                                     PCMStream *stream)
{
    stream->afx_cb = callback;
    stream->afx_cb_data = callbackData;
}

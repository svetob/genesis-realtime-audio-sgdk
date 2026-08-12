#include <genesis.h>
#include "resources.h"
#include "pcm_stream16.h"
#include "util.h"

// #define DEBUG_LOG

#define PCMS16_HEADROOM_BITS 3
#define PCMS16_DECIMAL_BITS  (8 - PCMS16_HEADROOM_BITS)

extern void PCM_STREAM16_renderToOutputBuffer_ASM(s16 *render, s8 *out);
extern void PCM_STREAM16_upscaleAndRenderSoundToStream_ASM(s8 *pcm, s16 *render);

// ===========================
// PRIVATE
// ===========================

static inline void *getBuffer0(PCMStream16 *stream)
{
    return stream->buffer;
}

static inline void *getBuffer1(PCMStream16 *stream)
{
    return stream->buffer + PCM_STREAM_BUFFERLEN_SAMPLES;
}

static inline void doInstrumentCallback(void *buf, PCMStream16 *stream)
{
    if (stream->inst_cb != NULL) {
        stream->inst_cb(buf, PCM_STREAM16_BUFFER_SIZE, stream->inst_cb_data);
    }
}

static inline void doProcessingCallback(void *buf, PCMStream16 *stream)
{
    if (stream->afx_cb != NULL) {
        stream->afx_cb(buf, PCM_STREAM16_BUFFER_SIZE, stream->afx_cb_data);
    }
}

static inline void renderSoundsToStream(PCMStream16 *stream)
{
#ifdef DEBUG_LOG
    KLog_U3("Playing sound stream16 - from 8bit PCM at ", stream->pcm_sound, ", to 16bit buf ",
            stream->render, ", len remaining ", stream->pcm_remain);
#endif

    PCM_STREAM16_upscaleAndRenderSoundToStream_ASM(stream->pcm_sound, stream->render);

    stream->pcm_remain -= PCM_STREAM_BUFFERLEN_SAMPLES;
    if (stream->pcm_remain <= 0) {
        stream->pcm_sound = NULL;
    } else {
        stream->pcm_sound += PCM_STREAM_BUFFERLEN_SAMPLES;
    }
}

/**
 * \brief
 *      Initial rendering of next stream buffer.<br>
 *      Renders currently playing sounds, otherwise clears buffer.
 */
static void renderStreamBuffer(s8 *buf, PCMStream16 *stream)
{
    s16 *render = stream->render;

    // Clear if no sound playing, or if instrument is playing
    // if (stream->pcm_sound == NULL || stream->inst_cb != NULL) {
#ifdef DEBUG_LOG
    KLog_U2("Clearing render buffer at ", render, ", len ", PCM_STREAM16_BUFFER_SIZE);
#endif
    memset(render, 0, PCM_STREAM16_BUFFER_SIZE);
    //}

    // Render instruments onto buffer
    doInstrumentCallback(render, stream);

    if (stream->pcm_sound != NULL) {
        // Render playing sound onto stream buffer
#ifdef DEBUG_LOG
        KLog_U3("Rendering sound ", stream->pcm_sound, " at ", render, ", remain ",
                stream->pcm_remain);
#endif
        renderSoundsToStream(stream);
    }

    doProcessingCallback(render, stream);

    PCM_STREAM16_renderToOutputBuffer_ASM(render, buf);
}

// ===========================
// PUBLIC
// ===========================

PCMStream16 *PCM_STREAM16_create(SoundPCMChannel channel)
{
    void *rndr = MEM_alloc(PCM_STREAM16_BUFFER_SIZE);

    void *buf = MEM_alloc(PCM_STREAM_LEN_SAMPLES + 256);
    // TODO: Temp hack to ensure 256-byte aligned buffer
    //       THIS __WILL__ BREAK FREEING THE MEMORY!! Must fix
    buf = (void *) (((u32) buf + 256) & 0x00FFFF00);

    PCMStream16 *stream = MEM_alloc(sizeof(PCMStream16));
    stream->render = rndr;

    stream->buffer = buf;
    stream->channel = channel;
    stream->status = PCM_STREAM_STATUS_STOPPED;

    stream->inst_cb = NULL;
    stream->inst_cb_data = NULL;

    stream->afx_cb = NULL;
    stream->afx_cb_data = NULL;

    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;

    return stream;
}

void PCM_STREAM16_reset(PCMStream16 *stream)
{
    // TODO if isplaying - stop

    stream->status = PCM_STREAM_STATUS_STOPPED;
    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;
}

void PCM_STREAM16_free(PCMStream16 *stream)
{
    // TODO: Check if playing, stop if so

    MEM_free(stream->buffer);
    MEM_free(stream);
}

void PCM_STREAM16_start(PCMStream16 *stream)
{
    stream->status = PCM_STREAM_STATUS_PLAYING_INIT;
#ifdef DEBUG_LOG
    KLog("Render Buf0 on start");
#endif
    renderStreamBuffer(getBuffer0(stream), stream);

    XGM2_playPCMEx(stream->buffer, PCM_STREAM_LEN_SAMPLES, stream->channel, 15, false, true);
}

void PCM_STREAM16_stop(PCMStream16 *stream)
{
    XGM2_stopPCM(stream->channel);
    stream->status = PCM_STREAM_STATUS_STOPPED;
}

void PCM_STREAM16_update(PCMStream16 *stream)
{
    if (stream->status == PCM_STREAM_STATUS_STOPPED) {
        return;
    }

    u32 addr;
    u16 len;
    u8 isPlaying;

    peek_XGM2_channel(stream->channel, &addr, &len, &isPlaying);

    PCMStream16Status statusPrev = stream->status;
    bool isBuffer0 = len > (PCM_STREAM_BUFFERLEN_SAMPLES / XGM2_PCM_CHUNK_SIZE);

#ifdef DEBUG_LOG
    KLog_U4("buf ", stream->buffer, ", addr ", (u32 *) addr, ", len ", len, ", isPlaying ",
            isPlaying);
    KLog_U3("status ", stream->status, ", statusPrev", statusPrev, ", isBuffer0 ", isBuffer0);
#endif

    if (!isPlaying) {
        stream->status = PCM_STREAM_STATUS_STOPPED;
    }

    if (isBuffer0) {
        stream->status = PCM_STREAM_STATUS_PLAYING_BUFFER0;
        if (statusPrev == PCM_STREAM_STATUS_PLAYING_BUFFER1 ||
            statusPrev == PCM_STREAM_STATUS_PLAYING_INIT) {
            // Z80 has started playing buffer 0 - prepare buffer 1
#ifdef DEBUG_LOG
            KLog("Render Buf1");
#endif
            renderStreamBuffer(getBuffer1(stream), stream);
        }

    } else {
        stream->status = PCM_STREAM_STATUS_PLAYING_BUFFER1;
        if (statusPrev == PCM_STREAM_STATUS_PLAYING_BUFFER0) {
            // Z80 has started playing buffer 1 - prepare buffer 0
#ifdef DEBUG_LOG
            KLog("Render Buf0");
#endif
            renderStreamBuffer(getBuffer0(stream), stream);
        }
    }
}

void PCM_STREAM16_playSound(u8 *pcm, u16 len, PCMStream16 *stream)
{
    stream->pcm_sound = pcm;
    stream->pcm_remain = len;
}

void PCM_STREAM16_setInstrumentCallback(PCMStream16InstrumentCallback *callback, void *callbackData,
                                        PCMStream16 *stream)
{
    stream->inst_cb = callback;
    stream->inst_cb_data = callbackData;
}

void PCM_STREAM16_setProcessingCallback(PCMStream16ProcessingCallback *callback, void *callbackData,
                                        PCMStream16 *stream)
{
    stream->afx_cb = callback;
    stream->afx_cb_data = callbackData;
}

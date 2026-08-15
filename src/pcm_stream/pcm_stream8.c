#include <genesis.h>
#include "const.h"
#include "pcm_stream8.h"
#include "xgm2_interface.h"

// #define DEBUG_LOG

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

/**
 * \brief
 *      Initial rendering of next stream buffer.<br>
 *      Renders currently playing sounds, otherwise clears buffer.
 */
static void renderStreamBuffer(u8 *buf, PCMStream8 *stream)
{
    // Clear if no sound playing, or if instrument is playing
    if (stream->pcm_sound == NULL || stream->inst_cb != NULL) {
#ifdef DEBUG_LOG
        KLog_U2("Clearing at ", buf, ", len ", len);
#endif
        memset(buf, 0, PCM_STREAM8_BUFFER_SIZE);
    }

    // Render instruments onto buffer
    doInstrumentCallback(buf, stream);

    if (stream->pcm_sound != NULL) {
        // Render playing sound onto stream buffer
#ifdef DEBUG_LOG
        KLog_U3("Rendering sound ", stream->pcm_sound, " at ", buf, ", remain ",
                stream->pcm_remain);
#endif
        int pcm_len = stream->pcm_remain;
        int process_len = pcm_len > PCM_STREAM8_BUFFER_SIZE ? PCM_STREAM8_BUFFER_SIZE : pcm_len;
        int len_remain = PCM_STREAM8_BUFFER_SIZE - process_len;

        stream->pcm_remain -= process_len;

        u8 *p_pcm = stream->pcm_sound;
        u8 *p_buf = buf;
        while (process_len--) {
            *p_buf++ = *p_pcm++;
        }

        // Clear remainder after finished sound
        if (len_remain > 0) {
            memset(p_buf, 0, len_remain);
        }

        if (stream->pcm_remain == 0) {
            stream->pcm_sound = NULL;
        } else {
            stream->pcm_sound += PCM_STREAM8_BUFFER_SIZE;
        }
    }

    doProcessingCallback(buf, stream);
}

// ===========================
// PUBLIC
// ===========================

PCMStream8 *PCM_STREAM_create(SoundPCMChannel channel)
{
    void *buf = MEM_alloc(PCM_STREAM8_SIZE + 256);
    // TODO: Temp hack to ensure 256-byte aligned buffer
    //       THIS __WILL__ BREAK FREEING THE MEMORY!! Must fix
    buf = (void *) (((u32) buf + 256) & 0x00FFFF00);

    PCMStream8 *stream = MEM_alloc(sizeof(PCMStream8));
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

void PCM_STREAM_reset(PCMStream8 *stream)
{
    // TODO if isplaying - stop

    stream->status = PCM_STREAM_STATUS_STOPPED;
    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;
}

void PCM_STREAM_free(PCMStream8 *stream)
{
    // TODO: Check if playing, stop if so

    MEM_free(stream->buffer);
    MEM_free(stream);
}

void PCM_STREAM_start(PCMStream8 *stream)
{
    stream->status = PCM_STREAM_STATUS_BUFFER0;
    renderStreamBuffer(getBuffer0(stream), stream);

    XGM2_playPCMEx(stream->buffer, PCM_STREAM8_SIZE, stream->channel, 15, false, true);
}

void PCM_STREAM_stop(PCMStream8 *stream)
{
    XGM2_stopPCM(stream->channel);
    stream->status = PCM_STREAM_STATUS_STOPPED;
}

void PCM_STREAM_update(PCMStream8 *stream)
{
    if (stream->status == PCM_STREAM_STATUS_STOPPED) {
        return;
    }

    u32 addr;
    u16 len;
    u8 isPlaying;

    peek_XGM2_channel(stream->channel, &addr, &len, &isPlaying);

#ifdef DEBUG_LOG
    KLog_U4("buf ", stream->buffer, ", addr ", (u32 *) addr, ", len ", len, ", isPlaying ",
            isPlaying);
#endif

    if (!isPlaying) {
        stream->status = PCM_STREAM_STATUS_STOPPED;
    }

    bool isBuffer0 = len > (PCM_STREAM8_BUFFER_SIZE / XGM2_PCM_CHUNK_SIZE);
    PCMStreamStatus statusPrev = stream->status;

    if (isBuffer0) {
        stream->status = PCM_STREAM_STATUS_BUFFER0;
        if (statusPrev == PCM_STREAM_STATUS_BUFFER1) {
            // Z80 has started playing buffer 0 - prepare buffer 1
#ifdef DEBUG_LOG
            KLog("Render Buf1");
#endif
            renderStreamBuffer(getBuffer1(stream), stream);
        }

    } else {
        stream->status = PCM_STREAM_STATUS_BUFFER1;
        if (statusPrev == PCM_STREAM_STATUS_BUFFER0) {
            // Z80 has started playing buffer 1 - prepare buffer 0
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

void PCM_STREAM_setProcessingCallback(PCMStreamProcessingCallback *callback, void *callbackData,
                                      PCMStream8 *stream)
{
    stream->afx_cb = callback;
    stream->afx_cb_data = callbackData;
}

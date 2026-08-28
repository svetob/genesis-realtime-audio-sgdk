#include <genesis.h>
#include "resources.h"
#include "pcmstream.h"
#include "xgm2pcm.h"

// #define DEBUG_LOG

extern void PCMSTREAM_clear64_ASM(void *buf, u16 len);
extern void PCMSTREAM_sound_raw_playback4_ASM(void *sounds, void *buf, u16 renderLen);

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
    // If first PCM sound is playing
    if (((u32 *) stream->pcmsound_raw_playback)[1]) {
#ifdef DEBUG_LOG
        KLog_U2("Playing 8bit PCMs to ", (u32) buf);
#endif
        PCMSTREAM_sound_raw_playback4_ASM(stream->pcmsound_raw_playback, buf, len);
    } else {
        // Clear stream if no sounds were played
        PCMSTREAM_clear64_ASM((void *) buf, len);
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

    void *soundBuf = MEM_alloc(PCMSTREAM_PLAYBACK_RAW_BUFSIZE);
    memset(buf, 0, PCMSTREAM_PLAYBACK_RAW_BUFSIZE);

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

    stream->pcmsound_raw_playback = soundBuf;

    return stream;
}

void PCMSTREAM_reset(PCMStream *stream)
{
    memsetU32(stream->buffer, 0, PCMSTREAM_SIZE >> 2);
    memset(stream->pcmsound_raw_playback, 0, PCMSTREAM_PLAYBACK_RAW_BUFSIZE);

    stream->mixer.bufferPos = 0;
    stream->mixer.ringPosPrev = XGM2PCM_peek_ringbuf_writepos();
    stream->mixer.pcmWasPlaying = XGM2PCM_peek_pcm_channel_status();

    stream->bufferPosPrev = 0;
    stream->isPlaying = false;
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
    /**
     * Inserts the new sound into the list of playing sounds,
     * such that the list is a sorted list, sorted descending
     * by remaining playback length. This allows the playback
     * ASM to make assumptions and skip checks, saving cycles
     * during playback.
     */
    u8 n = PCMSTREAM_PLAYBACK_RAW_MAX;
    PCMSoundPlaybackRaw *data = stream->pcmsound_raw_playback;

    // Find first slot that is free or has remaining playback length
    while (n-- && data->remain >= len) {
        data += sizeof(PCMSoundPlaybackRaw);
    }

    if (n >= 0) {
        // Slot found, insert and shift rest upwards
        // TODO: Increment playing counter?

        PCMSoundPlaybackRaw buf;
        PCMSoundPlaybackRaw tmp;

        // TODO memcpy?
        buf.pcm = data->pcm;
        buf.remain = data->remain;

        data->pcm = pcm;
        data->remain = len;

        data += sizeof(PCMSoundPlaybackRaw);

        while (n-- && data->remain) {
            tmp.pcm = data->pcm;
            tmp.remain = data->remain;

            data->pcm = buf.pcm;
            data->remain = buf.remain;

            buf.pcm = tmp.pcm;
            buf.remain = tmp.remain;

            data += sizeof(PCMSoundPlaybackRaw);
        }
    }
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

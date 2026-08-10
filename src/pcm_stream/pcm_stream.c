#include <genesis.h>
#include "pcm_stream.h"

// #define DEBUG_LOG

// === PRIVATE FUNCTIONS ===

#define PCM_STREAM_SIZE              512
#define PCM_STREAM_BUFFER_SIZE       256

// Addresses to internal Z80 vars
// TODO: Use existing constants instead
#define XGM2_PCM_VARS                (void *) (Z80_RAM + 0x0110 + 0xD0)
#define XGM2_PCM_VARS_CHANNEL_OFFSET 0x08

#define XGM2_PCM_CHUNK_SIZE          64

/**
 * \brief
 *      Reads current state of XGM2 PCM channel via Z80 bus.
 */
void peek_XGM2_channel(SoundPCMChannel channel, u32 *addr, u16 *len, u8 *isPlaying)
{

    // Find Z80 addresses to read from
    u8 *p_vars = XGM2_PCM_VARS + (channel * (XGM2_PCM_VARS_CHANNEL_OFFSET));
    vu8 *p_status = (vu8 *) Z80_DRV_STATUS;

    // Request bus and read values
    SYS_disableInts();
    Z80_getAndRequestBus(true);

    // References for address, len offsets: drv_xgm2.s80, xgm2.c
    u8 a0 = *p_vars++;
    u8 a1 = *p_vars++;
    u8 a2 = *p_vars++;
    u8 l0 = *p_vars++;
    u8 l1 = *p_vars;

    u8 status = *p_status;

    Z80_releaseBus();
    SYS_enableInts();

    // Build and return results
    *addr = (a2 << 16) + (a1 << 8) + a0;
    *len = (l1 << 8) + l0;
    *isPlaying =
        status &
        (1
         << channel); // TODO: Also check address location to confirm it is within stream boundaries
}

inline void *PCM_STREAM_getBuffer0(SoundPCMStream *stream)
{
    return stream->buffer;
}

inline void *PCM_STREAM_getBuffer1(SoundPCMStream *stream)
{
    return stream->buffer + PCM_STREAM_BUFFER_SIZE;
}

inline void doCallback(void *buf, int len, SoundPCMStream *stream)
{
    if (stream->cb == NULL) {
        return;
    }

    stream->cb(buf, len, stream->cb_data);
}

/**
 * \brief
 *      Initial rendering of next stream buffer.<br>
 *      Renders currently playing sounds, otherwise clears buffer.
 */
void renderStream(u8 *buf, int len, SoundPCMStream *stream)
{
    // Clear if no sound playing
    if (stream->pcm_sound == NULL) {
#ifdef DEBUG_LOG
        KLog_U2("Clearing at ", buf, ", len ", len);
#endif
        memset(buf, 0, len);

        doCallback(buf, len, stream);
        return;
    }

    // Render playing sound onto stream buffer
#ifdef DEBUG_LOG
    KLog_U3("Rendering sound ", stream->pcm_sound, " at ", buf, ", remain ", stream->pcm_remain);
#endif
    int pcm_len = stream->pcm_remain;
    int process_len = pcm_len > len ? len : pcm_len;
    int len_remain = len - process_len;

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
        stream->pcm_sound += len;
    }

    doCallback(buf, len, stream);
}

// === PUBLIC FUNCTIONS ===

SoundPCMStream *PCM_STREAM_create(SoundPCMChannel channel,
                                  SoundPCMStreamProcessingCallback *callback, void *callbackData)
{
    void *buf = MEM_alloc(PCM_STREAM_SIZE + 256);
    // TODO: Temp hack to ensure 256-byte aligned buffer
    //       THIS __WILL__ BREAK FREEING THE MEMORY!! Must fix
    buf = (void *) (((u32) buf + 256) & 0x00FFFF00);

    SoundPCMStream *stream = MEM_alloc(sizeof(SoundPCMStream));
    stream->buffer = buf;
    stream->channel = channel;
    stream->status = PCM_STREAM_STATUS_STOPPED;

    stream->cb = callback;
    stream->cb_data = callbackData;

    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;

    return stream;
}

void PCM_STREAM_reset(SoundPCMStream *stream)
{
    // TODO if isplaying - stop

    stream->status = PCM_STREAM_STATUS_STOPPED;
    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;
}

void PCM_STREAM_free(SoundPCMStream *stream)
{
    // TODO: Check if playing, stop if so

    MEM_free(stream->buffer);
    MEM_free(stream);
}

void PCM_STREAM_start(SoundPCMStream *stream)
{
    stream->status = PCM_STREAM_STATUS_BUFFER0;
    renderStream(PCM_STREAM_getBuffer0(stream), PCM_STREAM_BUFFER_SIZE, stream);

    XGM2_playPCMEx(stream->buffer, PCM_STREAM_SIZE, stream->channel, 15, false, true);
}

void PCM_STREAM_stop(SoundPCMStream *stream)
{
    XGM2_stopPCM(stream->channel);
    stream->status = PCM_STREAM_STATUS_STOPPED;
}

void PCM_STREAM_update(SoundPCMStream *stream)
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

    bool isBuffer0 = len > (PCM_STREAM_BUFFER_SIZE / XGM2_PCM_CHUNK_SIZE);
    SoundPCMStreamStatus statusPrev = stream->status;

    if (isBuffer0) {
        stream->status = PCM_STREAM_STATUS_BUFFER0;
        if (statusPrev == PCM_STREAM_STATUS_BUFFER1) {
            // Z80 has started playing buffer 0 - prepare buffer 1
#ifdef DEBUG_LOG
            KLog("Render Buf1");
#endif
            renderStream(PCM_STREAM_getBuffer1(stream), PCM_STREAM_BUFFER_SIZE, stream);
        }

    } else {
        stream->status = PCM_STREAM_STATUS_BUFFER1;
        if (statusPrev == PCM_STREAM_STATUS_BUFFER0) {
            // Z80 has started playing buffer 1 - prepare buffer 0
#ifdef DEBUG_LOG
            KLog("Render Buf0");
#endif
            renderStream(PCM_STREAM_getBuffer0(stream), PCM_STREAM_BUFFER_SIZE, stream);
        }
    }
}

void PCM_STREAM_playSound(u8 *pcm, u16 len, SoundPCMStream *stream)
{
    stream->pcm_sound = pcm;
    stream->pcm_remain = len;
}

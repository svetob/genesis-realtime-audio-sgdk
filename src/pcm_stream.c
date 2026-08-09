#include <genesis.h>
#include "pcm_stream.h"

// === PRIVATE FUNCTIONS ===

// Addresses to internal Z80 vars
// TODO: Use existing constants instead
#define XGM2_PCM_VARS                  Z80_RAM + 0x0110 + 0xD0
#define XGM2_PCM_VARS_CHANNEL_OFFSET   0x08

void XGM2_API_peek_channel(SoundPCMChannel channel, u32* addr, u16* len, u8* isPlaying) {

    // Find Z80 addresses to read from
    u8* p_vars = XGM2_PCM_VARS + (channel * (XGM2_PCM_VARS_CHANNEL_OFFSET));
    vu8* p_status = (vu8*) Z80_DRV_STATUS;

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
    *isPlaying = status & (1 << channel); // TODO: Also check address location to confirm it is within stream boundaries
}

void renderStream(u8* buf, int len, SoundPCMStream* stream) {
    // Clear if no sound playing
    if (stream->pcm_sound == NULL) {
        memset(buf, 0, len);
    }

    // Render playing sound onto stream buffer
    int pcm_len = stream->pcm_remain;
    int process_len = pcm_len > len ? len : pcm_len;
    len -= process_len;
    stream->pcm_remain -= process_len;

    u8* p_pcm = stream->pcm_sound;
    while (process_len--) {
        *buf++ = p_pcm++;
    }

    // Clear remainder after finished sound
    if (len > 0) {
        memset(buf, 0, len);
    }
    
}

// === PUBLIC FUNCTIONS ===

SoundPCMStream* PCM_STREAM_create(SoundPCMChannel channel, bool clear) {
    void* buf = MEM_alloc(PCM_STREAM_SIZE);
    if (clear) {
        memset(buf, 0, PCM_STREAM_SIZE);
    }

    SoundPCMStream* stream = MEM_alloc(sizeof(SoundPCMStream));
    stream->buffer = buf;
    stream->channel = channel;
    stream->status = PCM_STREAM_STATUS_STOPPED;

    stream->pcm_sound = NULL;
    stream->pcm_remain = 0;
}

void PCM_STREAM_free(SoundPCMStream* stream) {
    // TODO: Check if playing, stop if so

    MEM_free(stream->buffer);
    MEM_free(stream);
}

void PCM_STREAM_start(SoundPCMStream* stream) {
    XGM2_playPCMEx(stream->buffer, PCM_STREAM_SIZE, stream->channel, 15, false, true);
    stream->status = PCM_STREAM_STATUS_BUFFER0;
}

void PCM_STREAM_stop(SoundPCMStream* stream) {
    XGM2_stopPCM(stream->channel);
    stream->status = PCM_STREAM_STATUS_STOPPED;
}

SoundPCMStreamAction PCM_STREAM_updateAndGetAction(SoundPCMStream* stream) {
    if (stream->status == PCM_STREAM_STATUS_STOPPED) {
        return PCM_STREAM_ACTION_STOP;
    }

    u32 addr;
    u16 len;
    u8 isPlaying;

    XGM2_API_peek_channel(stream->channel, &addr, &len, &isPlaying);

    if (!isPlaying) {
        stream->status = PCM_STREAM_STATUS_STOPPED;
    }

    bool isBuffer0 = len > (512 / 64);
    SoundPCMStreamStatus statusPrev = stream->status;
    
    if (isBuffer0) {
        stream->status = PCM_STREAM_STATUS_BUFFER0;
        return statusPrev == PCM_STREAM_STATUS_BUFFER0
            ? PCM_STREAM_ACTION_PROCESS_BUFFER_1
            : PCM_STREAM_ACTION_NONE;
    } else {
        stream->status = PCM_STREAM_STATUS_BUFFER1;
        return statusPrev == PCM_STREAM_STATUS_BUFFER1
            ? PCM_STREAM_ACTION_PROCESS_BUFFER_0
            : PCM_STREAM_ACTION_NONE;
    }
}

void* PCM_STREAM_getBuffer0(SoundPCMStream* stream) {
    return stream->buffer;
}

void* PCM_STREAM_getBuffer1(SoundPCMStream* stream) {
    return stream->buffer + PCM_STREAM_BUFFER_SIZE;
}
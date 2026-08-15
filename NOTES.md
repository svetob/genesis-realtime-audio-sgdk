
## Z80 RAM access

Z80 can write but _not read_ RAM on hardware - reads return 0xFF.

<https://gendev.spritesmind.net/forum/viewtopic.php?p=14086&sid=aa07f0530e4cf59c619b5129bc69c576#p14086>

# Streaming 13.3khz PCM data to XGM2

Since the Z80 cannot read from RAM, the only viable approach is streaming the PCM audio directly on to the Z80 RAM from the 68000 processor.

There is not enough space to store additional PCM data on the XGM2 driver. Even if there were, the driver is quite complex to work with. Even ignoring all that, there would not be enough CPU time for another PCM buffer.

However, before rendering new PCM samples to the internal 256-byte PCM ring buffer, a process which is done in 64-byte chunks, the new chunk is cleared. If this clear does not happen, rendered PCM is added on top of whatever was already there.

By simply disabling this clear, and instead prefilling the chunks with PCM data, this is our opening to stream PCM audio to the driver without needing to heavily modify the XGM2 driver. Streaming freezes the Z80, which will cause minor PCM distortion, but it should be barely audible, and also means we dont need to worry about 68000/Z80 timing issues.

So we can as first pass just disable the clearing, and replace this with the PCM stream, and this should allow us to add a PCM stream on top of the existing XGM2 driver without sacrificing any existing functionality.

Actually - we don't strictly need to disable the clearing. Doing this would introduce tricky timing issues. Instead we can on 68K side to the PCM mixing, we can read and update the buffer. This adds a bit more halt time but avoids a bunch of timing issues. (XGM2 may rerender a ring buffer as soon as it is finished playing - the timing is too strict.) Soooo actually this can be done without any XGM2 driver updates here.

We might need one update still though - the playback pos is currently only in a CPU register, we need to find some reasonable place to update this to RAM.

No actually we don't even need to do that - we can just compare current write_pos to previous write_pos to determine the memory area to update! Maybe this can be done without any driver updates at all??

## PCM Ring buffer

XGM2 ring buffer is 256 samples. It is cleared and rerendered every 64 samples.

Ring buffer samples per frame is:
NTSC = 13.3khz / 60hz = 221
PAL  = 13.3khz / 50hz = 266

This means:

- On NTSC we can prefill the buffer each frame.
- On PAL we need to re-check mid-screen somehow.

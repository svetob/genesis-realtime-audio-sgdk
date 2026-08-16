# Sega Genesis Real-Time Audio Processing

This is a proof-of-concept for real-time audio generation and processing on the Sega Genesis / Mega Drive.

The ROM generates audio and applies effects (echo) in real time on the main 68000 processor. The audio is then streamed from RAM via the XGM2 driver on the Z80 processor to the YM2162 sound chip. No modifications to the XGM2 driver are necessary.

## Getting started

A prebuit rom is in [out/rom.bin](out/rom.bin). Press A to play a sound effect with echo applied.

## How the XGM2 driver integration is done

As the XGM2 driver is already at full capacity in both space and performance, there was a question of how to add another PCM source to it. The Z80 can't handle processing another PCM stream, there is no space in Z80 memory for more PCM data, and the PCM stream also must be streamed from RAM, which the Z80 cannot access on real hardware. (but interestingly, it can on some emulators like MAME and Gens...) In addition, the XGM2 source code is very tightly handcrafted ASM, making it a very major challenge to edit.

However, XGM2 renders PCM streams in advance to an __internal PCM ring buffer__. This buffer is 256 bytes long and is updated in 64-byte chunks. Luckily, outdated PCM chunks are re-rendered in a timely fashion (at least ~140 bytes in advance under normal circumstances). This means that we can add a live 13.3khz PCM audio stream to the XGM2 driver __without sacrificing any functionality or performance, with no driver modifications needed,__ by simply mixing it into the PCM ring buffer with well-timed updates! Under normal circumstances, only 2 bus transfers per frame are necessary on both PAL and NTSC, but to guarantee no missed audio chunks, we check for new chunks a few extra times per frame.

So, this library works by generating a 13.3khz signed 8-bit PCM stream in software, and then submitting it to the DAC output by mixing it into the XGM2's internal ring buffer with multiple writes per frame.

## Known Issues

### 8-bit vs 16-bit audio effects

The PCM output is 8-bit, so the straightforward approach for audio processing is 8-bit audio effects. However, in addition to the obvious lower audio fidelity, 8-bit audio effects also suffer from other problems, such as:

- Audio overflow, instead of clipping
- Audible noise floors
- Loss of precision when increasing gain on low audio signals

16-bit audio effects solve all the above problems, with the main tradeoff being more CPU cycles needed.

That said, you should be able to get away with using 8-bit audio effects, by being careful with audio levels when mixing to avoid overflows, and chaining effects carefully to avoid the other issues.

### Stream update timing

The timing of when new ring buffer chunks are generated, and the timing margin for updating them in time, depends on the workload of the XGM driver. To guarantee updating all chunks in time under normal operation, we need to check for new audio chunks multiple times per frame.

The library provides utility functions to handle this, but if your game loop consumes a large amount of the frame budget, you might also want to add one or two extra checks at well selected spots in your game loop to make sure these updates are performed mid-frame when necessary.

### Z80 freeze during updates

The Z80 is frozen while stream updates are being transfered to the Z80. In theory, this should cause minor audio glitches as DAC output freezes for a brief moment, but in practice this seem to be barely audible. (At least to my ears...!) The stream transfer function is nevertheless optimized to minimize freeze time.

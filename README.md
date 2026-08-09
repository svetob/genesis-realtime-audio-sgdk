# Sega Genesis Real-Time Audio Processing

This is a proof-of-concept for real-time audio generation and processing on the Sega Genesis / Mega Drive.

The ROM generates audio and applies effects (echo) in real time on the main 68000 processor. The audio is then streamed from RAM via the Z80 processor to the YM2162 sound chip.

## Getting started

First, you need to patch the XGM2 driver to allow for RAM streaming. See instruction in [PATCH.md](PATCH.md) - you only need to copy a few files :)

After that just build and run the ROM in an emulator. The rom can be found prebuit in [out/rom.bin](out/rom.bin). Press A to play a regular sound effect, press B to play a sound effect with echo applied.

## Known Issues

### Bus contention

The main design issue currently is the obvious bus contention when reading from RAM. This will cause some audio glitches, but can curreently be worked around by using this feature only during scenes with minimal VDP transfers. The XGM2 driver reads samples in batches so if the VDP transfer is small the chance of audio glitches is low.

The fix for this is to instead manually transfer the stream to a buffer in the Z80 RAM. To do this would require significant updates to the XGM2 driver. The current plan for minimizing this work is to modify the existing XGM2 driver into a variant that disables regular PCM SFX use and instead uses only streaming. In this case SFX mixing would be done on the 68000. With this approach you would use the regular XGM2 driver for standard scenes and the streaming XGM2 driver for scenes requiring real-time audio effects.

### XGM2 RAM addressing limitations

The XGM2 driver normally disables reading from RAM to avoid bus contention issues. For this reason it also does not store the highest address bits. Due to driver internals, this will cause the PCM Stream address to be lost when any non-PCM task (music) starts.

This issue can be patched in the driver, but would also become a non-issue with the above driver rewrite.

### Buffer alignment

There are also issues with buffer alignment (must be 256-byte aligned) and crossing 32k boundaries. This can be solved by manually specifying the address of the byte stream in RAM - see main.c. __TODO__

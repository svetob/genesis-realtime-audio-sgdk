# Patching the XGM2 driver

To allow for streaming PCM audio from RAM to the XGM2 driver on the Z80, we need to patch the driver.

Luckily the steps are simple - Make sure you are using SGDK 2.11, and then copy the following files into your SGDK path:

- `patch/inc/snd/z80_mac.i80` -> `<SGDK_PATH>/inc/snd/z80_mac.i80`
- `patch/src/snd/xgm2/drv_xgm2.s` -> `<SGDK_PATH>/src/snd/xgm2/drv_xgm2.s`
- `patch/src/snd/xgm2/drv_xgm2_pcm_fct.i80` -> `<SGDK_PATH>/src/snd/xgm2/drv_xgm2_pcm_fct.i80`
- `patch/src/snd/xgm2/drv_xgm2.s80` -> `<SGDK_PATH>/src/snd/xgm2/drv_xgm2.s80`

This patch prevents the driver from owerwriting bits 22-23 in the PCM address, allowing for streaming from RAM.

## Rebuilding

If you wish to do more edits to the files above: The XGM2 toolchain does not rebuild the Z80 drivers by default. To rebuild the driver after changes, you need to:

`rm <SGDK_PATH>/src/snd/xgm2/drv_xgm2.s`

You can also add this at the bottom of `makefile.gen` to let make do this:

```
$(SRC_LIB)/snd/xgm2/drv_xgm2.s: $(wildcard $(SRC_LIB)/snd/xgm2/*.i80) $(wildcard $(INCLUDE_LIB)/snd/*.i80)
```
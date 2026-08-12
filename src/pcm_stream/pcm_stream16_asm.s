#include "asm_mac.i"
#include "pcm_stream16_mac.i"

****************************************************************
* Important notes :
*
* m68k calling convention says that, between function calls,
* d0,d1,a0,a1 can be used as scratch registers, however,
* others should be preserved and
* return value -if there is- is placed in d0
*
* However, until confirmed that the compiler really does
* reliably leave d0,d1,a0,a1 as scratch regs, we are backing
* up these as well as extra safety measure.
*
* (%sp) contains returns address, so first argument is at 4(%sp)
*
* Each argument seems to be sent as a long-word (4 bytes)
* regardless of defined size in C. I.e. a u8 is sent
* as 0x000000FF not 0xFF and takes 4 bytes on stack not 1.
****************************************************************

* extern void PCM_STREAM16_renderToOutputBuffer_ASM(s16* render, s8* out)

func    PCM_STREAM16_renderToOutputBuffer_ASM
        movem.l %a0-%a2/%d0,-(sp)

pcms16_renderout_init:
        * renderBuf -> a0
        movea.l 20(%sp),%a0
        * outBuf -> a1
        movea.l 24(%sp),%a1
        
        lea     downscale_table,%a2
        move.l  #0,%d0

pcms16_renderout_loop:
        PCM_STREAM16_renderToOutputBuffer_doProcess256

pcms16_renderout_return:
        movem.l (sp)+,%a0-%a2/%d0
        rts


* extern void PCM_STREAM16_upscaleAndRenderSoundToStream_ASM(s8* pcm, s16* extern void PCM_STREAM16_upscaleAndRenderSoundToStream_ASM(s8 *pcm, s16 *render);

func    PCM_STREAM16_upscaleAndRenderSoundToStream_ASM
        movem.l %a0-%a2/%d0-%d3,-(sp)

pcms16_upscalerender_init:
        * pcmPtr -> a0
        movea.l 32(%sp),%a0
        * renderBuf -> a1
        movea.l 36(%sp),%a1

        lea     upscale_table,%a2

        * pcmSample -> d0
        move.l  #0,%d0
        * upscaledSample -> d1
        move.l  #0,%d1
        * outSample -> d2
        move.l  #0,%d2
        * i -> d3
        move.l  #16,%d3

pcms16_upscalerender_loop:
.L1:
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess16

        subq    #1,%d3
        bne     .L1

pcms16_upscalerender_return:
        movem.l (sp)+,%a0-%a2/%d0-%d3
        rts

#include "asm_mac.i"
#include "xgm2_interface_mac.i"

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
* (sp) contains returns address, so first argument is at 4(sp)
*
* Each argument seems to be sent as a long-word (4 bytes)
* regardless of defined size in C. I.e. a u8 is sent
* as 0x000000FF not 0xFF and takes 4 bytes on stack not 1.
****************************************************************

* extern void XGM2_PCM_mixIntoBuffer(s8* pcm, u8* ringbuf)

func    XGM2_PCM_mixIntoBuffer
        ;movem.l a0-a1/d0,-(sp)

xgm2pcm_mixbuffer_init:
        * pcmBuf  -> a0
        movea.l 16(sp),a0
        * ringBuf -> a1
        movea.l 20(sp),a1

xgm2pcm_mixbuffer_do64:
        xgm2pcm_writebuf_do64

xgm2pcm_mixbuffer_ret:
        ;movem.l (sp)+,a0-a1/d0
        rts
        

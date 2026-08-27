#include "asm_mac.i"
#include "pcm_stream8_mac.i"


* extern void PCM_STREAM8_clear64_ASM(void *buf, u16 len);

func    PCM_STREAM8_clear64_ASM
pcms8_clear_init:
        * bufPtr -> a0
        move.l  4(sp),a0
        * len -> d0
        move.w  10(sp),d0
        * 0 -> d1
        clr.l   d1

pcms8_clear_loop:

.LCLR1:
        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+

        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+

        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+

        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+
        move.l  d1,(a0)+

        subi.w  #64,d0
        bhi     .LCLR1

pcms8_clear_return:
        rts


* extern void PCM_STREAM8_mixAndClip64_ASM(s8 *in, s8 *out, u16 len);

func    PCM_STREAM8_mixAndClip64_ASM
        movem.l a2/d2-d4,-(sp)

pcms8_renderpcm_init:
        * pcmPtr -> a0
        movea.l 20(sp),a0
        * renderBuf -> a1
        movea.l 24(sp),a1

        lea     mixandclip_s8_table,a2

        * inSample -> d0
        move.w  #0,d0
        * outSample -> d1
        move.w  #0,d1
        * mixedSample -> d2
        move.w  #0,d2
        * i -> d3
        move.l  28(sp),d3
        * 0x80 -> d4
        move.b  #0x80,d4

pcms8_renderpcm_loop:
.L1:
        pcms8_renderpcm_do16

        subi.w  #16,d3
        bhi     .L1

pcms8_renderpcm_return:
        movem.l (sp)+,a2/d2-d4
        rts

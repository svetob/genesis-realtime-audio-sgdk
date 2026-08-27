#include "asm_mac.i"
#include "pcmstream_mac.i"


* extern void PCMSTREAM_clear64_ASM(void *buf, u16 len);

func    PCMSTREAM_clear64_ASM
pcmstream_clear_init:
        * bufPtr -> a0
        move.l  4(sp),a0
        * len -> d0
        move.w  10(sp),d0
        * 0 -> d1
        clr.l   d1

pcmstream_clear_loop:

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

pcmstream_clear_return:
        rts


* extern void PCMSTREAM_mixAndClip64_ASM(s8 *in, s8 *out, u16 len);

func    PCMSTREAM_mixAndClip64_ASM
        movem.l a2/d2-d4,-(sp)

pcmstream_renderpcm_init:
        * pcmPtr             -> a0
        movea.l 20(sp),a0
        * renderBuf          -> a1
        movea.l 24(sp),a1
        * mixAndClipTablePtr -> a2
        lea     mixandclip_s8_table,a2

        * inSample           -> d0
        move.w  #0,d0
        * outSample          -> d1
        move.w  #0,d1
        * mixedSample        -> d2
        move.w  #0,d2
        * i                  -> d3
        move.l  28(sp),d3
        * 0x80               -> d4
        move.b  #0x80,d4

pcmstream_renderpcm_loop:
.L1:
        pcmstream_renderpcm_do16

        subi.w  #16,d3
        bhi     .L1

pcmstream_renderpcm_return:
        movem.l (sp)+,a2/d2-d4
        rts

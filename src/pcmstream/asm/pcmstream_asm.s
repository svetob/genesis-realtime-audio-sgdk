#include "asm_mac.i"
#include "pcmstream_mac.i"


.set    MIXCLIP_XL_BIAS                         ,0x4000

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


* extern void PCMSTREAM_sound_raw_playback4_ASM(void* sounds, void* buf, u16 renderLen)

func    PCMSTREAM_sound_raw_playback4_ASM
        movem.l a2-a6/d2-d6,-(sp)

pcmstream_sound_raw_playback_init:
        * soundsPtr        -> a0
        move.l  44(sp),a0
        * bufPtr           -> a1
        move.l  48(sp),a1
        * clipTable        -> a2
        lea     mixandclip_s8_xl_table+MIXCLIP_XL_BIAS,a2
        * snd0             -> a3
        move.l  (a0),a3
        * snd1             -> a4
        move.l  8(a0),a4
        * snd2             -> a5
        move.l  16(a0),a5
        * snd3             -> a6
        move.l  24(a0),a6


        * inSample           -> d0
        move.w  #0,d0
        * outSample          -> d1
        move.w  #0,d1
        * i                  -> d2
        move.l  52(sp),d2
        * remain0            -> d3
        move.l  4(a0),d3

pcmstream_sound_raw_playback_body:
.LPRAW1:
        * Start with just lol-assume-slopping 1 sample and get that to work
        tst     d3
        beq     .LPRAWRET

        pcmstream_renderpcm_1_to_dirty_do64

        subi.w  #64,d2
        bhi     .LPRAW1

pcmstream_sound_raw_playback_return:
.LPRAWRET:
        * pcm0               -> memory
        move.l  a3,(a0)
        * remain0            -> memory
        move.l  d3,4(a0)

        movem.l (sp)+,a2-a6/d2-d6
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

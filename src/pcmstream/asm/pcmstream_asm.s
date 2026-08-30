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
        * remain1            -> d4
        move.l  12(a0),d4
        * remain2            -> d5
        move.l  20(a0),d5
        * remain3            -> d6
        move.l  28(a0),d6

pcmstream_sound_raw_playback_body:
.L_PCMRAW_BODY:
        * If no sample playing, return  TODO: Fill with silence
        tst.l   d3
        beq     .L_PCMRAW_RET

        * If 1 sample playing, play 1 sample
        tst.l   d4
        beq     .L_PCMRAW_BODY_1

        * If 2 samples playing, play 2 samples

pcmstream_sound_raw_playback_body_2pcm:
.L_PCMRAW_BODY_2:
        pcmstream_render2pcm_do64
        subi.l  #64,d3
        subi.l  #64,d4
        bra     .L_PCMRAW_LOOP

pcmstream_sound_raw_playback_body_1pcm:
.L_PCMRAW_BODY_1:
        pcmstream_render1pcm_do64
        subi.l  #64,d3

.L_PCMRAW_LOOP:
        subi.w  #64,d2
        bhi     .L_PCMRAW_BODY

pcmstream_sound_raw_playback_return:

.L_PCMRAW_RET:
        * pcm0               -> memory
        move.l  a3,(a0)
        * remain0            -> memory
        move.l  d3,4(a0)

        * pcm1               -> memory
        move.l  a4,8(a0)
        * remain1            -> memory
        move.l  d4,12(a0)

        * pcm2               -> memory
        move.l  a5,16(a0)
        * remain2            -> memory
        move.l  d5,20(a0)

        * pcm3               -> memory
        move.l  a6,24(a0)
        * remain3            -> memory
        move.l  d6,28(a0)

        movem.l (sp)+,a2-a6/d2-d6
        rts

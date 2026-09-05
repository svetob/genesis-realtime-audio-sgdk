.macro  xgm2pcm_writebuf_movep_do64
        movep.l 0(a0),d0
        eor.l d1,d0
        movep.l d0,0(a1)
        movep.l 1(a0),d0
        eor.l d1,d0
        movep.l d0,1(a1)

        movep.l 8(a0),d0
        eor.l d1,d0
        movep.l d0,8(a1)
        movep.l 9(a0),d0
        eor.l d1,d0
        movep.l d0,9(a1)

        movep.l 16(a0),d0
        eor.l d1,d0
        movep.l d0,16(a1)
        movep.l 17(a0),d0
        eor.l d1,d0
        movep.l d0,17(a1)

        movep.l 24(a0),d0
        eor.l d1,d0
        movep.l d0,24(a1)
        movep.l 25(a0),d0
        eor.l d1,d0
        movep.l d0,25(a1)

        movep.l 32(a0),d0
        eor.l d1,d0
        movep.l d0,32(a1)
        movep.l 33(a0),d0
        eor.l d1,d0
        movep.l d0,33(a1)

        movep.l 40(a0),d0
        eor.l d1,d0
        movep.l d0,40(a1)
        movep.l 41(a0),d0
        eor.l d1,d0
        movep.l d0,41(a1)

        movep.l 48(a0),d0
        eor.l d1,d0
        movep.l d0,48(a1)
        movep.l 49(a0),d0
        eor.l d1,d0
        movep.l d0,49(a1)

        movep.l 56(a0),d0
        eor.l d1,d0
        movep.l d0,56(a1)
        movep.l 57(a0),d0
        eor.l d1,d0
        movep.l d0,57(a1)

        adda.l d2,a0
        adda.l d2,a1
.endm                                           * 912 cycles = 14/samp

* Mix into ring buffer with no overflow protection.

.macro  xgm2pcm_mixbuf_do1
        move.b  (a0)+,d0
        add.b   d0,(a1)+
.endm                                           * 20 cycles

.macro  xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
        xgm2pcm_mixbuf_do1
.endm

.macro  xgm2pcm_mixbuf_do64
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
        xgm2pcm_mixbuf_do8
.endm                                           * 1280 cycles = 20/samp

* Mix into ring buffer with overflow protection through clipping

* TODO - Possible speedup for xgm2pcm_mixbuf_clip_do1:
* 64k LUT for high byte in a word from a0
* Read w from a0 -> write a1 into lower byte -> read from LUT
* Does not work for low byte - must keep above approach there
* Tradeoff probably not worth it - 64kb for saving a few scanlines only

.macro  xgm2pcm_mixbuf_clip_do1
        move.w  d2,d0
        add.b   (a0)+,d0

        move.b  (a1),d1
        add.w   d1,d0

        move.b  (a2,d0.w),(a1)+
.endm                                           * 42 cycles

.macro  xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
        xgm2pcm_mixbuf_clip_do1
.endm

.macro  xgm2pcm_mixbuf_clip_do64
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
        xgm2pcm_mixbuf_clip_do8
.endm                                           * 2688 cycles = 42/samp

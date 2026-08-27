* TODO - Should probably offset the stream buffer from s8 to u8
*        before here. This will give significant speed up in this
*        critical bus-hold code path. In some cases like
*        xgm2pcm_writebuf_do1 the performance gains here would
*        make up for the offset cost.

* Overwrite ring buffer. Cannot yet be used - see above todo.

.macro  xgm2pcm_writebuf_do1
        move.b  (a0)+,(a1)+
.endm                                           * 12 cycles

* Mix into ring buffer with no overflow protection.
* The end result if buffer is clean is equal to overwriting it.

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
.endm                                           * 1280 cycles

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
.endm                                           * 2688 cycles

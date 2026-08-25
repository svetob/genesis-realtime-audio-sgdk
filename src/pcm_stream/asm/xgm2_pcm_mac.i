.macro  xgm2pcm_writebuf_do1
        move.b  (a0)+,d0
        add.b   d0,(a1)+
.endm

.macro  xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
        xgm2pcm_writebuf_do1
.endm   * 20 cycles

.macro  xgm2pcm_writebuf_do64
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
.endm   * 1280 cycles

.macro  xgm2pcm_writebuf_clip_do1
        move.w  d2,d0
        add.b   (a0)+,d0

        move.b  (a1),d1
        add.w   d1,d0

        move.b  (a2,d0.w),(a1)+
.endm   * 42 cycles

.macro  xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
        xgm2pcm_writebuf_clip_do1
.endm

.macro  xgm2pcm_writebuf_clip_do64
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
        xgm2pcm_writebuf_clip_do8
.endm   * 2688 cycles
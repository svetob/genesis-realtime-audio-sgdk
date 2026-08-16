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
.endm

.macro  xgm2pcm_writebuf_do64
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
        xgm2pcm_writebuf_do8
.endm

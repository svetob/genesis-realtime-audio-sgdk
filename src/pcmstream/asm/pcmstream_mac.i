* 4 samples from 1 PCM to 8bit buffer
.macro  pcmstream_render1pcm_do4
        move.l  (a3)+,(a1)+
.endm                                           * 7 cycles per sample

.macro  pcmstream_render1pcm_do64
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4

        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4

        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4

        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
.endm                                           * 448 cycles per chunk

* 1 samples from 2 PCM to 8bit buffer
.macro  pcmstream_render2pcm_do1
        moveq   #0,d1
        move.b  (a3)+,d1

        move.b  (a4)+,d0
        add.w   d0,d1

        move.b  (a2,d1.w),(a1)+
.endm                                           * 46 cycles per sample

.macro  pcmstream_render2pcm_do8
        pcmstream_render2pcm_do1
        pcmstream_render2pcm_do1
        pcmstream_render2pcm_do1
        pcmstream_render2pcm_do1

        pcmstream_render2pcm_do1
        pcmstream_render2pcm_do1
        pcmstream_render2pcm_do1
        pcmstream_render2pcm_do1
.endm

.macro  pcmstream_render2pcm_do64
        pcmstream_render2pcm_do8
        pcmstream_render2pcm_do8
        pcmstream_render2pcm_do8
        pcmstream_render2pcm_do8

        pcmstream_render2pcm_do8
        pcmstream_render2pcm_do8
        pcmstream_render2pcm_do8
        pcmstream_render2pcm_do8
.endm                                           * 448 cycles per chunk

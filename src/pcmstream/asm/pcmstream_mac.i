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
* Uses 16bit register to add samples together then clips to 8bit using a LUT
.macro  pcmstream_render2pcm_do1
        moveq   #0,d1
        move.b  (a3)+,d1

        move.b  (a4)+,d0
        add.w   d0,d1

        * TODO: This should be wrong - it should be after a3 load - but moving it causes artefacts...? Investigate later
        ext.w   d1
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
.endm                                           * 2944 cycles per chunk


* 1 sample from 3 PCM to 8bit buffer
* Uses 16bit register to add samples together then clips to 8bit using a LUT
.macro  pcmstream_render3pcm_do1
        moveq   #0,d1
        move.b  (a3)+,d1

        move.b  (a4)+,d0
        add.w   d0,d1

        move.b  (a5)+,d0
        add.w   d0,d1

        ext.w   d1
        move.b  (a2,d1.w),(a1)+
.endm                                           * 58 cycles per sample

.macro  pcmstream_render3pcm_do8
        pcmstream_render3pcm_do1
        pcmstream_render3pcm_do1
        pcmstream_render3pcm_do1
        pcmstream_render3pcm_do1

        pcmstream_render3pcm_do1
        pcmstream_render3pcm_do1
        pcmstream_render3pcm_do1
        pcmstream_render3pcm_do1
.endm

.macro  pcmstream_render3pcm_do64
        pcmstream_render3pcm_do8
        pcmstream_render3pcm_do8
        pcmstream_render3pcm_do8
        pcmstream_render3pcm_do8

        pcmstream_render3pcm_do8
        pcmstream_render3pcm_do8
        pcmstream_render3pcm_do8
        pcmstream_render3pcm_do8
.endm                                           * 3172 cycles per chunk


* 1 sample from 4 PCM to 8bit buffer
* Uses 16bit register to add samples together then clips to 8bit using a LUT
.macro  pcmstream_render4pcm_do1
        moveq   #0,d1
        move.b  (a3)+,d1

        move.b  (a4)+,d0
        add.w   d0,d1

        move.b  (a5)+,d0
        add.w   d0,d1

        move.b  (a6)+,d0
        add.w   d0,d1

        ext.w   d1
        move.b  (a2,d1.w),(a1)+
.endm                                           * 70 cycles per sample

.macro  pcmstream_render4pcm_do8
        pcmstream_render4pcm_do1
        pcmstream_render4pcm_do1
        pcmstream_render4pcm_do1
        pcmstream_render4pcm_do1

        pcmstream_render4pcm_do1
        pcmstream_render4pcm_do1
        pcmstream_render4pcm_do1
        pcmstream_render4pcm_do1
.endm

.macro  pcmstream_render4pcm_do64
        pcmstream_render4pcm_do8
        pcmstream_render4pcm_do8
        pcmstream_render4pcm_do8
        pcmstream_render4pcm_do8

        pcmstream_render4pcm_do8
        pcmstream_render4pcm_do8
        pcmstream_render4pcm_do8
        pcmstream_render4pcm_do8
.endm                                           * 4480 cycles per chunk

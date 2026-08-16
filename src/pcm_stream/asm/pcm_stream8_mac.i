.macro  pcms8_renderpcm_do1

        move.b  (a0)+,d0
        move.b  (a1),d1

        add.b   d4,d0
        add.b   d4,d1
        
        move.w  d0,d2
        add.w   d1,d2

        move.b  (a2,d2.w),(a1)+

.endm

.macro  pcms8_renderpcm_do16
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1

        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1

        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1

        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
        pcms8_renderpcm_do1
.endm

* 1 PCM to dirty buffer
.macro  pcms8_renderpcm_1_to_dirty_do4
        move.l  (a0)+,(a1+)
.endm   * 7 cycles per sample

.macro  pcms8_renderpcm_1write_do64
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
        pcms8_renderpcm_1_to_dirty_do4
.endm

.macro  pcms8_renderpcm_2write_do1
        move.l  (a0)+,(a1+)
.endm   * 448 cycles per chunk


* 1 PCM to clean buffer
.macro  pcms8_renderpcm_do1

        move.b  (a0)+,d0
        move.b  (a1),d1

        add.b   d4,d0
        add.b   d4,d1
        
        move.w  d0,d2
        add.w   d1,d2

        move.b  (a2,d2.w),(a1)+
.endm   * 50 cycles per sample

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
.endm   * 3200 cycles per chunk
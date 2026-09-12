.macro  afx8_reverb_fixMinusOneDelaySampleD6
        * Fix for 0xFF >> 1 = 0xFF (-1 >> 1 = -1) leaving behind a noise floor in delay line
        move.l  d6,d7
        and.l   #0x80808080,d7
        lsr.l   #7,d7
        add.l   d7,d6
.endm

.macro  afx8_reverb_halveSampleD6
        * Get sign bits into d7
        move.l  d6,d7
        and.l   #0x80808080,d7

        * Halve sample
        asr.l   #1,d6

        * Reattach sign bits
        and.l   #0x7F7F7F7F,d6
        add.l   d7,d6

.endm

.macro  afx8_reverb_halveAndMixDelaySampleD6
        * Get delay line sign bits into d7
        move.l  d6,d7
        and.l   #0x80808080,d7

        * 50% delay line feedback (halve delay line sample)
        asr.l   #1,d6

        * Reattach sign bits
        and.l   #0x7F7F7F7F,d6
        add.l   d7,d6

        afx8_reverb_fixMinusOneDelaySampleD6

        * Add to delay accumulator (without overflow protection)
        add.l   d6,d1
.endm

.macro  afx8_reverb_quarterAndMixDelaySampleD6
        * Get delay line sign bits into d7
        move.l  d6,d7
        and.l   #0x80808080,d7

        * 25% delay line feedback
        asr.l   #2,d6

        * Reattach sign bits
        and.l   #0x3F3F3F3F,d6
        add.l   d7,d6
        lsr.l   #1,d7
        add.l   d7,d6

        afx8_reverb_fixMinusOneDelaySampleD6

        * Add to delay accumulator (without overflow protection)
        add.l   d6,d1
.endm

.macro  afx8_reverb_doProcess4
        moveq.l #0,d1

        * Process delay line sample 1
        move.l  (a1,d2.w),d6
        afx8_reverb_quarterAndMixDelaySampleD6

        * Process delay line sample 2
        move.l  (a1,d3.w),d6
        afx8_reverb_quarterAndMixDelaySampleD6

        * Process delay line sample 3
        move.l  (a1,d4.w),d6
        afx8_reverb_quarterAndMixDelaySampleD6

        * Process delay line sample 4
        move.l  (a1,d5.w),d6
        afx8_reverb_quarterAndMixDelaySampleD6

        * Now mix (without overflow protection)
        move    (a0),d6
        afx8_reverb_halveSampleD6
        add.l   d6,d1

        * Write result in out to delay line
        move.l  d1,(a0)+
        move.l  d1,(a1,d0.w)

        * Increment and wrap line pointers
        addq.w  #4,d0
        addq.w  #4,d2
        addq.w  #4,d3
        addq.w  #4,d4
        addq.w  #4,d5
        move.w  a2,d1
        and.w   d1,d0
        and.w   d1,d2
        and.w   d1,d3
        and.w   d1,d4
        and.w   d1,d5
.endm

.macro  afx8_reverb_doProcess64
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4

        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4

        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4

        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
        afx8_reverb_doProcess4
.endm

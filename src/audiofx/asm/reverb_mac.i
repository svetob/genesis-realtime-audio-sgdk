
.macro  afx8_reverb_halveAndMixDelaySample
        * -- Delay sample is in d6

        * Fix for 0xFF >> 1 = 0xFF (-1 >> 1 = -1) leaving behind a noise floor in delay line
        move.l  d6,d7
        and.l   #0x80808080,d7
        lsr.l   #7,d7
        add.l   d7,d6
        * '60c

        * Get delay line sign bits into d7
        move.l  d6,d7
        and.l   #0x80808080,d7

        * 50% delay line feedback (halve delay line sample)
        asr.l   #1,d6

        * Reattach sign bits
        and.l   #0x7F7F7F7F,d6
        add.l   d7,d6
        * '98c

        * Now mix directly into sample buffer (without overflow protection)
        add.l   d6,(a0)
.endm

.macro  afx8_reverb_doProcess4
        * Process delay line sample 1
        move.l  (a1,d2.w),d6
        afx8_reverb_halveAndMixDelaySample

        * Process delay line sample 2
        move.l  (a1,d3.w),d6
        afx8_reverb_halveAndMixDelaySample

        * Process delay line sample 3
        move.l  (a1,d4.w),d6
        afx8_reverb_halveAndMixDelaySample

        * Process delay line sample 4
        move.l  (a1,d5.w),d6
        afx8_reverb_halveAndMixDelaySample

        * Write result in out to delay line
        move.l  (a0)+,(a1,d0.w)

        * Increment and wrap line pointers
        addq.w  #4,d0
        addq.w  #4,d2
        addq.w  #4,d3
        and.w   d1,d0
        and.w   d1,d2
        and.w   d1,d3
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

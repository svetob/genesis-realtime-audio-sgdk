
.macro  afx8_reverb_doProcess4
        * Read delayed line sample into d6
        move.l  (a1,d2.w),d6
        * '18c

        * -- Mix em together, result in d6

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

        * Now mix (without overflow protection)
        add.l   (a0),d6

        * --Write result to out and line
        move.l  d6,(a0)+
        move.l  d6,(a1,d0.w)
        * '142c

        * Increment and wrap line pointers
        addq.w  #4,d0
        addq.w  #4,d2
        addq.w  #4,d3
        and.w   d1,d0
        and.w   d1,d2
        and.w   d1,d3
.endm                                           * 156 cycles = 39 / sample

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

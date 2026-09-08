
.macro  afx8_echo_doProcess4
        * Read samples into d4
        move.l  (a0),d4

        * Read delayed line sample into d5
        move.l  (a1,d3.w),d5

        * -- Mix em together, result in d5

        * Fix for 0xFF >> 1 = 0xFF (-1 >> 1 = -1) leaving behind a noise floor in delay line
        move.l  d5,d6
        and.l   d7,d6
        lsr.l   #7,d6
        add.l   d6,d5

        * Get delay line sign bits into d6
        move.l  d5,d6
        and.l   d7,d6

        * 50% delay line feedback (halve delay line sample)
        asr.l   #1,d5

        * Reattach sign bits
        andi.l  #0x7F7F7F7F,d5
        add.l   d6,d5

        * Now mix  TODO: Use clipping mixing
        add.l   d5,d4

        * --Write result to out and line
        move.l  d4,(a0)+
        move.l  d4,(a1,d1.w)

        * Increment and wrap line pointers
        addq.w  #4,d1
        addq.w  #4,d3
        and.w   d2,d1
        and.w   d2,d3
.endm                                           * 136 cycles

.macro  afx8_echo_doProcess64
        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4

        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4

        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4

        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4
        afx8_echo_doProcess4
.endm

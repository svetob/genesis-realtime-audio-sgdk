
.macro  afx8_echo_doProcess4
        * Read delayed line sample into d4
        move.l  (a1,d3.w),d4
        * '18c

        * -- Mix em together, result in d4

        * Fix for 0xFF >> 1 = 0xFF (-1 >> 1 = -1) leaving behind a noise floor in delay line
        move.l  d4,d5
        and.l   d7,d5
        lsr.l   #7,d5
        add.l   d5,d4
        * '60c

        * Get delay line sign bits into d5
        move.l  d4,d5
        and.l   d7,d5

        * 50% delay line feedback (halve delay line sample)
        asr.l   #1,d4

        * Reattach sign bits
        and.l   d6,d4
        add.l   d5,d4
        * '98c

        * Now mix (without overflow protection)
        add.l   (a0),d4

        * --Write result to out and line
        move.l  d4,(a0)+
        move.l  d4,(a1,d1.w)
        * '142c

        * Increment and wrap line pointers
        addq.w  #4,d1
        addq.w  #4,d3
        and.w   d2,d1
        and.w   d2,d3
.endm                                           * 156 cycles = 39 / sample

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

* TODO: Test and fix later after reverb

.macro  afx8_echo_feedback_doProcess1
        * Read line sample
        move.b  (a4)+,d4
        * Apply feedback
        move.b  (a2,d5.w),d4

        * Mix
        add.b   (a0),d4
        * '30c

        * Maybe clip
        bvs     .L\@noclip
        smi     d4
        eori.b  #0x80,d4
.L\@noclip
        * '42/54c

        * Write to out
        move.b  d4,(a0)+
        move.b  d4,(a3)+
.endm                                           * '58/70c

.macro  afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess1
        afx8_echo_feedback_doProcess1
        afx8_echo_feedback_doProcess1
        afx8_echo_feedback_doProcess1

        * Increment and wrap
        addq.w  #4,d1
        addq.w  #4,d3
        and.w   d2,d1
        and.w   d2,d3

        * Load line current pos ptr
        lea     (a1,d1.w),a3
        * Load line current pos ptr
        lea     (a1,d3.w),a4
.endm

.macro  afx8_echo_feedback_doProcess64
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4

        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4

        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4

        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
        afx8_echo_feedback_doProcess4
.endm

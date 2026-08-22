
.macro  afx8_echo_doProcess4
* Read samples into d3
        move.l  (a0),d3

* Read line samples into d4
        move.l  (a2),d4

* -- Mix em together, result in d4

* Fix for 0xFF >> 1 = 0xFF (-1 >> 1 = -1) leaving behind a noise floor in delay line
        move.l  d4,d6
        andi.l  #0x80808080,d6
        lsr.l   #7,d6
        add.l   d6,d4
    
* Get delay line sign bits into d6
        move.l  d4,d6
        andi.l  #0x80808080,d6

* 50 delay line feedback (halve delay line sample)
        asr.l   #1,d4

* Reattach sign bits
        andi.l  #0x7F7F7F7F,d4
        add.l   d6,d4

* Now mix  TODO: Use clipping mixing
        add.l   d4,d3

* --Write result to out and line
        move.l  d3,(a0)+
        move.l  d3,(a2)+
.endm

.macro afx8_echo_doProcess64
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
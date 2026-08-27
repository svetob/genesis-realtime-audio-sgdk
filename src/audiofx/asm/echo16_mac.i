
* 80 cycles
.macro  afx16_echo_doProcess
* Read 2 samples into d3
        move.l  (a0),d3
* Read 2 line samples into d4
        move.l  (a2),d4

* -- Mix em together, result in d3

* 50% delay line feedback (halve delay line sample)
        asr.w   #1,d4
        swap    d4
        asr.w   #1,d4
        swap    d4

* Now mix
        add.l   d4,d3

* --Write result to out and line
        move.l  d3,(a0)+
        move.l  d3,(a2)+
.endm

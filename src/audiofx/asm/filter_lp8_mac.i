.macro afx8_filter_lp_doProcess1
        * Read sample
        move.b (a0),d2
        * calc1 = buf0 - buf1
        move.b d0,d4
        sub.b  d1,d4

        * calc2 = fb_int * calc1
        move.b (a2,d4.w),d5
        * calc2 += fb_dec * calc1
        add.b (a3,d4.w),d5

        * in -= buf0
        sub.b  d0,d2
        * in += calc2
        add.b  d5,d2

        * buf0 += f * in
        add.b (a1,d2.w),d0

        * calc1 = buf0 - buf1
        move.b d0,d4
        sub.b  d1,d4

        * buf1 += f * calc1
        add.b (a1,d4.w),d1

        * Output sample (buf1 -> samplePtr++)
        move.b d1,(a0)+
.endm

.macro afx8_filter_lp_doProcess16
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1

        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1

        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1

        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
        afx8_filter_lp_doProcess1
.endm
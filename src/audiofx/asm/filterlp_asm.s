#include "asm_mac.i"
#include "filterlp_mac.i"

* extern void AFX_filter_lp_2pole_resonant_process16_ASM(
*     s8* samples,
*     u16 len,
*     void* mult_table_f_dec,
*     void* mult_table_fb_int,
*     void* mult_table_fb_dec,
*     s8* buf0,
*     s8* buf1
* );

func    AFX_filter_lp_2pole_resonant_process16_ASM
        movem.l a2-a3/d2-d5,-(sp)

afx8_filter_lp_2poleres_init:
        * *buf0 -> *a0 -> d0
        movea.l 48(sp),a0
        move.b  (a0),d0
        * *buf1 -> *a0 -> d1
        movea.l 52(sp),a0
        move.b  (a0),d1

        * samplePtr -> a0
        movea.l 28(sp),a0
        * mult_f_dec -> a1
        movea.l 36(sp),a1
        * mult_fb_int -> a2
        movea.l 40(sp),a2
        * mult_fb_dec -> a3
        movea.l 44(sp),a3

        * in -> d2
        clr.l   d2
        * len -> d3
        move.l  32(sp),d3
        * calc1 -> d4
        clr.l   d4
        * calc2 -> d5
        clr.l   d5

afx8_filter_lp_2poleres_loop:

.L2R1:
        afx8_filter_lp_2pole_resonant_doProcess16

afx8_filter_lp_2poleres_loop_dbra:
        sub.w   #16,d3
        bne     .L2R1

afx8_filter_lp_2poleres_ret:
        * Write buf0 back to memory
        movea.l 48(sp),a0
        move.b  d0,(a0)
        * Write buf1 back to memory
        movea.l 52(sp),a0
        move.b  d1,(a0)

        movem.l (sp)+,a2-a3/d2-d5
        rts


* extern void AFX_filter_lp_2pole_process16_ASM(
*     s8* samples,
*     u16 len,
*     void* mult_table_f_dec,
*     s8* buf0,
*     s8* buf1
* );

func    AFX_filter_lp_2pole_process16_ASM
        movem.l d2-d4,-(sp)

afx8_filter_lp_2pole_init:
        * *buf0 -> *a0 -> d0
        movea.l 28(sp),a0
        move.b  (a0),d0
        * *buf1 -> *a0 -> d1
        movea.l 32(sp),a0
        move.b  (a0),d1

        * samplePtr -> a0
        movea.l 16(sp),a0
        * mult_f_dec -> a1
        movea.l 24(sp),a1

        * in -> d2
        clr.l   d2
        * len -> d3
        move.l  20(sp),d3
        * calc1 -> d4
        clr.l   d4

afx8_filter_lp_2pole_loop:

.L21:
        afx8_filter_lp_2pole_doProcess16

afx8_filter_lp_2pole_loop_dbra:
        sub.w   #16,d3
        bne     .L21

afx8_filter_lp_2pole_ret:
        * Write buf0 back to memory
        movea.l 28(sp),a0
        move.b  d0,(a0)
        * Write buf1 back to memory
        movea.l 32(sp),a0
        move.b  d1,(a0)

        movem.l (sp)+,d2-d4
        rts




* extern void AFX_filter_lp_1pole_process64_ASM(
*     s8* samples,
*     u16 len,
*     void* mult_table_f_dec,
*     s8* buf0
* );

func    AFX_filter_lp_1pole_process64_ASM
        move.l  d2,-(sp)

afx8_filter_lp_1pole_init:
        * *buf0 -> *a0 -> d0
        movea.l 20(sp),a0
        move.b  (a0),d0

        * samplePtr -> a0
        movea.l 8(sp),a0
        * mult_f_dec -> a1
        movea.l 16(sp),a1

        * in -> d2
        clr.l   d2
        * len -> d1
        move.l  12(sp),d1

afx8_filter_lp_1pole_loop:

.L11:
        afx8_filter_lp_1pole_doProcess64

afx8_filter_lp_1pole_loop_dbra:
        sub.w   #64,d1
        bne     .L11

afx8_filter_lp_1pole_ret:
        * Write buf0 back to memory
        movea.l 28(sp),a0
        move.b  d0,(a0)

        move.l  (sp)+,d2
        rts

#include "asm_mac.i"

* extern void AFX8_filter_lp_process256_ASM(
*     s8* samples,
*     void* mult_table_f_dec,
*     void* mult_table_fb_int,
*     void* mult_table_fb_dec,
*     s8* buf0,
*     s8* buf1
* ); 

func    AFX8_filter_lp_process256_ASM
        movem.l a2-a3/d2-d6,-(sp)

afx8_filter_lp_init:
        * *buf0 -> *a0 -> d0
        movea.l 48(sp),a0
        move.b (a0),d0
        * *buf0 -> *a0 -> d1
        movea.l 52(sp),a0
        move.b (a0),d1

        * samplePtr -> a0
        movea.l 32(sp),a0
        * mult_f_dec -> a1
        movea.l 36(sp),a1
        * mult_fb_int -> a2
        movea.l 40(sp),a2
        * mult_fb_dec -> a3
        movea.l 44(sp),a3

        * in -> d2
        clr.l d2
        * i -> d3
        move.w #256,d3
        * calc1 -> d4
        clr.l d4
        * calc2 -> d5
        clr.l d5
        * calc3 -> d6
        clr.l d6

afx8_filter_lp_loop:

.L1:
        * Read sample
        move.b (a0),d2
        * calc1 = buf0 - buf1
        move.b d0,d4
        sub.b  d1,d4

        * calc2 = fb_int * calc1
        move.b (a2,d4.w),d5
        * calc3 = fb_dec * calc1
        move.b (a3,d4.w),d6
        * calc2 = calc2 + calc3
        add.b  d6,d5
        *add.b  (a3,d4.w),d5 <-- Optimisation for later

        * calc1 = in - buf0 + calc2
        move.b d2,d4
        sub.b  d0,d4
        add.b  d5,d4

        * calc2 = f * calc1
        move.b (a1,d4.w),d5

        * buf0 += calc2
        add.b d5,d0

        * calc1 = buf0 - buf1
        move.b d0,d4
        sub.b  d1,d4

        * calc2 = f * calc1
        move.b (a1,d4.w),d5

        * buf1 += calc2
        add.b  d5,d1

        * Output sample (buf1 -> samplePtr++)
        move.b d1,(a0)+
    
afx8_filter_lp_loop_bne:
        sub.w  #1,d3
        bne     .L1

afx8_filter_lp_ret:
        * Write buf0 back to memory
        movea.l 48(sp),a0
        move.b  d0,(a0)
        * Write buf1 back to memory
        movea.l 52(sp),a0
        move.b  d1,(a0)

        movem.l (sp)+,a2-a3/d2-d6
        rts

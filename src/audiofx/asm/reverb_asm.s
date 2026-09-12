#include "asm_mac.i"
#include "reverb_mac.i"


* extern void AFX_reverb_process(
*     s8* samples,
*     u16 len,
*     AFXEcho *afx
* );

func    AFX_reverb_process
        movem.l a2/d2-d7,-(sp)

afx_reverb_init:
        * samplesPtr   -> a0
        movea.l 32(sp),a0
        * afxPtr       -> a1
        movea.l 40(sp),a1

        * lineMaskTemp -> d1
        move.w  4(a1),d1
        subq.w  #1,d1

        * lineMaskPtr  -> a2
        move.w  4(a1),d1
        subq.w  #1,d1
        lea     -2(sp),a2
        move.w  d1,(a2)

        * linePos      -> d0
        move.w  6(a1),d0

        * delayPos1    -> d2
        move.l  d0,d2
        sub.w   8(a1),d2
        and.w   d1,d2
        * delayPos2    -> d3
        move.l  d0,d3
        sub.w   10(a1),d3
        and.w   d1,d3
        * delayPos3    -> d4
        move.l  d0,d4
        sub.w   12(a1),d4
        and.w   d1,d4
        * delayPos4    -> d5
        move.l  d0,d5
        sub.w   14(a1),d5
        and.w   d1,d5

        * delayAccum   -> d1

        * calc0        -> d6

        * calc1        -> d7

        * delayLinePtr -> a1
        movea.l (a1),a1

        * -- Result of init is: --
        * samplesPtr   -> a0
        * delayLinePtr -> a1
        * lineMaskPtr  -> a2

        * linePos      -> d0
        * delayAccum   -> d1
        * delayPos1    -> d2
        * delayPos2    -> d3
        * delayPos3    -> d4
        * delayPos4    -> d5
        * calc0        -> d6
        * calc1        -> d7

afx_reverb_loop:

.L1:
        afx8_reverb_doProcess64

        * Loop
        subi.w  #64,38(sp)
        bne     .L1

afx_reverb_return:
        * Write back pos
        movea.l 40(sp),a1
        move.w  d0,6(a1)

        movem.l (sp)+,a2/d2-d7
        rts

#include "asm_mac.i"
#include "reverb_mac.i"


* extern void AFX_reverb_process(
*     s8* samples,
*     u16 len,
*     AFXEcho *afx
* );

func    AFX_reverb_process
        movem.l d2-d7,-(sp)

afx_reverb_init:
        * samplesPtr   -> a0
        movea.l 28(sp),a0
        * afxPtr       -> a1
        movea.l 36(sp),a1

        * linePos      -> d0
        move.w  6(a1),d0
        * lineMask     -> d1
        move.w  4(a1),d1
        subq.w  #1,d1
        * delayPos1    -> d2
        move.l  d0,d2
        sub.w   8(a1),d2
        and.w   d1,d2
        * delayPos2    -> d3
        move.l  d0,d3
        sub.w   10(a1),d3
        and.w   d1,d3



        * calc0        -> d6

        * calc1        -> d7

        * linePtr      -> a1
        movea.l (a1),a1

afx_reverb_loop:

.L1:
        afx8_reverb_doProcess64

        * Loop
        subi.w  #64,34(sp)
        bne     .L1

afx_reverb_return:
        * Write back pos
        movea.l 36(sp),a1
        move.w  d0,6(a1)

        movem.l (sp)+,d2-d7
        rts

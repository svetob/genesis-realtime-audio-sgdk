#include "asm_mac.i"
#include "echo_mac.i"


* extern void AFX_echo_process(
*     s8* samples,
*     u16 len,
*     AFXEcho *afx
* );

func    AFX_echo_process
        movem.l d2-d7,-(sp)

afx_echo_init:
        * samplesPtr   -> a0
        movea.l 28(sp),a0
        * afxPtr       -> a1
        movea.l 36(sp),a1

        * samplesLen   -> d0
        move.l  32(sp),d0
        * linePos      -> d1
        move.w  8(a1),d1
        * lineMask     -> d2
        move.w  4(a1),d2
        subq.w  #1,d2
        * linePosDelay -> d3
        move.l  d1,d3
        sub.w   6(a1),d3
        and.w   d2,d3

        * linePtr      -> a1
        movea.l (a1),a1

        * calc0        -> d4

        * calc1        -> d5

        * 0x0x7F7F7F7F -> d6
        move.l  #0x7F7F7F7F,d6
        * 0x80808080   -> d7
        move.l  #0x80808080,d7

afx_echo_loop:

.L1:
        afx8_echo_50fb_doProcess64

        * Loop
        subi.w  #64,d0
        bne     .L1

afx_echo_return:
        * Write back pos
        movea.l 36(sp),a1
        move.w  d1,8(a1)

        movem.l (sp)+,d2-d7
        rts

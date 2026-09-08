#include "asm_mac.i"
#include "echo_mac.i"


* extern void AFX_echo_process64_ASM(
*     s8* samples,
*     u16 size,
*     s8* delay_line,
*     u16 pos,
*     u16 len,
*     u16 delay,
* );

func    AFX_echo_process64_ASM
        movem.l a2/d2-d7,-(sp)

afx_echo_init:
        * samplesPtr   -> a0
        movea.l 32(sp),a0
        * linePtr      -> a1
        movea.l 40(sp),a1

        * size         -> d0
        move.l  36(sp),d0
        * linePos      -> d1
        move.l  44(sp),d1
        * lineMask     -> d2
        move.l  48(sp),d2
        subq.l  #1,d2
        * linePosDelay -> d3
        move.l  d1,d3
        sub.l   52(sp),d3
        and.l   d2,d3
        * calc0        -> d4

        * calc1        -> d5

        * calc2        -> d6

        * 0x80808080   -> d7
        move.l  #0x80808080,d7

afx_echo_loop:

.L1:
        afx8_echo_doProcess64

        * Loop
        subi.w  #64,d0
        bne     .L1

afx_echo_return:
        movem.l (sp)+,a2/d2-d7
        rts

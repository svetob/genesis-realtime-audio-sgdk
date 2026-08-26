#include "asm_mac.i"
#include "echo8_mac.i"


* extern void AFX8_echo_process64_ASM(
*     s8* samples,
*     u16 size,
*     s8* delay_line,
*     u16 pos,
*     u16 len
* );

func    AFX8_echo_process64_ASM
        movem.l a0-a2/d0-d6,-(sp)

afx_echo_init:
* samplesPtr -> a0
        movea.l 44(sp),a0
* linePtr    -> a1
        movea.l 52(sp),a1

* size       -> s0
        move.l  48(sp),d0
* linePos        -> s0
        move.l  56(sp),d1
* lineLen        -> s0
        move.l  60(sp),d2


* linePtrAt  -> a2
        move.l  a1,a2
        add.l   d1,a2

* linePtrEnd -> d5
        move.l  a1,d5
        add.l   d2,d5


afx_echo_loop:

.L1:
        afx8_echo_doProcess64

        subi.w  #64,d0
        bne     .L1

afx_echo_return:
        movem.l (sp)+,a0-a2/d0-d6
        rts

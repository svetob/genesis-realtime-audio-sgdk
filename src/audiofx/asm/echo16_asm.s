#include "asm_mac.i"
#include "echo16_mac.i"


* extern void AFX16_echo_process256_ASM(
*     s8* samples,
*     u16 size,
*     s8* delay_line,
*     u16 pos,
*     u16 len
* );

func    AFX16_echo_process256_ASM
        movem.l a0-a2/d0-d6,-(sp)

afx_echo_init:
* samplesPtr -> a0
        movea.l 44(sp),a0
* linePtr    -> a1
        movea.l 52(sp),a1

* size       -> s0
        move.l  48(sp),d0
        lsr.w   #1,d0
* pos        -> s0
        move.l  56(sp),d1
* len        -> s0
        move.l  60(sp),d2


* linePtrAt  -> a2
        move.l  a1,a2
        add.l   d1,a2

* linePtrEnd -> d5
        move.l  a1,d5
        add.l   d2,d5


afx_echo_loop:

.L1:
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess
        afx16_echo_doProcess

.L2:
        subi.w  #16,d0
        bne     .L1



afx_echo_return:
        movem.l (sp)+,a0-a2/d0-d6
        rts

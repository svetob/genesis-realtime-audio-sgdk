#include "asm_mac.i"

****************************************************************
* Important notes :
*
* m68k calling convention says that, between function calls,
* d0,d1,a0,a1 can be used as scratch registers, however,
* others should be preserved and
* return value -if there is- is placed in d0
*
* However, until confirmed that the compiler really does
* reliably leave d0,d1,a0,a1 as scratch regs, we are backing
* up these as well as extra safety measure.
*
* (%sp) contains returns address, so first argument is at 4(%sp)
*
* Each argument seems to be sent as a long-word (4 bytes)
* regardless of defined size in C. I.e. a u8 is sent
* as 0x000000FF not 0xFF and takes 4 bytes on stack not 1.
****************************************************************


* extern void AFX_echo_process_ASM(
*     s8* samples,
*     u16 size,
*     s8* delay_line,
*     u16 pos,
*     u16 len
* ); 

func AFX_echo_process_ASM
    movem.l %a0-%a2/%d0-%d5, -(sp)

afx_echo_init:
    * samplesPtr -> a0
    movea.l 40(%sp), %a0
    * linePtr    -> a1
    movea.l 48(%sp), %a1

    * size       -> s0
    move.l 44(%sp), %d0
    * pos       -> s0
    move.l 52(%sp), %d1
    * len       -> s0
    move.l 56(%sp), %d2


    * linePtrAt -> a2
    move.l %a1, %a2
    add.l  %d1, %a2

    * linePtrEnd -> d5
    move.l %a1, %d5
    add.l  %d2, %d5


afx_echo_loop:
    
.L1:
    * Read sample into d3
    move.b (%a0), %d3

    * Read line sample into d4
    move.b (%a2), %d4

    * Mix em together, result in d4
    addq #1, %d4
    asr.b #1, %d4
    add.b %d4, %d3

    * Write result to out and line
    move.b %d3, (%a0)+
    move.b %d3, (%a2)+

    * Wrap delay_line
    cmp.l %a2, %d5
    bne .L2
    move.l %a1, %a2
.L2:

    * Loops if size > 0
    subq.w #1, %d0
    bne .L1
    
    

afx_echo_return:
    movem.l (sp)+, %a0-%a2/%d0-%d5
    rts

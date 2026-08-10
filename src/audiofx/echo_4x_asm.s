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


* extern void AFX_echo_process_4x_ASM(
*     s8* samples,
*     u16 size,
*     s8* delay_line,
*     u16 pos,
*     u16 len
* ); 

func AFX_echo_process_4x_ASM
    movem.l %a0-%a2/%d0-%d6, -(sp)

afx_echo_4x_init:
    * samplesPtr -> a0
    movea.l 44(%sp), %a0
    * linePtr    -> a1
    movea.l 52(%sp), %a1

    * size       -> s0
    move.l 48(%sp), %d0
    lsr.w #2, %d0
    * pos       -> s0
    move.l 56(%sp), %d1
    * len       -> s0
    move.l 60(%sp), %d2


    * linePtrAt -> a2
    move.l %a1, %a2
    add.l  %d1, %a2

    * linePtrEnd -> d5
    move.l %a1, %d5
    add.l  %d2, %d5


afx_echo_4x_loop:
    
.L1:
    * Read samples into d3
    move.l (%a0), %d3

    * Read line samples into d4
    move.l (%a2), %d4

    * -- Mix em together, result in d4

    * Fix for 0xFF >> 1 = 0xFF (-1 >> 1 = -1) leaving behind a noise floor in delay line
    move.l %d4, %d6
    andi.l #0x80808080, %d6
    lsr.l #7, %d6
    add.l %d6, %d4
    
    * Get delay line sign bits into d6
    move.l %d4, %d6
    andi.l #0x80808080, %d6

    * 50% delay line feedback (halce delay line sample)
    asr.l #1, %d4

    * Reattach sign bits
    andi.l #0x7F7F7F7F, %d4
    add.l %d6, %d4

    * Now mix
    add.l %d4, %d3

    * --Write result to out and line
    move.l %d3, (%a0)+
    move.l %d3, (%a2)+

    * Wrap delay_line
    cmp.l %a2, %d5
    bne .L2
    move.l %a1, %a2
.L2:

    * Loops if size > 0
    subq.w #1, %d0
    bne .L1
    
    

afx_echo_4x_return:
    movem.l (sp)+, %a0-%a2/%d0-%d6
    rts

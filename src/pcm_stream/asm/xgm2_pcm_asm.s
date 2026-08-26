#include "asm_mac.i"
#include "xgm2_pcm_mac.i"

* extern u16 XGM2_PCM_SYS_disableInts_fast_noStack()

func    XGM2_PCM_SYS_disableInts_fast_noStack
        move.w  sr,d0                           // d0 = previous SR value
        move.w  #0x2700,sr                      // d1 = value
        rts

* extern void XGM2_PCM_SYS_enableInts_fast_noStack(u16 intsPrev)

func    XGM2_PCM_SYS_enableInts_fast_noStack
        move.w  6(%sp),sr
        rts

* ORIGINAL:
*
* func SYS_getAndSetInterruptMaskLevel
*    move.w  6(%sp),d1                       // d1 = value
*    andi.w  #0x07,%d1
*    ori.w   #0x20,%d1
*    lsl.w   #8,%d1
*
*    move.w  %sr,%d0                         // d0 = previous SR value
*    move.w  %d1,%sr                         // SR = d1 (these 2 instructions should be canonical)
*
*    lsr.w   #8,%d0
*    andi.w  #0x07,%d0                       // d0 = previous interrupt mask
*    rts



* extern void XGM2_PCM_mixIntoRingBuffer_ASM(s8* pcm, vu8* ringbuf)

func    XGM2_PCM_mixIntoRingBuffer_ASM

xgm2pcm_mixbuffer_init:
        * pcmBuf  -> a0
        movea.l 4(sp),a0
        * ringBuf -> a1
        movea.l 8(sp),a1

xgm2pcm_mixbuffer_do64:
        xgm2pcm_writebuf_do64

xgm2pcm_mixbuffer_ret:
        rts


* extern void XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(
*     s8* pcmBuf,
*     vu8* ringBuf,
*     u16 pcmPos,
*     u8 ringPosWrite,
*     u8 ringPosStop
* )

func    XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM
        movem.l a2/d2-d6,-(sp)

xgm2pcm_mixbuffer_clip_init:
        *                                       // pcmBuf        -> a0
        *                                       // ringBuf       -> a1
        lea     mixandclip_u8_table,a2          // LUT           -> a2


        moveq.l #0,d0                           // pcmSample     -> d0
        moveq.l #0,d1                           // ringBufSample -> d1
        move.w  #0x0080,d2                      // S8toU8const   -> d2
        move.l  36(sp),d3                       // pcmPos        -> d3
        move.l  40(sp),d4                       // ringPosWrite  -> d
        move.w  46(sp),d5                       // ringPosStop   -> d5
        moveq   #3,d6                           // maxIters      -> d6

xgm2pcm_mixbuffer_clip_do64:
.L1:
        movea.l 28(sp),a0
        adda.l  d4,a0
        movea.l 32(sp),a1                       // TODO Make const instead of arg
        adda.l  d5,a1

        xgm2pcm_writebuf_clip_do64

xgm2pcm_mixbuffer_clip_loop:
        subq.b  #1,d6
        beq     .L2                             // Exit loop after max iterations

        addi.w  #0x40,d3                        // Increment and wrap pcmPos
        andi.w  #0x01FF,d3

        addi.b  #0x40,d4                        // Increment and wrap ringPosWrite

        cmp     d4,d5                           // Exit loop when ringPosStop reached
        bne     .L1


xgm2pcm_mixbuffer_clip_ret:
.L2:
        movem.l (sp)+,a2/d2-d6
        rts

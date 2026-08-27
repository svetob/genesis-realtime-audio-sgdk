#include "asm_mac.i"
#include "xgm2pcm_mac.i"

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
        xgm2pcm_mixbuf_do64

xgm2pcm_mixbuffer_ret:
        rts


* extern void XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM(
*     s8* pcmBuf,
*     XGM2PCMMixerStatus *status
* )

func    XGM2_PCM_mixIntoRingBuffer_withOverflowProtection_ASM
        movem.l a2-a3/d2-d6,-(sp)

xgm2pcm_mixbuffer_clip_init:
        moveq.l #0,d3
        moveq.l #0,d4
        moveq.l #0,d5

        * pcmBuf         -> a0

        * ringBuf        -> a1

        * LUT            -> a2
        lea     mixandclip_u8_table,a2
        * mixerStatusPtr -> a3
        move.l  36(sp),a3

        * pcmSample      -> d0
        moveq.l #0,d0
        * ringBufSample  -> d1
        moveq.l #0,d1
        * S8toU8const    -> d2
        move.w  #0x0080,d2
        * pcmPos         -> d3
        move.w  (a3),d3
        * ringPosWrite   -> d4
        move.b  2(a3),d4
        * ringPosStop    -> d5
        movea.l #0x00A001F8,a0
        move.b  (a0),d5
        * maxIters       -> d6
        moveq   #3,d6

xgm2pcm_mixbuffer_clip_do64:
.L1:
        movea.l 32(sp),a0
        adda.l  d3,a0
        * TODO Read from some existing const
        movea.l #0x00a01900,a1
        adda.l  d4,a1

        xgm2pcm_mixbuf_clip_do64

xgm2pcm_mixbuffer_clip_loop:
        addi.w  #0x40,d3                        // Increment and wrap pcmPos
        andi.w  #0x01FF,d3

        subq.b  #1,d6                           // Exit loop after max iterations
        beq     .L2

        addi.b  #0x40,d4                        // Increment and wrap ringPosWrite

        cmp     d4,d5                           // Exit loop when ringPosStop reached
        bne     .L1


xgm2pcm_mixbuffer_clip_ret:
.L2:
        move.w  d3,(a3)                         // Write back pcmPos into memory
        move.b  d4,2(a3)                        // Write back ringPosPrev into memory

        movem.l (sp)+,a2-a3/d2-d6
        rts


* extern void XGM2_PCM_overwriteRingBuffer_ASM(
*     s8* pcmBuf,
*     XGM2PCMMixerStatus *status
* )

func    XGM2_PCM_overwriteRingBuffer_ASM
        movem.l a2-a3/d2-d6,-(sp)

xgm2pcm_overwritebuffer_init:
        moveq.l #0,d3
        moveq.l #0,d4
        moveq.l #0,d5

        * pcmBuf         -> a0

        * ringBuf        -> a1

        * mixerStatusPtr -> a3
        move.l  36(sp),a3

        * pcmSample      -> d0
        moveq.l #0,d0
        * pcmPos         -> d3
        move.w  (a3),d3
        * ringPosWrite   -> d4
        move.b  2(a3),d4
        * ringPosStop    -> d5
        movea.l #0x00A001F8,a0
        move.b  (a0),d5
        * maxIters       -> d6
        moveq   #3,d6

xgm2pcm_overwritebuffer_do64:
.L21:
        movea.l 32(sp),a0
        adda.l  d3,a0
        * TODO Read from some existing const
        movea.l #0x00a01900,a1
        adda.l  d4,a1

        xgm2pcm_mixbuf_do64

xgm2pcm_overwritebuffer_loop:
        addi.w  #0x40,d3                        // Increment and wrap pcmPos
        andi.w  #0x01FF,d3

        subq.b  #1,d6                           // Exit loop after max iterations
        beq     .L22

        addi.b  #0x40,d4                        // Increment and wrap ringPosWrite

        cmp     d4,d5                           // Exit loop when ringPosStop reached
        bne     .L21


xgm2pcm_overwritebuffer_ret:
.L22:
        move.w  d3,(a3)                         // Write back pcmPos into memory
        move.b  d4,2(a3)                        // Write back ringPosPrev into memory

        movem.l (sp)+,a2-a3/d2-d6
        rts

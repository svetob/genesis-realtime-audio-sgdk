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
*    move.w  6(%sp),d1                      // d1 = value
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



* extern void XGM2_PCM_mixIntoRingBuffer(s8* pcm, vu8* ringbuf)

func    XGM2_PCM_mixIntoRingBuffer

xgm2pcm_mixbuffer_init:
        * pcmBuf  -> a0
        movea.l 4(sp),a0
        * ringBuf -> a1
        movea.l 8(sp),a1

xgm2pcm_mixbuffer_do64:
        xgm2pcm_writebuf_do64

xgm2pcm_mixbuffer_ret:
        rts


* extern void XGM2_PCM_mixIntoRingBuffer_withOverflowProtection(s8* pcm, vu8* ringbuf)

func    XGM2_PCM_mixIntoRingBuffer_withOverflowProtection
        movem.l a2/d2,-(sp)

xgm2pcm_mixbuffer_clip_init:
        * pcmBuf  -> a0
        movea.l 12(sp),a0
        * ringBuf -> a1
        movea.l 16(sp),a1
        * LUT     -> a2
        lea     mixandclip_u8_table,a2

        * pcmSample     -> d0
        moveq.l #0,d0
        * ringBufSample -> d1
        moveq.l #0,d1
        * S8toU8const   -> d2
        move.w  #0x0080,d2

xgm2pcm_mixbuffer_clip_do64:
        xgm2pcm_writebuf_clip_do64

xgm2pcm_mixbuffer_clip_ret:
        movem.l (sp)+,a2/d2
        rts

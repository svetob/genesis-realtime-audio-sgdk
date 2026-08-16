#include "asm_mac.i"
#include "xgm2_pcm_mac.i"


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
        

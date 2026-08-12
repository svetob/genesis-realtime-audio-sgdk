.macro  PCM_STREAM16_renderToOutputBuffer_doProcess1
        move.w  (%a0)+,%d0
        move.b  (%a2,%d0.l),(%a1)+
.endm

.macro  PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1

        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1

        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1

        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
        PCM_STREAM16_renderToOutputBuffer_doProcess1
.endm

.macro  PCM_STREAM16_renderToOutputBuffer_doProcess256
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16

        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16

        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16

        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
        PCM_STREAM16_renderToOutputBuffer_doProcess16
.endm



.macro  PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        move.b  (%a0)+,%d0
        lsl.w   #1,%d0
        move.w  (%a2,d0.l),%d1
        move.l  #0,%d0

        move.w  (%a1),%d2
        add.w   %d1,%d2
        move.w  %d2,(%a1)+
.endm

.macro  PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess16
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1

        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1

        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1

        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
        PCM_STREAM16_upscaleAndRenderSoundToStream_doProcess1
.endm

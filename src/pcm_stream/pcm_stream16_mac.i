.macro  PCM_STREAM16_renderToOutputBuffer_doProcess16
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+

        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+

        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+

        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
        move.w  (%a0)+,%d1
        move.b  (%a2,%d1.l),(%a1)+
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

# ASM Conventions / Style

## Style example

```
* extern void PCM_STREAM16_renderToOutputBuffer_ASM(s16* render, s8* out)

func    PCM_STREAM16_renderToOutputBuffer_ASM
        movem.l a0-a2/d0,-(sp)

pcms16_renderout_init:
        * renderBuf -> a0
        movea.l 20(sp),a0
        * outBuf -> a1
        movea.l 24(sp),a1
        
        lea     downscale_table,a2
        move.l  #0,d0

pcms16_renderout_loop:
        PCM_STREAM16_renderToOutputBuffer_doProcess256

pcms16_renderout_return:
        movem.l (sp)+,a0-a2/d0
        rts
```

## Function definitions

Always include the C extern function definition as comment
before the ASM function. This explains it purpose and input
arguments.

## Calling conventions

m68k calling convention says that, between function calls,
d0,d1,a0,a1 can be used as scratch registers, however,
others should be preserved, and return value -if there is
one- is placed in d0.

However, until confirmed that the compiler really does
reliably leave d0,d1,a0,a1 as scratch regs in all scenarios,
we are backing up these as well as extra safety measure.
Exceptions are performance critical functions where the
backup removal has been carefully tested.

(sp) contains returns address, so first argument is at 4(sp)

Each argument seems to be sent as a long-word (4 bytes)
regardless of defined size in C. I.e. a u8 is sent
as 0x000000FF not 0xFF and takes 4 bytes on stack not 1.

## Macros

Macros are mainly used to make ASM code more readable, by
grouping repeated blocks into one macro etc.

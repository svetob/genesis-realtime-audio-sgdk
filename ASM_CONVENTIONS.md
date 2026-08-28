# ASM Conventions

## Style conventions

```
* extern void PCMSTREAM_mixAndClip64_ASM(s8 *in, s8 *out, u16 len);

func    PCMSTREAM_mixAndClip64_ASM
        movem.l a2/d2-d4,-(sp)

pcmstream_renderpcm_init:
        * pcmPtr             -> a0
        movea.l 20(sp),a0
        * renderBuf          -> a1
        movea.l 24(sp),a1
        * mixAndClipTablePtr -> a2
        lea     mixandclip_s8_table,a2

        * inSample           -> d0
        move.w  #0,d0
        * outSample          -> d1
        move.w  #0,d1
        * mixedSample        -> d2
        move.w  #0,d2
        * i                  -> d3
        move.l  28(sp),d3
        * 0x80               -> d4
        move.b  #0x80,d4

pcmstream_renderpcm_loop:
.L1:
        pcmstream_renderpcm_do16

        subi.w  #16,d3
        bhi     .L1

pcmstream_renderpcm_return:
        movem.l (sp)+,a2/d2-d4
        rts

```

### Function definitions

__Always__ include the C extern function definition as comment
before the ASM function. This explains its purpose and input
arguments.

### Macros

Macros are mainly used to make ASM code more readable, by
grouping repeated blocks into one macro etc.

### Comments

While comments can be placed after lines like so, according
to standard ASM conventions:

```
        movem.l a0-a2/d0,-(sp)            * Back up values
        clr     d0                        | Clear register
        move    (a1),d0                   // Read from memory
```

...this unfortunely breaks some quite useful VSCode ASM
extensions, such as cycle counters.

So instead, comments __must__ be placed on their own lines,
and __must__ use `*` as prefix:

```
        * Back up values
        movem.l a0-a2/d0,-(sp)
        * Clear register
        clr     d0
        * Read from memory
        move    (a1),d0
```

### Labels

When debugging, it is very common to use symbols.txt to find
the location of problematic code, by looking up the label for
that code section.

For this to be possible, each section of each function __must__
be prefixed with a descriptive label, such as:

```
pcms16_renderout_loop:
```

See the top style example for a clear example.

## Code conventions

### Calling conventions

m68k calling convention says that, between function calls,
d0,d1,a0,a1 can be used as scratch registers, however,
others should be preserved, and return value -if there is
one- is placed in d0. This convention seems to always hold,
so we don't back up d0,d1,a0,a1.

(sp) contains returns address, so first argument is at 4(sp)

Each argument is sent as a long-word (4 bytes) regardless of
defined size in C. I.e. a u8 is sent as 0x00000010 not 0x10
and takes 4 bytes on stack not 1. The bit extensions
necessary for this are costly on the Genesis, hence it is
often worth sending pointers to u8/u16 structs or values
instead of passing them directly.

### Function structure

Functions should have clear init, body, and return blocks.
This is for readability and clarity.

Exceptions can be made if there is cycles to gain, or registers
to save, by using a different structure. In this case you should
still clearly comment in the init block which registers contain
which value(s).

### Cycle counts

For performance critical code (such as loops), it is common
to revisit performance vs space/register/function parameter
tradeoffs. For this to be possible, all performance critical
code blocks __must__ be annotated with an exact cycle count
of the full block.

### Loop unrolls

Loop unrolls are used basically everywhere. To make reading
and editing them easier, unrolled loops should use macros in
separate `_mac.i` files. The body should be its own macro, then
use another macro to repeat the body. The macro name should
contain the amount of unrolled loops, or items processed, etc.

```
* 4 samples from 1 PCM to dirty buffer
.macro  pcmstream_render1pcm_do4
        move.l  (a0)+,(a1)+
.endm                                           * 7 cycles per sample

.macro  pcmstream_render1pcm_do64
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4

        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4

        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4

        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
        pcmstream_render1pcm_do4
.endm                                           * 448 cycles per chunk
```

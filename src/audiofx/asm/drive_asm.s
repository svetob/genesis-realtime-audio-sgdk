#include "asm_mac.i"
#include "drive_mac.i"

* extern void AFX_drive_process(s8 *samples, u16 len, AFXDrive *drive);

func AFX_drive_process

    * samplesPtr    -> a0
    move.l 4(sp),a0
    * drivePtr      -> a1
    move.l 12(sp),a1
    * mulTablePtr   -> a1
    move.l (a1),a1

    * len           -> d0
    move.w 10(sp),d0
    * sample        -> d1
    moveq.l #0, d1

AFX_drive_process_body:
.L1:
    AFX_drive_process_do64

    sub.w #64,d0
    bne .L1

AFX_drive_process_ret:
    rts

.macro AFX_drive_process_do1
    move.b (a0),d1
    move.b (a1,d1.w),(a0)+
.endm

.macro AFX_drive_process_do8
    AFX_drive_process_do1
    AFX_drive_process_do1
    AFX_drive_process_do1
    AFX_drive_process_do1

    AFX_drive_process_do1
    AFX_drive_process_do1
    AFX_drive_process_do1
    AFX_drive_process_do1
.endm

.macro AFX_drive_process_do64
    AFX_drive_process_do8
    AFX_drive_process_do8
    AFX_drive_process_do8
    AFX_drive_process_do8

    AFX_drive_process_do8
    AFX_drive_process_do8
    AFX_drive_process_do8
    AFX_drive_process_do8
.endm

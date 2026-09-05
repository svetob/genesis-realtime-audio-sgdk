#ifndef __AFX_DRIVE_H
#define __AFX_DRIVE_H

#include <genesis.h>

typedef enum {
    DRIVE_DIGITAL_CLIP = 0,
} AFXDriveType;

typedef struct {
    void *mul_table;
    AFXDriveType type;
    u8 gain;
} AFXDrive;

AFXDrive *AFX_drive_create(AFXDriveType type, u8 gain);
void AFX_drive_free(AFXDrive *drive);
void AFX_drive_process(s8 *samples, u16 len, AFXDrive *drive);

#endif
#include <genesis.h>
#include "resources.h"
#include "pcm_stream.h"
#include "drive.h"
#include <test/log.h>

static inline void *getMulTableForGain(u8 gain)
{
    return (void *) ((u32) mult_s8_int + (((u32) gain) << 8));
}

AFXDrive *AFX_drive_create(AFXDriveType type, u8 gain)
{
    AFXDrive *drive = (AFXDrive *) MEM_alloc(sizeof(AFXDrive));
    drive->type = type;
    drive->gain = gain;

    drive->mul_table = getMulTableForGain(gain);

    return drive;
}

void AFX_drive_free(AFXDrive *drive)
{
    MEM_free(drive);
}

void AFX_drive_update(AFXDrive *drive, AFXDriveType type, u8 gain)
{
    drive->type = type;
    drive->mul_table = getMulTableForGain(gain);
}

extern void AFX_drive_process(s8 *samples, u16 len, AFXDrive *drive);

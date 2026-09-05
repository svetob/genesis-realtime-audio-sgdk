#include <genesis.h>
#include "resources.h"
#include "pcm_stream.h"
#include "drive.h"
#include <test/log.h>

AFXDrive *AFX_drive_create(AFXDriveType type, u8 gain)
{
    AFXDrive *drive = (AFXDrive *) MEM_alloc(sizeof(AFXDrive));
    drive->type = type;
    drive->gain = gain;

    drive->mul_table = (void *) ((u32) mult_s8_int + (((u32) gain) << 8));

    return drive;
}

void AFX_drive_free(AFXDrive *drive)
{
    MEM_free(drive);
}

void AFX_drive_process(s8 *samples, u16 len, AFXDrive *drive)
{
    u8 *s = (u8 *) ((void *) samples);
    u8 *mt = (u8 *) (drive->mul_table);

    while (len--) {
        *s++ = mt[*s];
    }
}

#ifndef _LOG_H
#define _LOG_H

#include <genesis.h>

void logS16(s16 val, u16 x, u16 y, u16 minsize);
void logU32(u32 val, u16 x, u16 y, u16 minsize);

void logU8H(u8 val, u16 x, u16 y);
void logU16H(u16 val, u16 x, u16 y);
void logS16H(s16 val, u16 x, u16 y);

void logNamedU8(const char *name, u8 val, u16 x, u16 y, u16 minsize);
void logNamedU8H(const char *name, u8 val, u16 x, u16 y);
void logNamedU16(const char *name, u16 val, u16 x, u16 y, u16 minsize);
void logNamedU16H(const char *name, u16 val, u16 x, u16 y);
void logNamedU32(const char *name, u32 val, u16 x, u16 y, u16 minsize);
void logNamedU32H(const char *name, u32 val, u16 x, u16 y);

void logNamedPtr(const char *name, void *ptr, u16 x, u16 y);
void logNamedPtrU16(const char *name, void *ptr, u16 x, u16 y);

void logNamedArrayU8H(const char *name, u8 *arr, u16 n, u8 width, u16 y);
void logNamedArrayU16H(const char *name, u16 *arr, u16 n, u8 width, u16 y);
void logNamedArrayU32H(const char *name, u32 *arr, u16 n, u8 width, u16 y);

#endif
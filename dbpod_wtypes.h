#ifndef DBPOD_WTYPES_H__INCLUDED
#define DBPOD_WTYPES_H__INCLUDED

/*
 * $Id$
 *
 * db-Pod Communications Messages - Fake Windows Types For Linux
 *
 * Copyright (c) 2008-2018 MEV Ltd., Bell Technology Ltd.
 * All rights reserved.
 *
 * MODULE CONTENTS
 *
 * dbpod_wtypes.h
 *
 * DESCRIPTION
 *
 * Defines types used in dbpod_coms.h for use in Linux.
 *
 * AMENDMENT RECORD
 *
 * $Log$
 *
 */

#ifdef _WIN32
#include <Windows.h>
#else
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef char CHAR, *PCHAR;
typedef unsigned char UCHAR, *PUCHAR;

typedef int16_t SHORT, *PSHORT;
typedef uint16_t USHORT, *PUSHORT;

typedef int32_t INT, *PINT, *LPINT;
typedef uint32_t UINT, *PUINT;

/* LONG is 32-bits even on 64-bit machines.  May need to cast printf args. */
typedef int32_t LONG, *PLONG, *LPLONG;
typedef uint32_t ULONG, *PULONG;

typedef int64_t LONGLONG, *PLONGLONG;
typedef uint64_t ULONGLONG, *PULONGLONG;

typedef int8_t INT8, *PINT8;
typedef uint8_t UINT8, *PUINT8;
typedef int16_t INT16, *PINT16;
typedef uint16_t UINT16, *PUINT16;
typedef int32_t INT32, *PINT32;
typedef uint32_t UINT32, *PUINT32;
typedef int64_t INT64, *PINT64;
typedef uint64_t UINT64, *PUINT64;

typedef LONG BOOL, *PBOOL, *LPBOOL;
typedef UCHAR BOOLEAN, *PBOOLEAN;
typedef UCHAR BYTE, *PBYTE, *LPBYTE;
typedef USHORT WORD, *PWORD, *LPWORD;
typedef ULONG DWORD, *PDWORD, *LPDWORD;

#ifdef __KERNEL__
/* intptr_t is missing from <linux/types.h> but long can be used instead. */
typedef long INT_PTR, *PINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
#else
typedef intptr_t INT_PTR, *PINT_PTR;
typedef intptr_t LONG_PTR, *PLONG_PTR;
#endif
typedef uintptr_t UINT_PTR, *PUINT_PTR;
typedef uintptr_t ULONG_PTR, *PULONG_PTR;

typedef struct _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;
typedef FILETIME *PFILETIME, *LPFILETIME;

#define FALSE   0
#define TRUE    1

#define FIELD_OFFSET(type, field)   offsetof(type, field)

#if defined(__cplusplus)
}
#endif

#endif

/* vi: set ai et sw=4 ts=4: */
#endif

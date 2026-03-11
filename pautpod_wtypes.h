#ifndef PAUTPOD_WTYPES_H__INCLUDED
#define PAUTPOD_WTYPES_H__INCLUDED

/*
 * $Id$
 *
 * db-Pod Communications Messages - Fake Windows Types For Linux
 *
 * Copyright (c) 2008 MEV Ltd., Bell Technology Ltd.
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

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef char CHAR, *PCHAR;
typedef int8_t SCHAR, *PSCHAR;
typedef uint8_t UCHAR, *PUCHAR;

typedef int16_t SHORT, *PSHORT;
typedef uint16_t USHORT, *PUSHORT;

typedef int INT, *PINT, *LPINT;
typedef unsigned int UINT, *PUINT;

/* LONG is 32-bits even on 64-bit machines.  May need to cast printf args. */
typedef int32_t LONG, *PLONG, *LPLONG;
typedef uint32_t ULONG, *PULONG;

typedef int64_t LONGLONG, *PLONGLONG;
typedef uint64_t ULONGLONG, *PULONGLONG;

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

#define MINCHAR     0x80    /* Assumes CHAR is signed! */
#define MAXCHAR     0x7F    /* Assumes CHAR is signed! */
#define MINSHORT    0x8000
#define MAXSHORT    0x7FFF
#define MINLONG     0x80000000
#define MAXLONG     0x7FFFFFFF
#define MINLONGLONG 0x800000000000000000000000
#define MAXLONGLONG 0x7FFFFFFFFFFFFFFFFFFFFFFF
#define MAXUCHAR    0xFF
#define MAXUSHORT   0xFFFF
#define MAXULONG    0xFFFFFFFF
#define MAXULONGLONG 0xFFFFFFFFFFFFFFFFFFFFFFFF
#define MAXUINT_PTR (~((UINT_PTR)0))
#define MAXINT_PTR  ((INT_PTR)(MAXUINT_PTR >> 1))
#define MININT_PTR  (~MAXINT_PTR)
#define MAXULONG_PTR (~((ULONG_PTR)0))
#define MAXLONG_PTR ((LONG_PTR)(MAXULONG_PTR >> 1))
#define MINLONG_PTR (~MAXLONG_PTR)
#define MAXBYTE     0xFF
#define MAXWORD     0xFFFF
#define MAXDWORD    0xFFFFFFFF

#define FIELD_OFFSET(type, field)   ((LONG)(LONG_PTR)&(((type *)0)->field))

#define CONTAINING_RECORD(address, type, field) ((type *)( \
            (PCHAR)(address) - \
            (ULONG_PTR)(&((type *)0)->field)))

#if defined(__cplusplus)
}
#endif

/* vi: set ai et sw=4 ts=4: */
#endif

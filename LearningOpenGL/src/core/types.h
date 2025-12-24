#pragma once
#include <iostream>
#include <assert.h>

// We may consider putting this as a precompiled header since we will be using
// these types a lot, and this file may get more macros that can be useful for the
// development process. For example, the assert macros is present here on debug mode.

// Typedef for precise bit counts
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16; 
typedef int8_t   s8;

typedef int32_t  bool32; // This is a custom bool that doesn't force non zero values to be one.

typedef float  float32;
typedef double float64;

#define KILOBYTES(value) ( (value) * 1024 )
#define MEGABYTES(value) ( KILOBYTES((value) * 1024) )
#define GIGABYTES(value) ( MEGABYTES((value) * 1024) )
#define TERABYTES(value) ( GIGABYTES((value) * 1024) )

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof((array)[0]) )
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
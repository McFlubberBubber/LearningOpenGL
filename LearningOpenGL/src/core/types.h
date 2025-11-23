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

#define kilobytes(value) ( (value) * 1024 )
#define megabytes(value) ( kilobytes((value) * 1024) )
#define gigabytes(value) ( megabytes((value) * 1024) )
#define terabytes(value) ( gigabytes((value) * 1024) )

#define array_count(array) ( sizeof(array) / sizeof((array)[0]) )

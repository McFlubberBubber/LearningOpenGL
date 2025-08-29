#pragma once
#include <iostream>
// This is mainly for ease of use when typing integers mainly but that's all.
// There have been forums / threads about utilizing "using" instead of typedef
// (something to do with templates), but for now we will be using typedef until we
// run into any issues.

// Long
typedef uint64_t u64;
typedef int64_t s64;

// Int (64 bit systems)
typedef uint32_t u32;
typedef int32_t s32;

// Int (32 bit systems)
typedef uint16_t u16;
typedef int16_t s16;

// Short
typedef uint8_t u8;
typedef int8_t s8;

// Chars + bytes will remain the same since they usually are always going to be
// 1 byte across all systems.

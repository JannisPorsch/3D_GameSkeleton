#ifndef __MAIN_H__
#define __MAIN_H__




// #####################################################################
// COMPILER SETTINGS
// #####################################################################

// Comment/Uncomment this line to disable/enable debug mode:
#define _DEBUG

// #####################################################################




#include <stdint.h>


typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;




#ifdef _DEBUG
    #include <stdio.h>
    #define DBG(string) fprintf(stdout, string)
    #define ERROR(string) fprintf(stderr, "[!] ERROR: %s failed in: %s - line %d\n", string, __FILE__, __LINE__)

#else
    #define DBG(string)
    #define ERROR(string)

#endif




#endif // !__MAIN_H__

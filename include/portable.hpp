#ifndef PORTABLE
#define PORTABLE

#include <cstdint>

#ifndef ABS_32BIT
    #if UINTPTR_MAX == 0xFFFFFFFFu || defined(__arm__) || defined(__i386__) || defined(_M_IX86)
        #define ABS_32BIT 1
    #else
        #define ABS_32BIT 0
    #endif
#endif

#endif
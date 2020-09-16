#ifndef __AXIS_STDINT_INCLUDED__
#define __AXIS_STDINT_INCLUDED__

#ifdef _LANGUAGE_C

typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef long long int           int64_t;
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;
typedef int                     BOOL;


typedef enum {
    false,
    true
} bool, boolean;

#else // for intellisense

#define int8_t signed char
#define int16_t short int
#define int32_t int
#define int64_t long long int
#define uint8_t unsigned char
#define uint16_t unsigned short int
#define uint32_t unsigned int
#define uint64_t unsigned long long
#define size_t unsigned long long int
#define BOOL int;

#define true 1
#define false 0

#endif

#endif /* __AXIS_STDINT_INCLUDED__ */
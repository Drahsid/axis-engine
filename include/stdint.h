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

#if __WORDSIZE == 64
typedef long int                intmax_t;
typedef unsigned long int       uintmax_t;
#else
typedef long long int           intmax_t;
typedef unsigned long long int  uintmax_t;
#endif

typedef enum {
    false,
    true
} bool, boolean;

#endif /* _LANGUAGE_C */

#endif /* __AXIS_STDINT_INCLUDED__ */
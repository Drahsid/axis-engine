#ifndef __AXIS_STDINT_INCLUDED__
#define __AXIS_STDINT_INCLUDED__

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

#endif /* __AXIS_STDINT_INCLUDED__ */
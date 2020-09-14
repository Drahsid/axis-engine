#ifndef __AXIS_COLORS_INCLUDED__
#define __AXIS_COLORS_INCLUDED__

typedef struct {
    unsigned char r, g, b;
} rgb8_t;

typedef struct {
    unsigned char r, g, b, a;
} rgba8_t;

typedef struct {
    float r, g, b;
} rgbf_t;

typedef struct {
    float r, g, b, a;
} rgbaf_t;

typedef struct {
    float h, s, v;
} hsvf_t;

#endif /* __AXIS_COLORS_INCLUDED__ */
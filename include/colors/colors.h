#ifndef __AXIS_COLORS_INCLUDED__
#define __AXIS_COLORS_INCLUDED__

#include "../include.h"

#define COLOR16_TO_COLOR32(CARG0) (                 \
      COLOR32((((CARG0) & 0xF800) >> 11) /* red */  \
    , (((CARG0) & 0x07C0) >> 6) /* green */         \
    , (((CARG0) & 0x003E) >> 1) /* blue  */         \
    , ((((CARG0) & 0x1) > 0) ? 255 : 0) /* alpha */ \
))

#define COLOR32_TO_COLOR16(CARG0) (                 \
      (((CARG0) & 0xF8000000) >> 16) /* red   */    \
    | (((CARG0) & 0x00F80000) >> 13) /* green */    \
    | (((CARG0) & 0x0000F800) >> 10) /* blue  */    \
    | (((CARG0) & 0x00000080) >>  7) /* alpha */    \
) /* Thanks, z64.me! */

#define COLOR24_TO_COLOR32(CARG0) ((CARG0) << 8)

#define COLOR32_TO_COLOR24(CARG0) ((CARG0) >> 8)

/* Create a 16-bit integer, representing a red, green, blue and alpha value. */
#define COLOR16(r, g, b, a) COLOR32_TO_COLOR16(COLOR32((r), (g), (b), (a)))

/* Create a 24-bit integer, representing a red, green and blue value. */
#define COLOR24(r, g, b) (uint32_t)((r << 16) | (g << 8) | b)

/* Create a 32-bit integer, representing a red, green, blue and alpha value. */
#define COLOR32(r, g, b, a) (uint32_t)(((r) << 24) | ((g) << 16) | ((b) << 8) | (a))

/* Isolate a red value from a COLOR32. */
#define RED32(RGBA0) (uint8_t)(((RGBA0) >> 24) & 0xFF)

/* Isolate a green value from a COLOR32. */
#define GREEN32(RGBA0) (uint8_t)(((RGBA0) >> 16) & 0xFF)

/* Isolate a blue value from a COLOR32. */
#define BLUE32(RGBA0) (uint8_t)(((RGBA0) >> 8) & 0xFF)

/* Isolate an alpha value from a COLOR32. */
#define ALPHA32(RGBA0) (uint8_t)(((RGBA0)) & 0xFF)

/* Isolate a red value from a COLOR16. */
#define RED16(RGBA0) RED32(COLOR16_TO_COLOR32(RGBA0))

/* Isolate a green value from a COLOR16. */
#define GREEN16(RGBA0) GREEN32(COLOR16_TO_COLOR32(RGBA0))

/* Isolate a blue value from a COLOR16. */
#define BLUE16(RGBA0) BLUE32(COLOR16_TO_COLOR32(RGBA0))

/* Isolate an alpha value from a COLOR16. */
#define ALPHA16(RGBA0) ALPHA32(COLOR16_TO_COLOR32(RGBA0))

/* Isolate a red value from a COLOR24. */
#define RED24(RGBA0) (uint8_t)(((RGBA0) >> 16) & 0xFF)

/* Isolate a green value from a COLOR24. */
#define GREEN24(RGBA0) (uint8_t)(((RGBA0) >> 8) & 0xFF)

/* Isolate a blue value from a COLOR24. */
#define BLUE24(RGBA0) (uint8_t)(((RGBA0)) & 0xFF)

/* Isolate an alpha value from a COLOR24. Note: COLOR24 is not intended to have an alpha zero. This exists for consistency. */
#define ALPHA24(RGBA0) 0

/* Convert a byte to a floating point percentage between 0.0f and 1.0f */
#define COLORF(RGBA0) (float)((RGBA0) / 255)
#define REDF(RGBA0) COLORF(RGBA0)
#define GREENF(RGBA0) COLORF(RGBA0)
#define BLUEF(RGBA0) COLORF(RGBA0)
#define ALPHAF(RGBA0) COLORF(RGBA0)

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

static inline uint32_t rgb8_to_color24(rgb8_t* rgb)
{
    uint32_t out = COLOR32(
        0x00
        , rgb->r
        , rgb->g
        , rgb->b
    );
    return out;
}

static inline uint32_t rgba8_to_color32(rgba8_t *rgba)
{
    uint32_t out = COLOR32(
        rgba->r
        , rgba->g
        , rgba->b
        , rgba->a
    );
    return out;
}

static inline rgbaf_t color32_to_rgbaf(uint32_t rgba)
{
    rgbaf_t out = (rgbaf_t){
        REDF(RED32(rgba))
        , GREENF(GREEN32(rgba))
        , BLUEF(BLUE32(rgba))
        , ALPHAF(ALPHA32(rgba))
    };
    return out;
}

static inline rgbaf_t rgba8_to_rgbaf(rgba8_t *rgba)
{
    rgbaf_t out = (rgbaf_t){
        REDF(rgba->r)
        , GREENF(rgba->g)
        , BLUEF(rgba->b)
        , ALPHAF(rgba->a)
    };
    return out;
}

#endif /* __AXIS_COLORS_INCLUDED__ */
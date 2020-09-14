#ifndef __AXIS_VECTOR_INCLUDED__
#define __AXIS_VECTOR_INCLUDED__

typedef struct {
    float x, y;
} vec2f_t;

typedef struct {
    int x, y;
} vec2i_t;

typedef struct {
    short x, y;
} vec2s_t;

typedef struct {
    float x, y, z;
} vec3f_t;

typedef struct {
    int x, y, z;
} vec3i_t;

typedef struct {
    short x, y, z;
} vec3s_t;

typedef struct {
    float x, y, z, w;
} vec4f_t;

typedef struct {
    int x, y, z, w;
} vec4i_t;

typedef struct {
    short x, y, z, w;
} vec4s_t;

void vec3f_t_construct(vec3f_t* xyz, float x, float y, float z) {
    xyz->x = x;
    xyz->y = y;
    xyz->z = z;
}

#endif /* __AXIS_VECTOR_INCLUDED__ */
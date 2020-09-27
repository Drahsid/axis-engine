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

/* Construct a Vector 3 Type from floating-point numbers. */
static inline void vec3f_construct(vec3f_t* xyz, float x, float y, float z) {
    xyz->x = x;
    xyz->y = y;
    xyz->z = z;
}

static inline vec3f_t vec3f_new_float(float x, float y, float z)
{
    return (vec3f_t){x, y, z};
}

static inline vec3f_t vec3f_new_vector(const vec3f_t v)
{
    return v;
}

#define vec3f_new(_a0, ...)      \
_Generic((_a0),                  \
    int: vec3f_new_float,        \
    uint32_t: vec3f_new_float,   \
    float: vec3f_new_float,      \
    double: vec3f_new_float,     \
    vec3f_t: vec3f_new_vector    \
)(_a0 __VA_OPT__(,) __VA_ARGS__)
    


static inline void vec3f_multiply_vector(void);
static inline void vec3f_multiply_float(void);
#define vec3f_multiply(VARG0, VARG1) \
_Generic((VARG1)),                 \
    vec3f_t: vec3f_multiply_vector,  \
    default: vec3f_multiply_float    \
)(VARG0, VARG1)

static inline void vec3f_divide_vector(void);
static inline void vec3f_divide_float(void);
#define vec3f_divide(VARG0, VARG1) \
_Generic((VARG1)),                 \
    vec3f_t: vec3f_divide_vector,  \
    default: vec3f_divide_float    \
)(VARG0, VARG1)

/* Computer the length of a Vector 3 Type (floating-point) */
static inline float vec3f_length(vec3f_t* a)
{
    float ls = (a->x * a->x) + (a->y * a->y) + (a->z * a->z);
    return (float)sqrtf(ls);
}

/* Normalize a Vector 3 Type (floating-point) */
static inline void vec3f_normalize(vec3f_t* a)
{
    float ls = (a->x * a->x) + (a->y * a->y) + (a->z * a->z);
    float length = (float)sqrtf(ls);
    a->x = (a->x / length);
    a->y = (a->y / length);
    a->z = (a->z / length);
}

/* Compute the distance between two Vector 3 Types (floating-point) */
static inline float vec3f_distance(vec3f_t* a, vec3f_t* b)
{
    float d[3] = {
        (float)((a->x) - (b->x))
        , (float)((a->y) - (b->y))
        , (float)((a->z) - (b->z))
    };

    float ls = (d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2]);
    return (float)sqrtf(ls);
}

// vec3f_construct();
// vec3f_length();
// vec3f_normalize();
// vec3f_distance();
// vec3f_dot();
// vec3f_cross();
// vec3f_reflect();
// vec3f_dot();
// vec3f_clamp();
// vec3f_lerp();
// vec3f_transform();
// vec3f_add();
// vec3f_subtract();

// vec3f_invert();


#endif /* __AXIS_VECTOR_INCLUDED__ */
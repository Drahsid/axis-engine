#ifndef __AXIS_VECTOR_INCLUDED__
#define __AXIS_VECTOR_INCLUDED__

#include <math.h>

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

#define VEC_OVERRIDE(_1, _2, _3, _4, N, ...) N
#define VEC2_NEW(x, y) (vec2f_t){(x), (y)}
#define VEC3_NEW(x, y, z) (vec3f_t){(x), (y), (z)}
#define VEC4_NEW(x, y, z, w) (vec4f_t){(x), (y), (z), (w)}
#define vec_new(...) (VEC_OVERRIDE(__VA_ARGS__, VEC4_NEW, VEC3_NEW, VEC2_NEW)(__VA_ARGS__))

// add the vec3f rhs to the vec3f lhs, storing the result in lhs ((vec3f_t)lhs += (vec3f_t)rhs)
static inline void vec3f_add_assignment(vec3f_t* lhs, vec3f_t* rhs) {
    lhs->x += rhs->x;
    lhs->y += rhs->y;
    lhs->z += rhs->z;
}

// subtract the vec3f rhs with the vec3f lhs, storing the result in lhs ((vec3f_t)lhs -= (vec3f_t)rhs)
static inline void vec3f_sub_assignment(vec3f_t* lhs, vec3f_t* rhs) {
    lhs->x -= rhs->x;
    lhs->y -= rhs->y;
    lhs->z -= rhs->z;
}

// scale the vec3f lhs to the vec3f rhs, storing the result in lhs ((vec3f_t)lhs *= (vec3f_t)rhs)
static inline void vec3f_multiply_assignment(vec3f_t* lhs, vec3f_t* rhs) {
    lhs->x *= rhs->x;
    lhs->y *= rhs->y;
    lhs->z *= rhs->z;
}

// divide the vec3f lhs by the vec3f rhs, storing the result in lhs ((vec3f_t)lhs /= (vec3f_t)rhs)
static inline void vec3f_divide_assignment(vec3f_t* lhs, vec3f_t* rhs) {
    lhs->x /= rhs->x;
    lhs->y /= rhs->y;
    lhs->z /= rhs->z;
}

// scale the vec3f lhs to the float rhs, storing the result in lhs ((vec3f_t)lhs *= (float)rhs)
static inline void vec3f_multiplyf_assignment(vec3f_t* lhs, float rhs) {
    lhs->x *= rhs;
    lhs->y *= rhs;
    lhs->z *= rhs;
}

// divide the vec3f lhs by the float rhs, storing the result in lhs ((vec3f_t)lhs /= (float)rhs)
static inline void vec3f_dividef_assignment(vec3f_t* lhs, float rhs) {
    lhs->x /= rhs;
    lhs->y /= rhs;
    lhs->z /= rhs;
}

// returns the sum of the vec3f lhs and the vec3f rhs (v = (vec3f_t)lhs + (vec3f_t)rhs)
static inline vec3f_t vec3f_add(vec3f_t lhs, vec3f_t rhs) {
    vec3f_add_assignment(&lhs, &rhs);
    return lhs;
}

// returns the vec3f lhs subtracted by the vec3f rhs (v = (vec3f_t)lhs - (vec3f_t)rhs)
static inline vec3f_t vec3f_sub(vec3f_t lhs, vec3f_t rhs) {
    vec3f_sub_assignment(&lhs, &rhs);
    return lhs;
}

// returns the vec3f lhs scaled to the vec3f rhs (v = (vec3f_t)lhs * (vec3f_t)rhs)
static inline vec3f_t vec3f_multiply(vec3f_t lhs, vec3f_t rhs) {
    vec3f_multiply_assignment(&lhs, &rhs);
    return lhs;
}

// returns the vec3f lhs divided by the vec3f rhs (v = (vec3f_t)lhs / (vec3f_t)rhs)
static inline vec3f_t vec3f_divide(vec3f_t lhs, vec3f_t rhs) {
    vec3f_divide_assignment(&lhs, &rhs);
    return lhs;
}

// returns the vec3f lhs scaled to the float rhs (v = (vec3f_t)lhs * (float)rhs)
static inline vec3f_t vec3f_multiplyf(vec3f_t lhs, float rhs) {
    vec3f_multiplyf_assignment(&lhs, rhs);
    return lhs;
}

// returns the vec3f lhs divided by the float rhs (v = (vec3f_t)lhs / (float)rhs)
static inline vec3f_t vec3f_dividef(vec3f_t lhs, float rhs) {
    vec3f_dividef_assignment(&lhs, rhs);
    return lhs;
}


// returns a float using the calculation vec3f lhs dot vec3f rhs
static inline float vec3f_dot(vec3f_t* lhs, vec3f_t* rhs) {
    return (lhs->x * rhs->x) + (lhs->y * rhs->y) + (lhs->z * rhs->z);
}

// returns a vec3f using the calculation vec3f lhs cross vec3f rhs
static inline vec3f_t vec3f_cross(vec3f_t* lhs, vec3f_t* rhs) {
    return vec_new( lhs->y * rhs->z - rhs->y * lhs->z,
                    lhs->z * rhs->x - rhs->z * lhs->x,
                    lhs->x * rhs->y - rhs->x * lhs->y);
}

// returns the square magnitude of the vec3f lhs
static inline float vec3f_square_magnitude(vec3f_t lhs) {
    return  (lhs.x * lhs.x) +
            (lhs.y * lhs.y) +
            (lhs.z * lhs.z);
}

// returns the magnitude of the vec3f lhs
static inline float vec3f_magnitude(vec3f_t lhs)
{
    return sqrtf(vec3f_square_magnitude(lhs));
}

// I needed these for optimization, _p isn't a very good way to name them. Open for suggestions
// returns the square magnitude of the vec3f lhs
static inline float vec3f_square_magnitude_p(vec3f_t* lhs) {
    return  (lhs->x * lhs->x) +
            (lhs->y * lhs->y) +
            (lhs->z * lhs->z);
}

// returns the magnitude of the vec3f lhs
static inline float vec3f_magnitude_p(vec3f_t* lhs)
{
    return sqrtf(vec3f_square_magnitude_p(lhs));
}

// normalizes the vec3f lhs into a unit vector
static inline void vec3f_normalize_assignment(vec3f_t* lhs)
{
    float magnitude = vec3f_magnitude_p(lhs);
    lhs->x = (lhs->x / magnitude);
    lhs->y = (lhs->y / magnitude);
    lhs->z = (lhs->z / magnitude);
}

// returns the unit vector of the vec3f lhs
static inline vec3f_t vec3f_normalize(vec3f_t lhs)
{
    vec3f_normalize_assignment(&lhs);
    return lhs;
}

// returns the magnitude of the vecf lhs subtracted by the vec3f rhs
static inline float vec3f_distance(vec3f_t lhs, vec3f_t rhs)
{
    vec3f_sub_assignment(&lhs, &rhs);
    return vec3f_magnitude_p(&lhs);
}

// iverses the vec3f lhs
static inline void vec3f_inverse_assignment(vec3f_t* lhs) {
    vec3f_multiplyf_assignment(lhs, -1.0f);
}

// returns the inverse vector of the vec3f lhs
static inline vec3f_t vec3f_inverse(vec3f_t lhs) {
    vec3f_multiplyf_assignment(&lhs, -1.0f);
    return lhs;
}

static const vec3f_t vec3f_right    = vec_new(1.0f, 0.0f, 0.0f);
static const vec3f_t vec3f_up       = vec_new(0.0f, 1.0f, 0.0f);
static const vec3f_t vec3f_forward  = vec_new(0.0f, 0.0f, 1.0f);
static const vec3f_t vec3f_left     = vec_new(-1.0f, 0.0f, 0.0f);
static const vec3f_t vec3f_down     = vec_new(0.0f, -1.0f, 0.0f);
static const vec3f_t vec3f_backward = vec_new(0.0f, 0.0f, -1.0f);
static const vec3f_t vec3f_zero     = vec_new(0.0f, 0.0f, 0.0f);


#endif /* __AXIS_VECTOR_INCLUDED__ */
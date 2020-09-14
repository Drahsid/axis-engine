#ifndef VECTORTYPES_H
#define VECTORTYPES_H

// TODO: Not this

typedef struct
{
    float x, y, z;
} vec3f_t;

void vec3f_t_construct(vec3f_t* xyz, float x, float y, float z) {
    xyz->x = x;
    xyz->y = y;
    xyz->z = z;
}

#endif

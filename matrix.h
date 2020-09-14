#ifndef MATRIX_H
#define MATRIX_H

// TODO: Not this

typedef union {
    float mf[4][4];
    struct {
        float xx, xy, xz, xw,
              yx, yy, yz, yw,
              zx, zy, zz, zw,
              wx, wy, wz, ww;
    };
} mtxf_t;

#endif
#ifndef __AXIS_MATRIX_INCLUDED__
#define __AXIS_MATRIX_INCLUDED__

typedef _mtxf2_t float[2][2];
typedef _mtxf3_t float[3][3];
typedef _mtxf4_t float[4][4];

typedef union {
    _mtxf2_t f;
    struct {
        xx, xy
      , yx, yy
    } mf;
} MtxF2_t, mtxf2_t, Matrix22;

typedef union {
    _mtxf3_t f;
    struct {
        xx, xy, xz
      , yx, yy, yz
      , zx, zy, zz
    } mf;
} MtxF3_t, mtxf3_t, Matrix33;

typedef union {
    _mtxf4_t f;
    struct {
        xx, xy, xz, xw
      , yx, yy, yz, yw
      , zx, zy, zz, zw
      , wx, wy, wz, ww
    } mf;
} MtxF4_t, mtxf4_t, Matrix44;

#endif /* __AXIS_MATRIX_INCLUDED__ */ 
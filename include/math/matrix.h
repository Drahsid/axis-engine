#ifndef __AXIS_MATRIX_INCLUDED__
#define __AXIS_MATRIX_INCLUDED__

typedef float _mtxf2_t[2][2];
typedef float _mtxf3_t[3][3];
typedef float _mtxf4_t[4][4];

typedef union {
    _mtxf2_t f;
    struct {
        float xx, xy
            , yx, yy
    } mf;
} MtxF2_t, mtxf2_t, Matrix22;

typedef union {
    _mtxf3_t f;
    struct {
        float xx, xy, xz
            , yx, yy, yz
            , zx, zy, zz
    } mf;
} MtxF3_t, mtxf3_t, Matrix33;

typedef union {
    _mtxf4_t f;
    struct {
        float xx, xy, xz, xw
            , yx, yy, yz, yw
            , zx, zy, zz, zw
            , wx, wy, wz, ww
    } mf;
} MtxF4_t, mtxf4_t, Matrix44;

#endif /* __AXIS_MATRIX_INCLUDED__ */ 
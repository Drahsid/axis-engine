#ifndef __AXIS_MATRIX_INCLUDED__
#define __AXIS_MATRIX_INCLUDED__

typedef union Matrix22 {
    float mf[2][2];
    struct {
        float xx, xy;
        float yx, yy;
    } m;
} MtxF2_t, mtxf2_t;

typedef union Matrix33 {
    float mf[3][3];
    struct {
        float xx, xy, xz;
        float yx, yy, yz;
        float zx, zy, zz;
    } m;
} MtxF3_t, mtxf3_t;

typedef union Matrix44 {
    float mf[4][4];
    struct {
        float xx, xy, xz, xw;
        float yx, yy, yz, yw;
        float zx, zy, zz, zw;
        float wx, wy, wz, ww;
    } m;
} MtxF4_t, mtxf4_t;

#endif /* __AXIS_MATRIX_INCLUDED__ */ 
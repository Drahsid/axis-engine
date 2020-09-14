#ifndef VIDEO_H
#define VIDEO_H

#define	SCREEN_HT   (320)
#define	SCREEN_WD	(240)

#define COLOR_DEPTH_TYPE unsigned short
#define COLOR_DEPTH_SIZE (sizeof(COLOR_DEPTH_TYPE))

enum {
   VI_MODE_NTSC_LPN1 = 0,
   VI_MODE_NTSC_LPF1 = 1,
   VI_MODE_NTSC_LAN1 = 2,
   VI_MODE_NTSC_LAF1 = 3,
   VI_MODE_NTSC_LPN2 = 4,
   VI_MODE_NTSC_LPF2 = 5,
   VI_MODE_NTSC_LAN2 = 6,
   VI_MODE_NTSC_LAF2 = 7,
   VI_MODE_NTSC_HPN1 = 8,
   VI_MODE_NTSC_HPF1 = 9,
   VI_MODE_NTSC_HAN1 = 10,
   VI_MODE_NTSC_HAF1 = 11,
   VI_MODE_NTSC_HPN2 = 12,
   VI_MODE_NTSC_HPF2 = 13,
   VI_MODE_PAL_LPN1 = 14,
   VI_MODE_PAL_LPF1 = 15,
   VI_MODE_PAL_LAN1 = 16,
   VI_MODE_PAL_LAF1 = 17,
   VI_MODE_PAL_LPN2 = 18,
   VI_MODE_PAL_LPF2 = 19,
   VI_MODE_PAL_LAN2 = 20,
   VI_MODE_PAL_LAF2 = 21,
   VI_MODE_PAL_HPN1 = 22,
   VI_MODE_PAL_HPF1 = 23,
   VI_MODE_PAL_HAN1 = 24,
   VI_MODE_PAL_HAF1 = 25,
   VI_MODE_PAL_HPN2 = 26,
   VI_MODE_PAL_HPF2 = 27,
   VI_MODE_MPAL_LPN1 = 28,
   VI_MODE_MPAL_LPF1 = 29,
   VI_MODE_MPAL_LAN1 = 30,
   VI_MODE_MPAL_LAF1 = 31,
   VI_MODE_MPAL_LPN2 = 32,
   VI_MODE_MPAL_LPF2 = 33,
   VI_MODE_MPAL_LAN2 = 34,
   VI_MODE_MPAL_LAF2 = 35,
   VI_MODE_MPAL_HPN1 = 36,
   VI_MODE_MPAL_HPF1 = 37,
   VI_MODE_MPAL_HAN1 = 38,
   VI_MODE_MPAL_HAF1 = 39,
   VI_MODE_MPAL_HPN2 = 40,
   VI_MODE_MPAL_HPF2 = 41,
   VI_MODE_FPAL_LPN1 = 42,
   VI_MODE_FPAL_LPF1 = 43,
   VI_MODE_FPAL_LAN1 = 44,
   VI_MODE_FPAL_LAF1 = 45,
   VI_MODE_FPAL_LPN2 = 46,
   VI_MODE_FPAL_LPF2 = 47,
   VI_MODE_FPAL_LAN2 = 48,
   VI_MODE_FPAL_LAF2 = 49,
   VI_MODE_FPAL_HPN1 = 50,
   VI_MODE_FPAL_HPF1 = 51,
   VI_MODE_FPAL_HAN1 = 52,
   VI_MODE_FPAL_HAF1 = 53,
   VI_MODE_FPAL_HPN2 = 54,
   VI_MODE_FPAL_HPF2 = 55
} OS_VI_MODE;

#endif


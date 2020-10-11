#ifndef __AXIS_MATH_COMMON__
#define __AXIS_MATH_COMMON__

#define PI (3.14159265358979323)
#define TAU (PI * 2.0)
#define HPI (PI * 0.5)

#define DEG2RAD (0.0174532925199433)
#define RAD2DEG (57.29577951308231)
#define S2RAD (0.0000958737992429)
#define S2DEG (0.0054931640625)
#define RAD2S (10430.37835047045)
#define DEG2S (182.0444444444444)

#define STOR(RHS) (S2RAD * RHS)
#define STOD(RHS) (S2DEG * RHS)
#define RTOS(RHS) (RAD2S * RHS)
#define DTOS(RHS) (DEG2S * RHS)
#define DTOR(RHS) (DEG2RAD * RHS)
#define RTOD(RHS) (RAD2DEG * RHS)

// If you ever need to explicitly truncate these
#define STORF(RHS) ((float)S2RAD * RHS)
#define STODF(RHS) ((float)S2DEG * RHS)
#define RTOSF(RHS) ((float)RAD2S * RHS)
#define DTOSF(RHS) ((float)DEG2S * RHS)
#define DTORF(RHS) ((float)DEG2RAD * RHS)
#define RTODF(RHS) ((float)RAD2DEG * RHS)

static const double MATH_PI = PI;
static const double MATH_TAU = TAU;
static const double MATH_HPI = HPI;
static const float MATHF_PI = (float)PI;
static const float MATHF_TAU = (float)TAU;
static const float MATHF_HPI = (float)HPI;

static const double MATH_DTOR = DEG2RAD;
static const double MATH_RTOD = RAD2DEG;
static const double MATH_STOR = S2RAD;
static const double MATH_STOD = S2DEG;
static const double MATH_RTOS = RAD2S;
static const double MATH_DTOS = DEG2S;
static const float MATHF_DTOR = (float)DEG2RAD;
static const float MATHF_RTOD = (float)RAD2DEG;
static const float MATHF_STOR = (float)S2RAD;
static const float MATHF_STOD = (float)S2DEG;
static const float MATHF_RTOS = (float)RAD2S;
static const float MATHF_DTOS = (float)DEG2S;

#endif /* __AXIS_MATH_COMMON__ */



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef __GNUC__
#define INLINE _inline
#else
#define INLINE inline
#endif

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#include "../config.h"

#ifdef WIN32
#include "timer-w.c"
#else
#include "timer-lx.c"
#endif

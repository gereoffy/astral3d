#include "../config.h"

#ifdef WIN32
#warning windoze
#include "audio_w.c"
#else
#warning linuksz
#include "audio_oss.c"
#endif

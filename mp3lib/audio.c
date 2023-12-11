#include "../config.h"

#ifdef __APPLE__
#warning Mekk
#include "audio_ns.c"
#else

#ifdef WIN32
#warning windoze
#include "audio_w.c"
#else
#warning linuksz
#include "audio_oss.c"
#endif

#endif

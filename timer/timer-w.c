// Precise timer routines for WINDOWS
#include <mmsystem.h>
//#include <conio.h>
#include "timer.h"

DWORD WINAPI timeGetTime(void);

// Returns current time in seconds
inline float GetTimer(){
  return (float)timeGetTime() * 0.001;
}  

// Returns current time in microseconds
inline int uGetTimer(){
  return timeGetTime() * 1000;
}  


static float RelativeTime=0.0;

// Returns time spent between now and last call in seconds
float GetRelativeTime(){
float t,r;
  t=GetTimer();
  r=t-RelativeTime;
  RelativeTime=t;
  return r;
}

// Initialize timer, must be called at least once at start
void InitTimer(){
  GetRelativeTime();
}


#if 0
void main(){
  float t=0;
  InitTimer();
  while(1){ t+=GetRelativeTime();printf("time= %10.6f\r",t);fflush(stdout); }
}
#endif


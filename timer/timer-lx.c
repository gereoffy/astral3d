// Precise timer routines for LINUX  (C) LGB & A'rpi/ASTRAL

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

// Returns current time in seconds
float GetTimer(){
  struct timeval tv;
  struct timezone tz;
  float s;
  gettimeofday(&tv,&tz);
  s=tv.tv_usec+1000000*tv.tv_sec;
  return (s*0.000001);
}  

// Returns current time in microseconds
int uGetTimer(){
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  return (int)(tv.tv_usec+1000000*tv.tv_sec);
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


#include <stdio.h>
#include <stdlib.h>
#include "mp3.h"

int main(){
int e=-1;
  if(!MP3_OpenDevice(NULL)){ printf("Can't open audio device\n");exit(1);}
  MP3_Play("apoc.mp3",0);
  while(1) if(MP3_frames!=e){
    e=MP3_frames;printf("%5d  \r",e);fflush(stdout);
  }
  MP3_Stop();
  MP3_CloseDevice();
return 0;
}

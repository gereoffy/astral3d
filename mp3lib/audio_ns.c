// nosound

#include <stdio.h>
#include <stdlib.h>

#include "audio.h"
#include "mp3.h"

int MP3_eof=0;
int MP3_frames=0;
FILE *MP3_file=NULL;
int MP3_SkipFrames=0;


int MP3_Play(char *filename,int pos){
  MP3_eof=0;
  MP3_frames=pos;
  return 0;
}

void MP3_Stop(){
}

int MP3_OpenDevice(char *devname){
  return 0;
}

void MP3_CloseDevice(){
 
}




// Scripting system

#if 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#ifdef __GNUC__
#include <sys/time.h>
#include <sys/types.h>
#include "../mp3lib/mp3.h"
#else
#include <windows.h>
#include "../mp3win/mp3.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "../3dslib/ast3d.h"
#include "../timer/timer.h"
#include "../blob/blob.h"
#include "../fdtunnel/fdtunnel.h"
#include "../spline/spline.h"
#include "../smoke/smoke.h"
#include "../sinpart/sinpart.h"
#include "../greets/greets.h"

#include "../render/render.h"
#include "../afs/afs.h"
#include "../loadmap/loadmaps.h"
#endif

#define byte unsigned char
#define word unsigned short
#define dword unsigned int
#define forall(i,n) for(i=0;i<n;i++)

int str2int(char *s){
char* hiba;
  if(strcmp(s,"on")==0)return 1;
  if(strcmp(s,"off")==0)return 0;
  if(strcmp(s,"true")==0)return 1;
  if(strcmp(s,"false")==0)return 0;
  if(strcmp(s,"yes")==0)return 1;
  if(strcmp(s,"no")==0)return 0;
  return strtol(s,&hiba,0);  // hex-t is tud!!
//  if(s[0]=='0' && toupper(s[1])=='X'){}
//  return atoi(s);
}

float str2float(char *s){ return atof(s);}

extern void ExitDemo();

int scr_playing=0;
int nosound=0;
int adk_clear_buffer_flag=1;

#include "scr_var.c"
#include "scr_event.c"
#include "scr_file.c"

#include "scr_init.c"
#include "scr_draw.c"
#include "scr_exec.c"




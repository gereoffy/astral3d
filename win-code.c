//-------------------------------------------------------------------
//	File:		YourCode.cpp
//	Comments:	Write your visual effect here.
//-------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "3dslib/ast3d.h"

#include "render/render.h"
#include "mp3lib/mp3.h"
#include "timer/timer.h"
#include "loadmap/load_map.h"
#include "loadmap/loadmaps.h"
#include "blob/blob.h"
#include "afs/afs.h"
#include "script/script.h"

#include "prof.h"

// Change your App Name.
char szAppName[] = "Astral DEMO System";

static int total_frames=0;

// Current window dimensions.
extern GLsizei window_w, window_h;
extern int fullscreen;
extern int fullscreen_bpp;
extern int fullscreen_xres;
extern int fullscreen_yres;

char* scriptname="astral.scr";

int Load(int argc,char* argv[]){
int i;

    for(i=0;i<argc;i++) printf("ARG(%d)=%s\n",i,argv[i]);

    fullscreen=1;
    nosound=0;
    for(i=1;i<argc;i++){
      if(strcmp(argv[i],"-window")==0) fullscreen=0; else
      if(strcmp(argv[i],"-nosound")==0) nosound=1; else
      if(strcmp(argv[i],"-16")==0) fullscreen_bpp=16; else
      if(strcmp(argv[i],"-24")==0) fullscreen_bpp=24; else
      if(strcmp(argv[i],"-32")==0) fullscreen_bpp=32; else
      if(strcmp(argv[i],"-640")==0){fullscreen_xres=640;fullscreen_yres=480; } else
      if(strcmp(argv[i],"-800")==0){fullscreen_xres=800;fullscreen_yres=600; } else
      if(strcmp(argv[i],"-1024")==0){fullscreen_xres=1024;fullscreen_yres=768; } else
      if(strcmp(argv[i],"-1280")==0){fullscreen_xres=1280;fullscreen_yres=1024; } else
      scriptname=argv[i];
    }
    printf("VIDEO:  %d x %d   %d bpp  %s\n",fullscreen_xres,fullscreen_yres,fullscreen_bpp,fullscreen?"fullscreen":"window");

    afs_init("",AFS_TYPE_FILES);
    afs_init("astral.pak",AFS_TYPE_PAK);

    ast3d_init (ast3d_transform |
                ast3d_hierarchy |
                ast3d_domorph   |
                ast3d_normcalc  |
                ast3d_calcnorm  );


    if(!nosound)
    if(!MP3_OpenDevice(NULL)){
      MessageBox(NULL, "MP3_OpenDevice() failed:  "
      "Cannot open SOUND device... NO MUSIC :(", "Error", MB_OK);
      nosound=1;
    }
    
    return 1;
}

// Startup Stuff.
int Init(){	// Called right after the window is created, and OpenGL is initialized.

    InitTimer();
    blob_init();
    scrInit();
    fx_init();
    scrLoad(scriptname);
    adk_time=0.0; // adk_frame=0.0;

    lightmap=LoadSimpleMap("light.bmp",512+1024);
    blobmap=load_blob_map("2.jpg");

            glClearColor( 0.0, 0.0, 0.0, 1.0 );
            glClearDepth( 1.0 );
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glFinish();

	return 1;
}

void Shutdown(){ // Called right after the window is destroyed.
  if(!nosound){
    MP3_Stop();
    MP3_CloseDevice();
  }
}

static int exit_demo=0;

// Draw all the scene related stuff.
int Render(){
//	if (!BASS_ChannelIsActive(music)) takarodj=1; // kilepes ha vege az mp3nak

  while(1){
    if(GetAsyncKeyState(VK_LBUTTON)&1) return 0;
    if(exit_demo) return 0;
    if(scr_playing){
      draw_scene();
      ++total_frames;
      if(scr_playing){
        glFinish();
        return 1;
      }
    }
    scrExec();
  }
  
}

void ExitDemo(){
  exit_demo=1;
  PrintMAPinfo();
  printf("\nTotal frames: %d\n",total_frames);
}

static  float time_dt=0;

void PrintMark(){
    printf("MARK:  p=%5d   t=%6.2f  dt=%5.2f  fps=%4.2f\n",MP3_frames,adk_time,adk_time-time_dt,(adk_time>2)?(total_frames/adk_time):0);
//    printf("MARK:  p=%5d   t=%6.2f  dt=%5.2f\n",MP3_frames,adk_time,adk_time-time_dt);
    time_dt=adk_time;
}


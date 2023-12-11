//#define FULLSCREEN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "3dslib/ast3d.h"

#include "render/render.h"
#include "mp3lib/mp3.h"
#include "timer/timer.h"
#include "loadmap/load_map.h"
#include "loadmap/loadmaps.h"
#include "loadmap/loadtxtr.h"
#include "fx/blob/blob.h"
#include "afs/afs.h"
#include "script/script.h"

#define DEFINE_PROF_VARS
#include "prof.h"

// #include "glide.h"

static int pause=0;
static int total_frames=0;

void ExitDemo(){
  if(!nosound){
    MP3_Stop();
    MP3_CloseDevice();
  }
  if(fx_debug) fclose(fx_debug);
//  printf("\nTexture memory used:  32bpp: %d k    16bpp: %d k\n",
//    map_memory_used/1024,map_memory_used_16bpp/1024);
  PrintMAPinfo();
  printf("\nTotal frames: %d\n",total_frames);
  exit(0);
}

static float time_dt=0;
static int MP3_frames_last=0;

/* handle KEY events */
static void key(unsigned char k, int x, int y){
  switch (k) {
  case 27:  /* Escape */
    ExitDemo();
  case 'p': 
    printf("MARK:  p=%5d  [dp=%4d]  t=%6.2f  [dt=%5.2f]  fps=%4.2f\n",MP3_frames,MP3_frames-MP3_frames_last,adk_time,adk_time-time_dt,(adk_time>2)?(total_frames/adk_time):0);
    print_currentfx_params();
    time_dt=adk_time;
    MP3_frames_last=MP3_frames;
    break;
  case 's': 
    pause^=1;MP3_eof=pause;
    GetRelativeTime();
    break;
  default:
    return;
  }
}

int window_w,window_h;

GLvoid resize_window(int w, int h){ 
  glViewport(0,0,w,h); window_w=w;window_h=h;
}

GLvoid DrawScene(){
  if(!scr_playing) return;
  if(pause) return;
//  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  draw_scene();
  if(!scr_playing) return;
  glFinish();
  glutSwapBuffers();
  ++total_frames;
}

GLvoid IdleFunc(){
  scrExec();
  if(scr_playing) glutPostRedisplay();
}


int main(int argc,char* argv[]){
char* scriptname="astral.scr";
int fullscreen=0;
int xs=1024;
int ys=768;

    { int i;
      for(i=1;i<argc;i++){
        if(strcmp(argv[i],"-window")==0) fullscreen=0; else
        if(strcmp(argv[i],"-fullscreen")==0) fullscreen=1; else
        if(strcmp(argv[i],"-nosound")==0) nosound=1; else
        if(strcmp(argv[i],"-640")==0){ xs=640;ys=480;} else
        if(strcmp(argv[i],"-800")==0){ xs=800;ys=600;} else
        if(strcmp(argv[i],"-1024")==0){ xs=1024;ys=768;} else
        if(strcmp(argv[i],"-1280")==0){ xs=1280;ys=1024;} else
        scriptname=argv[i];
      }
    }

    afs_init("",AFS_TYPE_FILES);
    afs_init("astral.pak",AFS_TYPE_PAK);

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//    if(!xs && !fullscreen){ xs=640;ys=480;}
    glutInitWindowSize(xs,ys);
//    glutInitWindowSize (1280,1024);
//    glutInitWindowSize (800, 600);
//    glutInitWindowSize (1280, 1024);
    glutInitWindowPosition (0, 0);
    glutCreateWindow (argv[0]);

    if(fullscreen) glutFullScreen();


#if 1
     printf("GL_VERSION: %s\n", (char *) glGetString(GL_VERSION));
     printf("GL_EXTENSIONS: %s\n", (char *) glGetString(GL_EXTENSIONS));
     printf("GL_RENDERER: %s\n", (char *) glGetString(GL_RENDERER));
     printf("GL_VENDOR: %s\n", (char *) glGetString(GL_VENDOR));
     printf("GLU_VERSION: %s\n", (char *) gluGetString(GLU_VERSION));
     printf("GLU_EXTENSIONS: %s\n", (char *) gluGetString(GLU_EXTENSIONS));
     printf("GLUT_API_VERSION: %d\n", GLUT_API_VERSION);
#endif

//    resize_window(640,480);

    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth( 1.0 );
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//    glFinish();
//    glutSwapBuffers();

        ast3d_init (ast3d_transform | 
                    ast3d_hierarchy | 
                    ast3d_domorph   |
                    ast3d_normcalc  |
                    ast3d_calcnorm  );

//    ast3d_init (ast3d_transform | ast3d_hierarchy | ast3d_domorph);

#if 0
    if(!MP3_OpenDevice(NULL)){ printf("Can't open audio device\n");exit(1);}
#else
    if(!MP3_OpenDevice(NULL)){ 
      printf("Cannot open SOUND device... silence suxx!\n");
      nosound=1;
    }
#endif
    InitTimer();
    blob_init();
    scrInit();
    fx_init();
    scrLoad(scriptname);
    adk_time=0.0; // adk_frame=0.0;
    
//    fx_debug=fopen("effects.log","wt");

    printf("lightmap loading\n");
    lightmap=LoadSimpleMap("light.bmp",512+1024);
    printf("lightmap OK\n");
    blobmap=load_blob_map("2.jpg");
//    fdtunnel_texture=load_blob_map("coins.jpg");

//    grDitherMode(GR_DITHER_DISABLE);
//    grDitherMode(GR_DITHER_4x4);

//    resize_window(640,480);

    glutDisplayFunc(DrawScene);
//    glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    glutReshapeFunc(resize_window);
    glutIdleFunc(IdleFunc);
    glutMainLoop();

return 0;
}


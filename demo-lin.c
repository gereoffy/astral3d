#define FULLSCREEN

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "3dslib/ast3d.h"

#include "render/render.h"
#include "mp3lib/mp3.h"
#include "timer/timer.h"
#include "loadmap/load_map.h"
#include "loadmap/loadmaps.h"
#include "blob/blob.h"
#include "afs/afs.h"
#include "script/script.h"

#define DEFINE_PROF_VARS
#include "prof.h"

// #include "glide.h"

void ExitDemo(){
  if(!nosound){
    MP3_Stop();
    MP3_CloseDevice();
  }
  if(fx_debug) fclose(fx_debug);
//  printf("\nTexture memory used:  32bpp: %d k    16bpp: %d k\n",
//    map_memory_used/1024,map_memory_used_16bpp/1024);
  exit(0);
}

/* handle KEY events */
static void key(unsigned char k, int x, int y){
  switch (k) {
  case 27:  /* Escape */
    ExitDemo();
  case 'p': 
    printf("MARK:  p=%5d   t=%4d\n",MP3_frames,(int)adk_time);
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
//  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  draw_scene();
  glFinish();
  glutSwapBuffers();
}

GLvoid IdleFunc(){
  scrExec();
  if(scr_playing) glutPostRedisplay();
}


int main(int argc,char* argv[]){

    afs_init("",AFS_TYPE_FILES);
    afs_init("astral.pak",AFS_TYPE_PAK);

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (640, 480);
//    glutInitWindowSize (800, 600);
//    glutInitWindowSize (1024, 768);
//    glutInitWindowSize (1280, 1024);
    glutInitWindowPosition (0, 0);
    glutCreateWindow (argv[0]);

#if 1
     printf("GL_VERSION: %s\n", (char *) glGetString(GL_VERSION));
     printf("GL_EXTENSIONS: %s\n", (char *) glGetString(GL_EXTENSIONS));
     printf("GL_RENDERER: %s\n", (char *) glGetString(GL_RENDERER));
     printf("GL_VENDOR: %s\n", (char *) glGetString(GL_VENDOR));
     printf("GLU_VERSION: %s\n", (char *) gluGetString(GLU_VERSION));
     printf("GLU_EXTENSIONS: %s\n", (char *) gluGetString(GLU_EXTENSIONS));
     printf("GLUT_API_VERSION: %d\n", GLUT_API_VERSION);
#endif


#ifdef FULLSCREEN
    glutFullScreen();
#endif

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
    if(argc>1) scrLoad(argv[1]); else scrLoad("alpha2.scr");
    adk_time=0.0; // adk_frame=0.0;
    
//    fx_debug=fopen("effects.log","wt");

    printf("lightmap loading\n");
    lightmap=LoadSimpleMap("light.bmp");
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


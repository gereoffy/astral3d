#define FPS 1

#include <stdio.h>
#include <stdlib.h>

#include "agl/agl.h"
#include <GL/glut.h>

#include "3dslib/ast3d.h"
#include "render/render.h"
#include "loadmap/loadmaps.h"
#include "timer/timer.h"
#include "afs/afs.h"
// #include "render/particle.h"

#define DEFINE_PROF_VARS
#include "prof.h"

int     step_frame = 0;
int     play_frame = 2;
int     play_back_frame = 0;
float   frames, frame;

int   mfps_frames=0;
float mfps_time=0;
float mfps_lasttime=0;
int window_w,window_h;

GLvoid draw_scene()
{
  ++mfps_frames;mfps_time+=GetRelativeTime();
  if(mfps_time>5+mfps_lasttime){
    if(mfps_frames) printf("FPS = %5.2f\n",mfps_frames/mfps_time);
    mfps_lasttime=mfps_time;
//    mfps_time=0; mfps_frames=0;
  }

/*---------------------- DRAW A FRAME ------------------------------------*/    
PROF_START(prof_total);

  PROF_START(prof_clear);
    aglInit();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  PROF_END(prof_clear);
  PROF_START(prof_update);
      ast3d_blend=1.0;
      ast3d_setframe(frame); // printf("Current frame = %f\n",frame);
      scene->cam->aspectratio=(float)window_w/(float)window_h;
//      printf("aspect ratio = %f\n",scene->cam->aspectratio);
//      ast3d_update();
  PROF_END(prof_update);
  PROF_START(prof_render);
      draw3dsframe(0.01);
  PROF_END(prof_render);
  PROF_START(prof_finish);
    glFinish();
  PROF_END(prof_finish);

//#define GANY
#ifdef GANY
    glBindTexture(GL_TEXTURE_2D,lightmap);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128,128, 256,256, 0);
    lights[0]->corona_scale=10;
#endif

  PROF_START(prof_swapbuffers);
    glutSwapBuffers();
  PROF_END(prof_swapbuffers);

PROF_END(prof_total);
/*------------------------------------------------------------------------*/    

}


GLvoid resize_window(int w, int h){ 
  glViewport(0,0,w,h); window_w=w;window_h=h;
}


void mouse(int button, int state, int x, int y) 
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         ++play_frame;
         break;
      case GLUT_MIDDLE_BUTTON:
	       resize_window(640,480);
//         directional_lighting^=1;refresh=1;
         break;
      case GLUT_RIGHT_BUTTON:
          ++play_back_frame;
         break;
      default:
         break;
   }
}

void ExitDemo(){
#ifdef PROFILING
  printf("============= PROFILING RESULTS ===================\n");
#define PROF_PRINT(str,var) printf("%-15s  %10d  (%7.3f%%)\n",str,var/mfps_frames,100.0F*(float)var/(float)prof_total);
  PROF_PRINT("TOTAL:",prof_total);
  PROF_PRINT("glClear:",prof_clear);
  PROF_PRINT("ast3d_Update:",prof_update);
  PROF_PRINT("render.c:",prof_render);
  PROF_PRINT("->glMatrixMode:",prof_3d_matrixmode);
  PROF_PRINT("->setup_light:",prof_3d_setuplight);
  PROF_PRINT("->calc:",prof_3d_calc);
  PROF_PRINT("  1-transform:",prof_3d_transform);
  PROF_PRINT("  1a-frustcull:",prof_3d_frustumcull);
  PROF_PRINT("  2-material:",prof_3d_material);
  PROF_PRINT("  3-lighting:",prof_3d_lighting);
  PROF_PRINT("->draw:",prof_3d_draw);
  PROF_PRINT("  4-texture:",prof_3d_texture);
  PROF_PRINT("  5-envmap:",prof_3d_envmap);
  PROF_PRINT("  6-lightmap:",prof_3d_lightmap);
  PROF_PRINT("  7-specmap:",prof_3d_specmap);
  PROF_PRINT("->put_lcoronas:",prof_3d_lightcorona);
  PROF_PRINT("glFinish:",prof_finish);
  PROF_PRINT("SwapBuffers:",prof_swapbuffers);
  printf("===================================================\n");
#endif
  exit(0);
}

/* exit upon ESC */
static void key(unsigned char k, int x, int y){
  switch (k) {
  case 27:  /* Escape */
    ExitDemo();
  case 'p':  printf("frame=%f\n",frame);
  default:
    return;
  }
}

static void idle(void){
	if(play_frame&2){
    if (frame<frames) frame+=FPS; else ast3d_getframes(&frame,&frames);
    glutPostRedisplay();
	} else
	if(play_back_frame&2){
    frame-=FPS; if(frame<=0) frame=frames;
    glutPostRedisplay();
	}
//    if(mfps_frames>=400) ExitDemo();
}

int main(int argc,char* argv[]){
int err;
char *filename="2.3ds";

    if(argc>1) filename=argv[1];
    afs_init("",AFS_TYPE_FILES);
    afs_init("astral.pak",AFS_TYPE_PAK);

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (640, 480);
//    glutInitWindowSize (1024, 768);
//    glutInitWindowSize (1280, 1024);
    glutInitWindowPosition (0, 0);
    glutCreateWindow (argv[0]);

//    glutFullScreen();

    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth( 1.0 );
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    
//    glEnable(GL_POLYGON_SMOOTH);
//    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

        ast3d_init (ast3d_transform | 
                    ast3d_hierarchy | 
                    ast3d_domorph   |
                    ast3d_normcalc  |
                    ast3d_calcnorm  );

    ast3d_alloc_scene (&scene);

    err = ast3d_load_world (filename,scene);
    if (err != ast3d_err_ok)
      {printf("Load World() failed: %s\n",ast3d_geterror(err));exit(1);}
    else
      printf("World loaded!\n");

    err = ast3d_load_motion (filename,scene);
    if (err != ast3d_err_ok)
      {printf("Load Motion() failed: %s\n",ast3d_geterror(err));exit(1);}
    else
      printf("Motion loaded!\n");

    ast3d_setactive_scene (scene);
    if((FindCameras(scene)==0) || (!scene->cam)){printf("No camera found!!\n");exit(1);}
    printf("Default camera: %s\n",scene->cam->name);
    ast3d_setactive_camera(scene->cam);
    
//    ast3d_fixUV(NULL);

    printf("Loading materials...\n");	
    LoadMaterials(scene);
    lightmap=LoadSimpleMap("light.bmp",512+1024);
    printf("Materials OK\n");

    ast3d_getframes(&frame, &frames);
    printf("FRAMES:  %f - %f\n",frame,frames);

    InitTimer();

    resize_window(640,480);
    glutDisplayFunc(draw_scene);
    glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    glutReshapeFunc(resize_window);
    glutIdleFunc(idle);
    glutMainLoop();

return 0;
}


// Sinus particle system

#include "../config.h"
#include "../agl/agl.h"

//#include "../3dslib/ast3d.h"
//#include "../3dslib/vector.h"

#include "sinzoom.h"

extern int window_w,window_h;
extern float ast3d_blend;

#define FALSE 0
#define TRUE 1

#define SCR_x 400
#define SCR_y 400

//static float vrnd(void){ return(((float)rand())/RAND_MAX); }

void draw_sinzoom(float frame,fx_sinzoom_struct *params){
  int i;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 640, 0.0f, 480, -10000.0f, 10000.0f);
//  gluPerspective(60.0,(GLfloat) window_w/(GLfloat) window_h, 1.0, 10000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_FOG);
  aglZbuffer(AGL_ZBUFFER_NONE);
  aglTexture(params->texture);
  aglBlend(AGL_BLEND_ADD);

  glBegin(GL_QUADS);  
  for(i=0;i<params->partnum;i++){
    float b=ast3d_blend*(1-(float)i/((float)params->partnum+1.0f));
    float s=params->scale*sin(frame);
//    if(s<0) s=0;
    s=params->size*(1+s);
//    printf("s=%8.3f\n",s);

  glColor3f(b*params->color[0],
            b*params->color[1],
            b*params->color[2]);

//    printf("xyz:  %f  %f  %f   s=%f  b=%f\n",x,y,z,s,b);
    glTexCoord2f(0.0,0.0); glVertex3f(params->x-s,params->y-s,0);
    glTexCoord2f(1.0,0.0); glVertex3f(params->x+s,params->y-s,0);
    glTexCoord2f(1.0,1.0); glVertex3f(params->x+s,params->y+s,0);
    glTexCoord2f(0.0,1.0); glVertex3f(params->x-s,params->y+s,0);
    // continue...
    frame+=params->d_phase;
  }
  glEnd();
  
}


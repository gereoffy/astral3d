#define CIRCLE_U 64
#define CIRCLE_V 64

#include "../../config.h"
#include "../../agl/agl.h"

#include "swirl.h"

#include "../../3dslib/ast3d.h"

/* ************************************************************************** */

extern float ast3d_blend;

/* ************************************************************************** */

static c_VECTOR circle_p[CIRCLE_U+1];
static float circle_t[CIRCLE_U+1][2];

static void DrawCircle(float pos,fx_swirl_struct *fx){
int ui,vi;
float u,v,ev;

v=0;
for(vi=0;vi<=CIRCLE_V;vi++){
  float R=fx->scale*v;
//  float St=1.0f; //+0.2*sin(15*v+3*pos);
  float St=fx->texturescale+fx->wave_amp*sin(fx->wave_freq*v+fx->wave_phase);
//  float Rt=0.5*v;
  float Rt=St*pow(v,fx->lens);
//  float Rt=0.5*(sin(v*M_PI/2.0f));
//  float Rt=0.5*(1-(1-v)*(1-v));
//  float Rt=0.5*pow(v,1+0.9*sin(pos));
//  float Dt=sin(pos)*(5-sin(pos)*4*v);
//  float Dt=sin(pos)*(1-v*sin(pos+0.3));
  float Dt=fx->twist_inner*cos(pos)+fx->twist_outer*v*sin(pos);
  u=0;
  if(vi) glBegin(GL_TRIANGLE_STRIP);
  for(ui=0;ui<=CIRCLE_U;ui++){
    if(vi){
      glTexCoord2fv(circle_t[ui]);
      glVertex3fv(&circle_p[ui].x);
    }
    circle_p[ui].x=fx->x+R*cos(u);
    circle_p[ui].y=fx->y+R*sin(u);
    circle_p[ui].z=-20;
    circle_t[ui][0]=0.5f+Rt*cos(u+Dt);
    circle_t[ui][1]=0.5f-Rt*sin(u+Dt);
    if(vi){
      glTexCoord2fv(circle_t[ui]);
      glVertex3fv(&circle_p[ui].x);
    }
    u+=2.0f*M_PI/(float)CIRCLE_U;
  }
  if(vi) glEnd();
  ev=v;v+=1.0f/(float)CIRCLE_V;
}

}

/* ************************************************************************** */
/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */

#define BLUR_LEVEL 10

void SWIRL_Render(float pos,fx_swirl_struct *fx){
int i;
float a,as;
float blur_alpha[BLUR_LEVEL];
//  glClear(GL_COLOR_BUFFER_BIT);
  aglZbuffer(AGL_ZBUFFER_NONE);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(60,640.0f/480.0f,1,1000);
//  glOrtho(0.0f, 400, 0.0f, 400, -10000.0f, 10000.0f);

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
//  glTranslatef(0,0,-20);
//  glRotatef(50*pos,0,1,0.3);

  a=1.0f;as=0;
  for(i=0;i<fx->blur_level;i++){
    blur_alpha[i]=a;as+=a;
    a*=fx->blur_alpha;
  }
  
//  ast_LookAt(v1.x,v1.y,v1.z,  v2.x,v2.y,v2.z,  0,1,0 );

  aglTexture(fx->texture);
  for(i=0;i<fx->blur_level;i++){
    float a=ast3d_blend*blur_alpha[i]/as;
    glColor4f(a,a,a,a);
    aglBlend((i||fx->type)?AGL_BLEND_ADD:AGL_BLEND_ALPHA);
    DrawCircle(pos-fx->blur_pos*i,fx);
  }

//  glFinish();
//  glutSwapBuffers();
}


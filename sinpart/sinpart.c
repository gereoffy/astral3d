// Sinus particle system

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#define M_PI 3.1415926527
#endif

#include "../agl/agl.h"

#include "../3dslib/ast3d.h"
#include "../3dslib/vector.h"

#include "sinpart.h"

extern int window_w,window_h;
extern float ast3d_blend;

#define FALSE 0
#define TRUE 1

#define SCR_x 400
#define SCR_y 400

static float vrnd(void){ return(((float)rand())/RAND_MAX); }

void draw_sinpart(float frame,fx_sinpart_struct *params){
  int i,j;
  float szog=frame*params->speed;
  float d_szog;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,(GLfloat) window_w/(GLfloat) window_h, 1.0, 10000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_FOG);
  aglZbuffer(AGL_ZBUFFER_NONE);
  aglTexture(params->texture);
  aglBlend(AGL_BLEND_ADD);

  d_szog=2.0F*M_PI;
  d_szog/=params->partnum;
//  printf("dszog=%f\n",d_szog);

//  printf("size=%f\n",params->size);

  glBegin(GL_QUADS);  
  for(i=0;i<params->partnum;i++){
    float x=0,y=0,z=0,s=0,b=0;
    float szog2=szog;
    float f=frame*params->sinspeed;
    for(j=0;j<params->sinnum;j++){
      float sinus=sin(szog2);
      x+=sinus*params->amp_sin_x[j]*sin(params->speed_sin_x[j]*f);
      y+=sinus*params->amp_sin_y[j]*sin(params->speed_sin_y[j]*f);
      z+=sinus*params->amp_sin_z[j]*sin(params->speed_sin_z[j]*f);
      s+=sinus*params->amp_sin_size[j]*sin(params->speed_sin_size[j]*f);
      b+=sinus*params->amp_sin_bright[j]*sin(params->speed_sin_bright[j]*f);
      szog2+=szog;
    }
    // let's draw it!
    x=x*params->size+params->ox;
    y=y*params->size+params->oy;
    z=z*params->size+params->oz;
    b=(0.5F+b)*ast3d_blend; glColor3f(b,b,b);
    s*=params->scale; if(s<20.0F) s=20.0F;
//    printf("xyz:  %f  %f  %f   s=%f  b=%f\n",x,y,z,s,b);
    glTexCoord2f(0.0,0.0); glVertex3f(x-s,y-s,z);
    glTexCoord2f(1.0,0.0); glVertex3f(x+s,y-s,z);
    glTexCoord2f(1.0,1.0); glVertex3f(x+s,y+s,z);
    glTexCoord2f(0.0,1.0); glVertex3f(x-s,y+s,z);
    // continue...
    szog+=d_szog;
  }
  glEnd();
  
}


void draw_sinpart2(float frame,fx_sinpart_struct *params){
  int i,j;
  float szog=frame*params->speed;
  float d_szog;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,(GLfloat) window_w/(GLfloat) window_h, 1.0, 10000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_FOG);
  aglZbuffer(AGL_ZBUFFER_NONE);
  aglTexture(params->texture);
  aglBlend(AGL_BLEND_ADD);

  d_szog=2.0F*M_PI;
  d_szog/=params->partnum;
//  printf("dszog=%f\n",d_szog);

//  printf("size=%f\n",params->size);

  glBegin(GL_QUADS);  
  for(i=0;i<params->partnum;i++){
    float x=0,y=0,z=0,s=0,b=0;
    c_VECTOR t,n,up;
    float szog2=szog;
    float f=frame*params->sinspeed;
    for(j=0;j<params->sinnum;j++){
      float sinus=sin(szog2);
      x+=sinus*params->amp_sin_x[j]*sin(params->speed_sin_x[j]*f);
      y+=sinus*params->amp_sin_y[j]*sin(params->speed_sin_y[j]*f);
      z+=sinus*params->amp_sin_z[j]*sin(params->speed_sin_z[j]*f);
      s+=sinus*params->amp_sin_size[j]*sin(params->speed_sin_size[j]*f);
      b+=sinus*params->amp_sin_bright[j]*sin(params->speed_sin_bright[j]*f);
      szog2+=szog;
    }
    t.x=-x; t.y=-y; t.z=-z;
    szog2=szog; f+=0.01;
    for(j=0;j<params->sinnum;j++){
      float sinus=sin(szog2);
      t.x+=sinus*params->amp_sin_x[j]*sin(params->speed_sin_x[j]*f);
      t.y+=sinus*params->amp_sin_y[j]*sin(params->speed_sin_y[j]*f);
      t.z+=sinus*params->amp_sin_z[j]*sin(params->speed_sin_z[j]*f);
      szog2+=szog;
    }
    up.x=0;up.y=1;up.z=0;
    vec_normalize(&t,&t);
    vec_cross(&t,&up,&n);
    vec_normalize(&n,&n);
    vec_cross(&t,&n,&up);
    
    // let's draw it!
    x=x*params->size+params->ox;
    y=y*params->size+params->oy;
    z=z*params->size+params->oz;
    b=(0.5F+b)*ast3d_blend; glColor3f(b,b,b);
    s*=params->scale; if(s<20.0F) s=20.0F;
//    printf("xyz:  %f  %f  %f   s=%f  b=%f\n",x,y,z,s,b);
#define MKVERT(xm,ym) glVertex3f(x+xm*n.x+ym*up.x,y+xm*n.y+ym*up.y,z+xm*n.z+ym*up.z)
    glTexCoord2f(0.0,0.0); MKVERT(-s,-s);
    glTexCoord2f(1.0,0.0); MKVERT(+s,-s);
    glTexCoord2f(1.0,1.0); MKVERT(+s,+s);
    glTexCoord2f(0.0,1.0); MKVERT(-s,+s);
#undef MKVERT
    // continue...
    szog+=d_szog;
  }
  glEnd();
  
}


void sinpart_init(fx_sinpart_struct *params,int texture,int partnum,int sinnum){
int i;
  params->partnum=partnum;
  params->sinnum=sinnum;
  params->texture=texture;
  
  params->amp_sin_x=malloc(sizeof(float)*sinnum);
  params->amp_sin_y=malloc(sizeof(float)*sinnum);
  params->amp_sin_z=malloc(sizeof(float)*sinnum);
  params->amp_sin_size=malloc(sizeof(float)*sinnum);
  params->amp_sin_bright=malloc(sizeof(float)*sinnum);

  params->speed_sin_x=malloc(sizeof(float)*sinnum);
  params->speed_sin_y=malloc(sizeof(float)*sinnum);
  params->speed_sin_z=malloc(sizeof(float)*sinnum);
  params->speed_sin_size=malloc(sizeof(float)*sinnum);
  params->speed_sin_bright=malloc(sizeof(float)*sinnum);

  for(i=0;i<sinnum;i++){
    float size=10.0/((float)i+10.0);
    float speed=1.0;
    params->amp_sin_x[i]=vrnd()*size;
    params->amp_sin_y[i]=vrnd()*size;
    params->amp_sin_z[i]=vrnd()*size;
    params->amp_sin_size[i]=vrnd()*size;
    params->amp_sin_bright[i]=vrnd()*size;
    params->speed_sin_x[i]=vrnd()*speed;
    params->speed_sin_y[i]=vrnd()*speed;
    params->speed_sin_z[i]=vrnd()*speed;
    params->speed_sin_size[i]=vrnd()*speed;
    params->speed_sin_bright[i]=vrnd()*speed;
  }
  
  params->speed=1;
  params->sinspeed=0.01;
  params->ox=0;
  params->oy=0;
  params->oz=-1000;
  params->size=300;
  params->scale=10;

}


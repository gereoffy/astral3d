#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#define M_PI 3.1415926527
#define inline _inline
#endif

#include "../agl/agl.h"

#include "../loadmap/load_map.h"

#include "fdwater.h"

#include "../3dslib/ast3d.h"
#include "../3dslib/vector.h"

extern int window_w,window_h;
extern float ast3d_blend;

#define FALSE 0
#define TRUE 1

#define BRIGHT 4*65536
#define TILE_SIZE 8
#define SCR_x 400
#define SCR_y 400

inline float interppixel(map_st* map,float u,float v){
  int ui,ui1,vi;
  float h1,h2;
  u-=(int)u; v-=(int)v;
  u*=map->xsize; v*=map->ysize; ui=u;u-=ui; vi=v;v-=vi;
  ui1=ui+1;if(ui1>=map->xsize) ui1-=map->xsize;
  h1=map->map[vi*map->xsize+ui]; h1=h1+u*(map->map[vi*map->xsize+ui1]-h1);
  ++vi;if(vi>=map->ysize) vi-=map->ysize;
  h2=map->map[vi*map->xsize+ui]; h2=h2+u*(map->map[vi*map->xsize+ui1]-h2);
  return h1+v*(h2-h1);
}

static float texcoord[81][3];
static unsigned char bright[81][2];

#define SQR(x) ((x)*(x))

static inline float ripple(float frame,float u,float v){
  float d1=5*sqrt(SQR(u-0.3)+SQR(v-0.2));
  float d2=5*sqrt(SQR(u-0.6)+SQR(v-0.6));
  return sin(frame+d1)+sin(frame+d2+1.2463);
}

void draw_fdwater(float frame,fx_fdwater_struct *params){
float u_offs1=1024+frame*params->u_speed1;
float v_offs1=1024+frame*params->v_speed1;
float u_offs2=1024+frame*params->u_speed2;
float v_offs2=1024+frame*params->v_speed2;
int x,y;
float u,v;
float du,dv;
int flag=0;

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_FOG);
  aglZbuffer(AGL_ZBUFFER_NONE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 80, 0.0f, 60, -10000.0f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  for(y=0;y<=60;y++){
    v=(float)y/60.0f;
    //----------- pass 1 ----------------
    aglTexture(params->texture);
    if(params->type&2)
      aglBlend(AGL_BLEND_ADD);
    else
      aglBlend((ast3d_blend==1)?AGL_BLEND_NONE:AGL_BLEND_ALPHA);
//    glColor3f(1,1,1);
//    glColor3f(0,0,0);
//    glColor3fv(params->color);
    glColor4f(params->color[0],params->color[1],params->color[2],ast3d_blend);
    glBegin(GL_TRIANGLE_STRIP);
    for(x=0;x<=80;x++){
      float u1,v1,u2,v2;
      float h1,h2,h3;
      u=(float)x/80.0f;
if(!(params->type&1)){
  // Heightmapped WATER
      if(y){
        glTexCoord2f(texcoord[x][0],texcoord[x][1]);
        glVertex2f(x,y-1);
      }
      u1=u_offs1+params->scale*u; v1=v_offs1+params->scale*v;
      u2=u_offs2+params->scale*u; v2=v_offs2+params->scale*v;
      h1=interppixel(&params->map1,u1,v1)-interppixel(&params->map2,u2,v2);
      h2=interppixel(&params->map1,u1+0.05,v1)-interppixel(&params->map2,u2+0.05,v2);
      h3=interppixel(&params->map1,u1,v1+0.05)-interppixel(&params->map2,u2,v2+0.05);
      h2-=h1;h3-=h1;
      du=params->amp*h2;
      dv=params->amp*h3;
      texcoord[x][0]=u+du;
      texcoord[x][1]=1-(v+dv);
      h1*=h1;
      h1*=h1*params->bright;
//      h1=h2*=params->bright;
      if(h1>255)h1=255;
      if(h1<0)h1=0;
      bright[x][flag]=h1;
      if(y){
        glTexCoord2f(texcoord[x][0],texcoord[x][1]);
        glVertex2f(x,y);
      }
} else {
  // Water Ripples
      if(y){
        float b=texcoord[x][2];
        glColor3f(params->color[0]*b,params->color[1]*b,params->color[2]*b);
        glTexCoord2f(texcoord[x][0],texcoord[x][1]);
        glVertex2f(x,y-1);
      }

#define RIPPLE(u,v,a,s) sin(a+s*sqrt(SQR(u)+SQR(v)))/(1+params->scale*sqrt(SQR(u)+SQR(v)))
#define RIPPLE1(u,v) RIPPLE(u-0.25,v-0.25,u_offs1,params->v_speed1)
#define RIPPLE2(u,v) RIPPLE(u-0.8,v-0.4,u_offs2+1.245362,params->v_speed2)
#define RIPPLE3(u,v) RIPPLE(u-0.4,v-0.8,u_offs2+1.945362,params->v_speed2)
#define RIPPLE12(u,v) RIPPLE1(u,v)-RIPPLE2(u,v)+RIPPLE3(u,v)
//#define RIPPLE12(u,v) RIPPLE1(u,v)
      h1=RIPPLE12(u,v);
      h2=RIPPLE12(u+0.05,v)-h1;
      h3=RIPPLE12(u,v+0.05)-h1;

#if 1
      texcoord[x][2]=(params->bright)/(sqrt(h2*h2+h3*h3+0.125)+0.001)+0.8f;
#else
      texcoord[x][2]=(h2)*params->bright+0.8f;
#endif
      du=params->amp*h2;
      dv=params->amp*h3;

      texcoord[x][0]=0.5+0.95*(u+du-0.5);
      texcoord[x][1]=0.5-0.95*(v+dv-0.5);

//      h1*=h1*params->bright;
//      h1=sqrt(h2*h2+0.3*h3*h3)*params->bright+32;
//      if(h1>255)h1=255;
//      if(h1<0)h1=0;
//      bright[x][flag]=h1;

      if(y){
        float b=texcoord[x][2];
        glColor3f(params->color[0]*b,params->color[1]*b,params->color[2]*b);
        glTexCoord2f(texcoord[x][0],texcoord[x][1]);
        glVertex2f(x,y);
      }

}
    }
    glEnd();
    if(!(params->type&1)) if(params->bright) if(y){
      //----------- pass 2 ----------------
      glDisable(GL_TEXTURE_2D);
      aglBlend(AGL_BLEND_ADD);
      glBegin(GL_TRIANGLE_STRIP);
      for(x=0;x<=80;x++){
        unsigned char b=bright[x][1-flag];
        glColor3ub(b,b,b); glVertex2f(x,y-1);
        b=bright[x][flag];
        glColor3ub(b,b,b); glVertex2f(x,y);
      }
      glEnd();
      glEnable(GL_TEXTURE_2D);
    } // if(y)
    flag=1-flag;
  }

//  u_offs+=0.01;
//  v_offs+=0.02753;

}


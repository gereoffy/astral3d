#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#define M_PI 3.1415926527
#endif

#include "../agl/agl.h"

#include "fdtunnel.h"

extern int window_w,window_h;
extern float ast3d_blend;

#define FALSE 0
#define TRUE 1

#define BRIGHT 4*65536
#define TILE_SIZE 8
#define SCR_x 400
#define SCR_y 400

typedef struct { float u,v,bright;} POINT_ST;
static POINT_ST points[400];

static float  rmat[3][3];
static float  alpha,beta,fd_gamma,zpos;
static float radmod=0,radszog=5;

static float fd_bright=1.0;

typedef struct { float x,y,z;}vector;
static vector ovec,pvec;

static void matrix_calc(){
float sxsz,sxcz,szcx,cxcz;
  sxsz=sin(alpha)*sin(fd_gamma);
  sxcz=sin(alpha)*cos(fd_gamma);
  szcx=sin(fd_gamma)*cos(alpha);
  cxcz=cos(alpha)*cos(fd_gamma);

  rmat[0][0]=-cos(beta)*cos(fd_gamma);
  rmat[0][1]=cos(beta)*sin(fd_gamma);
  rmat[0][2]=-sin(beta);
  rmat[1][0]=-sin(beta)*sxcz-szcx;
  rmat[1][1]=sin(beta)*sxsz+cxcz;
  rmat[1][2]=sin(alpha)*cos(beta);
  rmat[2][0]=-sin(beta)*cxcz+sxsz;
  rmat[2][1]=sin(beta)*szcx-sxcz;
  rmat[2][2]=cos(alpha)*cos(beta);
}


static void fd_tunnel(int x,int y,POINT_ST* p,float radius){
   vector ivec,dvec;
   float  a,b,c,t;
   float  u,v;
   float bright;

        // Calculate direction
        dvec.x=pvec.x*rmat[0][0]+pvec.y*rmat[1][0]+pvec.z*rmat[2][0];
        dvec.y=pvec.x*rmat[0][1]+pvec.y*rmat[1][1]+pvec.z*rmat[2][1];
        dvec.z=pvec.x*rmat[0][2]+pvec.y*rmat[1][2]+pvec.z*rmat[2][2];
        if(radmod) radius+=radmod*sin(atan2(dvec.x,dvec.y)*radszog);

        a = dvec.x*dvec.x + dvec.y*dvec.y;
        b = 2*(ovec.x*dvec.x + ovec.y*dvec.y);
        c = ovec.x*ovec.x + ovec.y*ovec.y - radius*radius;
        t = (-b + sqrt(b*b - 4*a*c)) / (2*a+0.0000001);
        ivec.x = ovec.x + dvec.x*t;
        ivec.y = ovec.y + dvec.y*t;
        ivec.z = ovec.z + dvec.z*t;
        u = ((ivec.z)*0.2/256);
        v = (fabs(atan2(ivec.y, ivec.x)/M_PI));
        bright=fd_bright/t; // BRIGHT/(t*1556);
//        bright=(bright>=1.0)?ast3d_blend:bright*ast3d_blend;

        if(y>0){
          glTexCoord2f(p->u,p->v);
          glColor4f(p->bright,p->bright,p->bright,ast3d_blend);
          glVertex2i(x,y-TILE_SIZE);
          glTexCoord2f(u,v);
          glColor4f(bright,bright,bright,ast3d_blend);
          glVertex2i(x,y);
        }
        p->u=u;p->v=v;p->bright=bright;
}

void draw_fdtunnel(float frame,fx_fdtunnel_struct *params){
  int x,y,j;

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_FOG);
  aglZbuffer(AGL_ZBUFFER_NONE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 400, 0.0f, 400, -10000.0f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

//  glViewport(0, 0, window_w, window_h);

  alpha=frame*params->speed[0];
  beta=frame*params->speed[1];
  zpos=frame*params->speed[2];
  matrix_calc();
  ovec.x=0;ovec.y=0;ovec.z=zpos;

for(j=0;j<=1;j++) if(params->texture[j]>0){
  if(j==0){
    aglBlend((ast3d_blend<1.0)?AGL_BLEND_ALPHA:AGL_BLEND_NONE);
    fd_bright=params->bright[j];
  } else {
    aglBlend(AGL_BLEND_ADD);
    fd_bright=params->bright[j]*ast3d_blend; //*BRIGHT/1556;
  }
  aglTexture(params->texture[j]);
  pvec.z=params->persp[j];
  radmod=params->rad_amp[j]*sin(frame*params->rad_speed[j]);
  radszog=params->rad_szog[j];
  for(y=0;y<=SCR_y;y+=TILE_SIZE){
    POINT_ST *p=&points[0];
    pvec.y=(y-200)/params->fovy[j];
    glBegin(GL_QUAD_STRIP);
    for(x=0;x<=SCR_x;x+=TILE_SIZE){
      pvec.x=(x-200)/params->fovx[j];
      fd_tunnel(x,y,p++,768);
    }  
    glEnd();
  }
}

}

void fdtunnel_setup(fx_fdtunnel_struct *fd,int j,
               float sp1,float sp2,float sp3,
               float bright,
               float persp,float fovx,float fovy,
               float radius, float rad_speed,float rad_szog,float rad_amp){
  fd->speed[0]=sp1;
  fd->speed[1]=sp2;
  fd->speed[2]=sp3;
  fd->bright[j]=bright;
  fd->persp[j]=persp;
  fd->fovx[j]=fovx;
  fd->fovy[j]=fovy;
  fd->radius[j]=radius;
  fd->rad_speed[j]=rad_speed;
  fd->rad_szog[j]=rad_szog;
  fd->rad_amp[j]=rad_amp;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

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
static float	     alpha,beta,fd_gamma,rad_alpha,zpos;
static float radmod=0;

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
        if(radmod) radius+=radmod*sin(atan2(dvec.x,dvec.y)*5);

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

void draw_fdtunnel(float frame,int texture,int texture2){
  int x,y;

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);  /* thanx to reptile/ai */
           

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 400, 0.0f, 400, -10000.0f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

//  glViewport(0, 0, window_w, window_h);

  glEnable(GL_TEXTURE_2D);

   alpha=frame*0.008f;
   beta=frame*0.009736f;
//   fd_gamma+=0.00456735f;
   zpos=frame*6;
   rad_alpha=frame*0.02354f;
   matrix_calc();

//   glMatrixMode(GL_PROJECTION);

  ovec.x=0;ovec.y=0;ovec.z=zpos;
  pvec.z=3;

  if(ast3d_blend<1){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else glDisable(GL_BLEND);  
  fd_bright=BRIGHT/1556;
  
  glBindTexture(GL_TEXTURE_2D, texture);

  radmod=0; //128*sin(rad_alpha);
  for(y=0;y<=SCR_y;y+=TILE_SIZE){
    POINT_ST *p=&points[0];
    pvec.y=(y-200)/120.0f;
    glBegin(GL_QUAD_STRIP);
    for(x=0;x<=SCR_x;x+=TILE_SIZE){
      pvec.x=(x-200)/120.0f;
      fd_tunnel(x,y,p++,768);
    }  
    glEnd();
  }


if(texture2>0){
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  fd_bright=ast3d_blend*BRIGHT/1556;

  glBindTexture(GL_TEXTURE_2D, texture2);

  radmod=128*sin(rad_alpha);
  for(y=0;y<=SCR_y;y+=TILE_SIZE){
    POINT_ST *p=&points[0];
    pvec.y=(y-200)/120.0f;
    glBegin(GL_QUAD_STRIP);
    for(x=0;x<=SCR_x;x+=TILE_SIZE){
      pvec.x=(x-200)/120.0f;
      fd_tunnel(x,y,p++,300);
    }  
    glEnd();
  }
}

  glDepthMask(GL_TRUE);  /* thanx to reptile/ai */
  glEnable(GL_DEPTH_TEST);
//  glutSwapBuffers();
}



/*
 * written by David Bucciarelli (tech.hmw@plus.it)
 *            Humanware s.r.l.
 * hacked by Strepto / Astral  :)      
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include "../3dslib/ast3d.h"
#include "render.h"

#define vinit(a,i,j,k) {\
  (a)[0]=i;\
  (a)[1]=j;\
  (a)[2]=k;\
} 

#define AGRAV -9.8

typedef struct {
  float color[3];
  float energy;
  c_VECTOR p; // float p[1][3];
  float v[3];
} part;

static int np;
static float eject_r,dt,eject_vy,eject_vl;
static float ridtri;
static part *p;

static float vrnd(void)
{
  return(((float)rand())/RAND_MAX);
}

static void setnewpart(part *p)
{
  float a,v[3];

  a=vrnd()*3.14159265359*2.0;

  vinit(v,sin(a)*eject_r*vrnd(),0.15,cos(a)*eject_r*vrnd());
  p->p.x=v[0]+vrnd()*ridtri;
  p->p.y=v[1]+vrnd()*ridtri;
  p->p.z=v[2]+vrnd()*ridtri;
//  vinit(p->p[0],v[0]+vrnd()*ridtri,v[1]+vrnd()*ridtri,v[2]+vrnd()*ridtri);
  vinit(p->color,vrnd(),vrnd(),vrnd());
  vinit(p->v,v[0]*eject_vl/(eject_r/2),
             vrnd()*eject_vy+eject_vy/2,
             v[2]*eject_vl/(eject_r/2));
  p->energy=1.0;
}

static void setpart(part *p)
{

  if(p->p.y<0.1) {
    setnewpart(p);
    return;
  }

  p->v[1]+=AGRAV*dt;
  p->p.x+=dt*p->v[0];
  p->p.y+=dt*p->v[1];
  p->p.z+=dt*p->v[2];
}


void particle_redraw(c_VECTOR *pos){
  int	j;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ONE);
//  glShadeModel(GL_FLAT);

  glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    for(j=0;j<np;j++) {
      c_VECTOR pt;
      float p_scale;
//      glColor3f(p[j].energy,p[j].energy,p[j].energy);

      { c_VECTOR b;
#define cammat scene->cam->matrix
        b.x=p[j].p.x+pos->x;
        b.y=p[j].p.y+pos->y;
        b.z=p[j].p.z+pos->z;
        pt.x= b.x*cammat[X][X] + b.y*cammat[X][Y] + b.z*cammat[X][Z] + cammat[X][W];
        pt.y= b.x*cammat[Y][X] + b.y*cammat[Y][Y] + b.z*cammat[Y][Z] + cammat[Y][W];
        pt.z= b.x*cammat[Z][X] + b.y*cammat[Z][Y] + b.z*cammat[Z][Z] + cammat[Z][W];
#undef cammat
      } // mat_mulvec(scene->cam->matrix,&p[j].p,&pt);  /* Transformation */
      p_scale=(0.2/(p[j].energy*2));

      glColor3fv(p[j].color);
      glTexCoord2f(0.0,0.0);
      glVertex3f(pt.x-p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,0.0);
      glVertex3f(pt.x+p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,1.0);
      glVertex3f(pt.x+p_scale,pt.y+p_scale,pt.z);
      glTexCoord2f(0.0,1.0);
      glVertex3f(pt.x-p_scale,pt.y+p_scale,pt.z);
      p[j].energy-=0.01;
#if 1
      p[j].color[0]-=0.05*p[j].color[0];
      p[j].color[1]-=0.05*p[j].color[1];
      p[j].color[2]-=0.05*p[j].color[2];
#else
      p[j].color[0]-=0.01;
      p[j].color[1]-=0.01;
      p[j].color[2]-=0.01;
#endif
      setpart(&p[j]);
    }
    glEnd();

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

}

void particle_init(){
int i;
  np=800;
  eject_r=0.1;
  dt=0.015;
  eject_vy=4;
  eject_vl=1;
  ridtri=0.1;

  p=malloc(sizeof(part)*np);

  for(i=0;i<np;i++) setnewpart(&p[i]);
}



/*
 * written by A'rpi & Strepto /ASTRAL
 */

//typedef struct _c_PART {
//  float color[3];
//  float energy;
//  c_VECTOR p;
//  float v[3];
//} c_PART;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#endif

#include "../agl/agl.h"

#include "../3dslib/ast3d.h"
#include "render.h"
//#include "particle.h"

#define RND ((float)rand()/(float)RAND_MAX)

static INLINE void init_particle(c_PART *p){
  p->p.x=
  p->p.y=
  p->p.z=0;
  p->v[0]=3*(RND*2-1);
  p->v[1]=3*(RND*2-1);
  p->v[2]=3*(RND*2-1);
  p->color[0]=RND*0.3+0.5;
  p->color[1]=RND*0.3+0.5;
  p->color[2]=RND*0.3+0.5;
  p->energy=1+0.657*RND;
}

static INLINE void move_particle(c_PART *p,float a){
//  p->e*=0.99;
  if(p->energy<0.02) init_particle(p);
  p->p.x+=p->energy*a*p->v[0];
  p->p.y+=p->energy*a*p->v[1];
  p->p.z+=p->energy*a*p->v[2];
  p->energy-=a;
}

void NEWparticle_redraw(c_PARTICLE *obj,c_MATRIX objmat, float dt){
  c_PART *particles=obj->p;
  int numparticles=obj->np;
  int numlines=obj->sizelimit;
  int i;
  
//  printf("Rendering partsys. numlines=%d  numparts=%d\n",numlines,numparticles);

  aglZbuffer(AGL_ZBUFFER_R);
  aglBlend(AGL_BLEND_ADD);

// -------------------- LINES ---------------------
  aglTexture(0);
  glLineWidth(3.0f);

  glBegin(GL_LINES);
  for(i=0;i<numlines;i++){
    float c=particles[i].energy;
    c_VECTOR *p=&particles[i].p;
    c_VECTOR p1,pm;

    p1.x= p->x*objmat[X][X] + p->y*objmat[X][Y] + p->z*objmat[X][Z] + objmat[X][W];
    p1.y= p->x*objmat[Y][X] + p->y*objmat[Y][Y] + p->z*objmat[Y][Z] + objmat[Y][W];
    p1.z= p->x*objmat[Z][X] + p->y*objmat[Z][Y] + p->z*objmat[Z][Z] + objmat[Z][W];

    glColor3f(0,0,0);
    glVertex3fv(&p1.x);
    
    pm.x=p->x+0.1*particles[i].v[0];
    pm.y=p->y+0.1*particles[i].v[1];
    pm.z=p->z+0.1*particles[i].v[2];

    p1.x= pm.x*objmat[X][X] + pm.y*objmat[X][Y] + pm.z*objmat[X][Z] + objmat[X][W];
    p1.y= pm.x*objmat[Y][X] + pm.y*objmat[Y][Y] + pm.z*objmat[Y][Z] + objmat[Y][W];
    p1.z= pm.x*objmat[Z][X] + pm.y*objmat[Z][Y] + pm.z*objmat[Z][Z] + objmat[Z][W];

    glColor3f(c*particles[i].color[0],c*particles[i].color[1],c*particles[i].color[2]);
    glVertex3fv(&p1.x);

    if(dt>0) move_particle(&particles[i],dt);
  }
  glEnd();
  glLineWidth(1.0f);

// -------------------- LIGHTS ---------------------
  aglTexture(obj->texture_id);
  glBegin(GL_QUADS);
  for(i=numlines;i<numparticles;i++){
    float c=particles[i].energy*particles[i].energy*obj->energy;
    c_VECTOR *p=&particles[i].p;
    c_VECTOR p1;

    p1.x= p->x*objmat[X][X] + p->y*objmat[X][Y] + p->z*objmat[X][Z] + objmat[X][W];
    p1.y= p->x*objmat[Y][X] + p->y*objmat[Y][Y] + p->z*objmat[Y][Z] + objmat[Y][W];
    p1.z= p->x*objmat[Z][X] + p->y*objmat[Z][Y] + p->z*objmat[Z][Z] + objmat[Z][W];

    glColor3f(particles[i].color[0],particles[i].color[1],particles[i].color[2]);
    glTexCoord2f(0,0); glVertex3f(p1.x-c,p1.y+c,p1.z);
    glTexCoord2f(1,0); glVertex3f(p1.x+c,p1.y+c,p1.z);
    glTexCoord2f(1,1); glVertex3f(p1.x+c,p1.y-c,p1.z);
    glTexCoord2f(0,1); glVertex3f(p1.x-c,p1.y-c,p1.z);

    if(dt>0) move_particle(&particles[i],dt);
  }
  glEnd();

}

void NEWparticle_preplay(c_PARTICLE *obj,float dt,int times){
  int i,j;
  c_PART *p=obj->p;
  printf("Calculating particle system...");fflush(stdout);
  for(j=0;j<obj->np;j++)
    for(i=0;i<times;i++)
      move_particle(&p[j],dt);
  printf("ready\n");
}

void NEWparticle_init(c_PARTICLE *obj,int texture,int np,int npl){
int i;
  if(!(obj->p=malloc(sizeof(c_PART)*np))) np=0;
  
  obj->type=1;
  obj->maxnp=np;
  obj->np=np;
  obj->texture_id=texture;
  obj->sizelimit=(npl<=np)?npl:np;

  printf("NEWparticle.init. np=%d  npl=%d\n",np,npl);
  for(i=0;i<np;i++) init_particle(&obj->p[i]);
}

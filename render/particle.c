/*
 * written by David Bucciarelli (tech.hmw@plus.it)
 *            Humanware s.r.l.
 * hacked by Strepto / Astral  :)      and A'rpi :)
 */

#include "../config.h"

#include "../agl/agl.h"

#include "../3dslib/ast3d.h"
#include "render.h"
//#include "particle.h"

#define vinit(a,i,j,k) {\
  (a)[0]=i;\
  (a)[1]=j;\
  (a)[2]=k;\
} 

// #define AGRAV -9.8

//static int np;
//static float eject_r,dt,eject_vy,eject_vl;
//static float ridtri;
//static part *p;

static INLINE float vrnd(void){ return(((float)rand())/RAND_MAX); }

static void setnewpart(c_PART *p,c_OBJECT *obj){
  float a,v[3];
  a=vrnd()*3.14159265359*2.0;
  p->p.x=  p->p.y=  p->p.z=0;
  p->v[0]=vrnd()*sin(a)*obj->particle.eject_r;
  p->v[1]=(1+vrnd())*obj->particle.eject_vy;
  p->v[2]=vrnd()*cos(a)*obj->particle.eject_r;
  vinit(p->color,vrnd(),vrnd(),vrnd());
  p->energy=1;
}


void particle_redraw(c_OBJECT *obj,c_MATRIX objmat, float dt){
  int	j;
  c_PART *p=obj->particle.p;

//  glShadeModel(GL_FLAT);
//  aglZbuffer(AGL_ZBUFFER_NONE);
  aglZbuffer(AGL_ZBUFFER_R);
//  aglTexture(lightmap);
  aglTexture(obj->particle.texture_id);
  aglBlend(AGL_BLEND_ADD);

    glBegin(GL_QUADS);
    for(j=0;j<obj->particle.np;j++){
      c_VECTOR pt;
      float p_scale;
//      glColor3f(p[j].energy,p[j].energy,p[j].energy);

      pt.x= p[j].p.x*objmat[X][X] + p[j].p.y*objmat[X][Y] + p[j].p.z*objmat[X][Z] + objmat[X][W];
      pt.y= p[j].p.x*objmat[Y][X] + p[j].p.y*objmat[Y][Y] + p[j].p.z*objmat[Y][Z] + objmat[Y][W];
      pt.z= p[j].p.x*objmat[Z][X] + p[j].p.y*objmat[Z][Y] + p[j].p.z*objmat[Z][Z] + objmat[Z][W];
//      p_scale=(0.8/(p[j].energy*2));
      p_scale=(obj->particle.energy/p[j].energy);
      if(obj->particle.sizelimit!=0){
        if (p_scale>obj->particle.sizelimit) p_scale=obj->particle.sizelimit;
        if (p_scale<-obj->particle.sizelimit) p_scale=-obj->particle.sizelimit;
      }

//      glColor3fv(p[j].color);
      glColor3f(p[j].color[0]*ast3d_blend,p[j].color[1]*ast3d_blend,p[j].color[2]*ast3d_blend);
      glTexCoord2f(0.0,0.0);
      glVertex3f(pt.x-p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,0.0);
      glVertex3f(pt.x+p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,1.0);
      glVertex3f(pt.x+p_scale,pt.y+p_scale,pt.z);
      glTexCoord2f(0.0,1.0);
      glVertex3f(pt.x-p_scale,pt.y+p_scale,pt.z);

      p[j].energy-=obj->particle.dieratio;
      p[j].color[0]*=obj->particle.colordecrement;
      p[j].color[1]*=obj->particle.colordecrement;
      p[j].color[2]*=obj->particle.colordecrement;

#if 1
//      if(p[j].p.y<0.1|| p[j].energy<=0.001) {  // 
      if(p[j].p.y>obj->particle.max_y) {  // 
#else
      if(p[j].p.y<0.1) {  // 
#endif
	setnewpart(&p[j],obj);
//	putchar('.');fflush(stdout);
      } else {
        p[j].v[1]+=obj->particle.agrav*dt;
        p[j].p.x+=dt*p[j].v[0];
        p[j].p.y+=dt*p[j].v[1];
        p[j].p.z+=dt*p[j].v[2];
//        if(j==10) printf("PART  dt=%8.5f   %8.3f  %8.3f  %8.3f    V: %8.3f  %8.3f  %8.3f\n",dt,p[j].p.x,p[j].p.y,p[j].p.z,p[j].v[0],p[j].v[1],p[j].v[2]);
      }
      
    }
    glEnd();

}

void particle_preplay(c_OBJECT *obj,float dt,int times){
  int i,j;
  c_PART *p=obj->particle.p;
  printf("Calculating particle system...");fflush(stdout);
  for(j=0;j<obj->particle.np;j++){
    for(i=0;i<times;i++){
      p[j].energy-=obj->particle.dieratio;
      p[j].color[0]*=obj->particle.colordecrement;
      p[j].color[1]*=obj->particle.colordecrement;
      p[j].color[2]*=obj->particle.colordecrement;
#if 1
//      if(p[j].p.y<0.1|| p[j].energy<=0.001){
      if(p[j].p.y>obj->particle.max_y) {  // 
#else
      if(p[j].p.y<0.1){
#endif
        setnewpart(&p[j],obj);
      } else {
        p[j].v[1]+=obj->particle.agrav*dt;
        p[j].p.x+=dt*p[j].v[0];
        p[j].p.y+=dt*p[j].v[1];
        p[j].p.z+=dt*p[j].v[2];
      }
    }
  }
  printf("ready\n");
}

void particle_init(c_OBJECT *obj,int texture,int np){
int i;
  if(!(obj->particle.p=malloc(sizeof(c_PART)*np))) np=0;
  obj->particle.type=0;
  obj->particle.maxnp=np;
  obj->particle.np=np;
  obj->particle.texture_id=texture;
  obj->particle.eject_r=0.75;
  obj->particle.eject_vy=30;
  obj->particle.eject_vl=7.5;
  obj->particle.ridtri=0.75;
  obj->particle.energy=0.8;
  obj->particle.sizelimit=0;
  obj->particle.dieratio=0.0018;
  obj->particle.agrav=-9.81;
  obj->particle.colordecrement=0.999;
  printf("particle.init. np=%d\n",np);
  for(i=0;i<np;i++) setnewpart(&obj->particle.p[i],obj);
}

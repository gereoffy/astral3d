#include "../config.h"
//#include <sys/time.h>
//#include <unistd.h>

#include "bsptunel.h"

#include "../agl/agl.h"
//#include <GL/glut.h>

//#include "../afs/afs.h"
//#include "../loadmap/loadmaps.h"
//#include "../timer/timer.h"

#include "../3dslib/ast3d.h"
#include "../3dslib/vector.h"

#define CTRLPOINTS_U 1024
#define CTRLPOINTS_V 12
#define CTRLPOINTS_V_SUB 8

//typedef struct { float x,y,z; } c_VECTOR;

static c_VECTOR ctrlpoints_main[CTRLPOINTS_U+4];
static c_VECTOR ctrlpoints[CTRLPOINTS_U][CTRLPOINTS_V];
static c_VECTOR ctrlpoints_now[CTRLPOINTS_V];

static float sulytab[CTRLPOINTS_V_SUB][3];

static int CsomoK=3;  // fokszam

#if 0
#define CsomoN 8
int CsomoF(int i){
    if(i<CsomoK) return 0;
    if(i>CsomoN) return CsomoN-CsomoK+2;
    return i-CsomoK+1;
}
#else
#define CsomoF(i) (i-CsomoK+1)
#endif

static float NSuly(int i,int k,float u){
  int t;
  float v=0;
  if(k==1){
    if( (CsomoF(i)<=u) && (u<CsomoF(i+1)) ) v=1;
  } else {
    t=CsomoF(i+k-1) - CsomoF(i);
    if(t) v=(u-CsomoF(i)) * NSuly(i, k-1, u) / t;
    t=CsomoF(i+k) - CsomoF(i+1);
    if(t) v+=(CsomoF(i+k)-u) * NSuly(i+1,k-1,u) / t;
  }
  return v;
}

static inline float NSuly_FAST_K3_k2(int i,float u){
//  k==2
    // Speci eset, erdemes kulon megirni:
    float v=0;
    if( ((i-2)<=u) && (u<(i-1)) ) v= (u-i+2);
    if( ((i-1)<=u) && (u<(i)) )  v+= (i-u);
    return v;
}

static inline float NSuly_FAST_K3(int i,float u){
  // k==3
//  if(k!=3) printf("Error: NSuly_FAST_K3 called with k!=3\n");
  return ( (u-i+2) * NSuly_FAST_K3_k2(i,u) + (i+1-u) * NSuly_FAST_K3_k2(i+1,u) ) / 2.0f;
}

static void get_main_spline(float u,c_VECTOR *v){
  float N[4];
  int ui=u;
  u-=ui;
  CsomoK=4;
  N[0]=NSuly(0,CsomoK,u);
  N[1]=NSuly(1,CsomoK,u);
  N[2]=NSuly(2,CsomoK,u);
  N[3]=NSuly(3,CsomoK,u);
  v->x = ctrlpoints_main[ui+0].x*N[0]+
         ctrlpoints_main[ui+1].x*N[1]+
         ctrlpoints_main[ui+2].x*N[2]+
         ctrlpoints_main[ui+3].x*N[3];
  v->y = ctrlpoints_main[ui+0].y*N[0]+
         ctrlpoints_main[ui+1].y*N[1]+
         ctrlpoints_main[ui+2].y*N[2]+
         ctrlpoints_main[ui+3].y*N[3];
  v->z = ctrlpoints_main[ui+0].z*N[0]+
         ctrlpoints_main[ui+1].z*N[1]+
         ctrlpoints_main[ui+2].z*N[2]+
         ctrlpoints_main[ui+3].z*N[3];
}

static void interp_spline_now(float u){
  float N[3];
  int vi;
  int ui=u;
  u-=ui;
//  CsomoK=3;
  N[0]=NSuly_FAST_K3(0,u);
  N[1]=NSuly_FAST_K3(1,u);
  N[2]=NSuly_FAST_K3(2,u);
  for(vi=0;vi<CTRLPOINTS_V;vi++){
    ctrlpoints_now[vi].x =
         ctrlpoints[(ui+0)][vi].x*N[0]+
         ctrlpoints[(ui+1)][vi].x*N[1]+
         ctrlpoints[(ui+2)][vi].x*N[2];
    ctrlpoints_now[vi].y =
         ctrlpoints[(ui+0)][vi].y*N[0]+
         ctrlpoints[(ui+1)][vi].y*N[1]+
         ctrlpoints[(ui+2)][vi].y*N[2];
    ctrlpoints_now[vi].z =
         ctrlpoints[(ui+0)][vi].z*N[0]+
         ctrlpoints[(ui+1)][vi].z*N[1]+
         ctrlpoints[(ui+2)][vi].z*N[2];
  }
}

static inline void get_tunnel_spline(int ui,int us,c_VECTOR *v){
  register c_VECTOR* p;
  register float N;
  N=sulytab[us][0]; p=&ctrlpoints_now[ui];
  v->x=p->x*N; v->y=p->y*N; v->z=p->z*N;
  if(++ui==CTRLPOINTS_V) ui=0;
  N=sulytab[us][1]; p=&ctrlpoints_now[ui];
  v->x+=p->x*N; v->y+=p->y*N; v->z+=p->z*N;
  if(++ui==CTRLPOINTS_V) ui=0;
  N=sulytab[us][2]; p=&ctrlpoints_now[ui];
  v->x+=p->x*N; v->y+=p->y*N; v->z+=p->z*N;
//  if(++ui==CTRLPOINTS_V) ui=0;
//  N=sulytab[us][3]; p=&ctrlpoints_now[ui];
//  v->x+=p->x*N; v->y+=p->y*N; v->z+=p->z*N;
}

static void init_ctrl_main(){
float dist=16;
c_VECTOR P;
c_VECTOR V;
int i;

P.x=P.y=P.z=0;
V.x=V.y=0;V.z=-1; // kezdo irany


for(i=0;i<CTRLPOINTS_U+4;i++){
    V.x+=3*(float)rand()/RAND_MAX-1.98;
    V.y+=3*(float)rand()/RAND_MAX-1.03;
    V.z+=0.1f*(float)rand()/RAND_MAX-0.05;
    vec_normalize(&V,&V);
    if(fabs(V.y)>0.8){ // upvector workaround
      V.z-=0.3;
      vec_normalize(&V,&V);
    }
    P.x+=V.x*dist;P.y+=V.y*dist;P.z+=V.z*dist;
//    printf("%8.3f  %8.3f  %8.3f\n",P.x,P.y,P.z);
    ctrlpoints_main[i].x=P.x;
    ctrlpoints_main[i].y=P.y;
    ctrlpoints_main[i].z=P.z;
}


for(i=0;i<CTRLPOINTS_U;i++){
  c_VECTOR p,t,up,n;
  int j;

  get_main_spline(i,&p);
  get_main_spline(i+0.01,&t);
  t.x-=p.x; t.y-=p.y; t.z-=p.z;
  vec_normalize(&t,&t);
  up.x=0;up.y=1;up.z=0;
  vec_cross(&t,&up,&n);
  vec_normalize(&n,&n);
  vec_cross(&t,&n,&up);

  for(j=0;j<CTRLPOINTS_V;j++){
    float s=2*3.14169265*(float)j/CTRLPOINTS_V;
    float R=1.5f+5*(float)rand()/RAND_MAX;
    ctrlpoints[i][j].x=p.x + R*( n.x*sin(s) + up.x*cos(s) );
    ctrlpoints[i][j].y=p.y + R*( n.y*sin(s) + up.y*cos(s) );
    ctrlpoints[i][j].z=p.z + R*( n.z*sin(s) + up.z*cos(s) );
  }

}

// Calc spline tables:
CsomoK=3;
for(i=0;i<CTRLPOINTS_V_SUB;i++){
  float u=(float)i/(float)CTRLPOINTS_V_SUB;
  sulytab[i][0]=NSuly_FAST_K3(0,u);
  sulytab[i][1]=NSuly_FAST_K3(1,u);
  sulytab[i][2]=NSuly_FAST_K3(2,u);
//  sulytab[i][3]=NSuly(3,CsomoK,u);
}

}


//**************************************************************************//

static c_VECTOR tunnelpoints[CTRLPOINTS_V_SUB*CTRLPOINTS_V];

void BSPTUNNEL_Render(float pos,int texture){
//  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(60,640.0f/480.0f,1,5000);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
{ c_VECTOR v1,v2;
  get_main_spline(pos,&v1);
  get_main_spline(pos+0.4,&v2);
  gluLookAt(v1.x,v1.y,v1.z,
            v2.x,v2.y,v2.z,
            0,1,0 );
}

//  aglBlend(AGL_BLEND_ADD);
  aglZbuffer(AGL_ZBUFFER_NONE);

{ int i,j,k;
  float eu=0;
  float tu=(int)(16*pos);
  tu=tu/64.0f; tu-=(int)tu;

  for(i=0;i<8*8;i++){
    int ui=16*pos;
    float u=(float)(63-i+ui)/16.0f-1;
//    float tu=(float)(64-i+ui)/32.0f;
    float v=0;
    c_VECTOR tempv;
    c_VECTOR* vp=tunnelpoints;
    
    tempv.x=vp->x;
    tempv.y=vp->y;
    tempv.z=vp->z;
    
    interp_spline_now(u);
    if(i){
      float c=i*0.02;

      aglBlend(AGL_BLEND_ADD);
      aglTexture(0);
      glLineWidth(2);
//      glColor3f(0.5,0.5,0.5);
      glColor3f(0.5*c,0.5*c,0.5*c);
      glBegin(GL_LINE_LOOP);
      vp=tunnelpoints;
      for(j=0;j<CTRLPOINTS_V;j++) for(k=0;k<CTRLPOINTS_V_SUB;k++){
          //glTexCoord2f(tu,v); 
          glVertex3fv(&vp->x);
          ++vp;
          v+=1.0f/(float)(CTRLPOINTS_V*CTRLPOINTS_V_SUB);
      }
      glEnd();

      vp=tunnelpoints;
      aglBlend(AGL_BLEND_NONE);
      aglTexture(texture);
      glColor3f(c,c,c);
      glBegin(GL_TRIANGLE_STRIP);
      for(j=0;j<CTRLPOINTS_V;j++) for(k=0;k<CTRLPOINTS_V_SUB;k++){
          glTexCoord2f(eu,v); glVertex3fv(&vp->x);
          get_tunnel_spline(j,k,vp);
          glTexCoord2f(tu,v); glVertex3fv(&vp->x);
          ++vp;
          v+=1.0f/(float)(CTRLPOINTS_V*CTRLPOINTS_V_SUB);
      }
      glTexCoord2f(eu,v); glVertex3fv(&tempv.x);
      glTexCoord2f(tu,v); glVertex3fv(&tunnelpoints->x);
      glEnd();
      
    } else {
      for(j=0;j<CTRLPOINTS_V;j++)
        for(k=0;k<CTRLPOINTS_V_SUB;k++)
          get_tunnel_spline(j,k,vp++);
    }
    eu=tu;tu-=1.0f/64.0f;
  }
}

}

void BSPTUNNEL_Init(int seed){
  srand(seed);
  init_ctrl_main();
}



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "../agl/agl.h"
//#include <GL/glut.h>

#define Local static

// negyszogek szama
#define mm              2
#define nu              (mm * 2 - 1)
#define nv              4
#define nn              (nv * 2 - 1)

// kontrollpontok szama egy negyszogon belul
#define um              7
#define vm              4

// n=detail  nm=max(n)
//#define n               16
#define nm              16

static int n=10;

typedef float mu[nu+1];
typedef float mv[nn+1];

typedef struct {
  float xp, yp, zp;
} pont3;

typedef struct {
  float xp, yp, zp;
//  short xr, yr;
} kpont;

typedef struct szem {
  pont3 c[4];
} szem;

typedef szem kpsz[(um+1)*(vm+1)];  /* kontrollpontracs-szemek  */
typedef szem rsz[nm * nm];         /* haloszemek egy kontrollpontracs-szemben */
typedef szem *halo[(um+1)*(vm+1)]; /* megjelenitesi halo       */

typedef kpont mp[um+1][vm+1];                     /* kontrollpontok     */
typedef pont3 *mr[(um+1)*nm + 1][(vm+1)*nm + 1];  /* megjelenitesi halo */

static mu gx, gy, gz;
static float hu[nu+1], ku[nu+1], lu[nu+1];
static mv fx, fy, fz;
static float hv[nn+1], kv[nn+1], lv[nn+1];

static mp a;
static mr rp;
static halo hl;

static float nv12;  // ez mi???

static void megjelen(float face_blend,float wire_blend){
  float normal_x;
  float normal_y;
  float normal_z;
  float normal_len;
  float* xyz[4];
  int k,l,p,r,km,t;

    for (k=0; k<(nu+1); k++)
      for (l=0; l<nv; l++)
        for (p=0; p<n; p++)
	        for (r=0; r<n; r++){
            pont3 *sk=hl[k*nv + l][p*n + r].c;
//----------------------------------------------------------------------
            xyz[0]= &rp[k*n + p+1][l*n + r  ]->xp;
            xyz[1]= &rp[k*n + p+1][l*n + r+1]->xp;
            xyz[2]= &rp[k*n + p  ][l*n + r+1]->xp;
            xyz[3]= &rp[k*n + p  ][l*n + r  ]->xp;

#define SET_SK(i) { sk[i].xp=xyz[i][0]; sk[i].yp=xyz[i][1]; sk[i].zp=xyz[i][2]; }
  if(r>0){
    sk[0] = hl[k*nv + l][p*n + r-1].c[1];
    SET_SK(1);
    if(p>0) sk[2] = hl[k*nv + l][(p-1)*n + r].c[1]; else SET_SK(2);
    sk[3] = hl[k*nv + l][p*n + r-1].c[2];
  } else {
    SET_SK(0);
    SET_SK(1);
    if(p>0){
      sk[2] = hl[k*nv + l][(p-1)*n + r].c[1];
      sk[3] = hl[k*nv + l][(p-1)*n + r].c[0];
    } else {
      SET_SK(2);
      SET_SK(3);
    }
  }
#undef SET_SK
//----------------------------------------------------------------------
          }
  
  for (k = 0; k < (nu+1); k++) {
    for (l = 0; l < nv; l++) {
      for (km = 0; km < (n*n); km++) {
        pont3 dh[5];
        for (t = 0; t < 4; t++) dh[t] = hl[k*nv+l][km].c[t];
        normal_x=-(dh[1].yp-dh[2].yp)*(dh[1].zp-dh[3].zp)+(dh[1].zp-dh[2].zp)*(dh[1].yp-dh[3].yp);
        normal_y=-(dh[1].zp-dh[2].zp)*(dh[1].xp-dh[3].xp)+(dh[1].xp-dh[2].xp)*(dh[1].zp-dh[3].zp);
        normal_z=-(dh[1].xp-dh[2].xp)*(dh[1].yp-dh[3].yp)+(dh[1].yp-dh[2].yp)*(dh[1].xp-dh[3].xp);
        normal_len=sqrt(normal_x*normal_x+normal_y*normal_y+normal_z*normal_z);
        if(normal_len>0){
          normal_len=1.0/normal_len;
//          normal_x*=normal_len;
//          normal_y*=normal_len;
          normal_z*=normal_len;
        }

//--------------------------------------------------------------
  glBegin(GL_TRIANGLE_STRIP);
    normal_z=fabs(normal_z);
    glColor3f(0.1*face_blend*normal_z,0.05*face_blend*normal_z,0.3*face_blend*normal_z);
  	  glVertex3f(dh[0].xp,dh[0].yp,dh[0].zp);
  	  glVertex3f(dh[1].xp,dh[1].yp,dh[1].zp);
  	  glVertex3f(dh[3].xp,dh[3].yp,dh[3].zp);
  	  glVertex3f(dh[2].xp,dh[2].yp,dh[2].zp);
  glEnd();
  glBegin(GL_LINE_STRIP);
    glColor3f(0.1*wire_blend*normal_z,0.05*wire_blend*normal_z,0.3*wire_blend*normal_z);
  	  glVertex3f(dh[0].xp,dh[0].yp,dh[0].zp);
  	  glVertex3f(dh[1].xp,dh[1].yp,dh[1].zp);
	    glVertex3f(dh[3].xp,dh[3].yp,dh[3].zp);
  	  glVertex3f(dh[2].xp,dh[2].yp,dh[2].zp);
  glEnd();
//--------------------------------------------------------------
      }
    }
  }
}


#include "szamol.c"


static float rndtabla[nu+1][nv+1][3];       // speed
static float szogtabla[nu+1][nv+1][3];      // phase



void splinesurface_redraw(float frame,float rg,float face_blend,float wire_blend,int spl_n){
int i,j;

  n=spl_n; if(n>nm) n=nm;

//  update_rnd(frame);
  for(i=0;i<=nu;i++)
  for(j=0;j<=nv;j++){
    a[i][j].xp = rg*sin((szogtabla[i][j][0]+frame*rndtabla[i][j][0]));
    a[i][j].yp = rg*sin((szogtabla[i][j][1]+frame*rndtabla[i][j][1]));
    a[i][j].zp = rg*sin((szogtabla[i][j][2]+frame*rndtabla[i][j][2]));
  }

  rajzracs();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40,1.0,10,1000);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0,0,-600);

  aglBlend(AGL_BLEND_ADD);
  aglZbuffer(AGL_ZBUFFER_NONE);
  aglTexture(0);

  megjelen(face_blend,wire_blend);   /* Interpolalt felulet rajzolasa */

}


void splinesurface_init(float rnd_scale,float szog_scale){
  int i,j,k,l;
  static float ap, bp, cp;  // ez mi???
  
  n=nm;
  
  for (k=0; k<=(nu+1)*n; k++)
    for (l=0; l<=nv*n; l++)
      rp[k][l]=(pont3 *)malloc(sizeof(pont3));
  for (k=0; k<(nu+1)*nv; k++) hl[k]=(szem *)malloc(sizeof(rsz));

  ap = 0.3; bp = -0.7; cp = 0.2;
  nv12=1.0F/sqrt(ap * ap + bp * bp + cp * cp);

  for(i=0;i<=nu;i++)
  for(j=0;j<=nv;j++)
  for(k=0;k<3;k++){
    rndtabla[i][j][k]=rnd_scale*((float)random())/((float)RAND_MAX);
    szogtabla[i][j][k]=szog_scale*((float)random())/((float)RAND_MAX);
  }

}


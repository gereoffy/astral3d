#define SIN_COMPONENTS 10

#define SPHERE_X_SEGS 16
#define SPHERE_Y_SEGS 8
#define SPHERE_R 150

#define LOGO_X_DIF 100
#define LOGO_Yx_DIF -5
#define LOGO_Yy_DIF 55

#include "../config.h"
//#include <unistd.h>

#include "../agl/agl.h"

#include "../afs/afs.h"
//#include "../afs/afsmangl.h"
#include "../loadmap/loadmaps.h"

#include "../3dslib/ast3d.h"
#include "../3dslib/vector.h"

#include "hjbtunel.h"

#define CTRLPOINTS_U 512
#define CTRLPOINTS_V 12
#define CTRLPOINTS_V_SUB 8

#define LOGO_V_TILE 6
#define LOGO_U_TILE 16
#define LOGO_U_SIZE 0.5f

#define LOGO_V_SIZE ((float)CTRLPOINTS_V/(float)LOGO_V_TILE)
#define V_SCALE (LOGO_V_SIZE/(float)LOGO_X_DIF)
#define U_SCALE (LOGO_U_SIZE/(float)LOGO_Yy_DIF)

#define PARTICLE_LEN 6
#define PARTICLE_TILE 80
#define PARTICLE_SUBTILE 1
#define PARTICLE_SIZE 0.2f
#define PARTICLE_R 0.8f

static c_VECTOR particles[PARTICLE_SUBTILE*PARTICLE_TILE*CTRLPOINTS_U];
static c_VECTOR particles_c[PARTICLE_SUBTILE*PARTICLE_TILE*CTRLPOINTS_U];

//static int pauseflag=0;

//static int part_texture=0;
//static int sphere_texture=0;

static c_MATRIX tmatrix;

/* ************************************************************************** */
/* ************************************************************************** */

static int poly_db=0;
static int poly_pdb[2];
static int poly_points[2][256][2];  // [polydb][vertexdb][xy]

static void read_logo_file(){
char sor[256];
// cat akarmi.dxf | egrep -v ^\  > yyy2
afs_FILE *f=afs_fopen("logo.vec","rt");
int pdb;
  printf("HJBTUNNEL: Reading vector file...\n");
  if(!f){
    return;
    printf("HJBTUNNEL: FILE NOT FOUND!!!\n");
  }
  while(afs_fgets(sor,250,f)){
    if(strncmp(sor,"POLYLINE",8)==0){
      pdb=0;
      continue;
    }
    if(strncmp(sor,"SEQEND",6)==0){
      poly_pdb[poly_db]=pdb;
      printf("Poly %d: %d points\n",poly_db,pdb);
      ++poly_db;
      continue;
    }
    if(strncmp(sor,"VERTEX",6)==0){
      float x,y;
      int xx,yy;
      afs_fgets(sor,250,f); // "7"
      afs_fgets(sor,250,f);sscanf(sor,"%f",&x);
      afs_fgets(sor,250,f);sscanf(sor,"%f",&y);
      y-=0.3;
      xx=x+((x<0)?(-0.5):(0.5));// kerekites, CorelDraw5 pontatlansaga miatt :(
      yy=y+((y<0)?(-0.5):(0.5));
      poly_points[poly_db][pdb][0]=xx;
      poly_points[poly_db][pdb][1]=yy;
      ++pdb;
    }
    printf("HJBTUNNEL: readed %d polys, %d vertices\n",poly_db,pdb);
  }
  afs_fclose(f);
}

static int poly_vertex_db=0;
static int poly_line_db=0;
static int poly_lines[300][2];
static unsigned char poly_lines_flag[300];
static int poly_vertex[300][2];
static c_VECTOR poly_vertex_v[300];
static float poly_vertex_c[300];

// 1-et ad vissza ha ez a vonal mar definialva van:
static int poly_check_line(int x1,int y1,int x2,int y2,int flag){
  int i;
  for(i=0;i<poly_line_db;i++){
    int a=poly_lines[i][0];
    int b=poly_lines[i][1];
    if( (poly_vertex[a][0]==x1 && poly_vertex[a][1]==y1 &&
         poly_vertex[b][0]==x2 && poly_vertex[b][1]==y2)
    ||  (poly_vertex[b][0]==x1 && poly_vertex[b][1]==y1 &&
         poly_vertex[a][0]==x2 && poly_vertex[a][1]==y2) ) {
      poly_lines_flag[i]|=flag;
      return 1;
    }
  }
  return 0;
}

static int poly_add_point(int x,int y){
int i;
  for(i=0;i<poly_vertex_db;i++){
    if(poly_vertex[i][0]==x && poly_vertex[i][1]==y) return i;
  }
//  i=poly_vertex_db;
  poly_vertex[i][0]=x;
  poly_vertex[i][1]=y;
  ++poly_vertex_db;
  return i;
}

static void poly_add_line(int x1,int y1,int x2,int y2,int flag){
int i,j;
  for(i=-1;i<=+1;i++)
   for(j=-1;j<=+1;j++){
     if(poly_check_line(i*LOGO_X_DIF+j*LOGO_Yx_DIF+x1,j*LOGO_Yy_DIF+y1,
            i*LOGO_X_DIF+j*LOGO_Yx_DIF+x2,j*LOGO_Yy_DIF+y2,flag)) return;
  }
  poly_lines[poly_line_db][0]=poly_add_point(x1,y1);
  poly_lines[poly_line_db][1]=poly_add_point(x2,y2);
  poly_lines_flag[poly_line_db]=flag;
  ++poly_line_db;
}

static void poly_optimize(){
  int i;
  for(i=0;i<2;i++){
    int j;
    for(j=0;j<poly_pdb[i];j++){
      int k=j+1; if(k>=poly_pdb[i]) k-=poly_pdb[i];
      poly_add_line(poly_points[i][j][0],poly_points[i][j][1], 
                    poly_points[i][k][0],poly_points[i][k][1],i+1);
    }
  }
  printf("Optimized:  %d vertex, %d lines\n",poly_vertex_db,poly_line_db);
}

/* ************************************************************************** */
/* ************************************************************************** */

//typedef struct { float x,y,z; } c_VECTOR;

static c_VECTOR ctrlpoints_main[CTRLPOINTS_U+4];
static c_VECTOR ctrlpoints_main_x[CTRLPOINTS_U+4];
static c_VECTOR ctrlpoints_main_y[CTRLPOINTS_U+4];

static float ctrlpoints_sins[CTRLPOINTS_U][CTRLPOINTS_V][SIN_COMPONENTS];

static c_VECTOR ctrlpoints[CTRLPOINTS_U][CTRLPOINTS_V];

//static float sulytab[CTRLPOINTS_V_SUB][4];

static int CsomoK=3;  // fokszam
//static int CsomoN=8;   // csomopontok szama

#define CsomoF(i) (i-CsomoK+1)

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
//  if(k==2){
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

static void get_tunnel_spline4(int u1,float v0,c_VECTOR *v){
  int u1_end=u1+3;
  float N[3];
  int ui=v0;
  v0-=ui;
//  CsomoK=3;
  N[0]=NSuly_FAST_K3(0,v0);
  N[1]=NSuly_FAST_K3(1,v0);
  N[2]=NSuly_FAST_K3(2,v0);
  for(;u1<u1_end;u1++){
  v[0].x = ctrlpoints[u1][(ui+0)%CTRLPOINTS_V].x*N[0]+
         ctrlpoints[u1][(ui+1)%CTRLPOINTS_V].x*N[1]+
         ctrlpoints[u1][(ui+2)%CTRLPOINTS_V].x*N[2];
  v[0].y = ctrlpoints[u1][(ui+0)%CTRLPOINTS_V].y*N[0]+
         ctrlpoints[u1][(ui+1)%CTRLPOINTS_V].y*N[1]+
         ctrlpoints[u1][(ui+2)%CTRLPOINTS_V].y*N[2];
  v[0].z = ctrlpoints[u1][(ui+0)%CTRLPOINTS_V].z*N[0]+
         ctrlpoints[u1][(ui+1)%CTRLPOINTS_V].z*N[1]+
         ctrlpoints[u1][(ui+2)%CTRLPOINTS_V].z*N[2];
  ++v;
  }
}



static void get_tunnel_spline(float u,float v0,c_VECTOR *v){
  c_VECTOR p[3];
  float N[3];
  int ui=u;
  u-=ui;
  CsomoK=3;
  N[0]=NSuly_FAST_K3(0,u);
  N[1]=NSuly_FAST_K3(1,u);
  N[2]=NSuly_FAST_K3(2,u);
  get_tunnel_spline4(ui,v0,p);
//  get_tunnel_spline1(ui+0,v0,&p[0]);
//  get_tunnel_spline1(ui+1,v0,&p[1]);
//  get_tunnel_spline1(ui+2,v0,&p[2]);
//  get_tunnel_spline1(ui+3,v0,&p[3]);
  v->x = p[0].x*N[0]+ p[1].x*N[1]+ p[2].x*N[2];
  v->y = p[0].y*N[0]+ p[1].y*N[1]+ p[2].y*N[2];
  v->z = p[0].z*N[0]+ p[1].z*N[1]+ p[2].z*N[2];
}


static void init_ctrl_main(){
float dist=4;
c_VECTOR P;
c_VECTOR V;
int i,k;

printf("HJBTUNNEL: Generating NURBS control points...\n");

P.x=P.y=P.z=0;
V.x=V.y=0;V.z=-1; // kezdo irany

#if 0
// TORUSKNOT
for(i=0;i<CTRLPOINTS_U+4;i++){
    float s=0.1f*i;
    float s1=1*s;
    float s2=2*s;
    float s3=3*s;
    float R=15;
    ctrlpoints_main[i].x=R*sin(s1);
    ctrlpoints_main[i].y=R*sin(s2);
    ctrlpoints_main[i].z=R*sin(s3);
}
#else
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
#endif

for(i=0;i<CTRLPOINTS_U;i++){
  c_VECTOR p,t,up,n;
  int j,k;

  get_main_spline(i,&p);
  get_main_spline(i+0.05,&t);
  t.x-=p.x; t.y-=p.y; t.z-=p.z;
  vec_normalize(&t,&t);
  up.x=0;up.y=1;up.z=0;
  vec_cross(&t,&up,&n);
//  vec_normalize(&n,&n);
  vec_cross(&t,&n,&up);
  
  vec_copy(&n,&ctrlpoints_main_x[i]);
  vec_copy(&up,&ctrlpoints_main_y[i]);

  for(j=0;j<CTRLPOINTS_V;j++)
    for(k=0;k<SIN_COMPONENTS;k++){
      float size=1.0f; //float size=5.0/((float)k+5.0);
      ctrlpoints_sins[i][j][k]=size*(((float)rand()/RAND_MAX)-0.5f);
    }

}

// PARTICLES:
printf("HJBTUNNEL: Generating particle system elements...\n");
k=0;
for(i=0;i<CTRLPOINTS_U*PARTICLE_TILE;i++){
  float u=(float)i/(float)PARTICLE_TILE;
  c_VECTOR p,t,up,n;
  int j;

  get_main_spline(u,&p);
  get_main_spline(u+0.01,&t);
  t.x-=p.x; t.y-=p.y; t.z-=p.z;
  vec_normalize(&t,&t);
  up.x=0;up.y=1;up.z=0;
  vec_cross(&t,&up,&n);
  vec_normalize(&n,&n);
  vec_cross(&t,&n,&up);

  for(j=0;j<PARTICLE_SUBTILE;j++){
    float s=2*3.14169265*(float)j/PARTICLE_SUBTILE;
#if 0
    float R=0.2f*(float)rand()/RAND_MAX;
//    float R=2;
    particles[k].x=p.x + R*( n.x*sin(s) + up.x*cos(s) );
    particles[k].y=p.y + R*( n.y*sin(s) + up.y*cos(s) );
    particles[k].z=p.z + R*( n.z*sin(s) + up.z*cos(s) );
#else
    particles[k].x=p.x + PARTICLE_R*((float)rand()/(float)RAND_MAX-0.5f);
    particles[k].y=p.y + PARTICLE_R*((float)rand()/(float)RAND_MAX-0.5f);
    particles[k].z=p.z + PARTICLE_R*((float)rand()/(float)RAND_MAX-0.8f);
#endif
    particles_c[k].x=0.3f+0.4*(float)rand()/RAND_MAX;
    particles_c[k].y=0.2f+0.5*(float)rand()/RAND_MAX;
    particles_c[k].z=0.3f+0.4*(float)rand()/RAND_MAX;
    ++k;
  }

}

#if 0
// Calc spline tables:
CsomoK=3;
for(i=0;i<CTRLPOINTS_V_SUB;i++){
  float u=(float)i/(float)CTRLPOINTS_V_SUB;
  sulytab[i][0]=NSuly(0,CsomoK,u);
  sulytab[i][1]=NSuly(1,CsomoK,u);
  sulytab[i][2]=NSuly(2,CsomoK,u);
  sulytab[i][3]=NSuly(3,CsomoK,u);
}
#endif

}

/* ************************************************************************** */

void update_ctrlpoints(float szog,int first,int last,float amp){
int i;

for(i=first;i<last;i++){
  c_VECTOR *up;
  c_VECTOR *n;
  c_VECTOR p;
  int j,k;

  get_main_spline(i,&p);
  n=&ctrlpoints_main_x[i];
  up=&ctrlpoints_main_y[i];

  for(j=0;j<CTRLPOINTS_V;j++){
    float R=0;
    float szog2=szog*5;
    for(k=0;k<SIN_COMPONENTS;k++){
//    for(k=0;k<1;k++){
      R+=sin(szog2)*ctrlpoints_sins[i][j][k];
      szog2+=szog;
    }
    R=3+amp*R;
    { float s=2*3.14169265*(float)j/CTRLPOINTS_V;
      float sin_s=sin(s);
      float cos_s=cos(s);
      ctrlpoints[i][j].x=p.x + R*( n->x*sin_s + up->x*cos_s );
      ctrlpoints[i][j].y=p.y + R*( n->y*sin_s + up->y*cos_s );
      ctrlpoints[i][j].z=p.z + R*( n->z*sin_s + up->z*cos_s );
    }
  }

}
}

/* ************************************************************************** */

//static float pos=0;
//static int frames=0;
//static float fps=0;

static void DrawLogo(float pos,float u0,float v0){
int i;

  for(i=0;i<poly_vertex_db;i++){
    float u=u0+U_SCALE*poly_vertex[i][1];
    float c=0.2f*((pos-1+LOGO_U_TILE*LOGO_U_SIZE)-u);
    c_VECTOR v;
    if(c>0.7)c=0.7;
    poly_vertex_c[i]=c;
    get_tunnel_spline(u,v0+V_SCALE*poly_vertex[i][0],&v);
    v.x-=tmatrix[X][W];
    v.y-=tmatrix[Y][W];
    v.z-=tmatrix[Z][W];
    poly_vertex_v[i].x=v.x*tmatrix[X][X] + v.y*tmatrix[X][Y] + v.z*tmatrix[X][Z];
    poly_vertex_v[i].y=v.x*tmatrix[Y][X] + v.y*tmatrix[Y][Y] + v.z*tmatrix[Y][Z];
    poly_vertex_v[i].z=v.x*tmatrix[Z][X] + v.y*tmatrix[Z][Y] + v.z*tmatrix[Z][Z];

  }

  glBegin(GL_LINES);
  for(i=0;i<poly_line_db;i++){
    int a=poly_lines[i][0];
    int b=poly_lines[i][1];
    float c=poly_vertex_c[a];
    switch(poly_lines_flag[i]){
      case 1:  glColor3f(0.6*c,0.5*c,0.8*c);break;
      case 2:  glColor3f(0.8*c,0.5*c,0.6*c);break;
      default: glColor3f(c,0.7*c,c);break;
    }
    glVertex3fv(&poly_vertex_v[a].x);
    glVertex3fv(&poly_vertex_v[b].x);
  }
  glEnd();

}

static void ast_LookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
                         GLdouble centerx, GLdouble centery, GLdouble centerz,
                         GLdouble upx, GLdouble upy, GLdouble upz )
{
//   GLdouble m[16];
   GLdouble x[3], y[3], z[3];
   GLdouble mag;

   /* Make rotation matrix */

   /* Z vector */
   z[0] = eyex - centerx;
   z[1] = eyey - centery;
   z[2] = eyez - centerz;
   mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
   if (mag) {  /* mpichler, 19950515 */
      z[0] /= mag;
      z[1] /= mag;
      z[2] /= mag;
   }

   /* Y vector */
   y[0] = upx;
   y[1] = upy;
   y[2] = upz;

   /* X vector = Y cross Z */
   x[0] =  y[1]*z[2] - y[2]*z[1];
   x[1] = -y[0]*z[2] + y[2]*z[0];
   x[2] =  y[0]*z[1] - y[1]*z[0];

   /* Recompute Y = Z cross X */
   y[0] =  z[1]*x[2] - z[2]*x[1];
   y[1] = -z[0]*x[2] + z[2]*x[0];
   y[2] =  z[0]*x[1] - z[1]*x[0];

   /* mpichler, 19950515 */
   /* cross product gives area of parallelogram, which is < 1.0 for
    * non-perpendicular unit-length vectors; so normalize x, y here
    */

   mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
   if (mag) {
      x[0] /= mag;
      x[1] /= mag;
      x[2] /= mag;
   }

   mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
   if (mag) {
      y[0] /= mag;
      y[1] /= mag;
      y[2] /= mag;
   }

//#define M(row,col)  m[col*4+row]
#define M(row,col)  tmatrix[row][col]
   M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = eyex;
   M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = eyey;
   M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = eyez;
//   M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
//   glMultMatrixd( m );

   /* Translate Eye to Origin */
//   glTranslated( -eyex, -eyey, -eyez );

}


static void DrawSphere(int SPHERE_X_TILE,int SPHERE_Y_TILE){
int i,j;
float s1,s2,R,R0;
float x,y,z;
float u,v=0,ev;
c_VECTOR points[SPHERE_X_SEGS+1];
c_VECTOR p;
for(i=0;i<=SPHERE_Y_SEGS;i++){
  ev=v; v=i;v/=SPHERE_Y_SEGS; s1=M_PI*v; v*=SPHERE_Y_TILE;
  p.z=SPHERE_R*cos(s1);
  R0=SPHERE_R*sin(s1);
  if(i) glBegin(GL_TRIANGLE_STRIP);
  for(j=0;j<=SPHERE_X_SEGS;j++){
    u=j;u/=SPHERE_X_SEGS; s2=2*M_PI*u; u*=SPHERE_X_TILE;
    if(i){
      glTexCoord2f(u,ev);
      glVertex3fv(&points[j].x);
    }
    p.x=R0*cos(s2); p.y=R0*sin(s2);
    points[j].x=p.x*tmatrix[X][X] + p.y*tmatrix[X][Y] + p.z*tmatrix[X][Z];
    points[j].y=p.x*tmatrix[Y][X] + p.y*tmatrix[Y][Y] + p.z*tmatrix[Y][Z];
    points[j].z=p.x*tmatrix[Z][X] + p.y*tmatrix[Z][Y] + p.z*tmatrix[Z][Z];
    if(i){
      glTexCoord2f(u,v);
      glVertex3fv(&points[j].x);
    }
  }
  if(i)glEnd();
}


}

void HJBTUNNEL_Render(fx_hjbtunnel_struct* param,float pos){
float pos_=0.5*pos;
int i,k;
int u0;
c_VECTOR v1,v2;
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(60,640.0f/480.0f,1,1000);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  get_main_spline(pos,&v1);
  get_main_spline(pos+1.1,&v2);
//  gluLookAt(v1.x,v1.y,v1.z,  v2.x,v2.y,v2.z,  0,1,0 );
  ast_LookAt(v1.x,v1.y,v1.z,  v2.x,v2.y,v2.z,  0,1,0 );
  
  aglZbuffer(AGL_ZBUFFER_NONE);

// =============== SPHERE ====================
  aglBlend(AGL_BLEND_NONE);
  aglTexture(param->sphere_texture);
  glColor3f(1,1,1);
  DrawSphere(param->sphere_x_tile,param->sphere_y_tile);

// ============== TUNNEL ======================

  aglBlend(AGL_BLEND_ADD);
  aglTexture(0);
  u0=(int)(pos/LOGO_U_SIZE)-2;
//  for(i=0;i<LOGO_U_TILE;i++){

//  u0=100; // hack

  update_ctrlpoints(1.37527362+param->morph_speed*pos_,0,CTRLPOINTS_U,param->morph_amp);

  for(i=LOGO_U_TILE-1;i>=0;i--){
    int j;
    float u=(u0+i)*LOGO_U_SIZE;
    for(j=0;j<LOGO_V_TILE;j++){
      float v=j*LOGO_V_SIZE+(u0+i)*LOGO_Yx_DIF*V_SCALE;
      DrawLogo(pos,u,32768+v);
      
    }
  }

// ================== PARTICLE SYSTEM ====================  
#if 1
  aglBlend(AGL_BLEND_ADD);
  aglTexture(param->part_texture);
  glBegin(GL_QUADS);
  k=pos*PARTICLE_TILE*PARTICLE_SUBTILE;
  for(i=0;i<PARTICLE_TILE*PARTICLE_SUBTILE*PARTICLE_LEN;i++){
    c_VECTOR v,v2;
    v.x=particles[k].x-tmatrix[X][W];
    v.y=particles[k].y-tmatrix[Y][W];
    v.z=particles[k].z-tmatrix[Z][W];
    v2.x=v.x*tmatrix[X][X] + v.y*tmatrix[X][Y] + v.z*tmatrix[X][Z];
    v2.y=v.x*tmatrix[Y][X] + v.y*tmatrix[Y][Y] + v.z*tmatrix[Y][Z];
    v2.z=v.x*tmatrix[Z][X] + v.y*tmatrix[Z][Y] + v.z*tmatrix[Z][Z];
    glColor3fv(&particles_c[k].x);
    glTexCoord2f(0,0);
    glVertex3f(v2.x-PARTICLE_SIZE,v2.y-PARTICLE_SIZE,v2.z);
    glTexCoord2f(1,0);
    glVertex3f(v2.x+PARTICLE_SIZE,v2.y-PARTICLE_SIZE,v2.z);
    glTexCoord2f(1,1);
    glVertex3f(v2.x+PARTICLE_SIZE,v2.y+PARTICLE_SIZE,v2.z);
    glTexCoord2f(0,1);
    glVertex3f(v2.x-PARTICLE_SIZE,v2.y+PARTICLE_SIZE,v2.z);
    ++k;
  }
  glEnd();
#endif

//  glFinish();
//  glutSwapBuffers();
}


void HJBTUNNEL_Init(int seed){

//if(seed==0) seed=time(NULL);
srand(seed);

init_ctrl_main();
printf("HJBTUNNEL: Reading and optimizing logo vectors...\n");
read_logo_file();poly_optimize();

}


/*---------------------------------------------------------------------------
/  Poligonised BLOB engine v2.5     by A'rpi/Astral
/  uses some tricks described by rod/mandula & mrz/ai
/  marching cubes:  doc by Paul Bourke, tables by Cory Gene Bloyd
/---------------------------------------------------------------------------
/ ! wlink-nek kell az "option stack=65536" !!! kulonben stack overflow lesz !
/---------------------------------------------------------------------------
Alapelv: csak a felulet menten szamolja ki a kockakat ->fuggetlen a racsmerettol
optim/fejlesztesek:
  - hivas elott vizsgalja a framenum-ot, nem utana -> kevesebb rekurzio
  - ix,iy,iz globalis a rekurzional -> kevesebb stack hasznalat (csak b2 es ret)
  - tomb meretek 2 hatvanyai -> egy SHL eleg a szorzashoz (a watcom jol elbassza
    a szorzasokat kulonben)
  - a points[] kulonszedve points2D[] es points3D[] tombokre.
  - a drawpoly() es a calcnormal() a pointert kapja a points2D[]-re, nem
    a pont szamat
  - points3D[] helyett point3D, a calcnormal() pedig csak a point2D parameteret
    kapja bemenetkent
    uj: p2Dtable[12], ez tartalmazza a 12 db 2D pont _pointeret_
    -> a pontok egyenesen a cubepoints[]-ba vannak kiszamolva, es a p2dtable[]
       oda mutat bele
  - sideTable es bitnumTable -> konnyebb eldonteni hogy a vonal oldallapon van-e
  - kulon tabla vonalakra: lineTable[]  (a polikhoz van a triTable[])
    es meg optimalizalva is hogy ne legyen tobbszor ua. a vonal, meg csak 3
    oldalon stb. -> kirajzolt vonalak szama felere csokkent (kep persze ugyanaz)
  - megcsinaltam a framebit-es zbuffert -> C00L!!!  uolyan gyors az egesz mint
    zbuffer nelkul!!! :)        (#define view_zbuffer 1)
  - FLAT polyfillerrel is kirajzolja a blobot odebb, csak zbuffer tesztelesre
  - megcsinaltam a LINE-ra is a framebit-es gyors zbuffert
Ú - calc_value()-t beirtam inline, plusz a blob strukturaban eltarolom
³   a delta-k negyzetet, igy eleg 1x kiszamolni, es 4x fel lehet hasznalni
Ã - (double)-kat kiszedtem az elek menten interpolalobol, felesleges volt mar.
Ã - normalvektor szamoloban dx,dy,dz reszeredmenyek tarolasa
ÀÄÄÄÄ> fps megnott polynal 17->20, line-nal 20.3->26.6  :)
  - line_blob es Vlimit valtozo lett, igy tobb blobot is lehet szamolni
    -> louis lane -fele blob effekt

5letek/tervek:
  - a rekurziot teljesen ki kene kuszobolni
  - blob_vrecurse-t megirni kulon a 6 esetre (6 irany, amerrol jon)
  - dupla blobnal fel lehetne hasznalni az 1. blob altal kiszamolt Value-kat

/---------------------------------------------------------------------------*/
#include "blob.h"

int blobmap=0;

extern float ast3d_blend;

static int line_blob=1;
/* #define no_zbuffer 1 */

#define max_blobs 5
static int blob_db=5;

#define Vconst 0x10000000

static long Vlimit=Vlimit1;

//--- Tablazatos kozelites:
#define blobtable_size 0x4000
#define blobtable_bits 16
#define blob_function(v,v2) if(v2<blobtable_size) v+=blobtable[v2];

//--- Pontos, de lassu:
//#define blobtable_size 2
//#define blobtable_bits 0
//#define blob_function(v,v2) v+=Vconst/sqrt(v2);

//---------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#define M_PI 3.1415926527
#endif

#include "../agl/agl.h"

extern int window_w,window_h;

typedef  unsigned char          byte;
typedef  unsigned short int     word;
typedef  unsigned long int      dword;

/************************ MATH *********************************/

// double fsqrt(double __x);
// #pragma aux fsqrt parm [8087] value [8087] modify [8087] = "fsqrt"

#define sqr(a)          ((a)*(a))
#define sgn(a)    (((a) > 0) ? 1 : -1)

/************************ BLOB *********************************/

typedef struct
{
  long x;
  long y;
  long z;
  long dx2;     //  dx^2
  long dy2;     //  dx^2
  long dz2;     //  dz^2
  long dx12;    //  dx1^2
  long dy12;    //  dy1^2
  long dz12;    //  dz1^2
  byte rgb[3];
} BLOB_TSTRUCT;

typedef struct {
 long x;   // 3-D
 long y;
 long z;
} POINT3D_TSTRUCT;

typedef struct {
  long xx,yy,zz;  // 2-D
  float xn,yn,zn;
  float rgb[3];
} POINT2D_TSTRUCT;

static POINT3D_TSTRUCT point3D;
static POINT2D_TSTRUCT * p2Dtable[12];


// R cs m‚rete:    blob sebessegere nincs hatassal, csak a memoria foglalasra
#define nx 48
#define ny nx
#define nz nx

// Egy kocka oldalainak m‚rete:
// ez szinkronban kell legyen a blob koordinatak ertektartomanyaval!
#define sx2 768
#define sy2 sx2
#define sz2 sx2

static POINT2D_TSTRUCT cubepoints[nx][ny][nz][3];  // ez qrvanagy tomb lesz!!!

#define MAX_POLYS 8192
static POINT2D_TSTRUCT *polys[MAX_POLYS][3];
static int poly_db;

static unsigned short int bytesort_next[2][MAX_POLYS];
static int bytesort_start[2][256];
static int bytesort_end[2][256];
static int bytesortdata[MAX_POLYS];



static word cubes[nx][ny][nz];     // frameszam
static int framenum=0;
static int rec_level=0;

static int ix,iy,iz;
static int i,j,k;

static BLOB_TSTRUCT blobs[max_blobs];

// DEBUG/Profile valtozok:    (vegleges verziobol kiszedni!)
static long Vxint;
static long Vyint;
static long Vzint;
static long Vpolydb;  // szamolt poligonok
static long Vpolydb2; // kirajzolt polik (backface cull utan)
static long Vcubedb;  // vizsgalt kockak
static long Vcubedb2; // felhasznalt kockak
static long Vskipped=0; // eszrevett normalvektor ismetlodesek
static long Vcalced=0;  // kiszamolt normalvektorok  (elvben calced=3*skipped)
static long Vlines=0;
static long Vskipped2=0; // elozo blobnal kiszamolt Value
static double Vmin=100000000000000000000000.0;   // normalvektor hossz norm. elott
static double Vmax=0;
static int Vmaxk=0;

// marching cubes tablak:  (edgeTable[256] es triTable[256][16])
#include "cubes.h"

// Bitek:  xxxFABHJE      megadja melyik ‚l (pont) melyik 2 oldalhoz tartozik
unsigned char sideTable[12]={0x11,0x12,0x14,0x18,0x21,0x22,0x24,0x28,9,3,6,12};

static long blobtable[blobtable_size];
static byte bitnumTable[64];
static signed char lineTable[256][16];


static void blob_AddLineToTable(int p1,int p2){
int i;
  if(bitnumTable[(sideTable[p1]|sideTable[p2])]!=3) return;  // ez nem kell :)
  if(!(sideTable[p1]&sideTable[p2]&0x29))return; // 0x29= 3 szomszedos oldal
  if(p1<p2){i=p1;p1=p2;p2=i;}  // csere ha kell
// nezzuk +, hatha mar volt ilyen vonal a tablaban!
  i=0;
  while(i<k){
    if((lineTable[j][i]==p1)&&(lineTable[j][i+1]==p2)) return; //mar volt ilyen!
    i+=2;
  }
  // Okey, atment minden proban! taroljuk! :)
  lineTable[j][k]=p1;
  lineTable[j][k+1]=p2;
  k+=2;
}


void blob_init(){           // precalcoljunk!!
byte b;
double v;
  v=(1<<blobtable_bits);
  for(i=1;i<blobtable_size;i++){
    blobtable[i]=Vconst/sqrt(v*i);
  }
  blobtable[0]=blobtable[1]; // fontos!!!
  //
  for(i=0;i<64;i++){
    b=0;
    if(i&1)b++;
    if(i&2)b++;
    if(i&4)b++;
    if(i&8)b++;
    if(i&16)b++;
    if(i&32)b++;
    bitnumTable[i]=b;
  }
  //
  for(j=0;j<256;j++){
    k=0;
    i=0;
    while(triTable[j][i]!=-1){
       blob_AddLineToTable(triTable[j][i],triTable[j][i+1]);
       blob_AddLineToTable(triTable[j][i],triTable[j][i+2]);
       blob_AddLineToTable(triTable[j][i+1],triTable[j][i+2]);
       i+=3;
    }
    if(k>Vmaxk)Vmaxk=k;
    while(k<16){lineTable[j][k]=-1;k++;}
  }
}

static float light_vector[3]={0.3,0.3,0.9055};

static void blob_calcnormal2(POINT2D_TSTRUCT *p2D){
float x1,y1,z1,a;
float dx,dy,dz;

++Vcalced;

//if(!line_blob){

  x1=y1=z1=0;
  for(i=0;i<blob_db;i++){
    a=sqr((dx=point3D.x-blobs[i].x));
    a+=sqr((dy=point3D.y-blobs[i].y));
    a+=sqr((dz=point3D.z-blobs[i].z));
    a=1/sqr(a);
    x1+=a*dx;
    y1+=a*dy;
    z1+=a*dz;
  }

  a=1/sqrt(sqr(x1)+sqr(y1)+sqr(z1));

//  if(a<Vmin)Vmin=a;
//  if(a>Vmax)Vmax=a;
//  points[p].tx=65536*(128+128*x1/a);
//  points[p].ty=65536*(128+128*y1/a);
//  a=100000000000;

  p2D->xn=x1*a;
  p2D->yn=y1*a;
  p2D->zn=-z1*a;
//}

//  a=800;
//  a=6000*10000.0/(10000.0+(double)point3D.z);
  p2D->xx=point3D.x-(nx/2)*sx2;
  p2D->yy=point3D.y-(ny/2)*sy2;
  p2D->zz=point3D.z-(nz/2)*sz2;

//!!!!!!!!!!!!!!!!!!!!!
{ int i;
  float v2,v=0,r,g,b,vm;
  int vv;
  r=g=b=0;
  for(i=0;i<blob_db;i++) {
//    printf("blobs.x=%d  3D.x=%d\n",blobs[i].x,point3D.x);
      vv=sqr(blobs[i].x-point3D.x);
      vv+=sqr(blobs[i].y-point3D.y);
      vv+=sqr(blobs[i].z-point3D.z);
      vv>>=blobtable_bits;
      if(vv<blobtable_size){
        v2=blobtable[vv];
        r+=v2*blobs[i].rgb[0];
        g+=v2*blobs[i].rgb[1];
        b+=v2*blobs[i].rgb[2];
        v+=v2;
      }
//      v2>>=blobtable_bits;
//      blob_function(vv[0],v2)  // v+=1/sqrt(v2)
  }
//  printf("v=%f  RGB: %f  %f  %f\n",v,r,g,b);
//  if(p2D->zn<=0) v=0; else v=p2D->zn/v;
  vm=p2D->xn*light_vector[0]+p2D->yn*light_vector[1]+p2D->zn*light_vector[2];
  if(vm<0) vm=-vm;
  v=vm/v;

//  if(p2D->zn<=0) v=-p2D->zn/v; else v=p2D->zn/v;
  v*=ast3d_blend;
  p2D->rgb[0]=v*r;
  p2D->rgb[1]=v*g;
  p2D->rgb[2]=v*b;
//  printf("RGB: %d  %d  %d\n",p2D->rgb[0],p2D->rgb[1],p2D->rgb[2]);
}

}


static void blob_drawlineGL(POINT2D_TSTRUCT *p1,POINT2D_TSTRUCT *p2){
double x,y,z;
//  Line_Normal(p1->xx,p1->yy,p1->zz, p2->xx,p2->yy,p2->zz );

//glBegin(GL_LINES);

  x=p1->xx;
  y=p1->yy;
  z=p1->zz;
  z+=15000;
  x/=10000;
  y/=10000;
  z/=-4000;
  glVertex3f(x,y,z);

  x=p2->xx;
  y=p2->yy;
  z=p2->zz;
  z+=15000;
  x/=10000;
  y/=10000;
  z/=-4000;
  glVertex3f(x,y,z);
//glEnd();

}


static void blob_drawpoly_addp_GL(POINT2D_TSTRUCT *p1){
float x,y,z;
//  glTexCoord2f((double)(p1->tx)/(256*65536),(double)(p1->ty)/(256*65536));

//  x=p1->xn; x/=0x1000000;
//  y=p1->yn; y/=0x1000000;
//  z=p1->zn; z/=0x1000000;
  glNormal3f(p1->xn,p1->yn,p1->zn);
//  glTexCoord2f(p1->xn,p1->yn);
  glTexCoord2f(0.5+0.5*p1->xn,0.5+0.5*p1->yn);
  glColor3ub(p1->rgb[0],p1->rgb[1],p1->rgb[2]);

  x=p1->xx; x/=3000;
  y=p1->yy; y/=3000;
  z=p1->zz; z/=-2000;
//  printf("x=%f  y=%f  z=%f\n",x,y,z);
  glVertex3f(x,y,z-10.0);

}

static void blob_drawpoly(POINT2D_TSTRUCT *p1,POINT2D_TSTRUCT *p2,POINT2D_TSTRUCT *p3){

//if( (double)(p1->xx-p2->xx)*(p1->yy-p3->yy)-(double)(p1->xx-p3->xx)*(p1->yy-p2->yy) >0 ){

    polys[poly_db][0]=p1;
    polys[poly_db][1]=p2;
    polys[poly_db][2]=p3;
    ++poly_db;

  ++Vpolydb2;
//  printf("polydb=%d\n",Vpolydb2);

//}

}

static void blob_drawline(POINT2D_TSTRUCT *p1,POINT2D_TSTRUCT *p2){
    polys[poly_db][0]=p1;
    polys[poly_db][1]=p2;
    ++poly_db;
}


// 1 kockat szamol es rajzol ki  (marching cubes algoritmus)
static int blob_do_cube(long xx,long yy,long zz){
int b;
int i,b2;
long v,v2;
long vv[8];

++Vcubedb;  // vizsgalt kockak szama

if(ix<1 || iy<1 || iz<0 || ix>=nx-1 || iy>=ny-1 || iz>=nz-1) return 0;

#define xx1 (xx+sx2)
#define yy1 (yy+sy2)
#define zz1 (zz+sz2)

//if(line_blob && (cubes[ix][iy][iz]==framenum-1)) ++Vskipped2;

cubes[ix][iy][iz]=framenum;

   b=0;

//   vv[0]=blob_value(xx, yy1,zz );   if(vv[0]>Vlimit)b|=1;
   vv[0]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx2=sqr(blobs[i].x-xx));
      v2+=(blobs[i].dy12=sqr(blobs[i].y-yy1));
      v2+=(blobs[i].dz2=sqr(blobs[i].z-zz));
      v2>>=blobtable_bits;
      blob_function(vv[0],v2)  // v+=1/sqrt(v2)
   }
   if(vv[0]>Vlimit)b|=1;

//     vv[1]=blob_value(xx1,yy1,zz );   if(vv[1]>Vlimit)b|=2;
   vv[1]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx12=sqr(blobs[i].x-xx1));
      v2+=(blobs[i].dy12);
      v2+=(blobs[i].dz2);
      v2>>=blobtable_bits;
      blob_function(vv[1],v2)  // v+=1/sqrt(v2)
   }
   if(vv[1]>Vlimit)b|=2;

//     vv[2]=blob_value(xx1,yy1,zz1);   if(vv[2]>Vlimit)b|=4;
   vv[2]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx12);
      v2+=(blobs[i].dy12);
      v2+=(blobs[i].dz12=sqr(blobs[i].z-zz1));
      v2>>=blobtable_bits;
      blob_function(vv[2],v2)  // v+=1/sqrt(v2)
   }
   if(vv[2]>Vlimit)b|=4;

//     vv[3]=blob_value(xx, yy1,zz1);   if(vv[3]>Vlimit)b|=8;
   vv[3]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx2);
      v2+=(blobs[i].dy12);
      v2+=(blobs[i].dz12);
      v2>>=blobtable_bits;
      blob_function(vv[3],v2)  // v+=1/sqrt(v2)
   }
   if(vv[3]>Vlimit)b|=8;

//     vv[4]=blob_value(xx, yy, zz );   if(vv[4]>Vlimit)b|=16;
   vv[4]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx2);
      v2+=(blobs[i].dy2=sqr(blobs[i].y-yy));
      v2+=(blobs[i].dz2);
      v2>>=blobtable_bits;
      blob_function(vv[4],v2)  // v+=1/sqrt(v2)
   }
   if(vv[4]>Vlimit)b|=16;

//     vv[5]=blob_value(xx1,yy, zz );   if(vv[5]>Vlimit)b|=32;
   vv[5]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx12);
      v2+=(blobs[i].dy2);
      v2+=(blobs[i].dz2);
      v2>>=blobtable_bits;
      blob_function(vv[5],v2)  // v+=1/sqrt(v2)
   }
   if(vv[5]>Vlimit)b|=32;

//     vv[6]=blob_value(xx1,yy, zz1);   if(vv[6]>Vlimit)b|=64;
   vv[6]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx12);
      v2+=(blobs[i].dy2);
      v2+=(blobs[i].dz12);
      v2>>=blobtable_bits;
      blob_function(vv[6],v2)  // v+=1/sqrt(v2)
   }
   if(vv[6]>Vlimit)b|=64;

//     vv[7]=blob_value(xx, yy, zz1);   if(vv[7]>Vlimit)b|=128;
   vv[7]=0;
   for(i=0;i<blob_db;i++) {
      v2=(blobs[i].dx2);
      v2+=(blobs[i].dy2);
      v2+=(blobs[i].dz12);
      v2>>=blobtable_bits;
      blob_function(vv[7],v2)  // v+=1/sqrt(v2)
   }
   if(vv[7]>Vlimit)b|=128;


     b2=edgeTable[b];
     if(b2==0) return 0;   // nincs egy el se amit metszene!

++Vcubedb2;  // felhasznalt kockak szama

   // van polygon!!!!!

   //  Also lap:
   if(b2&1){      // egde #0
     p2Dtable[0]=(&cubepoints[ix][iy+1][iz][0]);
     if(cubes[ix][iy+1][iz]<framenum && cubes[ix][iy+1][iz-1]<framenum && cubes[ix][iy][iz-1]<framenum){
       point3D.x=xx+(sx2*(Vlimit-vv[0]))/(vv[1]-vv[0]);
       point3D.y=yy1;
       point3D.z=zz;
       blob_calcnormal2(p2Dtable[0]);
     } else ++Vskipped;
   }
   if(b2&2){      // egde #1
     p2Dtable[1]=(&cubepoints[ix+1][iy+1][iz][2]);
     if(cubes[ix+1][iy+1][iz]<framenum && cubes[ix+1][iy][iz]<framenum && cubes[ix+1][iy+1][iz]<framenum){
       point3D.x=xx1;
       point3D.y=yy1;
       point3D.z=zz+(sz2*(Vlimit-vv[1]))/(vv[2]-vv[1]);
       blob_calcnormal2(p2Dtable[1]);
     } else ++Vskipped;
   }
   if(b2&4){      // egde #2
     p2Dtable[2]=(&cubepoints[ix][iy+1][iz+1][0]);
     if(cubes[ix][iy+1][iz+1]<framenum && cubes[ix][iy+1][iz]<framenum && cubes[ix][iy][iz+1]<framenum){
       point3D.x=xx+(sx2*(Vlimit-vv[3]))/(vv[2]-vv[3]);
       point3D.y=yy1;
       point3D.z=zz1;
       blob_calcnormal2(p2Dtable[2]);
     } else ++Vskipped;
   }
   if(b2&8){      // egde #3
     p2Dtable[3]=(&cubepoints[ix][iy+1][iz][2]);
     if(cubes[ix][iy+1][iz]<framenum && cubes[ix-1][iy+1][iz]<framenum && cubes[ix-1][iy][iz]<framenum){
       point3D.x=xx;
       point3D.y=yy1;
       point3D.z=zz+(sz2*(Vlimit-vv[0]))/(vv[3]-vv[0]);
       blob_calcnormal2(p2Dtable[3]);
     } else ++Vskipped;
   }

   //  Felso lap:
   if(b2&16){      // egde #4
     p2Dtable[4]=(&cubepoints[ix][iy][iz][0]);
     if(cubes[ix][iy][iz-1]<framenum && cubes[ix][iy-1][iz]<framenum && cubes[ix][iy-1][iz-1]<framenum){
       point3D.x=xx+(sx2*(Vlimit-vv[4]))/(vv[5]-vv[4]);
       point3D.y=yy;
       point3D.z=zz;
       blob_calcnormal2(p2Dtable[4]);
     } else ++Vskipped;
   }
   if(b2&32){      // egde #5
     p2Dtable[5]=(&cubepoints[ix+1][iy][iz][2]);
     if(cubes[ix+1][iy][iz]<framenum && cubes[ix][iy-1][iz]<framenum && cubes[ix+1][iy-1][iz]<framenum){
       point3D.x=xx1;
       point3D.y=yy;
       point3D.z=zz+(sz2*(Vlimit-vv[5]))/(vv[6]-vv[5]);
       blob_calcnormal2(p2Dtable[5]);
     } else ++Vskipped;
   }
   if(b2&64){      // egde #6
     p2Dtable[6]=(&cubepoints[ix][iy][iz+1][0]);
     if(cubes[ix][iy][iz+1]<framenum && cubes[ix][iy-1][iz]<framenum && cubes[ix][iy-1][iz+1]<framenum){
       point3D.x=xx+(sx2*(Vlimit-vv[7]))/(vv[6]-vv[7]);
       point3D.y=yy;
       point3D.z=zz1;
       blob_calcnormal2(p2Dtable[6]);
     } else ++Vskipped;
   }
   if(b2&128){      // egde #7
     p2Dtable[7]=(&cubepoints[ix][iy][iz][2]);
     if(cubes[ix][iy-1][iz]<framenum && cubes[ix-1][iy-1][iz]<framenum && cubes[ix-1][iy][iz]<framenum){
       point3D.x=xx;
       point3D.y=yy;
       point3D.z=zz+(sz2*(Vlimit-vv[4]))/(vv[7]-vv[4]);
       blob_calcnormal2(p2Dtable[7]);
     } else ++Vskipped;
   }

   //  Oldalelek:
   if(b2&256){      // egde #8
     p2Dtable[8]=(&cubepoints[ix][iy][iz][1]);
     if(cubes[ix-1][iy][iz]<framenum && cubes[ix-1][iy][iz-1]<framenum && cubes[ix][iy][iz-1]<framenum){
       point3D.x=xx;
       point3D.y=yy+(sy2*(Vlimit-vv[4]))/(vv[0]-vv[4]);
       point3D.z=zz;
       blob_calcnormal2(p2Dtable[8]);
     } else ++Vskipped;
   }
   if(b2&512){      // egde #9
     p2Dtable[9]=(&cubepoints[ix+1][iy][iz][1]);
     if(cubes[ix+1][iy][iz]<framenum && cubes[ix+1][iy][iz-1]<framenum && cubes[ix][iy][iz-1]<framenum){
       point3D.x=xx1;
       point3D.y=yy+(sy2*(Vlimit-vv[5]))/(vv[1]-vv[5]);
       point3D.z=zz;
       blob_calcnormal2(p2Dtable[9]);
     } else ++Vskipped;
   }
   if(b2&1024){      // egde #10
     p2Dtable[10]=(&cubepoints[ix+1][iy][iz+1][1]);
     if(cubes[ix+1][iy][iz+1]<framenum && cubes[ix+1][iy][iz]<framenum && cubes[ix][iy][iz+1]<framenum){
       point3D.x=xx1;
       point3D.y=yy+(sy2*(Vlimit-vv[6]))/(vv[2]-vv[6]);
       point3D.z=zz1;
       blob_calcnormal2(p2Dtable[10]);
     } else ++Vskipped;
   }
   if(b2&2048){      // egde #11
     p2Dtable[11]=(&cubepoints[ix][iy][iz+1][1]);
     if(cubes[ix][iy][iz+1]<framenum && cubes[ix-1][iy][iz]<framenum && cubes[ix-1][iy][iz+1]<framenum){
       point3D.x=xx;
       point3D.y=yy+(sy2*(Vlimit-vv[7]))/(vv[3]-vv[7]);
       point3D.z=zz1;
       blob_calcnormal2(p2Dtable[11]);
     } else ++Vskipped;
   }

if(line_blob){
   i=0;
   while(lineTable[b][i]!=-1){
     blob_drawline(p2Dtable[lineTable[b][i]],p2Dtable[lineTable[b][i+1]]);
     ++Vlines;
     i+=2;
   }
}else{
   i=0;
   while(triTable[b][i]!=-1){
//     blob_drawline(p2Dtable[lineTable[b][i]],p2Dtable[lineTable[b][i+1]]);
     blob_drawpoly(p2Dtable[triTable[b][i]],p2Dtable[triTable[b][i+1]],p2Dtable[triTable[b][i+2]]);
//     blob_drawpolyFLAT(p2Dtable[triTable[b][i]],p2Dtable[triTable[b][i+1]],p2Dtable[triTable[b][i+2]]);
     ++Vpolydb;
     i+=3;
   }
}

//          for(i=0;i<12;i++){ if(b2&(1<<i))blob_drawpoint(i); }


  return b2;
}


#define blob_vrecurse if(cubes[ix][iy][iz]<framenum) blob_recurse();

// ez elvileg a blob felulete menten halad vegig. -> nem szamol feleslegesen
// ix/iy/iz a blob feluleten kell legyen!
static int blob_recurse(){
int b2;
  if(ix<0 || ix>=nx || iy<0 || iy>=ny || iz<0 || iz>=nz) return 0;  // kil¢g!

  b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2);
  // b2=bitmaszk, melyik eleket metszi a blobfelulet

  if(b2&(1+2+4+8))        {++iy;blob_vrecurse;--iy;}    // A = lefel‚
  if(b2&(16+32+64+128))   {--iy;blob_vrecurse;++iy;}    // F = felfel‚
  if(b2&(8+128+256+2048)) {--ix;blob_vrecurse;++ix;}    // B = balra
  if(b2&(2+32+512+1024))  {++ix;blob_vrecurse;--ix;}    // J = jobbra

  if(b2&(1+16+256+512))   {--iz;blob_vrecurse;++iz;}    // E = el“re
//if(line_blob){
  if(b2&(4+64+1024+2048))  {++iz;blob_vrecurse;--iz;}    // H = h tra <- kell?
//}

  return b2;
}


// ix,iy,iz a blob belsejeben kell legyen!
// megkeresi a blob felszinet, hogy utana a blob_recurse()-vel bejarja
void blob_find(){
int b2;
int ix2=ix;
int iy2=iy;
int iz2=iz;
char i[10];

  for(ix=ix2+1;ix<nx;ix++){
    if(b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2)) goto megvan;
  }
  for(ix=ix2-1;ix>=0;ix--){
    if(b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2)) goto megvan;
  }

  for(iy=iy2+1;iy<ny;iy++){
    if(b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2)) goto megvan;
  }
  for(iy=iy2-1;iy>=0;iy--){
    if(b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2)) goto megvan;
  }

  for(iz=iz2+1;iz<nz;iz++){
    if(b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2)) goto megvan;
  }
  for(iz=iz2-1;iz>=0;iz--){
    if(b2=blob_do_cube(ix*sx2,iy*sy2,iz*sz2)) goto megvan;
  }

//  printf("BLOB not found!!!\n");getch();
return;  // nem talalta meg!!! nincs blob??? elloptak? :)

megvan:
  if(b2&(1+2+4+8))        {++iy;blob_vrecurse;--iy;}    // A = lefel‚
  if(b2&(16+32+64+128))   {--iy;blob_vrecurse;++iy;}    // F = felfel‚
  if(b2&(8+128+256+2048)) {--ix;blob_vrecurse;++ix;}    // B = balra
  if(b2&(2+32+512+1024))  {++ix;blob_vrecurse;--ix;}    // J = jobbra

  if(b2&(1+16+256+512))   {--iz;blob_vrecurse;++iz;}    // E = el“re
//if(line_blob){
  if(b2&(4+64+1024+2048))  {++iz;blob_vrecurse;--iz;}    // H = h tra <- kell?
//}

}

void DrawBlob(float szog,fx_blob_struct *params){
int i,j;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(GLfloat) window_w/(GLfloat) window_h, 1.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_LIGHTING);

#define BLOB_RGB(i,r,g,b) blobs[i].rgb[0]=r;blobs[i].rgb[1]=g;blobs[i].rgb[2]=b;

      blobs[0].x=params->pos[0]+params->rad[0]*(cos(szog*0.763333));
      blobs[0].y=params->pos[1]+params->rad[1]*(sin(szog*0.763333));
      blobs[0].z=params->pos[2]+params->rad[2]*(cos(szog*0.363333));

      blobs[1].x=params->pos[0]+params->rad[0]*(cos(szog*0.2433333));
      blobs[1].y=params->pos[1]+params->rad[1]*(sin(szog*0.243333));
      blobs[1].z=params->pos[2]+params->rad[2]*(cos(szog*0.4633333));

      blobs[2].x=params->pos[0]+params->rad[0]*(cos(szog*0.4072433333));
      blobs[2].y=params->pos[1]+params->rad[1]*(sin(szog*0.407243333));
      blobs[2].z=params->pos[2]+params->rad[2]*(cos(szog*0.26894633333));

      blobs[3].x=params->pos[0]+params->rad[0]*(cos(szog*0.29003633));
      blobs[3].y=params->pos[1]+params->rad[1]*(sin(szog*0.29003633));
      blobs[3].z=params->pos[2]+params->rad[2]*(cos(szog*0.9824633333));

      blobs[4].x=params->pos[0]+params->rad[0]*(cos(szog*0.89364072433333));
      blobs[4].y=params->pos[1]+params->rad[1]*(sin(szog*0.8936407243333));
      blobs[4].z=params->pos[2]+params->rad[2]*(cos(szog*0.5326894633333));

//      BLOB_RGB(0, 230,0,80);
//      BLOB_RGB(1, 30,170,210);
//      BLOB_RGB(2, 130,50,130);
//      BLOB_RGB(3, 50,0,255);
//      BLOB_RGB(4, 150,240,55);

      BLOB_RGB(0, 255,0,0);
      BLOB_RGB(1, 0,255,0);
      BLOB_RGB(2, 0,0,255);
      BLOB_RGB(3, 255,0,255);
      BLOB_RGB(4, 255,255,0);

      Vpolydb=0;
      Vpolydb2=0;
      Vcubedb=0;
      Vcubedb2=0;
      Vlines=0;

      line_blob=params->line_blob;
      Vlimit=params->vlimit;
      poly_db=0;
      ++framenum;   // enelkul nem muxik!!!
      for(i=0;i<blob_db;i++){
         ix=blobs[i].x/sx2;
         iy=blobs[i].y/sy2;
         iz=blobs[i].z/sz2;
         blob_find();
      }


if(!line_blob){
    //----------- POLYGON-BLOB    ---------->ZSORT
    for(i=0;i<poly_db;i++){
      int p;
      int z=polys[i][0]->zz;
         z+=polys[i][1]->zz;
         z+=polys[i][2]->zz;
         z=10000-(z/5);
         bytesortdata[i+1]=z; z&=255;
        /* beszuras lancolt lista vegere */
        if((p=bytesort_end[0][z])) bytesort_next[0][p]=bytesort_end[0][z]=i+1;
                              else bytesort_start[0][z]=bytesort_end[0][z]=i+1;
    }

    for(j=0;j<256;j++){
      if((i=bytesort_start[0][j])) while(1){
        int z=(bytesortdata[i]>>8)&255;
        int p;
        /* beszuras lancolt lista vegere */
        if((p=bytesort_end[1][z])) bytesort_next[1][p]=bytesort_end[1][z]=i;
                              else bytesort_start[1][z]=bytesort_end[1][z]=i;
        if(i==bytesort_end[0][j]) break;
        i=bytesort_next[0][i];
      } bytesort_end[0][j]=bytesort_start[0][j]=0;
    }

    aglZbuffer(AGL_ZBUFFER_NONE);
    aglTexture(blobmap);
    aglBlend(AGL_BLEND_ADD);

    if(params->blob_alpha>0){
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GEQUAL,params->blob_alpha);
    }

    glBegin(GL_TRIANGLES);
    for(j=0;j<256;j++){
      if((i=bytesort_start[1][j])) while(1){
        blob_drawpoly_addp_GL(polys[i-1][0]);
        blob_drawpoly_addp_GL(polys[i-1][1]);
        blob_drawpoly_addp_GL(polys[i-1][2]);
        if(i==bytesort_end[1][j]) break;
        i=bytesort_next[1][i];
      } bytesort_end[1][j]=bytesort_start[1][j]=0;
    }
    glEnd();
    aglZbuffer(AGL_ZBUFFER_RW);
    glDisable(GL_ALPHA_TEST);
    
} else {
    //----------- LINE-BLOB ----------
      aglTexture(0);
      aglBlend(AGL_BLEND_NONE);

      glBegin(GL_LINES);
      for(i=0;i<poly_db;i++){
        blob_drawpoly_addp_GL(polys[i][0]);
        blob_drawpoly_addp_GL(polys[i][1]);
      }
      glEnd();
}


//      printf("Rendered blob frame#%d,  %d triangles\n",framenum,Vpolydb2);
}




// Astral Script System II.    (C) 1999. A'rpi/ASTRAL

//#define SCR_DEBUG
#define FX_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#ifdef __GNUC__
#include <sys/time.h>
#include <sys/types.h>
#else
#include <windows.h>
#endif

#include "../agl/agl.h"

#include "../3dslib/ast3d.h"
#include "../timer/timer.h"
#include "../mp3lib/mp3.h"
#include "../afs/afs.h"
#include "../loadmap/load_map.h"
#include "../loadmap/loadtxtr.h"

#include "../blob/blob.h"
#include "../fdtunnel/fdtunnel.h"
#include "../spline/spline.h"
#include "../smoke/smoke.h"
#include "../sinpart/sinpart.h"
#include "../greets/greets.h"
#include "../render/render.h"
#include "../hjbtunel/hjbtunel.h"
#include "../bsptunel/bsptunel.h"
#include "../swirl/swirl.h"
#include "../fdwater/fdwater.h"


// ------ LIMITS ---------------
// vars:
#define scrMAXVARS 512
#define scrHASH 256
// draw:
#define FX_DB 20
#define MAX_FADER 32
// file:
#define SCRIPT_MAXSIZE 65536
#define SOR_MAXSIZE 8192

// ------ TYPEs ----------------
#define scrTYPE_command 0
#define scrTYPE_int 1
#define scrTYPE_float 2
#define scrTYPE_str 3
#define scrTYPE_pic 5
#define scrTYPE_scene 6
#define scrTYPE_flag 7
#define scrTYPE_picanim 8
#define scrTYPE_const 10
#define scrTYPE_newvar 11

#define scrCLASS_global 0
#define scrCLASS_fx 1
#define scrCLASS_scene 2
#define scrCLASS_light 3
#define scrCLASS_object 4
#define scrCLASS_material 5

#define FXTYPE_NONE 0
#define FXTYPE_SCENE 1
#define FXTYPE_FDTUNNEL 2
#define FXTYPE_BLOB 3
#define FXTYPE_PICTURE 4
#define FXTYPE_SPLINESURFACE 5
#define FXTYPE_SMOKE 6
#define FXTYPE_SINPART 7
#define FXTYPE_GREETS 8
#define FXTYPE_HJBTUNNEL 9
#define FXTYPE_SWIRL 10
#define FXTYPE_BSPTUNNEL 11
#define FXTYPE_FDWATER 12

#define MAX_PDB 20

typedef struct {          // commands
   int code,type;         // pl. code=picture  type=pic/sprite/addsprite/colorbox...
   int pdb_min,pdb_max;   // number of parameters
   int ptype[MAX_PDB];    // types of the parameters
   float defval[MAX_PDB]; // default value for optional params
} scrCmdStruct;

typedef struct scrVarStruct_st {
   char *Name;
   int Type,Class;
   void* ptr;
   int param;  // pl. scrTYPE_flag-hoz
   struct scrVarStruct_st *next;  // For HASH search!
} scrVarStruct;

scrVarStruct *scrHashTable[scrHASH];
scrVarStruct scrVariables[scrMAXVARS];
int scrVarDB=0;

typedef struct {
  int type;        /* 0=linear  1=sinus */
  float *ptr;
  float blend;          /* 0..1 megy */
  float speed;          /* blend+=speed*relative_time */
  float start,end;
  float offs,amp;  /* sinus-hoz */
} fade_struct;

fade_struct fader[MAX_FADER];

typedef struct {
  int nummaps;
  texture_st** maps;
} picanim_st;

typedef struct {
  int type;
  float x1,y1,x2,y2,z;
  int id;
  float rgb[3];
  int alphamode,zbuffer;
  float alphalevel;
  float angle,xscale,yscale,xoffs,yoffs; // noise params
} pic_struct;  

typedef struct {
  int type;
  float blend;
  float fps;
  float frame;
  fx_blob_struct blob;
  fx_smoke_struct smoke;
  fx_sinpart_struct sinpart;
  fx_fdtunnel_struct fdtunnel;
  fx_hjbtunnel_struct hjbtunnel;
  fx_greets_struct greets;
  fx_swirl_struct swirl;
  fx_fdwater_struct fdwater;
  /* 3DS player: */
  c_SCENE *scene;
  int loop_scene;
  /* PIC: */
  pic_struct pic;
  /* Spline surface */
  float face_blend,wire_blend,spline_size,spline_n;
} fx_struct;

fx_struct fxlist[FX_DB];
fx_struct fxdefault;
fx_struct *current_fx=&fxlist[0];

c_LIGHT*  current_light=(c_LIGHT*)NULL;
c_OBJECT* current_object=(c_OBJECT*)NULL;
c_SCENE*  current_scene=(c_SCENE*)NULL;
c_MATERIAL* current_material=(c_MATERIAL *)NULL;

float adk_time=0.0;
float adk_time_corrected=0.0;

int nosound=0;
int scr_playing=0;
int adk_clear_buffer_flag=1;

extern void ExitDemo();

#ifdef FX_DEBUG
FILE *fx_debug=NULL;
#endif

//================ PROCEDURES ===========================


static int scrHashFV(char *s){
  int len=strlen(s);
  int hash=0;
  int i;
  for(i=0;i<len;i++) hash=((int)s[i])^(hash<<5)^(hash>>27);
  return (hash&(scrHASH-1));
}

scrVarStruct* scrAddNewVar(char *Name,int Type,int Class,int param,void *ptr){
scrVarStruct *v;
int hash=scrHashFV(Name);
  if(scrVarDB>=scrMAXVARS){ printf("scrAddNewVar: too many variables!\n");exit(1);}
#ifdef SCR_DEBUG
  printf("AddNewVar: %3d. type=%2d  class=%2d  name='%s'\n",scrVarDB,Type,Class,Name);
#endif
  v=&scrVariables[scrVarDB++];
  v->Name=Name;
  v->Type=Type;
  v->Class=Class;
  v->param=param;
  v->ptr=ptr;
  v->next=scrHashTable[hash]; scrHashTable[hash]=v;
  return v;
}

scrVarStruct* scrSearchVar(char *Name){
scrVarStruct *v=scrHashTable[scrHashFV(Name)];
  while(v){
    if(strcmp(v->Name,Name)==0) break;
    v=v->next;
  }
  return v;
}

scrVarStruct* scrAddNewCmd(char *Name,int Class,int code,int type,int pdb_min,int pdb_max){
scrCmdStruct *c=malloc(sizeof(scrCmdStruct));
int i;
  if(!c) return (scrVarStruct *)NULL;
  c->code=code;
  c->type=type;
  if(pdb_max<0 || pdb_max>=MAX_PDB) pdb_max=MAX_PDB-1;
  c->pdb_min=pdb_min;
  c->pdb_max=pdb_max;
  for(i=0;i<=pdb_max;i++){ c->ptype[i]=0; c->defval[i]=0.0;}
  return scrAddNewVar(Name,scrTYPE_command,Class,0,(void*)c);
}

void scrInit(){
int i;
  scrVarStruct *var=(scrVarStruct *)NULL;
  for(i=0;i<scrHASH;i++) scrHashTable[i]=(scrVarStruct *)NULL;
  { fx_struct *fx=(fx_struct *)NULL;
    c_OBJECT *obj=(c_OBJECT *)NULL;
    c_LIGHT *light=(c_LIGHT *)NULL;
    c_SCENE *scene=(c_SCENE *)NULL;
    c_MATERIAL *mat=(c_MATERIAL *)NULL;
#include "vars.h"
  }
  { scrCmdStruct *cmd=(scrCmdStruct *)NULL;
#include "cmds.h"
  }
}


int str2int(char *s){
char* hiba;
  if(strcmp(s,"on")==0)return 1;
  if(strcmp(s,"off")==0)return 0;
  if(strcmp(s,"true")==0)return 1;
  if(strcmp(s,"false")==0)return 0;
  if(strcmp(s,"yes")==0)return 1;
  if(strcmp(s,"no")==0)return 0;
  return strtol(s,&hiba,0);  // hex-t is tud!!
//  if(s[0]=='0' && toupper(s[1])=='X'){}
//  return atoi(s);
}

float str2float(char *s){ return atof(s);}

void fx_init(){
  int f;
  for(f=0;f<FX_DB;f++) memcpy(&fxlist[f],&fxdefault,sizeof(fxdefault));
}

#define FX_RESET(fx) memcpy(fx,&fxdefault,sizeof(fxdefault))

#include "scr_evnt.c"
#include "scr_draw.c"

#include "file.c"

#if 0
int main(){
  scrInit();
  printf("sizeof(fxdefault)=%d\n",sizeof(fxdefault));
  return 0;
}
#endif



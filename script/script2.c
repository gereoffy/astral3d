// Astral Script System II.    (C) 1999-2000. A'rpi/ASTRAL

//#define SCR_DEBUG
#define FX_DEBUG
#define MAX_SUPPORT

#include "../config.h"
#include "../agl/agl.h"
// system:
#include "../timer/timer.h"
#include "../mp3lib/mp3.h"
#include "../afs/afs.h"
#include "../loadmap/load_map.h"
#include "../loadmap/loadtxtr.h"
// .3ds player:
#include "../3dslib/ast3d.h"
#include "../render/render.h"
// .max player:
#include "../MAX/max3d.h"
#include "../MAX/render.h"
// 2D effects:
#include "../fx/libastFX.h"

// ------ LIMITS ---------------
// vars:
#define scrMAXVARS 512
#define scrHASH 256
// draw:
#define FX_DB 20
#define MAX_FADER 64
// file:
#define SCRIPT_MAXSIZE 65536
#define SOR_MAXSIZE 8192
#define MAX_PDB 20

// ------ TYPEs ----------------
#define scrTYPE_command 0
#define scrTYPE_int 1
#define scrTYPE_float 2
#define scrTYPE_str 3
#define scrTYPE_pic 5
#define scrTYPE_scene 6
#define scrTYPE_flag 7
#define scrTYPE_picanim 8
#define scrTYPE_maxscene 9
#define scrTYPE_const 10
#define scrTYPE_newvar 11

#define scrCLASS_global 0
#define scrCLASS_fx 1
#define scrCLASS_scene 2
#define scrCLASS_light 3
#define scrCLASS_object 4
#define scrCLASS_material 5
#define scrCLASS_maxscene 6

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
#define FXTYPE_SINZOOM 13
#define FXTYPE_MAXSCENE 14
#define FXTYPE_GEARS 15

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
  float blend;
  float fps;
  float frame;
  /* 2D Effects: */
  fx_blob_struct blob;
  fx_smoke_struct smoke;
  fx_sinpart_struct sinpart;
  fx_fdtunnel_struct fdtunnel;
  fx_hjbtunnel_struct hjbtunnel;
  fx_greets_struct greets;
  fx_swirl_struct swirl;
  fx_fdwater_struct fdwater;
  fx_sinzoom_struct sinzoom;
  fx_gears_struct gears;
  fx_pic_struct pic;
  fx_spline_struct spline;
  /* 3DS/MAX player: */
  c_SCENE *scene;
  c_CAMERA *cam;
  Scene *maxscene;
  int loop_scene;
  /* Spline surface */
//  float face_blend,wire_blend,spline_size,spline_n;
} fx_struct;

fx_struct fxlist[FX_DB];              // layers
fx_struct fxdefault;                  // fx template (default values)
fx_struct *current_fx=&fxlist[0];     // current (selected) fx

void print_currentfx_params(){
  if(!current_fx) return;
  printf("frame=%8.4f   blend=%5.3f   fps=%5.2f\n",current_fx->frame,current_fx->blend,current_fx->fps);
}

c_LIGHT*  current_light=(c_LIGHT*)NULL;
c_OBJECT* current_object=(c_OBJECT*)NULL;
c_SCENE*  current_scene=(c_SCENE*)NULL;
c_MATERIAL* current_material=(c_MATERIAL *)NULL;

Scene*  current_maxscene=(Scene*)NULL;

float adk_time=0.0;
float adk_time_corrected=0.0;
float adk_mp3_frame_correction=0;

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
    Scene *maxscene=(Scene *)NULL;
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



// #define SHOW_UNKNOWN_CHUNKS
//#define SHOW_CHUNK_TREE
//#define SHOW_CHUNK_NAMES

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ast3d.h"
#include "ast3di.h"

#include "vector.h"
#include "matrix.h"

#ifdef SHOW_CHUNK_NAMES
#include "chunknam.c"
#endif

/*****************************************************************************
  chunks/readers definitions, structures
*****************************************************************************/

enum ast3d_3ds_chunks_ { /* Chunk ID */
  CHUNK_RGBF         = 0x0010, CHUNK_RGBB         = 0x0011,
  CHUNK_PRJ          = 0xC23D, CHUNK_MLI          = 0x3DAA,
  CHUNK_MAIN         = 0x4D4D, CHUNK_OBJMESH      = 0x3D3D,
  CHUNK_BKGCOLOR     = 0x1200, CHUNK_AMBCOLOR     = 0x2100,
  CHUNK_FOGPARAMS    = 0x2200,

  CHUNK_EN_FOG       = 0x2201,
  CHUNK_EN_DISTCUE   = 0x2301,
  CHUNK_EN_LAYERED   = 0x2303,

  CHUNK_EN_BG_BMP    = 0x1101,
  CHUNK_EN_BG_SOLID  = 0x1201,
  CHUNK_EN_BG_GRAD   = 0x1301,

  CHUNK_OBJBLOCK     = 0x4000, CHUNK_TRIMESH      = 0x4100,
  CHUNK_VERTLIST     = 0x4110, CHUNK_VERTFLAGS    = 0x4111,
  CHUNK_FACELIST     = 0x4120, CHUNK_FACEMAT      = 0x4130,
  CHUNK_MAPLIST      = 0x4140, CHUNK_SMOOTHLIST   = 0x4150,
  CHUNK_TRMATRIX     = 0x4160, CHUNK_MESHCOLOR    = 0x4165,
  CHUNK_TXTINFO      = 0x4170, 

  CHUNK_LIGHT        = 0x4600,  CHUNK_SPOTLIGHT    = 0x4610,
  CHUNK_CAMERA       = 0x4700,  CHUNK_HIERARCHY    = 0x4F00,
  CHUNK_VIEWPORT     = 0x7001,
  CHUNK_MATERIAL     = 0xAFFF, 
  
  CHUNK_INNER_RANGE  = 0x4659,
  CHUNK_OUTER_RANGE  = 0x465A,
  CHUNK_LIGHT_ATTENUATE = 0x4625,

  CHUNK_MATNAME      = 0xA000, CHUNK_AMBIENT      = 0xA010,
  CHUNK_DIFFUSE      = 0xA020, CHUNK_SPECULAR     = 0xA030,
  CHUNK_SHININESS    = 0xA040, CHUNK_SHINSTRENGTH = 0xA041,
  CHUNK_TRANSPARENCY = 0xA050, CHUNK_TRANSFALLOFF = 0xA052,
  CHUNK_REFBLUR      = 0xA053, CHUNK_SELFILLUM    = 0xA084,
  CHUNK_TWOSIDED     = 0xA081, CHUNK_TRANSADD     = 0xA083,
  CHUNK_WIREON       = 0xA085, CHUNK_SOFTEN       = 0xA08C,
  CHUNK_MATTYPE      = 0xA100, CHUNK_AMOUNTOF     = 0x0030,
  CHUNK_TEXTURE      = 0xA200,
  CHUNK_TEXTURE2     = 0xA33A,
  CHUNK_OPACITYMAP   = 0xA210,
  CHUNK_BUMPMAP      = 0xA230,
  CHUNK_SPECULARMAP  = 0xA204,
  CHUNK_SHINENESSMAP = 0xA33C,
  CHUNK_SELFILLMAP   = 0xA33D,
  CHUNK_REFLECTION   = 0xA220,

  CHUNK_MTEXTURE     = 0xA33E,
  CHUNK_MTEXTURE2    = 0xA340,
  CHUNK_MOPACITYMAP  = 0xA342,
  CHUNK_MBUMPMAP     = 0xA344,
  CHUNK_MSPECULARMAP = 0xA348,
  CHUNK_MSHINENESSMAP= 0xA346,
  CHUNK_MSELFILLMAP  = 0xA34A,
  CHUNK_MREFLECTION  = 0xA34C,
  
  CHUNK_MAPFILE      = 0xA300, CHUNK_MAPFLAGS     = 0xA351,
  CHUNK_MAPBLUR      = 0xA353, CHUNK_MAPUSCALE    = 0xA354, 
  CHUNK_MAPVSCALE    = 0xA356, CHUNK_MAPUOFFSET   = 0xA358, 
  CHUNK_MAPVOFFSET   = 0xA35A, CHUNK_MAPROTANGLE  = 0xA35C,

  CHUNK_KEYFRAMER    = 0xB000, CHUNK_AMBIENTKEY   = 0xB001,
  CHUNK_TRACKINFO    = 0xB002, CHUNK_TRACKOBJNAME = 0xB010,
  CHUNK_TRACKPIVOT   = 0xB013, CHUNK_TRACKPOS     = 0xB020,
  CHUNK_TRACKROTATE  = 0xB021, CHUNK_TRACKSCALE   = 0xB022,
  CHUNK_TRACKMORPH   = 0xB026, CHUNK_TRACKHIDE    = 0xB029,
  CHUNK_OBJNUMBER    = 0xB030, CHUNK_TRACKCAMERA  = 0xB003,
  CHUNK_TRACKFOV     = 0xB023, CHUNK_TRACKROLL    = 0xB024,
  CHUNK_TRACKCAMTGT  = 0xB004, CHUNK_TRACKLIGHT   = 0xB005,
  CHUNK_TRACKLIGTGT  = 0xB006, CHUNK_TRACKSPOTL   = 0xB007,
  CHUNK_TRACKCOLOR   = 0xB025, CHUNK_FRAMES       = 0xB008,
  CHUNK_DUMMYNAME    = 0xB011
 
};

typedef struct { /* 3DS chunk structure */
  word  chunk_id;                /* chunk id (ast3d_3ds_chunks_) */
  dword chunk_size;              /* chunk length                */
} c_CHUNK;

typedef struct { /* Chunk reader list */
  word id;                       /* chunk id        */
  int  sub;                      /* has subchunks   */
  int  (*func) (afs_FILE *f);        /* reader function */
} c_LISTKEY, c_LISTWORLD;

static int read_NULL         (afs_FILE *f); /* (skip chunk)            */
static int read_RGBF         (afs_FILE *f); /* RGB float               */
static int read_RGBB         (afs_FILE *f); /* RGB byte                */
static int read_AMOUNTOF     (afs_FILE *f); /* Amount of               */
static int read_ASCIIZ       (afs_FILE *f); /* ASCIIZ string           */
static int read_TRIMESH      (afs_FILE *f); /* Triangular mesh         */
static int read_VERTLIST     (afs_FILE *f); /* Vertex list             */
static int read_FACELIST     (afs_FILE *f); /* Face list               */
static int read_FACEMAT      (afs_FILE *f); /* Face material           */
static int read_SMOOTHLIST   (afs_FILE *f); /* Smoothing groups        */
static int read_MAPLIST      (afs_FILE *f); /* Mapping list            */
static int read_TRMATRIX     (afs_FILE *f); /* Transformation matrix   */
static int read_LIGHT        (afs_FILE *f); /* Light                   */
static int read_SPOTLIGHT    (afs_FILE *f); /* Spotlight               */
static int read_CAMERA       (afs_FILE *f); /* Camera                  */
static int read_MATERIAL     (afs_FILE *f); /* Material                */
static int read_MATNAME      (afs_FILE *f); /* Material name           */
static int read_FRAMES       (afs_FILE *f); /* Number of frames        */
static int read_OBJNUMBER    (afs_FILE *f); /* Object number           */
static int read_TRACKOBJNAME (afs_FILE *f); /* Track object name       */
static int read_DUMMYNAME    (afs_FILE *f); /* Dummy object name       */
static int read_TRACKPIVOT   (afs_FILE *f); /* Track pivot point       */
static int read_TRACKPOS     (afs_FILE *f); /* Track position          */
static int read_TRACKCOLOR   (afs_FILE *f); /* Track color             */
static int read_TRACKROT     (afs_FILE *f); /* Track rotation          */
static int read_TRACKSCALE   (afs_FILE *f); /* Track scale             */
static int read_TRACKFOV     (afs_FILE *f); /* Track fov               */
static int read_TRACKROLL    (afs_FILE *f); /* Track roll              */
static int read_TRACKMORPH   (afs_FILE *f); /* Track morph             */
static int read_TRACKHIDE    (afs_FILE *f); /* Track hide              */
static int read_MATTYPE      (afs_FILE *f); /* Material: type          */
static int read_MATTWOSIDED  (afs_FILE *f); /* Material: two sided     */
static int read_MATSOFTEN    (afs_FILE *f); /* Material: soften        */
static int read_MATWIRE      (afs_FILE *f); /* Material: wire          */
static int read_MATTRANSADD  (afs_FILE *f); /* Material: transparency  */
static int read_MAPFLAGS     (afs_FILE *f); /* Map flags               */
static int read_MAPFILE      (afs_FILE *f); /* Map file                */
static int read_MAPUSCALE    (afs_FILE *f); /* Map 1/U scale           */
static int read_MAPVSCALE    (afs_FILE *f); /* Map 1/V scale           */
static int read_MAPUOFFSET   (afs_FILE *f); /* Map U offset            */
static int read_MAPVOFFSET   (afs_FILE *f); /* Map V offset            */
static int read_MAPROTANGLE  (afs_FILE *f); /* Map rotation angle      */
static int read_TXTINFO (afs_FILE *f);
static int read_FOGPARAMS (afs_FILE *f);
static int read_ENABLE (afs_FILE *f);
static int read_INNER_RANGE(afs_FILE *f);
static int read_OUTER_RANGE(afs_FILE *f);
static int read_LIGHT_ATTEN(afs_FILE *f);

static c_MAP* get_map_pointer(c_MATERIAL *mat);

static c_LISTWORLD world_chunks[] = { /* World definition chunks */
  {CHUNK_RGBF,         0, read_RGBF},
  {CHUNK_RGBB,         0, read_RGBB},
  {CHUNK_AMOUNTOF,     0, read_AMOUNTOF},
  {CHUNK_PRJ,          1, read_NULL},
  {CHUNK_MLI,          1, read_NULL},
  {CHUNK_MAIN,         1, read_NULL},
  {CHUNK_OBJMESH,      1, read_NULL},
  {CHUNK_BKGCOLOR,     1, read_NULL},
  {CHUNK_AMBCOLOR,     1, read_NULL},
  {CHUNK_FOGPARAMS,    0, read_FOGPARAMS},

  {CHUNK_EN_FOG,       0, read_ENABLE},
  {CHUNK_EN_DISTCUE,   0, read_ENABLE},
  {CHUNK_EN_LAYERED,   0, read_ENABLE},
  {CHUNK_EN_BG_BMP,    0, read_ENABLE},
  {CHUNK_EN_BG_SOLID,  0, read_ENABLE},
  {CHUNK_EN_BG_GRAD,   0, read_ENABLE},

  {CHUNK_OBJBLOCK,     1, read_ASCIIZ},
  {CHUNK_TRIMESH,      1, read_TRIMESH},
  {CHUNK_VERTLIST,     0, read_VERTLIST},
  {CHUNK_VERTFLAGS,    0, read_NULL},
  {CHUNK_FACELIST,     1, read_FACELIST},
  {CHUNK_MESHCOLOR,    0, read_NULL},
  {CHUNK_FACEMAT,      0, read_FACEMAT},
  {CHUNK_MAPLIST,      0, read_MAPLIST},
  {CHUNK_TXTINFO,      0, read_TXTINFO},
  {CHUNK_SMOOTHLIST,   0, read_SMOOTHLIST},
  {CHUNK_TRMATRIX,     0, read_TRMATRIX},
  {CHUNK_LIGHT,        1, read_LIGHT},
  {CHUNK_SPOTLIGHT,    0, read_SPOTLIGHT},
  {CHUNK_CAMERA,       0, read_CAMERA},
  {CHUNK_HIERARCHY,    1, read_NULL},
  {CHUNK_VIEWPORT,     0, read_NULL},
  {CHUNK_MATERIAL,     1, read_MATERIAL},
  {CHUNK_MATNAME,      0, read_MATNAME},
  {CHUNK_AMBIENT,      1, read_NULL},
  {CHUNK_DIFFUSE,      1, read_NULL},
  {CHUNK_SPECULAR,     1, read_NULL},
  
  {CHUNK_TEXTURE,      1, read_NULL},
  {CHUNK_TEXTURE2,     1, read_NULL},
  {CHUNK_OPACITYMAP,   1, read_NULL},
  {CHUNK_BUMPMAP,      1, read_NULL},
  {CHUNK_SPECULARMAP,  1, read_NULL},
  {CHUNK_SHINENESSMAP, 1, read_NULL},
  {CHUNK_SELFILLMAP,   1, read_NULL},
  {CHUNK_REFLECTION,   1, read_NULL},

  {CHUNK_MTEXTURE,      1, read_NULL},
  {CHUNK_MTEXTURE2,     1, read_NULL},
  {CHUNK_MOPACITYMAP,   1, read_NULL},
  {CHUNK_MBUMPMAP,      1, read_NULL},
  {CHUNK_MSPECULARMAP,  1, read_NULL},
  {CHUNK_MSHINENESSMAP, 1, read_NULL},
  {CHUNK_MSELFILLMAP,   1, read_NULL},
  {CHUNK_MREFLECTION,   1, read_NULL},
 
  {CHUNK_MAPFILE,      0, read_MAPFILE},
  {CHUNK_MAPFLAGS,     0, read_MAPFLAGS},
  {CHUNK_MAPUSCALE,    0, read_MAPUSCALE},
  {CHUNK_MAPVSCALE,    0, read_MAPVSCALE},
  {CHUNK_MAPUOFFSET,   0, read_MAPUOFFSET},
  {CHUNK_MAPVOFFSET,   0, read_MAPVOFFSET},
  {CHUNK_MAPROTANGLE,  0, read_MAPROTANGLE},
  
  {CHUNK_SHININESS,    1, read_NULL},
  {CHUNK_SHINSTRENGTH, 1, read_NULL},
  {CHUNK_TRANSPARENCY, 1, read_NULL},
  {CHUNK_TRANSFALLOFF, 1, read_NULL},
  {CHUNK_REFBLUR,      1, read_NULL},
  {CHUNK_SELFILLUM,    1, read_NULL},
  {CHUNK_TWOSIDED,     0, read_MATTWOSIDED},
  {CHUNK_TRANSADD,     0, read_MATTRANSADD},
  {CHUNK_WIREON,       0, read_MATWIRE},
  {CHUNK_SOFTEN,       0, read_MATSOFTEN},
  {CHUNK_MATTYPE,      0, read_MATTYPE},
  {CHUNK_INNER_RANGE,  0, read_INNER_RANGE},
  {CHUNK_OUTER_RANGE,  0, read_OUTER_RANGE},
  {CHUNK_LIGHT_ATTENUATE,  0, read_LIGHT_ATTEN}

};

static c_LISTKEY key_chunks[] = { /* Keyframer chunks */
  {CHUNK_MAIN,         1, read_NULL},
  {CHUNK_KEYFRAMER,    1, read_NULL},
  {CHUNK_AMBIENTKEY,   1, read_NULL},
  {CHUNK_TRACKINFO,    1, read_NULL},
  {CHUNK_FRAMES,       0, read_FRAMES},
  {CHUNK_TRACKOBJNAME, 0, read_TRACKOBJNAME},
  {CHUNK_DUMMYNAME,    0, read_DUMMYNAME},
  {CHUNK_TRACKPIVOT,   0, read_TRACKPIVOT},
  {CHUNK_TRACKPOS,     0, read_TRACKPOS},
  {CHUNK_TRACKCOLOR,   0, read_TRACKCOLOR},
  {CHUNK_TRACKROTATE,  0, read_TRACKROT},
  {CHUNK_TRACKSCALE,   0, read_TRACKSCALE},
  {CHUNK_TRACKMORPH,   0, read_TRACKMORPH},
  {CHUNK_TRACKHIDE,    0, read_TRACKHIDE},
  {CHUNK_OBJNUMBER,    0, read_OBJNUMBER},
  {CHUNK_TRACKCAMERA,  1, read_NULL},
  {CHUNK_TRACKCAMTGT,  1, read_NULL},
  {CHUNK_TRACKLIGHT,   1, read_NULL},
  {CHUNK_TRACKLIGTGT,  1, read_NULL},
  {CHUNK_TRACKSPOTL,   1, read_NULL},
  {CHUNK_TRACKFOV,     0, read_TRACKFOV},
  {CHUNK_TRACKROLL,    0, read_TRACKROLL}
};

static int   c_chunk_last;       /* parent chunk      */
static int   c_chunk_prev;       /* previous chunk    */
static int   c_chunk_curr;       /* current chunk     */
static int   c_id;               /* current id        */
static char  c_string[64];       /* current name      */
static void *c_node;             /* current node      */

/*****************************************************************************
  internal functions
*****************************************************************************/

static void vec_swap (c_VECTOR *a)
{
/*
  swap_vert: swap y/z in vector.
*/
  float tmp;

#ifdef ast3d_SWAP_YZ
  tmp  = a->y;
  a->y = a->z;
  a->z = tmp;
#endif
}

#if 0
static void qt_swap (c_QUAT *a)
{
/*
  swap_vert: swap y/z in vector.
*/
  float tmp;

#ifdef ast3d_SWAP_YZ
  tmp  = a->y;
  a->y = a->z;
  a->z = tmp;
#endif
}
#endif

static void mat_swap (c_MATRIX a)
{
/*
  swap_mat: swap y/z in matrix.
*/
  int   i;
  float tmp;

#ifdef ast3d_SWAP_YZ
  for (i = 0; i < 3; i++) { /* swap columns */
    tmp = a[i][Y];
    a[i][Y] = a[i][Z];
    a[i][Z] = tmp;
  }
  for (i = 0; i < 4; i++) { /* swap rows */
    tmp = a[Y][i];
    a[Y][i] = a[Z][i];
    a[Z][i] = tmp;
  }
#endif
}

static char *strcopy (char *src)
{
/*
  strcopy: copies asciiz string with memory allocation.
           i know, i could use strdup(), but its not ANSI C.
*/
  char *temp;

  if ((temp = (char *)malloc (strlen (src)+1)) == NULL) return NULL;
  strcpy (temp, src);
  return temp;
}

static void clear_map (c_MAP *map)
{
/*
  clear_map: reset material map to default;
*/
  map->file = NULL;
  map->flags = 0;
  map->amountof = 1.0;
  map->U_scale = 1.0;
  map->V_scale = 1.0;
  map->U_offset = 0.0;
  map->V_offset = 0.0;
  map->rot_angle = 0.0;
}

static void clear_mat (c_MATERIAL *mat)
{
/*
  clear_mat: reset material to default.
*/
  mat->shading = 0;
  mat->flags = 0;
  clear_map (&mat->texture);
  clear_map (&mat->texture2);
  clear_map (&mat->bump);
  clear_map (&mat->reflection);
  clear_map (&mat->opacitymap);
  clear_map (&mat->specularmap);
  clear_map (&mat->shinenessmap);
  clear_map (&mat->selfillmap);

  clear_map (&mat->texture_mask);
  clear_map (&mat->texture2_mask);
  clear_map (&mat->bump_mask);
  clear_map (&mat->reflection_mask);
  clear_map (&mat->opacitymap_mask);
  clear_map (&mat->specularmap_mask);
  clear_map (&mat->shinenessmap_mask);
  clear_map (&mat->selfillmap_mask);

}

static t_TRACK *alloc_track ()
{
/*
  alloc_track: allocate memory for a track.
*/
  t_TRACK *track;

  track = (t_TRACK *)malloc (sizeof (t_TRACK));
  track->keys = NULL;
  track->last = NULL;
  track->flags = 0;
  track->frames = 0.0;
  track->total_frames = ast3d_scene->f_end;
  track->numkeys = 0;
  return track;
}

static int add_key (t_TRACK *track, t_KEY *key, int frame){
/*
  add_key: add a key to track.
*/
  if (track == NULL || key == NULL) return ast3d_err_nullptr;
  key->frame = frame;
  key->next = NULL;
  if (track->keys == NULL) {
    // first key
    key->loop_prev = key->prev = NULL;
    track->keys = key;
  } else {
    key->loop_prev = key->prev = track->last;
    track->last->loop_next = track->last->next = key;
  }
  
  if(track->flags&ast3d_track_loop){
    track->keys->loop_prev = key;  // az 1. key-nek az utolso az elozo...
    key->loop_next = track->keys;  // az utolsonak a kovetekezoje az elso
  } else key->loop_next=NULL;
  
  track->frames = key->frame;
  track->last = key;
  track->numkeys++;
  return ast3d_err_ok;
}

/*****************************************************************************
  chunk readers (world)
*****************************************************************************/

static int read_ENABLE (afs_FILE *f){
  if (f) {} /* to skip the warning */
  switch (c_chunk_curr) {
    case CHUNK_EN_FOG:      { ast3d_scene->fog.type|=ast3d_fog_fog; break;}
    case CHUNK_EN_DISTCUE:  { ast3d_scene->fog.type|=ast3d_fog_distcue; break;}
    case CHUNK_EN_LAYERED:  { ast3d_scene->fog.type|=ast3d_fog_layered; break;}
    case CHUNK_EN_BG_BMP:   { ast3d_scene->backgr.type|=ast3d_backgr_solidcolor; break;}
    case CHUNK_EN_BG_SOLID: { ast3d_scene->backgr.type|=ast3d_backgr_gradient; break;}
    case CHUNK_EN_BG_GRAD:  { ast3d_scene->backgr.type|=ast3d_backgr_bitmap; break;}
  }
  return ast3d_err_ok;
}

static int read_NULL (afs_FILE *f)
{
/*
  read_NULL: "dummy" chunk reader.
*/
  if (f) {} /* to skip the warning */
  return ast3d_err_ok;
}

static int read_RGBF (afs_FILE *f){
/*
  read_RGBF: RGB float reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_RGB      *rgb = NULL;
  float       c[3];

  switch (c_chunk_last) {
    case CHUNK_LIGHT:        rgb = &(((c_LIGHT *)c_node)->color); break;
    case CHUNK_AMBIENT:      rgb = &(mat->ambient); break;
    case CHUNK_DIFFUSE:      rgb = &(mat->diffuse); break;
    case CHUNK_SPECULAR:     rgb = &(mat->specular); break;
  }
  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  if (rgb) {
    rgb->rgb[0] = c[0];
    rgb->rgb[1] = c[1];
    rgb->rgb[2] = c[2];
//    printf("Load.color %f %f %f\n",c[0],c[1],c[2]);
  }
  return ast3d_err_ok;
}

static int read_RGBB (afs_FILE *f)
{
/*
  read_RGBB: RGB Byte reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_RGB      *rgb = NULL;
  byte        c[3];

  switch (c_chunk_last) {
    case CHUNK_LIGHT:        rgb = &(((c_LIGHT *)c_node)->color); break;
    case CHUNK_AMBIENT:      rgb = &(mat->ambient); break;
    case CHUNK_DIFFUSE:      rgb = &(mat->diffuse); break;
    case CHUNK_SPECULAR:     rgb = &(mat->specular); break;
  }
  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  if (rgb) {
    rgb->rgb[0] = (float)c[0] / 255.0;
    rgb->rgb[1] = (float)c[1] / 255.0;
    rgb->rgb[2] = (float)c[2] / 255.0;
  }
  return ast3d_err_ok;
}

static int read_AMOUNTOF (afs_FILE *f)
{
/*
  read_AMOUNTOF: "amount of" reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  float      *fl = NULL;
  word        w;

  if(map) fl=&map->amountof;
  switch (c_chunk_last) {
    case CHUNK_SHININESS:    fl = &(mat->shininess); break;
    case CHUNK_SHINSTRENGTH: fl = &(mat->shin_strength); break;
    case CHUNK_TRANSPARENCY: fl = &(mat->transparency); break;
    case CHUNK_TRANSFALLOFF: fl = &(mat->trans_falloff); break;
    case CHUNK_REFBLUR:      fl = &(mat->refblur); break;
    case CHUNK_SELFILLUM:    fl = &(mat->self_illum);
  }
  if (afs_fread (&w, sizeof (w), 1, f) != 1) return ast3d_err_badfile;
  if (fl) *fl = (float)w / 100.0;
  else printf("Found AMOUNTOF=%d for unknown chunk %04X\n",w,c_chunk_last);
  return ast3d_err_ok;
}

static int read_ASCIIZ (afs_FILE *f)
{
/*
  read_ASCIIZ: ASCIIZ string reader.
*/
  char *s = c_string;
  int   c;

  while ((c = afs_fgetc (f)) != EOF && c != '\0') *s++ = (char)c;
  if (c == EOF) return ast3d_err_badfile;
  *s = '\0';
  return ast3d_err_ok;
}

static int read_TRIMESH (afs_FILE *f)
{
/*
  read_TRIMESH: Triangular mesh reader.
*/
  c_OBJECT *obj;

  if (f) {} /* to skip the warning */
  if ((obj = (c_OBJECT *)malloc (sizeof (c_OBJECT))) == NULL)
    return ast3d_err_nomem;
  if ((obj->name = strcopy (c_string)) == NULL) return ast3d_err_nomem;
//  printf("Loading object '%s'\n",c_string);
  obj->id = c_id++;
  obj->parent = -1;
  obj->numverts=0;
  obj->numfaces=0;
  obj->vertices=NULL;
  obj->faces=NULL;
  obj->vert_visible=NULL;
  obj->face_visible=NULL;
  obj->smoothing=NULL;
  obj->flags = 0; //ast3d_obj_lmapmake;
  obj->bumpdepth=0.005;
  obj->enable_zbuffer=1;
  obj->vertexlights=0.0;
  obj->explode_speed=obj->explode_frame=0.0;
//  obj->additivetexture=0;
  obj->pmat = Default_MATERIAL;
  obj->lightmap_id=0;
  obj->hair_len=0.0;
  obj->receive_laser=0;

  if(strncmp(obj->name,"PARTICLE",8)==0) obj->flags|=ast3d_obj_particle;
  obj->particle.np=obj->particle.maxnp=0;
  vec_zero (&obj->pivot);
  vec_zero (&obj->translate);
  vec_zero (&obj->scale);
  qt_zero (&obj->rotate);
  mat_zero (obj->matrix);
  c_node = obj;
  ast3d_add_world (ast3d_obj_object, obj);
  return ast3d_err_ok;
}

static int read_VERTLIST (afs_FILE *f)
{
/*
  read_VERTLIST: Vertex list reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;
  c_VERTEX *v;
  float     c[3];
  word      nv;

  if (afs_fread (&nv, sizeof (nv), 1, f) != 1) return ast3d_err_badfile;
  if ((v = (c_VERTEX *)malloc (nv * sizeof (c_VERTEX))) == NULL)
    return ast3d_err_nomem;
  if(!(obj->vert_visible = (char *)malloc(nv))) return ast3d_err_nomem;
  obj->vertices = v;
  obj->numverts = nv;
//  printf("vertices=%d\n",nv);
  while (nv-- > 0) {
    if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
    vec_make (c[0], c[1], c[2], &v->vert);
    vec_swap (&v->vert);
    v->u = 0.0;
    v->v = 0.0;
//    v->visible = 0;
    v++;
  }
  return ast3d_err_ok;
}

static int read_SMOOTHLIST (afs_FILE *f){
/*
  read_SMOOTHLIST: Smoothing group list reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;
  
  obj->smoothing=malloc(obj->numfaces*sizeof(int));
  if(!obj->smoothing) return ast3d_err_nomem;
  if(afs_fread(obj->smoothing,obj->numfaces*sizeof(int),1,f) != 1) return ast3d_err_badfile;
  return ast3d_err_ok;
}

static int read_FACELIST (afs_FILE *f){
/*
  read_FACELIST: Face list reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;
  c_FACE   *fc;
  word      c[4];
  word      nv;

  if (afs_fread (&nv, sizeof (nv), 1, f) != 1) return ast3d_err_badfile;
  if ((fc = (c_FACE *)malloc (nv * sizeof (c_FACE))) == NULL) return ast3d_err_nomem;
  if(!(obj->face_visible = (char *)malloc(nv))) return ast3d_err_nomem;
  obj->faces = fc;
  obj->numfaces = nv;
//  printf("faces=%d\n",nv);
  while (nv-- > 0) {
    if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
    fc->a = c[0];
    fc->b = c[1];
    fc->c = c[2];
    fc->flags = c[3];
    fc->pa = &obj->vertices[c[0]];
    fc->pb = &obj->vertices[c[1]];
    fc->pc = &obj->vertices[c[2]];
//    fc->mat = 0;
//    fc->pmat = NULL;
//    fc->visible = 0;
    fc++;
  }
  return ast3d_err_ok;
}

static int read_FACEMAT (afs_FILE *f){
/*
  read_FACEMAT: Face material reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;
//  c_FACE     *fc = obj->faces;
  w_NODE     *node;
  c_MATERIAL *mat;
  word        n, nf;

  if (read_ASCIIZ (f)) return ast3d_err_badfile;
  if (afs_fread (&n, sizeof (n), 1, f) != 1) return ast3d_err_badfile;
  ast3d_byname (c_string, &node);
  if (!node) return ast3d_err_undefined;
  mat = (c_MATERIAL *)node->object;
//  if(obj->pmat) printf("WARNING: object has multiple materials!\n");
  obj->pmat=mat;
//  printf("Setting material %d to faces:",(int)mat);
  while (n-- > 0) {
    if (afs_fread (&nf, sizeof (nf), 1, f) != 1) return ast3d_err_badfile;
//    fc[nf].mat = mat->id;
//    fc[nf].pmat = mat;
//    printf(" %d",nf);
  }
//  printf("\n");
  return ast3d_err_ok;
}

static int read_MAPLIST (afs_FILE *f)
{
/*
  read_MAPLIST: Map list reader.
*/
  c_VERTEX *v = ((c_OBJECT *)c_node)->vertices;
  float     c[2];
  word      nv;

  if (afs_fread (&nv, sizeof (nv), 1, f) != 1) return ast3d_err_badfile;
//  printf("reading texture UVs for %d vertices.\n",nv);
  while (nv-- > 0) {
    if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
    v->u = c[0];
    v->v = c[1];
    v++;
  }
  return ast3d_err_ok;
}

static int read_TRMATRIX (afs_FILE *f){
/*
  read_TRMATRIX: Transformation matrix reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;
  c_VERTEX *v = obj->vertices;
  c_VECTOR  piv;
  c_MATRIX  mat;
  c_MATRIX  mat2;
  float     pivot[3];
  int       i, j;

  mat_identity (mat);
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      if (afs_fread (&mat[i][j], sizeof (float), 1, f) != 1)
        return ast3d_err_badfile;
  if (afs_fread (pivot, sizeof (pivot), 1, f) != 1) return ast3d_err_badfile;
  vec_make (pivot[0], pivot[1], pivot[2], &piv);
  vec_swap (&piv);
//  printf("TRMATRIX.PIVOT: %f %f %f\n",piv.x,piv.y,piv.z);
  mat_swap (mat);
  mat_invscale (mat, mat2);
//  mat_inverse_v02(mat, mat2);
#if 1
  for (i = 0; i < obj->numverts; i++) {
    vec_sub (&v->vert, &piv, &v->vert);
    mat_mulvec (mat2, &v->vert, &v->vert);
    v++;
  }
#endif
  return ast3d_err_ok;
}

static int read_FOGPARAMS (afs_FILE *f)
{
/*
  read_FOGPARAMS: Fog reader.
*/
  float   c[4];
  float t;
  c_FOG *fog=&ast3d_scene->fog;
  if (afs_fread (c, 4*4, 1, f) != 1) return ast3d_err_badfile;
  fog->znear=c[0];
  fog->fognear=c[1];  
  fog->zfar=c[2];  
  fog->fogfar=c[3];
//  
  c[1]/=100.0;
  c[3]/=100.0;
  t=(c[2]-c[0])/(c[3]-c[1]);
  fog->fog_zfar=(1-c[1])*t+c[0];
  fog->fog_znear=c[0]-(c[1]*t+c[0]);

//  printf("fog Znear/Zfar: %f  %f\n",fog->znear,fog->zfar);
//  printf("fog Dnear/Dfar: %f  %f\n",fog->fognear,fog->fogfar);  
//  printf("fog znear/far:  %f  %f\n",fog->fog_znear,fog->fog_zfar);  
  
//  fog->type|=ast3d_fog_fog;
  if (afs_fread (c, 3*2, 1, f) != 1) return ast3d_err_badfile;
  if (afs_fread (&fog->color, 3*4, 1, f) != 1) return ast3d_err_badfile;
  if (afs_fread (c, 3*2, 1, f) != 1) return ast3d_err_badfile;
  return ast3d_err_ok;
}

static int read_LIGHT (afs_FILE *f)
{
/*
  read_LIGHT: Light reader.
*/
  float   c[3];
  c_LIGHT *light;

  if ((light = (c_LIGHT *)malloc (sizeof (c_LIGHT))) == NULL)
    return ast3d_err_nomem;
  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  if ((light->name = strcopy (c_string)) == NULL) return ast3d_err_nomem;
  light->id = c_id++;
  light->flags = ast3d_light_omni;
  light->enabled=1;
  light->use_zbuffer=1;
  light->corona_scale=1.0;
  light->attenuation[0]=1.0;
  light->attenuation[1]=0.0;
  light->attenuation[2]=0.0;
  light->lightmap_calc_normal=1;
  light->laser=0;
  
  vec_make (c[0], c[1], c[2], &light->pos);
  vec_swap (&light->pos);
  c_node = light;
  ast3d_add_world (ast3d_obj_light, light);
  return ast3d_err_ok;
}

static int read_SPOTLIGHT (afs_FILE *f)
{
/*
  read_SPOTLIGHT: Spot light reader.
*/
  float   c[5];
  c_LIGHT *light = (c_LIGHT *)c_node;

  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  light->target.x = c[0];
  light->target.y = c[1];
  light->target.z = c[2];
  light->hotspot = c[3];
  light->falloff = c[4];
  light->flags = ast3d_light_spot;
  light->roll = 0.0;
  light->attenuate=0;
  vec_swap (&light->target);
  return ast3d_err_ok;
}

static int read_INNER_RANGE(afs_FILE *f){
  float   c;
  c_LIGHT *light = (c_LIGHT *)c_node;
  if (afs_fread (&c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  light->inner_range=c;
//  printf("Light inner range: %f\n",c);
  return ast3d_err_ok;
}

static int read_OUTER_RANGE(afs_FILE *f){
  float   c;
  c_LIGHT *light = (c_LIGHT *)c_node;
  if (afs_fread (&c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  light->outer_range=c;
//  printf("Light outer range: %f\n",c);
  return ast3d_err_ok;
}

static int read_LIGHT_ATTEN(afs_FILE *f){
  c_LIGHT *light = (c_LIGHT *)c_node;
  light->attenuate=1;
//  printf("Light attenuation\n");

  // 1/(a0+d*a1+d*d*a2)
//  light->attenuation[0]=light->inner_range;
//  light->attenuation[1]=light->outer_range;
//  light->attenuation[2]=0.0;

  return ast3d_err_ok;
}

static int read_CAMERA (afs_FILE *f)
{
/*
  read_CAMERA: Camera reader.
*/
  float    c[8];
  c_CAMERA *cam;

  if(!(cam = (c_CAMERA *)malloc (sizeof (c_CAMERA)))) return ast3d_err_nomem;
  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  if ((cam->name = strcopy (c_string)) == NULL) return ast3d_err_nomem;
  cam->id = c_id++;
  cam->roll = c[6];
  cam->fov = cam_lens_fov (c[7]);
  cam->aspectratio=1.0;
  vec_make (c[0], c[1], c[2], &cam->pos);
  vec_make (c[3], c[4], c[5], &cam->target);
  vec_swap (&cam->pos);
  vec_swap (&cam->target);
  c_node = cam;
  ast3d_add_world (ast3d_obj_camera, cam);
  return ast3d_err_ok;
}

static int read_TXTINFO (afs_FILE *f){
  c_OBJECT *obj = (c_OBJECT *)c_node;
  word mt;
  float    c[2];
  int i,j;
  
//  printf("Reading MAPPING INFO for obj '%s'\n",obj->name);
  if (afs_fread (&mt, sizeof (mt), 1, f) != 1) return ast3d_err_badfile;
  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  obj->mapping.type=mt;
  obj->mapping.utile=c[0];
  obj->mapping.vtile=c[1];
  
  mat_identity(obj->mapping.matrix);
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      if (afs_fread (&obj->mapping.matrix[i][j], sizeof (float), 1, f) != 1)
        return ast3d_err_badfile;

  for (i = 0; i < 3; i++)
      if (afs_fread (&obj->mapping.matrix[i][3], sizeof (float), 1, f) != 1)
        return ast3d_err_badfile;

//  if (afs_fread (pivot, sizeof (pivot), 1, f) != 1) return ast3d_err_badfile;
//  vec_make (pivot[0], pivot[1], pivot[2], &piv);

/*  
  obj->mapping.pos.x=c[2];
  obj->mapping.pos.y=c[3];
  obj->mapping.pos.z=c[4];
  if (afs_fread (obj->mapping.matrix, sizeof (c_MATRIX), 1, f) != 1) return ast3d_err_badfile;
*/

#if 0
  printf("tile: %1.1f x %1.1f,  pos: %3.3f ; %3.3f ; %3.3f\n",c[0],c[1],c[2],c[3],c[4]);
  { int i,j;
    for(i=0;i<3;i++){
      printf("M%d:  ",i);
      for(j=0;j<4;j++) printf("%8.3f",obj->mapping.matrix[i][j]);
      printf("\n");
    }  
  }
#endif
  return ast3d_err_ok;
}

static int read_MATERIAL (afs_FILE *f)
{
/*  read_MATERIAL: Material reader.*/
  c_MATERIAL *mat;

  if (f) {} /* to skip the warning */
  if ((mat = (c_MATERIAL *)malloc (sizeof (c_MATERIAL))) == NULL)
    return ast3d_err_nomem;
  clear_mat (mat);
  mat->id = c_id++;
  c_node = mat;
  ast3d_add_world (ast3d_obj_material, mat);
  return ast3d_err_ok;
}

c_MATERIAL* create_Default_MATERIAL (){
  c_MATERIAL *mat;
  if ((mat = (c_MATERIAL *)malloc (sizeof (c_MATERIAL))) == NULL) return NULL;
  clear_mat (mat);
  mat->id = c_id++;
  mat->name="Default";
  mat->shading=ast3d_mat_metal;
  mat->flags=0;
  mat->ambient.rgb[0]=  mat->ambient.rgb[1]=  mat->ambient.rgb[2]= 1.0F;
  mat->diffuse.rgb[0]=  mat->diffuse.rgb[1]=  mat->diffuse.rgb[2]= 1.0F;
//  mat->diffuse.rgb[1]=0;
  mat->shininess=0.9;
  mat->shin_strength=0.15;
  c_node = mat;
  ast3d_add_world (ast3d_obj_material, mat);
  return mat;
}

static int read_MATNAME (afs_FILE *f)
{
/*
  read_MATNAME: Material name reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;

  if (read_ASCIIZ (f)) return ast3d_err_badfile;
  if ((mat->name = strcopy (c_string)) == NULL) return ast3d_err_nomem;
  return ast3d_err_ok;
}

static int read_MATTYPE (afs_FILE *f)
{
/*
  read_MATTYPE: Material type reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  word        type;

  if (afs_fread (&type, sizeof (type), 1, f) != 1) return ast3d_err_badfile;
  mat->shading = type;
  return ast3d_err_ok;
}

static int read_MATTWOSIDED (afs_FILE *f)
{
/*
  read_MATTWOSIDED: Material two sided reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;

  if (f) {} /* to skip the warning */
  mat->flags |= ast3d_mat_twosided;
  return ast3d_err_ok;
}

static int read_MATSOFTEN (afs_FILE *f)
{
/*
  read_MATSOFTEN: Material soften reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;

  if (f) {} /* to skip the warning */
  mat->flags |= ast3d_mat_soften;
  return ast3d_err_ok;
}

static int read_MATWIRE (afs_FILE *f)
{
/*
  read_MATWIRE: Material wireframe reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;

  if (f) {} /* to skip the warning */
  mat->flags |= ast3d_mat_wire;
  return ast3d_err_ok;
}

static int read_MATTRANSADD (afs_FILE *f)
{
/*
  read_MATTRANSADD: Material transparency add reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;

  if (f) {} /* to skip the warning */
  mat->flags |= ast3d_mat_transadd;
  return ast3d_err_ok;
}

static c_MAP* get_map_pointer(c_MATERIAL *mat){
  c_MAP      *map = NULL;
  switch (c_chunk_last) {
    case CHUNK_TEXTURE: map = &(mat->texture); break;
    case CHUNK_TEXTURE2: map = &(mat->texture2); break;
    case CHUNK_BUMPMAP: map = &(mat->bump); break;
    case CHUNK_OPACITYMAP: map = &(mat->opacitymap); break;
    case CHUNK_SPECULARMAP: map = &(mat->specularmap); break;
    case CHUNK_SHINENESSMAP: map = &(mat->shinenessmap); break;
    case CHUNK_SELFILLMAP: map = &(mat->selfillmap); break;
    case CHUNK_REFLECTION: map = &(mat->reflection); break;

    case CHUNK_MTEXTURE: map = &(mat->texture_mask); break;
    case CHUNK_MTEXTURE2: map = &(mat->texture2_mask); break;
    case CHUNK_MBUMPMAP: map = &(mat->bump_mask); break;
    case CHUNK_MOPACITYMAP: map = &(mat->opacitymap_mask); break;
    case CHUNK_MSPECULARMAP: map = &(mat->specularmap_mask); break;
    case CHUNK_MSHINENESSMAP: map = &(mat->shinenessmap_mask); break;
    case CHUNK_MSELFILLMAP: map = &(mat->selfillmap_mask); break;
    case CHUNK_MREFLECTION: map = &(mat->reflection_mask);
  }
//  if(!map) printf("Found BITMAP for unknown chunk %04X\n",c_chunk_last);
  return map;
}

static int read_MAPFILE (afs_FILE *f)
{
/*
  read_MAPFILE: MAP file reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);

  if (read_ASCIIZ (f)) return ast3d_err_badfile;
//  printf("Found mapfile '%s' for chunk %04X\n",c_string,c_chunk_last);
  if (map){
    if ((map->file = strcopy (c_string)) == NULL) return ast3d_err_nomem;
  } else printf("Found MAPFILE=%s for unknown chunk %04X\n",c_string,c_chunk_last);

  return ast3d_err_ok;
}

static int read_MAPFLAGS (afs_FILE *f)
{
/*
  read_MAPFLAGS: MAP flags reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  word        flags;

  if (afs_fread (&flags, sizeof (flags), 1, f) != 1) return ast3d_err_badfile;
  if (map) map->flags = flags;
  else printf("Found MAPFLAGS=%04X for unknown chunk %04X\n",flags,c_chunk_last);
  return ast3d_err_ok;
}

static int read_MAPUSCALE (afs_FILE *f)
{
/*
  read_MAPUSCALE: MAP U scale reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  float       U;

  if (afs_fread (&U, sizeof (U), 1, f) != 1) return ast3d_err_badfile;
  if (map) map->U_scale = U;
  return ast3d_err_ok;
}

static int read_MAPVSCALE (afs_FILE *f)
{
/*
  read_MAPUSCALE: MAP U scale reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  float       V;

  if (afs_fread (&V, sizeof (V), 1, f) != 1) return ast3d_err_badfile;
  if (map) map->V_scale = V;
  return ast3d_err_ok;
}

static int read_MAPUOFFSET (afs_FILE *f)
{
/*
  read_MAPUSCALE: MAP U offset reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  float       U;

  if (afs_fread (&U, sizeof (U), 1, f) != 1) return ast3d_err_badfile;
  if (map) map->U_offset = U;
  return ast3d_err_ok;
}

static int read_MAPVOFFSET (afs_FILE *f)
{
/*
  read_MAPUSCALE: MAP V offset reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  float       V;

  if (afs_fread (&V, sizeof (V), 1, f) != 1) return ast3d_err_badfile;
  if (map) map->V_offset = V;
  return ast3d_err_ok;
}

static int read_MAPROTANGLE (afs_FILE *f)
{
/*
  read_MAPUSCALE: MAP rotation angle reader.
*/
  c_MATERIAL *mat = (c_MATERIAL *)c_node;
  c_MAP      *map = get_map_pointer(mat);
  float       angle;

  if (afs_fread (&angle, sizeof (angle), 1, f) != 1) return ast3d_err_badfile;
  if (map) map->rot_angle = angle;
  return ast3d_err_ok;
}


/*****************************************************************************
  chunk readers (keyframer)
*****************************************************************************/

static int read_FRAMES (afs_FILE *f)
{
/*
  read_FRAMES: Frames reader.
*/
  dword c[2];

  if (afs_fread (c, sizeof (c), 1, f) != 1) return ast3d_err_badfile;
  ast3d_scene->f_start = c[0];
  ast3d_scene->f_end = c[1];
  return ast3d_err_ok;
}

static int read_OBJNUMBER (afs_FILE *f)
{
/*
  read_OBJNUMBER: Object number reader. (3DS 4.0+)
*/
  word n;

  if (afs_fread (&n, sizeof (n), 1, f) != 1) return ast3d_err_badfile;
  c_id = n;
  return ast3d_err_ok;
}

static int read_DUMMYNAME (afs_FILE *f)
{
/*
  read_DUMMYNAME: Dummy object name reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;

  if (read_ASCIIZ (f)) return ast3d_err_badfile;
  if ((obj->name = strcopy (c_string)) == NULL) return ast3d_err_nomem;
  return ast3d_err_ok;
}

static int read_TRACKOBJNAME (afs_FILE *f)
{
/*
  read_TRACKOBJNAME: Track object name reader.
*/
  w_NODE    *node;
  k_NODE    *pnode;
  c_OBJECT  *obj = NULL; /* to skip the warning */
  c_LIGHT   *light = NULL;
  c_CAMERA  *cam = NULL;
  c_AMBIENT *amb = NULL;
  void      *track;
  word       flags[2];
  sword      parent;
  int        wparent = -1;

  /* for 3DS 3.0 compatibility */
  if (c_chunk_prev != CHUNK_OBJNUMBER) c_id++;

  if (read_ASCIIZ (f)) return ast3d_err_badfile;
  if (strcmp (c_string, "$AMBIENT$") == 0) {
    if ((amb = (c_AMBIENT *)malloc (sizeof (c_AMBIENT))) == NULL)
      return ast3d_err_nomem;
    if ((amb->name = strcopy (c_string)) == NULL) return ast3d_err_nomem;
    amb->id = 1024+c_id;
    vec_zero ((c_VECTOR *)&amb->color);
    ast3d_add_world (ast3d_obj_ambient, amb);
  } else if (strcmp (c_string, "$$$DUMMY") == 0) {
    if ((obj = (c_OBJECT *)malloc (sizeof (c_OBJECT))) == NULL)
      return ast3d_err_nomem;
    obj->id = 1024+c_id;
    obj->flags = ast3d_obj_dummy;
    obj->numverts = 0;
    obj->numfaces = 0;
    obj->vertices = NULL;
    obj->faces = NULL;
    vec_zero (&obj->translate);
    vec_zero (&obj->scale);
    qt_zero (&obj->rotate);
    ast3d_add_world (ast3d_obj_object, obj);
  } else {
    ast3d_byname (c_string, &node);
    if (!node) return ast3d_err_undefined;
    obj = (c_OBJECT *)node->object;
    cam = (c_CAMERA *)node->object;
    light = (c_LIGHT *)node->object;
  }
  if (afs_fread (flags, sizeof (flags), 1, f) != 1) return ast3d_err_badfile;
  if (afs_fread (&parent, sizeof (parent), 1, f) != 1) return ast3d_err_badfile;
  if (parent != -1) {
    for (pnode = ast3d_scene->keyframer; pnode; pnode = pnode->next)
      if (pnode->id == parent)
        wparent = ((c_OBJECT *)pnode->object)->id;
  }
  if (c_chunk_last == CHUNK_TRACKINFO) {
    obj->parent = wparent;
    if (flags[0] & 0x800) obj->flags |= ast3d_obj_chidden;
    if ((track = malloc (sizeof (t_OBJECT))) == NULL) return ast3d_err_nomem;
    memset (track, 0, sizeof (t_OBJECT));
    ast3d_add_track (ast3d_track_object, c_id, parent, track, obj);
    c_node = obj;
  }
  if (c_chunk_last == CHUNK_TRACKCAMERA) {
    cam->parent1 = wparent;
    if ((track = malloc (sizeof (t_CAMERA))) == NULL) return ast3d_err_nomem;
    memset (track, 0, sizeof (t_CAMERA));
    ast3d_add_track (ast3d_track_camera, c_id, parent, track, cam);
  }
  if (c_chunk_last == CHUNK_TRACKCAMTGT) {
    cam->parent2 = wparent;
    if ((track = malloc (sizeof (t_CAMERATGT))) == NULL)
      return ast3d_err_nomem;
    memset (track, 0, sizeof (t_CAMERATGT));
    ast3d_add_track (ast3d_track_cameratgt, c_id, parent, track, cam);
  }
  if (c_chunk_last == CHUNK_TRACKLIGHT) {
    light->parent1 = wparent;
    if ((track = malloc (sizeof (t_LIGHT))) == NULL) return ast3d_err_nomem;
    memset (track, 0, sizeof (t_LIGHT));
    ast3d_add_track (ast3d_track_light, c_id, parent, track, light);
  }
  if (c_chunk_last == CHUNK_TRACKSPOTL) {
    light->parent1 = wparent;
    if ((track = malloc (sizeof (t_SPOTLIGHT))) == NULL)
      return ast3d_err_nomem;
    memset (track, 0, sizeof (t_SPOTLIGHT));
    ast3d_add_track (ast3d_track_spotlight, c_id, parent, track, light);
  }
  if (c_chunk_last == CHUNK_TRACKLIGTGT) {
    light->parent2 = wparent;
    if ((track = malloc (sizeof (t_LIGHTTGT))) == NULL)
      return ast3d_err_nomem;
    memset (track, 0, sizeof (t_LIGHTTGT));
    ast3d_add_track (ast3d_track_lighttgt, c_id, parent, track, light);
  }
  if (c_chunk_last == CHUNK_AMBIENTKEY) {
    if ((track = malloc (sizeof (t_AMBIENT))) == NULL) return ast3d_err_nomem;
    memset (track, 0, sizeof (t_AMBIENT));
    ast3d_add_track (ast3d_track_ambient, c_id, parent, track, amb);
  }
  return ast3d_err_ok;
}

static int read_TRACKPIVOT (afs_FILE *f)
{
/*
  read_TRACKPIVOT: Track pivot point reader.
*/
  c_OBJECT *obj = (c_OBJECT *)c_node;
  float     pos[3];
  int       i;

  if (afs_fread (pos, sizeof (pos), 1, f) != 1) return ast3d_err_badfile;
  vec_make (pos[0], pos[1], pos[2], &obj->pivot);
  vec_swap (&obj->pivot);
//  printf("TRACKPIVOT: %f %f %f\n",obj->pivot.x,obj->pivot.y,obj->pivot.z);
  for (i = 0; i < obj->numverts; i++) vec_sub (&obj->vertices[i].vert, &obj->pivot, &obj->vertices[i].vert);
  return ast3d_err_ok;
}

static int read_KFLAGS (afs_FILE *f, word *nf, t_KEY *key)
{
/*
  read_KFLAGS: Key flags/spline reader.
*/
  word  unknown, flags;
  int   i;
  float dat;

  key->tens = 0.0;
  key->cont = 0.0;
  key->bias = 0.0;
  key->easeto = 0.0;
  key->easefrom = 0.0;
  if (afs_fread (nf, sizeof (word), 1, f) != 1) return ast3d_err_badfile;
  if (afs_fread (&unknown, sizeof (word), 1, f) != 1) return ast3d_err_badfile;
  if (afs_fread (&flags, sizeof (flags), 1, f) != 1) return ast3d_err_badfile;
  for (i = 0; i < 16; i++) {
    if (flags & (1 << i)) {
      if (afs_fread (&dat, sizeof (dat), 1, f) != 1) return ast3d_err_badfile;
      switch (i) {
        case 0: key->tens = dat; break;
        case 1: key->cont = dat; break;
        case 2: key->bias = dat; break;
        case 3: key->easeto = dat; break;
        case 4: key->easefrom = dat;
      }
    }
  }
  return ast3d_err_ok;
}

static int read_TFLAGS (afs_FILE *f, t_TRACK *track, word *n)
{
/*
  read_TFLAGS: Track flags reader.
*/
  word flags[7];

  if (afs_fread (flags, sizeof (flags), 1, f) != 1) return ast3d_err_badfile;
  if ((flags[0] & 0x03) == 0x02) track->flags = ast3d_track_repeat; else
  if ((flags[0] & 0x03) == 0x03) track->flags = ast3d_track_loop;
  *n = flags[5];
  return ast3d_err_ok;
}

static int read_TRACKPOS (afs_FILE *f)
{
/*
  read_TRACKPOS: Track position reader.
*/
  t_TRACK *track;
  t_KEY  *key;
  float  pos[3];
  word   n, nf;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (afs_fread (pos, sizeof (pos), 1, f) != 1) return ast3d_err_badfile;
    vec_make (pos[0], pos[1], pos[2], &key->val._vect);
    vec_swap (&key->val._vect);
    add_key (track, key, nf);
  }
  spline_init (track);
  ast3d_set_track (ast3d_key_pos, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKCOLOR (afs_FILE *f)
{
/*
  read_TRACKCOLOR: Track color reader.
*/
  t_TRACK *track;
  t_KEY *key;
  float  pos[3];
  word   n, nf;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (afs_fread (pos, sizeof (pos), 1, f) != 1) return ast3d_err_badfile;
    vec_make (pos[0], pos[1], pos[2], &key->val._vect);
    vec_swap (&key->val._vect);
    add_key (track, key, nf);
  }
  spline_init (track);
  ast3d_set_track (ast3d_key_color, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKROT (afs_FILE *f)
{
/*
  read_TRACKROT: Track rotation reader.
*/
  t_TRACK *track;
  t_KEY   *key;
  c_QUAT   q, old;
  float    pos[4];
  word     keys,n, nf;
  float angle = 0;

  track = alloc_track();
  qt_identity (&old); qt_identity (&q);
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  keys = n;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (afs_fread (pos, sizeof(pos), 1, f) != 1) return ast3d_err_badfile;
    angle=pos[0]; // !!!!!!!!!
    { float len=pos[1]*pos[1]+pos[2]*pos[2]+pos[3]*pos[3];
      if(len>0) len=1.0F/sqrt(len);      // normalize axis vector
#ifdef ast3d_SWAP_YZ
      qt_make (angle, len*pos[1], len*pos[3], len*pos[2], &key->val._quat);
#else
      qt_make (angle, len*pos[1], len*pos[2], len*pos[3], &key->val._quat);
#endif
    }
    add_key (track, key, nf);
  }
  spline_initrot (track); // egyeb parameterek (pl. key->qa) kiszamitasa
  ast3d_set_track (ast3d_key_rotate, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKSCALE (afs_FILE *f)
{
/*
  read_TRACKSCALE: Track scale reader.
*/
  t_TRACK *track;
  t_KEY *key;
  float  pos[3];
  word   n, nf;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (afs_fread (pos, sizeof (pos), 1, f) != 1) return ast3d_err_badfile;
    vec_make (pos[0], pos[1], pos[2], &key->val._vect);
    vec_swap (&key->val._vect);
    add_key (track, key, nf);
  }
  spline_init (track);
  ast3d_set_track (ast3d_key_scale, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKFOV (afs_FILE *f)
{
/*
  read_TRACKFOV: Track FOV reader.
*/
  t_TRACK *track;
  t_KEY *key;
  word  n, nf;
  float fov;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (afs_fread (&fov, sizeof (fov), 1, f) != 1) return ast3d_err_badfile;
    key->val._float = fov;
    add_key (track, key, nf);
  }
  spline_init (track);
  ast3d_set_track (ast3d_key_fov, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKROLL (afs_FILE *f)
{
/*
  read_TRACKROLL: Track ROLL reader.
*/
  t_TRACK *track;
  t_KEY *key;
  word   n, nf;
  float  roll;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (afs_fread(&roll, sizeof(roll), 1, f) != 1) return ast3d_err_badfile;
    key->val._float = roll;
    add_key (track, key, nf);
  }
  spline_init (track);
  ast3d_set_track (ast3d_key_roll, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKMORPH (afs_FILE *f)
{
/*
  read_TRACKMORPH: Track morph reader.
*/
  t_TRACK *track;
  t_KEY  *key;
  w_NODE *node;
  word    n, nf;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (read_KFLAGS (f, &nf, key)) return ast3d_err_badfile;
    if (read_ASCIIZ (f)) return ast3d_err_badfile;
    ast3d_byname (c_string, &node);
    if (!node) return ast3d_err_undefined;
    key->val._obj = (c_OBJECT *)node->object;
    add_key (track, key, nf);
  }
  spline_init (track); /*!! NEW !!*/
  ast3d_set_track (ast3d_key_morph, c_id, track);
  return ast3d_err_ok;
}

static int read_TRACKHIDE (afs_FILE *f)
{
/*
  read_TRACKHIDE: Track hide reader.
*/
  t_TRACK *track;
  t_KEY *key;
  word   unknown[2];
  word   n, nf;
  int    hide = 0;

  track = alloc_track();
  if (read_TFLAGS (f, track, &n) != 0) return ast3d_err_badfile;
  while (n-- > 0) {
    if ((key = (t_KEY *)malloc (sizeof (t_KEY))) == NULL)
      return ast3d_err_nomem;
    if (afs_fread (&nf, sizeof (nf), 1, f) != 1) return ast3d_err_badfile;
    if (afs_fread (unknown, sizeof (word), 2, f) != 2) return ast3d_err_badfile;
    key->val._int = (hide ^= 1);
    add_key (track, key, nf);
  }
  ast3d_set_track (ast3d_key_hide, c_id, track);
  return ast3d_err_ok;
}

static int read_CHUNK (afs_FILE *f, c_CHUNK *h)
{
/*
  read_CHUNK: Chunk reader.
*/
  if (afs_fread (&h->chunk_id, sizeof (word), 1, f) != 1)
    return ast3d_err_badfile;
  if (afs_fread (&h->chunk_size, sizeof (dword), 1, f) != 1)
    return ast3d_err_badfile;
  return ast3d_err_ok;
}

/*****************************************************************************
  chunk readers control
*****************************************************************************/
static int chunk_level=0;

static int ChunkReaderWorld (afs_FILE *f, long p, word parent){
/*
  ChunkReaderWorld: Recursive chunk reader (world).
*/
  c_CHUNK h;
  long    pc;
  int     n, i, error;

  c_chunk_last = parent;
  pc = afs_ftell (f);
  while (pc < p) {
    if (read_CHUNK (f, &h) != 0) return ast3d_err_badfile;
#if 0
    if(h.chunk_id==0x4150 && h.chunk_size>4000){
      char *mem=malloc(h.chunk_size);
      FILE *f2=fopen("chunk4150.dat","wb");
      afs_fread(mem,1,h.chunk_size,f);
      fwrite(mem,1,h.chunk_size,f2);
      fclose(f2);
      free(mem);
      afs_fseek(f,pc+6,SEEK_SET);
    }
#endif    
    c_chunk_curr=h.chunk_id;
    pc+=h.chunk_size;
#ifdef SHOW_CHUNK_TREE
#ifdef SHOW_CHUNK_NAMES
    printf("%*s%04X (%ld)  %s\n",chunk_level*2,"",h.chunk_id,h.chunk_size,get_chunk_name(h.chunk_id));
#else
    printf("%*s%04X (%ld)\n",chunk_level*2,"",h.chunk_id,h.chunk_size);
#endif
#endif
    n=-1;
    for (i = 0; i < sizeof (world_chunks) / sizeof (world_chunks[0]); i++)
      if (h.chunk_id == world_chunks[i].id){ n = i; break; }
    if (n < 0) {
#ifdef SHOW_UNKNOWN_CHUNKS
      int i;
      for (i=0; i < sizeof (key_chunks) / sizeof (key_chunks[0]); i++)
        if (h.chunk_id == key_chunks[i].id) goto key_chunk;
      printf("Unknown chunk: %04X  parent=%04X  size=%ld\n",h.chunk_id,parent,h.chunk_size-6);
      key_chunk:
#endif
    } else {
      if ((error = world_chunks[n].func (f)) != 0) return error;
      if (world_chunks[n].sub){
        ++chunk_level;
        if ((error = ChunkReaderWorld (f, pc, h.chunk_id)) != 0) return error;
        --chunk_level;
      }
      c_chunk_prev = h.chunk_id;
    }
    afs_fseek (f, pc, SEEK_SET);
  }
  return ast3d_err_ok;
}

static int ChunkReaderKey (afs_FILE *f, long p, word parent)
{
/*
  ChunkReaderKey: Recursive chunk reader (keyframer).
*/
  c_CHUNK h;
  long    pc;
  int     n, i, error;
  
//  putchar('.');fflush(stdout);

  c_chunk_last = parent;
  while ((pc = afs_ftell (f)) < p) {
    if (read_CHUNK (f, &h) != 0) return ast3d_err_badfile;
    c_chunk_curr = h.chunk_id;
    n = -1;
    for (i = 0; i < sizeof (key_chunks) / sizeof (key_chunks[0]); i++)
      if (h.chunk_id == key_chunks[i].id) {
        n = i;
        break;
      }
    if (n < 0) afs_fseek (f, pc + h.chunk_size, SEEK_SET);
    else {
      pc = pc + h.chunk_size;
      if ((error = key_chunks[n].func (f)) != 0) return error;
      if (key_chunks[n].sub)
        if ((error = ChunkReaderKey (f, pc, h.chunk_id)) != 0) return error;
      afs_fseek (f, pc, SEEK_SET);
      c_chunk_prev = h.chunk_id;
    }
//    if (ferror (f)) return ast3d_err_badfile;
  }
  return ast3d_err_ok;
}

/*****************************************************************************
  world/motion load routines
*****************************************************************************/

int32 ast3d_load_mesh_3DS (afs_FILE *f)
{
/*
  ast3d_load_mesh_3DS: loads mesh data from 3ds file "filename"
                      into scene "scene".
*/
  byte version;
  long length;

  c_id = 0;
  afs_fseek (f, 0, SEEK_END);
  length = afs_ftell (f);
  afs_fseek (f, 28L, SEEK_SET);
  if (afs_fread (&version, sizeof (byte), 1, f) != 1) return ast3d_err_badfile;
  if (version < 2) return ast3d_err_badver; /* 3DS 3.0+ supported */
  afs_fseek (f, 0, SEEK_SET);
  return ChunkReaderWorld (f, length, 0);
}

int32 ast3d_load_motion_3DS (afs_FILE *f)
{
/*
  ast3d_loadmotion: loads motion data from 3ds file "filename"
                   into scene "scene".
*/
  byte version;
  long length;

  c_id = -1;
  afs_fseek (f, 0, SEEK_END);
  length = afs_ftell (f);
  afs_fseek (f, 28L, SEEK_SET);
  if (afs_fread (&version, sizeof (byte), 1, f) != 1) return ast3d_err_badfile;
  if (version < 2) return ast3d_err_badver; /* 3DS 3.0+ supported */
  afs_fseek (f, 0, SEEK_SET);
  return ChunkReaderKey (f, length, 0);
}

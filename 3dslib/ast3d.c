#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "ast3d.h"
#include "ast3di.h"

#include "vector.h"
#include "matrix.h"

/*****************************************************************************
  copyright, format table, etc...
*****************************************************************************/

  char    ast3d_version[]   = "Astral 3d Engine v0.1a";
  char    ast3d_copyright[] = "copyright (c) 1998 RoBSoN of Astral";

  c_SCENE  *ast3d_scene;  /* current active scene  */
  c_CAMERA *ast3d_camera; /* current active camera */
  int32     ast3d_flags;  /* curreng flags         */

struct {
  char   *name;                   /* file extension        */
  int32 (*load_mesh)   (afs_FILE *f); /* loads mesh            */
  int32 (*load_motion) (afs_FILE *f); /* loads motion          */
  int32 (*save_scene)  (afs_FILE *f); /* saves the whole scene */
} ast3d_drivers[] = {
  {"3DS", ast3d_load_mesh_3DS, ast3d_load_motion_3DS, NULL},
//  {"AST", ast3d_load_mesh_AST, ast3d_load_motion_AST, ast3d_save_AST}
  {"AST", NULL, NULL, NULL}
};

/*****************************************************************************
  internal functions
*****************************************************************************/

#include "fix_uv.c"

#ifdef TRIANGLE_STRIP
#include "sortface.c"
#endif

#ifdef OTIMIZE_VERTEX
#include "optimvert.c"
#endif

static void calc_objnormals (c_OBJECT *obj)
{
  c_VECTOR  normal;
  int32     i, j;

  /* PASS-1  vertexnormals=0 */
  for (i = 0; i < obj->numverts; i++) { 
    vec_zero (&obj->vertices[i].norm);
  }

  /* PASS-2  calculate facenormals & vertexnormals in same time */
  for (i = 0; i < obj->numfaces; i++) { 
    /* facenormals: */
    c_VECTOR *p1=&obj->faces[i].pa->vert;
    c_VECTOR *p2=&obj->faces[i].pb->vert;
    c_VECTOR *p3=&obj->faces[i].pc->vert;
    normal.x=-(p1->y-p2->y)*(p1->z-p3->z) + (p1->z-p2->z)*(p1->y-p3->y);
    normal.y=-(p1->z-p2->z)*(p1->x-p3->x) + (p1->x-p2->x)*(p1->z-p3->z);
    normal.z=-(p1->x-p2->x)*(p1->y-p3->y) + (p1->y-p2->y)*(p1->x-p3->x);

    /* vertexnormals: */
    j=obj->faces[i].a;vec_add (&normal, &obj->vertices[j].norm, &obj->vertices[j].norm);
    j=obj->faces[i].b;vec_add (&normal, &obj->vertices[j].norm, &obj->vertices[j].norm);
    j=obj->faces[i].c;vec_add (&normal, &obj->vertices[j].norm, &obj->vertices[j].norm);

    vec_normalize (&normal, &normal);
    vec_copy (&normal, &obj->faces[i].norm);
    
    obj->faces[i].D=normal.x*p1->x+normal.y*p1->y+normal.z*p1->z;
    
  }

  /* PASS-3  normalize vertex normals */
  for (i = 0; i < obj->numverts; i++) { 
    vec_normalize (&obj->vertices[i].norm, &obj->vertices[i].norm);
  }
  
}

#if 0
static void recalc_objnormals (c_OBJECT *obj)
{
  c_VECTOR  normal;
  int32     i, j;

  /* PASS-1  vertexnormals=0 */
  for (i = 0; i < obj->numverts; i++) { 
    vec_zero (&obj->vertices[i].pnorm);
  }

  /* PASS-2  calculate facenormals & vertexnormals in same time */
  for (i = 0; i < obj->numfaces; i++) { 
    /* facenormals: */
    c_VECTOR *p1=&obj->faces[i].pa->pvert;
    c_VECTOR *p2=&obj->faces[i].pb->pvert;
    c_VECTOR *p3=&obj->faces[i].pc->pvert;
    normal.x=+(p1->y-p2->y)*(p1->z-p3->z) - (p1->z-p2->z)*(p1->y-p3->y);
    normal.y=+(p1->z-p2->z)*(p1->x-p3->x) - (p1->x-p2->x)*(p1->z-p3->z);
    normal.z=+(p1->x-p2->x)*(p1->y-p3->y) - (p1->y-p2->y)*(p1->x-p3->x);

    vec_normalize (&normal, &normal);
    vec_copy (&normal, &obj->faces[i].pnorm);
    /* vertexnormals: */
    j=obj->faces[i].a;vec_add (&normal, &obj->vertices[j].pnorm, &obj->vertices[j].pnorm);
    j=obj->faces[i].b;vec_add (&normal, &obj->vertices[j].pnorm, &obj->vertices[j].pnorm);
    j=obj->faces[i].c;vec_add (&normal, &obj->vertices[j].pnorm, &obj->vertices[j].pnorm);
  }

  /* PASS-3  normalize vertex normals */
  for (i = 0; i < obj->numverts; i++) { 
    vec_normalize (&obj->vertices[i].pnorm, &obj->vertices[i].pnorm);
  }
}
#endif

static void calc_normals ()
{
/*
  calc_normals: calculates face/vertex normals.
*/
  w_NODE *node;

  for (node = ast3d_scene->world; node; node = node->next)
    if (node->type == ast3d_obj_object){
#ifdef OTIMIZE_VERTEX
      optimize_vertex ((c_OBJECT *)node->object);
#endif
      calc_objnormals ((c_OBJECT *)node->object);
    }

  ast3d_NOfixUV();

}

static void calc_bbox ()
{
/*
  calc_bbox: calculate bounding boxes for objects.
*/
  w_NODE   *node;
  c_OBJECT *obj;
  int       i;

  for (node = ast3d_scene->world; node; node = node->next)
    if (node->type == ast3d_obj_object) {
      obj = (c_OBJECT *)node->object;
      vec_copy (&obj->vertices[0].vert, &obj->bbox.min);
      vec_copy (&obj->vertices[0].vert, &obj->bbox.max);
      for (i = 1; i < obj->numverts; i++) {
        if (obj->vertices[i].vert.x < obj->bbox.min.x)
          obj->bbox.min.x = obj->vertices[i].vert.x;
        if (obj->vertices[i].vert.y < obj->bbox.min.y)
          obj->bbox.min.y = obj->vertices[i].vert.y;
        if (obj->vertices[i].vert.z < obj->bbox.min.z)
          obj->bbox.min.z = obj->vertices[i].vert.z;
        if (obj->vertices[i].vert.x > obj->bbox.max.x)
          obj->bbox.max.x = obj->vertices[i].vert.x;
        if (obj->vertices[i].vert.y > obj->bbox.max.y)
          obj->bbox.max.y = obj->vertices[i].vert.y;
        if (obj->vertices[i].vert.z > obj->bbox.max.z)
          obj->bbox.max.z = obj->vertices[i].vert.z;
      }
      for(i=0;i<8;i++){
        obj->bbox.p[i].x= (i&1) ? obj->bbox.max.x : obj->bbox.min.x;
        obj->bbox.p[i].y= (i&2) ? obj->bbox.max.y : obj->bbox.min.y;
        obj->bbox.p[i].z= (i&4) ? obj->bbox.max.z : obj->bbox.min.z;
      }
    }
}

#include "do_trans.c"

static void ast3d_free_track (t_TRACK *track)
{
/*
  ast3d_free_track: deallocated memory used by track.
*/
  t_KEY *key, *next;

  if (!track) return;
  for (key = track->keys; key; key = next) {
    next = key->next;
    free (key);
  }
  free (track);
}

static int32 strucmp (char *s1, char *s2)
{
/*
  strucmp: non-case sensitive string compare.
*/
  int32 diff = 0;

  do {
    diff += (toupper (*s1) - toupper (*s2));
  } while (*s1++ && *s2++);
  return diff;
}

int ast3d_insidebox (c_BOUNDBOX *a, c_VECTOR *b)
{
/*
  ast3d_insidebox: collision detection (point inside bounding box).
*/
  if( b->x > a->min.x && b->y > a->min.y && b->z > a->min.z &&
      b->x < a->max.x && b->y < a->max.y && b->z < a->max.z) return 1;
  return 0;
}


/*****************************************************************************
  astral 3d library (initialization, error handling)
*****************************************************************************/

int32 ast3d_init (int32 flags)
{
/*
  ast3d_init: astral 3d initialization.
*/
  ast3d_scene  = NULL;
  ast3d_camera = NULL;
  ast3d_flags  = flags;
  return ast3d_err_ok;
}

int32 ast3d_done ()
{
/*
  ast3d_done: astral 3d deinitialization.
*/
  ast3d_scene = NULL;
  ast3d_flags = 0;
  return ast3d_err_ok;
}

char *ast3d_geterror (int32 code)
{
/*
  ast3d_geterror: return error string.
*/
  switch (code) {
    case ast3d_err_nomem:     return "not enough memory";
    case ast3d_err_nofile:    return "file not found";
    case ast3d_err_badfile:   return "corrupted file";
    case ast3d_err_badver:    return "unsupported version";
    case ast3d_err_badformat: return "unsupported format";
    case ast3d_err_badframe:  return "invalid frame number";
    case ast3d_err_badname:   return "invalid object name";
    case ast3d_err_noframes:  return "no frames in keyframer";
    case ast3d_err_notloaded: return "scene not loaded";
    case ast3d_err_nullptr:   return "null pointer assignment";
    case ast3d_err_invparam:  return "invalid parameter";
    case ast3d_err_spline:    return "less than 2 keys in spline";
    case ast3d_err_singular:  return "cannot inverse singular matrix";
    case ast3d_err_badid:     return "bad object id";
    case ast3d_err_exist:     return "object already exist";
    case ast3d_err_undefined:
    default:                 return "internal error";
  }
}

/*****************************************************************************
  astral 3d library (time and world handling)
*****************************************************************************/

int32 ast3d_getframes (float *start, float *end)
{
/*
  ast3d_getframes: return number of frames.
*/
  if (!ast3d_scene) return ast3d_err_notloaded;
  if (!ast3d_scene->keyframer) return ast3d_err_notloaded;
  if (ast3d_scene->f_end - ast3d_scene->f_start == 0) return ast3d_err_noframes;
  *start = ast3d_scene->f_start;
  *end = ast3d_scene->f_end;
  return ast3d_err_ok;
}

int32 ast3d_setframe (float frame)
{
/*
  ast3d_setframe: set current frame number.
*/
  if (!ast3d_scene) return ast3d_err_notloaded;
  if (!ast3d_scene->keyframer) return ast3d_err_notloaded;
  if (ast3d_scene->f_end - ast3d_scene->f_start == 0) return ast3d_err_noframes;
  if (ast3d_scene->f_start <= frame && ast3d_scene->f_end > frame) {
    ast3d_scene->f_current = frame;
    return ast3d_err_ok;
  } else return ast3d_err_badframe;
}

int32 ast3d_getframe (float *frame)
{
/*
  ast3d_getframe: get current frame number.
*/
  if (!ast3d_scene) return ast3d_err_notloaded;
  if (!ast3d_scene->keyframer) return ast3d_err_notloaded;
  if (ast3d_scene->f_end - ast3d_scene->f_start == 0) return ast3d_err_noframes;
  *frame = ast3d_scene->f_current;
  return ast3d_err_ok;
}

int32 ast3d_setactive_scene (c_SCENE *scene)
{
/*
  ast3d_setactive_scene: set active scene.
*/
  if (!scene) return ast3d_err_nullptr;
  ast3d_scene = scene;
  return ast3d_err_ok;
}

int32 ast3d_setactive_camera (c_CAMERA *cam)
{
/*
  ast3d_setactive_camera: set active camera.
*/
  ast3d_camera = cam;
  return ast3d_err_ok;
}

int32 ast3d_getactive_scene (c_SCENE **scene)
{
/*
  ast3d_getactive_scene: get active scene.
*/
  *scene = ast3d_scene;
  return ast3d_err_ok;
}

int32 ast3d_getactive_camera (c_CAMERA **camera)
{
/*
  ast3d_getactive_camera: get active camera.
*/
  *camera = ast3d_camera;
  return ast3d_err_ok;
}

int32 ast3d_byname (char *name, w_NODE **node)
{
/*
  ast3d_byname: find object by name (world tree).
*/
  if (!ast3d_scene || !ast3d_scene->world) return ast3d_err_notloaded;
  for (*node = ast3d_scene->world; *node; *node = (*node)->next) {
    switch ((*node)->type) {
      case ast3d_obj_camera:
        if (strcmp (((c_CAMERA *)((*node)->object))->name, name) == 0)
          return ast3d_err_ok; break;
      case ast3d_obj_object:
        if (strcmp (((c_OBJECT *)((*node)->object))->name, name) == 0)
          return ast3d_err_ok; break;
      case ast3d_obj_light:
        if (strcmp (((c_LIGHT *)((*node)->object))->name, name) == 0)
          return ast3d_err_ok; break;
      case ast3d_obj_material:
        if (strcmp (((c_MATERIAL *)((*node)->object))->name, name) == 0)
          return ast3d_err_ok; break;
      case ast3d_obj_ambient:
        if (strcmp (((c_AMBIENT *)((*node)->object))->name, name) == 0)
          return ast3d_err_ok;
    }
  }
  return ast3d_err_ok; /* return NULL */
}

int32 ast3d_byid (int32 id, w_NODE **node)
{
/*
  ast3d_byid: find object by id (world tree).
*/
  if (!ast3d_scene || !ast3d_scene->world) return ast3d_err_notloaded;
  for (*node = ast3d_scene->world; *node; *node = (*node)->next) {
    switch ((*node)->type) {
      case ast3d_obj_camera:
        if (((c_CAMERA *)((*node)->object))->id == id) return ast3d_err_ok;
        break;
      case ast3d_obj_object:
        if (((c_OBJECT *)((*node)->object))->id == id) return ast3d_err_ok;
        break;
      case ast3d_obj_light:
        if (((c_LIGHT *)((*node)->object))->id == id) return ast3d_err_ok;
        break;
      case ast3d_obj_material:
        if (((c_MATERIAL *)((*node)->object))->id == id) return ast3d_err_ok;
        break;
      case ast3d_obj_ambient:
        if (((c_AMBIENT *)((*node)->object))->id == id) return ast3d_err_ok;
    }
  }
  return ast3d_err_ok; /* return NULL */
}

int32 ast3d_findfirst (int32 attr, w_NODE **node)
{
/*
  ast3d_findfirst: finds first node with attribute "attr" (world tree).
*/
  if (!ast3d_scene || !ast3d_scene->world) return ast3d_err_notloaded;
  for (*node = ast3d_scene->world; *node; *node = (*node)->next)
    if ((*node)->type & attr) return ast3d_err_ok;
  return ast3d_err_ok; /* return NULL */
}

int32 ast3d_findnext (int32 attr, w_NODE **node)
{
/*
  ast3d_findnext: finds next node with attribute "attr" (world tree).
*/
  if (!ast3d_scene || !ast3d_scene->world) return ast3d_err_notloaded;
  for (*node = (*node)->next; *node; *node = (*node)->next)
    if ((*node)->type & attr) return ast3d_err_ok;
  return ast3d_err_ok; /* return NULL */
}

/*****************************************************************************
  astral 3d library (world/keyframer constructors)
*****************************************************************************/

int32 ast3d_add_world (int32 type, void *obj)
{
/*
  ast3d_add_world: add object to world list.
*/
  w_NODE *node;

  if (!ast3d_scene) return ast3d_err_notloaded;
  if ((node = (w_NODE *)malloc (sizeof (w_NODE))) == NULL)
    return ast3d_err_nomem;
  node->type = type;
  node->object = obj;
  node->next = NULL;
  if (!ast3d_scene->world) {
    node->prev = NULL;
    ast3d_scene->world = node;
    ast3d_scene->wtail = node;
  } else {
    node->prev = ast3d_scene->wtail;
    ast3d_scene->wtail->next = node;
    ast3d_scene->wtail = node;
  }
  return ast3d_err_ok;
}

int32 ast3d_add_track (int32 type, int32 id, int32 parent, void *track, void *obj)
{
/*
  ast3d_add_track: add track to keyframer list.
*/
  k_NODE *node, *pnode;

  if (!ast3d_scene) return ast3d_err_notloaded;
  if ((node = (k_NODE *)malloc (sizeof (k_NODE))) == NULL)
    return ast3d_err_nomem;
  node->type = type;
  node->id = id;
  node->track = track;
  node->brother = NULL;
  node->child = NULL;
  node->next = NULL;
  node->object = obj;
  if (!ast3d_scene->keyframer) {
    node->prev = NULL;
    ast3d_scene->keyframer = node;
    ast3d_scene->ktail = node;
  } else {
    node->prev = ast3d_scene->ktail;
    ast3d_scene->ktail->next = node;
    ast3d_scene->ktail = node;
  }
  if (parent != -1) { /* update hierarchy tree */
    for (pnode = ast3d_scene->keyframer; pnode; pnode = pnode->next)
      if (pnode->id == parent) {
        node->parent = pnode;
        if (pnode->child == NULL) pnode->child = node; else {
          node->brother = pnode->child;
          pnode->child = node;
        }
      }
  } else node->parent = NULL;
  return ast3d_err_ok;
}

int32 ast3d_set_track (int32 type, int32 id, t_TRACK *track)
{
/*
  ast3d_set_track: assign a track to keyframer node.
*/
  k_NODE *node = ast3d_scene->keyframer;
  void   *obj;

  if (!ast3d_scene || !ast3d_scene->keyframer) return ast3d_err_notloaded;
  while (node && node->id != id) node = node->next;
  if (!node) return ast3d_err_undefined;
  obj = node->track;

  switch (node->type) {
    case ast3d_track_camera:
      switch (type) {
        case ast3d_key_pos:  ((t_CAMERA *)obj)->pos = track; break;
        case ast3d_key_fov:  ((t_CAMERA *)obj)->fov = track; break;
        case ast3d_key_roll: ((t_CAMERA *)obj)->roll = track;
      }
      break;
    case ast3d_track_cameratgt:
      switch (type) {
        case ast3d_key_pos: ((t_CAMERATGT *)obj)->pos = track;
      }
      break;
    case ast3d_track_light:
      switch (type) {
        case ast3d_key_pos:   ((t_LIGHT *)obj)->pos = track; break;
        case ast3d_key_color: ((t_LIGHT *)obj)->color = track;
      }
      break;
    case ast3d_track_lighttgt:
      switch (type) {
        case ast3d_key_pos:   ((t_LIGHTTGT *)obj)->pos = track; break;
      }
    case ast3d_track_spotlight:
      switch (type) {
        case ast3d_key_pos:   ((t_SPOTLIGHT *)obj)->pos = track; break;
        case ast3d_key_color: ((t_SPOTLIGHT *)obj)->color = track; break;
        case ast3d_key_roll:  ((t_SPOTLIGHT *)obj)->roll = track;
      }
      break;
    case ast3d_track_object:
      switch (type) {
        case ast3d_key_pos:    ((t_OBJECT *)obj)->translate = track; break;
        case ast3d_key_scale:  ((t_OBJECT *)obj)->scale = track; break;
        case ast3d_key_rotate: ((t_OBJECT *)obj)->rotate = track; break;
        case ast3d_key_morph:  ((t_OBJECT *)obj)->morph = track; break;
        case ast3d_key_hide:   ((t_OBJECT *)obj)->hide = track;
      }
      break;
    case ast3d_track_ambient:
      switch (type) {
        case ast3d_key_color: ((t_AMBIENT *)obj)->color = track;
      }
  }
  return ast3d_err_ok;
}

/*****************************************************************************
  astral 3d library (scene load/save/free functions)
*****************************************************************************/

int32 ast3d_alloc_scene (c_SCENE **scene)
{
/*
  ast3d_alloc_scene: allocates memory for a new scene.
*/
  if ((*scene = (c_SCENE *)malloc (sizeof (c_SCENE))) == NULL)
    return ast3d_err_nomem;
  (*scene)->world = NULL;
  (*scene)->wtail = NULL;
  (*scene)->keyframer = NULL;
  (*scene)->ktail = NULL;
  (*scene)->fog.type = 0;
  (*scene)->backgr.type = 0;
  (*scene)->directional_lighting = 0;
  (*scene)->sphere_map = 0;
  return ast3d_err_ok;
}

int32 ast3d_load_world (char *filename, c_SCENE *scene)
{
/*
  ast3d_load_world: loads mesh data from file "filename" into scene "scene".
*/
  int32   (*loader)(afs_FILE *f) = NULL;
  c_SCENE *old_scene = ast3d_scene;
  afs_FILE    *f;
  char    *s;
  int32    i, error;

  for (i = 0; i < sizeof (ast3d_drivers) / sizeof (ast3d_drivers[0]); i++) {
    s = strchr (filename, '.') + 1;
    if (strucmp (s, ast3d_drivers[i].name) == 0)
      loader = ast3d_drivers[i].load_mesh;
  }
  if (!loader) return ast3d_err_badformat;
  scene->f_start = 0;
  scene->f_end = 0;
  scene->f_current = 0;
  scene->world = NULL;
  scene->wtail = NULL;
  scene->keyframer = NULL;
  scene->ktail = NULL;
  if ((f = afs_fopen (filename, "rb")) == NULL) return ast3d_err_nofile;
  ast3d_setactive_scene (scene);
  error = loader (f);
  afs_fclose (f);
  if (error) {
    ast3d_setactive_scene (old_scene);
    ast3d_free_world (scene);
    return error;
  }
  calc_bbox ();
  calc_normals ();
  ast3d_setactive_scene (old_scene);
  return ast3d_err_ok;
}

int32 ast3d_load_motion (char *filename, c_SCENE *scene)
{
/*
  ast3d_load_motion: loads motion data from file "filename"
                    into scene "scene".
*/
  int32    (*loader)(afs_FILE *f) = NULL;
  c_SCENE  *old_scene = ast3d_scene;
  afs_FILE     *f;
  char     *s;
  int32     i, error;

  if (!scene->world) return ast3d_err_notloaded;
  for (i = 0; i < sizeof (ast3d_drivers) / sizeof (ast3d_drivers[0]); i++) {
    s = strchr (filename, '.') + 1;
    if (strucmp (s, ast3d_drivers[i].name) == 0)
      loader = ast3d_drivers[i].load_motion;
  }
  if (!loader) return ast3d_err_badformat;
  if ((f = afs_fopen (filename, "rb")) == NULL) return ast3d_err_nofile;
  ast3d_setactive_scene (scene);
  error = loader (f);
  afs_fclose (f);
  if (error) {
    ast3d_setactive_scene (old_scene);
    ast3d_free_motion (scene);
    return error;
  }
  ast3d_setframe (0);
  ast3d_setactive_scene (old_scene);
  return ast3d_err_ok;
}

int32 ast3d_load_scene (char *filename, c_SCENE *scene)
{
/*
  ast3d_load_scene: loads mesh and keyframer data from file "filename"
                   into scene "scene".
*/
  int32 error;

  if ((error = ast3d_load_world (filename, scene)) != ast3d_err_ok)
    return error;
  return ast3d_load_motion (filename, scene);
}

int32 ast3d_save_scene (char *filename, c_SCENE *scene)
{
/*
  ast3d_save_scene: saves scene "scene" to filename "filename".
*/
  int32    (*loader)(afs_FILE *f) = NULL;
  c_SCENE  *old_scene = ast3d_scene;
  afs_FILE     *f;
  char     *s;
  int32     i, error;

  if (!scene || !scene->world) return ast3d_err_notloaded;
  for (i = 0; i < sizeof (ast3d_drivers) / sizeof (ast3d_drivers[0]); i++) {
    s = strchr (filename, '.') + 1;
    if (strucmp (s, ast3d_drivers[i].name) == 0)
      loader = ast3d_drivers[i].save_scene;
  }
  if (!loader) return ast3d_err_badformat;
  if ((f = afs_fopen (filename, "wb")) == NULL) return ast3d_err_undefined;
  ast3d_setactive_scene (scene);
  error = loader (f);
  afs_fclose (f);
  ast3d_setactive_scene (old_scene);
  if (error) return error;
  return ast3d_err_ok;
}

int32 ast3d_free_world (c_SCENE *scene)
{
/*
  ast3d_free_world: release all memory used by world.
*/
  w_NODE     *node, *next;
  c_LIGHT    *light;
  c_CAMERA   *cam;
  c_OBJECT   *obj;
  c_MATERIAL *mat;
  c_AMBIENT  *amb;

  if (!scene || !scene->world) return ast3d_err_nullptr;
  for (node = scene->world; node; node = next) {
    switch (node->type) {
      case ast3d_obj_light:
        light = (c_LIGHT *)node->object;
        free (light->name);
        break;
      case ast3d_obj_camera:
        cam = (c_CAMERA *)node->object;
        free (cam->name);
        break;
      case ast3d_obj_object:
        obj = (c_OBJECT *)node->object;
        free (obj->name);
        free (obj->vertices);
        free (obj->faces);
        break;
      case ast3d_obj_material:
        mat = (c_MATERIAL *)node->object;
        free (mat->name);
        free (mat->texture.file);
        free (mat->bump.file);
        free (mat->reflection.file);
        break;
      case ast3d_obj_ambient:
        amb = (c_AMBIENT *)node->object;
        free (amb->name);
    }
    next = node->next;
    free (node->object);
    free (node);
  }
  scene->world = NULL;
  scene->wtail = NULL;
  return ast3d_err_ok;
}

int32 ast3d_free_motion (c_SCENE *scene)
{
/*
  ast3d_free_motion: release all memory used by keyframer.
*/
  k_NODE      *node, *next;
  t_CAMERA    *cam;
  t_CAMERATGT *camtgt;
  t_LIGHT     *light;
  t_LIGHTTGT  *litgt;
  t_SPOTLIGHT *spotl;
  t_OBJECT    *obj;
  t_AMBIENT   *amb;

  if (!scene || !scene->keyframer) return ast3d_err_nullptr;
  for (node = scene->keyframer; node; node = next) {
    switch (node->type) {
      case ast3d_track_camera:
        cam = (t_CAMERA *)node->track;
        ast3d_free_track (cam->pos);
        ast3d_free_track (cam->fov);
        ast3d_free_track (cam->roll);
        break;
      case ast3d_track_cameratgt:
        camtgt = (t_CAMERATGT *)node->track;
        ast3d_free_track (camtgt->pos);
        break;
      case ast3d_track_light:
        light = (t_LIGHT *)node->track;
        ast3d_free_track (light->pos);
        ast3d_free_track (light->color);
        break;
      case ast3d_track_spotlight:
        spotl = (t_SPOTLIGHT *)node->track;
        ast3d_free_track (spotl->pos);
        ast3d_free_track (spotl->color);
        ast3d_free_track (spotl->roll);
        break;
      case ast3d_track_lighttgt:
        litgt = (t_LIGHTTGT *)node->track;
        ast3d_free_track (litgt->pos);
        break;
      case ast3d_track_object:
        obj = (t_OBJECT *)node->track;
        ast3d_free_track (obj->translate);
        ast3d_free_track (obj->scale);
        ast3d_free_track (obj->rotate);
        ast3d_free_track (obj->morph);
        ast3d_free_track (obj->hide);
        break;
      case ast3d_track_ambient:
        amb = (t_AMBIENT *)node->track;
        ast3d_free_track (amb->color);
    }
    next = node->next;
    free (node->track);
    free (node);
  }
  scene->keyframer = NULL;
  scene->ktail = NULL;
  return ast3d_err_ok;
}

int32 ast3d_free_mesh (c_SCENE *scene)
{
/*
  ast3d_free_mesh: release all memory used by meshes.
*/
  w_NODE     *node;
  c_OBJECT   *obj;

  if (!scene || !scene->world ||
      (ast3d_flags & ast3d_transform)) return ast3d_err_nullptr;
  for (node = scene->world; node; node = node->next) {
    switch (node->type) {
      case ast3d_obj_object:
        obj = (c_OBJECT *)node->object;
        free (obj->vertices); obj->vertices = NULL;
        free (obj->faces);    obj->faces = NULL;
        break;
    }
  }
  return ast3d_err_ok;
}

int32 ast3d_free_scene (c_SCENE *scene)
{
  int32 error;

  if ((error = ast3d_free_world (scene)) != ast3d_err_ok) return error;
  return ast3d_free_motion (scene);
}

/*****************************************************************************
  astral 3d library (track handling)
*****************************************************************************/

int32 ast3d_getkey_float (t_TRACK *track, float frame, float *out)
{
/*
  ast3d_getkey_float: return float key at frame "frame".
*/
  t_KEY *keys;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) return ast3d_err_nullptr;
  keys = track->keys;

  if (track->flags != 0) frame = fmod (frame, track->frames);

  if (!keys->next || frame < keys->frame) {
    *out = keys->val._float;
    return ast3d_err_ok;
  }
  /* more than one key, spline interpolation */
  return spline_getkey_float (track, frame, out);
}

int32 ast3d_getkey_vect (t_TRACK *track, float frame, c_VECTOR *out)
{
/*
  ast3d_getkey_vect: return vector key at frame "frame".
*/
  t_KEY *keys;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) return ast3d_err_nullptr;
  keys = track->keys;

  if (track->flags != 0) frame = fmod (frame, track->frames);

  if (!keys->next || frame < keys->frame) {
    vec_copy (&keys->val._vect, out);
    return ast3d_err_ok;
  }
  /* more than one key, spline interpolation */
  return spline_getkey_vect (track, frame, out);
}

int32 ast3d_getkey_rgb (t_TRACK *track, float frame, c_RGB *out)
{
/*
  ast3d_getkey_rgb: return rgb key at frame "frame".
*/
  c_VECTOR vect;
  int32    aa;

  aa = spline_getkey_vect (track, frame,&vect);
  out->rgb[0]=vect.x;
  out->rgb[1]=vect.y;
  out->rgb[2]=vect.z;
  return aa;
}

int32 ast3d_getkey_quat (t_TRACK *track, float frame, c_QUAT *out)
{
/*
  ast3d_getkey_quat: return quaternion key at frame "frame".
*/
  t_KEY *keys;
  float  alpha;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) return ast3d_err_nullptr;
  keys = track->keys;

  if (track->flags != 0) frame = fmod (frame, track->frames);

  if (ast3d_flags & ast3d_slerp) {
    if (frame < track->last->frame) keys = track->keys; else
      keys = track->last;
    while (keys->next && frame > keys->next->frame) keys = keys->next;
    track->last = keys;
    if (!keys->next || frame < keys->frame) {
      qt_copy (&keys->qa, out);
      return ast3d_err_ok;
    }
    alpha = (frame - keys->frame) / (keys->next->frame - keys->frame);
    alpha = spline_ease (alpha, keys->easefrom, keys->next->easeto);
    qt_slerp (&keys->qa, &keys->next->qa, 0, alpha, out);
    return ast3d_err_ok;
  } else {
    if (!keys->next || frame < keys->frame) {
      qt_copy (&keys->qa, out);
      return ast3d_err_ok;
    }
    return spline_getkey_quat (track, frame, out);
  }
}

int32 ast3d_getkey_hide (t_TRACK *track, float frame, int32 *out)
{
/*
  ast3d_getkey_hide: return hide key at frame "frame".
*/
  t_KEY *keys;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) {
    *out = 0;
    return ast3d_err_nullptr;
  }
  keys = track->keys;
  if (track->flags != 0) frame = fmod (frame, track->frames);
  if (frame < keys->frame) {
    *out = 0;
    return ast3d_err_ok;
  }
  if (frame < track->last->frame) keys = track->keys; else
    keys = track->last;
  while (keys->next && frame > keys->next->frame) keys = keys->next;
  track->last = keys;
  *out = keys->val._int;
  return ast3d_err_ok;
}

int32 ast3d_getkey_morph (t_TRACK *track, float frame, c_MORPH *out)
{
/*
  ast3d_getkey_morph: return morph key at frame "frame".
*/
  t_KEY *keys;
  float  alpha;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) return ast3d_err_nullptr;
  if (frame < track->last->frame) keys = track->keys; else keys = track->last;
  while (keys->next && frame > keys->next->frame) keys = keys->next;
  track->last = keys;
  
  if (!keys->next || frame < keys->frame) {
    out->from = keys->prev->val._int;
    out->to = keys->val._int;
    out->alpha = 1.0;
    out->key=(t_KEY*) NULL;
    return ast3d_err_ok;
  }
  
  out->from = keys->val._int;
  out->to = keys->next->val._int;
  alpha = (frame - keys->frame) / (keys->next->frame - keys->frame);
  out->alpha = spline_ease (alpha, keys->easefrom, keys->next->easeto);
  out->key = keys;
  return ast3d_err_ok;
}

/*****************************************************************************
  astral 3d library (keyframer)
*****************************************************************************/

int32 ast3d_collide (w_NODE *a, w_NODE *b, int32 *result)
{
/*
  ast3d_collide: collision detection (using bounding box).
*/
  c_OBJECT *o, *obj;
  c_CAMERA *cam;
  c_LIGHT  *light;
  int32     type;

  if (a->type != ast3d_obj_object && b->type != ast3d_obj_object)
    return ast3d_err_invparam;
  if (a->type == ast3d_obj_object) {
    o = (c_OBJECT *)a->object;
    obj = (c_OBJECT *)b->object;
    cam = (c_CAMERA *)b->object;
    light = (c_LIGHT *)b->object;
    type = b->type;
  } else {
    o = (c_OBJECT *)b->object;
    obj = (c_OBJECT *)a->object;
    cam = (c_CAMERA *)a->object;
    light = (c_LIGHT *)a->object;
    type = a->type;
  }
  switch (type) {
    case ast3d_obj_object:
      *result = (ast3d_insidebox (&o->pbbox, &obj->pbbox.min) ||
                 ast3d_insidebox (&o->pbbox, &obj->pbbox.max) ||
                 ast3d_insidebox (&obj->pbbox, &o->pbbox.min) ||
                 ast3d_insidebox (&obj->pbbox, &o->pbbox.max));
      return ast3d_err_ok;
    case ast3d_obj_camera:
      *result = (ast3d_insidebox (&o->pbbox, &cam->pos) ||
                 ast3d_insidebox (&o->pbbox, &cam->target));
      return ast3d_err_ok;
    case ast3d_obj_light:
      if (light->flags == ast3d_light_spot)
        *result = (ast3d_insidebox (&o->pbbox, &light->pos) ||
                   ast3d_insidebox (&o->pbbox, &light->target));
      else
        *result = (ast3d_insidebox (&o->pbbox, &light->pos));
      return ast3d_err_ok;
    case ast3d_obj_material:
    case ast3d_obj_ambient: return ast3d_err_invparam;
  }
  return ast3d_err_undefined;
}

#include "update.c"


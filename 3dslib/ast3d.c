#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "ast3d.h"
#include "ast3di.h"

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
  int32 (*load_mesh)   (FILE *f); /* loads mesh            */
  int32 (*load_motion) (FILE *f); /* loads motion          */
  int32 (*save_scene)  (FILE *f); /* saves the whole scene */
} ast3d_drivers[] = {
  {"3DS", ast3d_load_mesh_3DS, ast3d_load_motion_3DS, NULL},
//  {"AST", ast3d_load_mesh_AST, ast3d_load_motion_AST, ast3d_save_AST}
  {"AST", NULL, NULL, NULL}
};

/*****************************************************************************
  internal functions
*****************************************************************************/

static void calc_objnormals (c_OBJECT *obj)
{
/*
  calc_normals: calculates face/vertex normals.
*/
  c_VECTOR  a, b, normal;
  int32     i, j, num;

  for (i = 0; i < obj->numfaces; i++) { /* face normals */
    vec_sub (&obj->faces[i].pa->vert,
             &obj->faces[i].pb->vert, &a);
    vec_sub (&obj->faces[i].pb->vert,
             &obj->faces[i].pc->vert, &b);
    vec_cross (&a, &b, &normal);
    vec_normalize (&normal, &obj->faces[i].norm);
  }
  for (i = 0; i < obj->numverts; i++) { /* vertex normals */
    num = 0;
    vec_zero (&normal);
    for (j = 0; j < obj->numfaces; j++) {
      if (obj->faces[j].a == i ||
          obj->faces[j].b == i ||
          obj->faces[j].c == i) {
        vec_add (&normal, &obj->faces[j].norm, &normal);
        num++;
      }
    }
    if (num) vec_scale (&normal, 1.0 / (float)num, &normal);
      vec_normalize (&normal, &obj->vertices[i].norm);
  }
}

static void recalc_objnormals (c_OBJECT *obj)
{
/*
  calc_normals: calculates face/vertex normals.
*/
  c_VECTOR  a, b, normal;
  int32     i, j, num;

  for (i = 0; i < obj->numfaces; i++) { /* face normals */
    vec_sub (&obj->faces[i].pa->pvert,
             &obj->faces[i].pb->pvert, &a);
    vec_sub (&obj->faces[i].pb->pvert,
             &obj->faces[i].pc->pvert, &b);
    vec_cross (&a, &b, &normal);
    vec_normalize (&normal, &obj->faces[i].pnorm);
  }
  for (i = 0; i < obj->numverts; i++) { /* vertex normals */
    num = 0;
    vec_zero (&normal);
    for (j = 0; j < obj->numfaces; j++) {
      if (obj->faces[j].a == i ||
          obj->faces[j].b == i ||
          obj->faces[j].c == i) {
        vec_add (&normal, &obj->faces[j].pnorm, &normal);
        num++;
      }
    }
    if (num) vec_scale (&normal, 1.0 / (float)num, &normal);
      vec_normalize (&normal, &obj->vertices[i].pnorm);
  }
}

static void calc_normals ()
{
/*
  calc_normals: calculates face/vertex normals.
*/
  w_NODE *node;

  for (node = ast3d_scene->world; node; node = node->next)
    if (node->type == ast3d_obj_object)
      calc_objnormals ((c_OBJECT *)node->object);
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
    }
}

static void do_transform ()
{
/*
  do_transform: transform all world (see notes in the documentation).
*/
  w_NODE   *node, *from, *to;
  c_OBJECT *obj, *o1, *o2;
  c_VERTEX *v1, *v2;
  c_FACE   *f1, *f2;
  c_VECTOR  vec,c,cit;
  c_MATRIX  objmat, normat, tnsmat;
  c_CAMERA *cam;
  float     alpha;
  int32     i;

  for (node = ast3d_scene->world; node; node = node->next)
    if (node->type == ast3d_track_camera)
      cam = (c_CAMERA *)node->object;
	vec_sub (&cam->target,&cam->pos,&c);

  for (node = ast3d_scene->world; node; node = node->next) {
    if (node->type == ast3d_obj_object) {
      obj = (c_OBJECT *)node->object;
      if (ast3d_camera) mat_mul (ast3d_camera->matrix, obj->matrix, objmat);
        else mat_copy (obj->matrix, objmat);
      mat_normalize (obj->matrix, normat);
      if ((ast3d_flags & ast3d_domorph) && (obj->flags & ast3d_obj_morph)) {
        ast3d_byid (obj->morph.from, &from);
        ast3d_byid (obj->morph.to, &to);
        o1 = (c_OBJECT *)from->object;
        o2 = (c_OBJECT *)to->object;
        v1 = o1->vertices;
        v2 = o2->vertices;
        f1 = o1->faces;
        f2 = o2->faces;
        alpha = obj->morph.alpha;
        vec_lerp (&o1->bbox.min, &o2->bbox.min, alpha, &vec);
        mat_mulvec (obj->matrix, &vec, &obj->pbbox.min);
        vec_lerp (&o1->bbox.max, &o2->bbox.max, alpha, &vec);
        mat_mulvec (obj->matrix, &vec, &obj->pbbox.max);
        for (i = 0; i < obj->numverts; i++) {
          vec_lerp (&v1[i].vert, &v2[i].vert, alpha, &vec);
          mat_mulvec (objmat, &vec, &obj->vertices[i].pvert);
        }
        if ((ast3d_flags & ast3d_normlerp) && (ast3d_flags & ast3d_calcnorm)) {
          for (i = 0; i < obj->numverts; i++) {
            vec_lerp (&v1[i].norm, &v2[i].norm, alpha, &vec);
            mat_mulnorm (normat, &vec, &obj->vertices[i].pnorm);
          }
          for (i = 0; i < obj->numfaces; i++) {
            vec_lerp (&f1[i].norm, &f2[i].norm, alpha, &vec);
            mat_mulnorm (normat, &vec, &obj->faces[i].pnorm);
          }
        } else if ((ast3d_flags & ast3d_normcalc) && (ast3d_flags & ast3d_calcnorm)) {
          recalc_objnormals (obj);
        } else if (ast3d_flags & ast3d_calcnorm) {
          for (i = 0; i < obj->numverts; i++)
            mat_mulnorm (normat, &obj->vertices[i].norm,
                                 &obj->vertices[i].pnorm);
          for (i = 0; i < obj->numfaces; i++)
            mat_mulnorm (normat, &obj->faces[i].norm, &obj->faces[i].pnorm);
        }
      } else {
        mat_mulvec (obj->matrix, &obj->bbox.min, &obj->pbbox.min);
        mat_mulvec (obj->matrix, &obj->bbox.max, &obj->pbbox.max);

  cit.x = c.x*obj->matrix[X][X] + c.y*obj->matrix[Y][X] + c.z*obj->matrix[Z][X];
  cit.y = c.x*obj->matrix[X][Y] + c.y*obj->matrix[Y][Y] + c.z*obj->matrix[Z][Y];
  cit.z = c.x*obj->matrix[X][Z] + c.y*obj->matrix[Y][Z] + c.z*obj->matrix[Z][Z];

        vec_normalize(&cit,&cit);

        for (i = 0; i < obj->numfaces; i++){
	vec_copy(&obj->faces[i].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}

	if(!obj->faces[i].visible){
	vec_copy(&obj->vertices[obj->faces[i].a].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}

	if(!obj->faces[i].visible){
	vec_copy(&obj->vertices[obj->faces[i].b].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}

	if(!obj->faces[i].visible){
	vec_copy(&obj->vertices[obj->faces[i].c].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}}}}}


//	if (ast3d_flags & ast3d_calcnorm) {
          for (i = 0; i < obj->numverts; i++)
            mat_mulnorm (normat, &obj->vertices[i].norm,
                                 &obj->vertices[i].pnorm);
          for (i = 0; i < obj->numfaces; i++)
            mat_mulnorm (normat, &obj->faces[i].norm, &obj->faces[i].pnorm);
//        }
      }
    }
  }
}

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
  return ast3d_err_ok;
}

int32 ast3d_load_world (char *filename, c_SCENE *scene)
{
/*
  ast3d_load_world: loads mesh data from file "filename" into scene "scene".
*/
  int32   (*loader)(FILE *f) = NULL;
  c_SCENE *old_scene = ast3d_scene;
  FILE    *f;
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
  if ((f = fopen (filename, "rb")) == NULL) return ast3d_err_nofile;
  ast3d_setactive_scene (scene);
  error = loader (f);
  fclose (f);
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
  int32    (*loader)(FILE *f) = NULL;
  c_SCENE  *old_scene = ast3d_scene;
  FILE     *f;
  char     *s;
  int32     i, error;

  if (!scene->world) return ast3d_err_notloaded;
  for (i = 0; i < sizeof (ast3d_drivers) / sizeof (ast3d_drivers[0]); i++) {
    s = strchr (filename, '.') + 1;
    if (strucmp (s, ast3d_drivers[i].name) == 0)
      loader = ast3d_drivers[i].load_motion;
  }
  if (!loader) return ast3d_err_badformat;
  if ((f = fopen (filename, "rb")) == NULL) return ast3d_err_nofile;
  ast3d_setactive_scene (scene);
  error = loader (f);
  fclose (f);
  if (error) {
    ast3d_setactive_scene (old_scene);
    ast3d_free_motion (scene);
    return error;
  }
  ast3d_setframe (0);
  ast3d_update ();
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
  int32    (*loader)(FILE *f) = NULL;
  c_SCENE  *old_scene = ast3d_scene;
  FILE     *f;
  char     *s;
  int32     i, error;

  if (!scene || !scene->world) return ast3d_err_notloaded;
  for (i = 0; i < sizeof (ast3d_drivers) / sizeof (ast3d_drivers[0]); i++) {
    s = strchr (filename, '.') + 1;
    if (strucmp (s, ast3d_drivers[i].name) == 0)
      loader = ast3d_drivers[i].save_scene;
  }
  if (!loader) return ast3d_err_badformat;
  if ((f = fopen (filename, "wb")) == NULL) return ast3d_err_undefined;
  ast3d_setactive_scene (scene);
  error = loader (f);
  fclose (f);
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
//  c_VECTOR *vect = (c_VECTOR *)out;
  c_VECTOR *vect;
  int32    aa;    

  aa = spline_getkey_vect (track, frame, vect);
  out->rgb[0]=vect->x;
  out->rgb[1]=vect->y;
  out->rgb[2]=vect->z;
  return aa;
}

int32 ast3d_getkey_quat (t_TRACK *track, float frame, c_QUAT *out)
{
/*
  ast3d_getkey_quat: return quaternion key at frame "frame".
*/
  t_KEY *keys;
  float  alpha;
  int    error;

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
  if (frame < track->last->frame) keys = track->keys; else
    keys = track->last;
  while (keys->next && frame > keys->next->frame) keys = keys->next;
  track->last = keys;
  if (!keys->next || frame < keys->frame) {
    out->from = keys->prev->val._int;
    out->to = keys->val._int;
    out->alpha = 1.0;
    return ast3d_err_ok;
  }
  out->from = keys->val._int;
  out->to = keys->next->val._int;
  alpha = (frame - keys->frame) / (keys->next->frame - keys->frame);
  out->alpha = spline_ease (alpha, keys->easefrom, keys->next->easeto);
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

int32 ast3d_update ()
{
/*
  ast3d_update: update all keyframer data.
*/
  k_NODE      *node, *child;
  c_CAMERA    *cam;
  t_CAMERA    *tcam;
  t_CAMERATGT *tcamt;
  c_LIGHT     *light;
  t_LIGHT     *tlight;
  t_SPOTLIGHT *tspotl;
  t_LIGHTTGT  *tlightt;
  c_AMBIENT   *amb;
  t_AMBIENT   *tamb;
  c_OBJECT    *obj,*cobj;
  t_OBJECT    *tobj;
  float        frame = ast3d_scene->f_current;
  c_MATRIX     c, d;
  int32        hidden;

  if (!ast3d_scene) return ast3d_err_notloaded;
  if (!ast3d_scene->world || !ast3d_scene->keyframer)
    return ast3d_err_notloaded;

  /* update objects */
  for (node = ast3d_scene->keyframer; node; node = node->next) {
    switch (node->type) {
      case ast3d_track_camera:
        cam = (c_CAMERA *)node->object;
        tcam = (t_CAMERA *)node->track;
        ast3d_getkey_vect (tcam->pos, frame, &cam->pos);
        ast3d_getkey_float (tcam->fov, frame, &cam->fov);
        ast3d_getkey_float (tcam->roll, frame, &cam->roll);
        break;
      case ast3d_track_cameratgt:
        cam = (c_CAMERA *)node->object;
        tcamt = (t_CAMERATGT *)node->track;
        ast3d_getkey_vect (tcamt->pos, frame, &cam->target);
        break;
      case ast3d_track_light:
        light = (c_LIGHT *)node->object;
        tlight = (t_LIGHT *)node->track;
        ast3d_getkey_vect (tlight->pos, frame, &light->pos);
        ast3d_getkey_rgb (tlight->color, frame, &light->color);
        break;
      case ast3d_track_spotlight:
        light = (c_LIGHT *)node->object;
        tspotl = (t_SPOTLIGHT *)node->track;
        ast3d_getkey_vect (tspotl->pos, frame, &light->pos);
        ast3d_getkey_rgb (tspotl->color, frame, &light->color);
        ast3d_getkey_float (tspotl->roll, frame, &light->roll);
        break;
      case ast3d_track_lighttgt:
        light = (c_LIGHT *)node->object;
        tlightt = (t_LIGHTTGT *)node->track;
        ast3d_getkey_vect (tlightt->pos, frame, &light->target);
        break;
      case ast3d_track_object:
        obj = (c_OBJECT *)node->object;
        tobj = (t_OBJECT *)node->track;
        ast3d_getkey_vect (tobj->translate, frame, &obj->translate);
        ast3d_getkey_vect (tobj->scale, frame, &obj->scale);
        ast3d_getkey_quat (tobj->rotate, frame, &obj->rotate);
        ast3d_getkey_hide (tobj->hide, frame, &hidden);
        if (ast3d_getkey_morph (tobj->morph, frame, &obj->morph) ==
            ast3d_err_ok) obj->flags |= ast3d_obj_morph;
            else obj->flags &= ~ast3d_obj_morph;
        if (hidden) obj->flags |= ast3d_obj_hidden; else
          obj->flags &= ~ast3d_obj_hidden;
        qt_invmatrix (&obj->rotate, c);
        mat_setscale (&obj->scale, d);
        c[X][W] = obj->translate.x;
        c[Y][W] = obj->translate.y;
        c[Z][W] = obj->translate.z;
        mat_mul (c, d, obj->matrix);
        break;
      case ast3d_track_ambient:
        amb = (c_AMBIENT *)node->object;
        tamb = (t_AMBIENT *)node->track;
        ast3d_getkey_rgb (tamb->color, frame, &amb->color);
    }
  }

  /* update hierarchy */
  if (ast3d_flags & ast3d_hierarchy) {
    for (node = ast3d_scene->keyframer; node; node = node->next) {
      if (node->type == ast3d_track_object) {
        for (child = node->child; child; child = child->brother) {
          if (child->type == ast3d_track_object) {
            obj = (c_OBJECT *)node->object;
            cobj = (c_OBJECT *)child->object;
            mat_mul (obj->matrix, cobj->matrix, cobj->matrix);
          }
          if (child->type == ast3d_track_camera) {
            obj = (c_OBJECT *)node->object;
            cam = (c_CAMERA *)child->object;
            mat_mulvec (obj->matrix, &cam->pos, &cam->pos);
          }
          if (child->type == ast3d_track_cameratgt) {
            obj = (c_OBJECT *)node->object;
            cam = (c_CAMERA *)child->object;
            mat_mulvec (obj->matrix, &cam->target, &cam->target);
          }
          if (child->type == ast3d_track_light) {
            obj = (c_OBJECT *)node->object;
            light = (c_LIGHT *)child->object;
            mat_mulvec (obj->matrix, &light->pos, &light->pos);
          }
          if (child->type == ast3d_track_spotlight) {
            obj = (c_OBJECT *)node->object;
            light = (c_LIGHT *)child->object;
            mat_mulvec (obj->matrix, &light->pos, &light->pos);
          }
          if (child->type == ast3d_track_lighttgt) {
            obj = (c_OBJECT *)node->object;
            light = (c_LIGHT *)child->object;
            mat_mulvec (obj->matrix, &light->target, &light->target);
          }
        }
      }
      if (node->type == ast3d_track_camera) {
        for (child = node->child; child; child = child->brother) {
          if (child->type == ast3d_track_light) {
            cam = (c_CAMERA *)node->object;
            light = (c_LIGHT *)child->object;
            vec_add (&cam->pos, &light->pos, &light->pos);
          }
          if (child->type == ast3d_track_spotlight) {
            cam = (c_CAMERA *)node->object;
            light = (c_LIGHT *)child->object;
            vec_add (&cam->pos, &light->pos, &light->pos);
          }
        }
      }
    }
  }

  /* update camera matrices */
  for (node = ast3d_scene->keyframer; node; node = node->next)
    if (node->type == ast3d_track_camera) {
      cam = (c_CAMERA *)node->object;
      cam_update (cam);
    }

  /* do transformation if neccesary */
  if (ast3d_flags & ast3d_transform) do_transform ();
  return ast3d_err_ok;
}


/*CLAX.C**********************************************************************

   Clax: Portable keyframing library
         see inclosed LICENSE.TXT for licensing terms.

         for documentation, refer to CLAX.TXT

   author           : Borzom
   file created     : 17/04/97
   file description : main clax routines

   revision history :
     v0.10 (17/04/97) Borzom: Initial version.

   notes            :
     the library itself, if it has bugs, its probably here :)

*****************************************************************************/

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

  char    ast3d_version[]   = "clax version 0.10(beta)";
  char    ast3d_copyright[] = "copyright (c) 1997 borzom";

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
  {"CLX", ast3d_load_mesh_CLX, ast3d_load_motion_CLX, ast3d_save_CLX}
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
  c_VECTOR  vec;
  c_MATRIX  objmat, normat;
  float     alpha;
  int32     i;

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
        for (i = 0; i < obj->numverts; i++)
          mat_mulvec (objmat, &obj->vertices[i].vert,
                              &obj->vertices[i].pvert);
        if (ast3d_flags & ast3d_calcnorm) {
          for (i = 0; i < obj->numverts; i++)
            mat_mulnorm (normat, &obj->vertices[i].norm,
                                 &obj->vertices[i].pnorm);
          for (i = 0; i < obj->numfaces; i++)
            mat_mulnorm (normat, &obj->faces[i].norm, &obj->faces[i].pnorm);
        }
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

static void print_track (t_TRACK *track, char *n, int32 type)
{
/*
  print_track: output track data to stdout.
*/
  t_KEY  *keys;
  w_NODE *node;

  if (!track) return;
#ifdef CLAX_DEBUG
  printf ("%s %3d, frames:%8.3f, flags: ", n, track->numkeys, track->frames);
  if (track->flags == ast3d_track_repeat)
    printf ("repeat\n"); else if (track->flags == ast3d_track_loop)
      printf ("loop\n"); else printf ("none\n");
  switch (type) {
    case 0: /* float */
      for (keys = track->keys; keys; keys = keys->next)
        printf ("    frame: %9.3f  || value: %9.3f\n", keys->frame, keys->val._float);
      break;
    case 1: /* vector */
      for (keys = track->keys; keys; keys = keys->next)
        printf ("    frame: %9.3f  || x: %9.3f, y: %9.3f, z: %9.3f\n",
                keys->frame,
                keys->val._vect.x, keys->val._vect.y, keys->val._vect.z);
      break;
    case 2: /* rgb color */
      for (keys = track->keys; keys; keys = keys->next)
        printf ("    frame: %9.3f  || r: %9.3f, g: %9.3f, b: %9.3f\n",
                keys->frame,
                keys->val._vect.x, keys->val._vect.y, keys->val._vect.z);
      break;
    case 3: /* quaternion */
      for (keys = track->keys; keys; keys = keys->next)
        printf ("    frame: %9.3f  || x: %9.3f, y: %9.3f, z: %9.3f, w: %9.3f\n",
                keys->frame,
                keys->val._quat.x, keys->val._quat.y, keys->val._quat.z,
                keys->val._quat.w);
      break;
    case 4: /* morph */
      for (keys = track->keys; keys; keys = keys->next) {
        ast3d_byid (keys->val._int, &node);
        printf ("    frame: %9.3f  || morph to: %s\n",
                keys->frame, ((c_OBJECT *)node->object)->name);
      }
      break;
    case 5: /* hide */
      for (keys = track->keys; keys; keys = keys->next) {
        printf ("    frame: %9.3f  || hidden: ", keys->frame);
        if (keys->val._int) printf ("yes\n"); else printf ("no\n");
      }
  }
#elif
  if (form) {} /* to skip the warning */
  if (type) {} /* to skip the warning */
#endif
}

static void print_map (c_MAP *map, char *n)
{
/*
  print_map: output map data to stdout.
*/
  printf ("  %s", n);
  printf ("    filename:  %s\n", map->file);
  printf ("    flags:     ");
  if (map->flags & ast3d_map_mirror) printf ("mirror ");
  if (map->flags & ast3d_map_negative) printf ("negative ");
  printf ("\n    1/u scale: %9.3f\n", map->U_scale);
  printf ("    1/v scale: %9.3f\n", map->V_scale);
  printf ("    u offset:  %9.3f\n", map->U_offset);
  printf ("    v offset:  %9.3f\n", map->V_offset);
  printf ("    angle:     %9.3f\n", map->rot_angle);
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
  if (ast3d_flags & ast3d_calcnorm) calc_normals ();
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
  astral 3d library (debugging)
*****************************************************************************/

void ast3d_print_world ()
{
/*
  ast3d_print_world: output world information to stdout.
*/
  w_NODE      *node;
  c_CAMERA    *cam;
  c_LIGHT     *light;
  c_OBJECT    *obj;
  c_MATERIAL  *mat;
  c_AMBIENT   *amb;
  int32        i;

  if (!ast3d_scene) {
    printf ("error: scene not loaded.\n");
    return;
  }
  if (!ast3d_scene->world) {
    printf ("error: null world tree.\n");
    return;
  }
  printf ("<<start of world tree>>\n\n");
  for (node = ast3d_scene->world; node; node = node->next) {
    switch (node->type) {
      case ast3d_obj_camera:
        cam = (c_CAMERA *)node->object;
        printf ("<<camera>> name: %s, id: %d\n", cam->name, cam->id);
        printf ("  parents:     %d, %d\n",
                cam->parent1, cam->parent2);
        printf ("  position:    x: %9.3f, y: %9.3f, z: %9.3f\n",
                cam->pos.x, cam->pos.y, cam->pos.z);
        printf ("  target:      x: %9.3f, y: %9.3f, z: %9.3f\n",
                cam->target.x, cam->target.y, cam->target.z);
        printf ("  fov:            %9.3f\n", cam->fov);
        printf ("  roll:           %9.3f\n\n", cam->roll);
        break;
      case ast3d_obj_light:
        light = (c_LIGHT *)node->object;
        if (light->flags == ast3d_light_omni) {
          printf ("<<omni light>> name: %s, id: %d\n", light->name, light->id);
          printf ("  parent:      %d\n", light->parent1);
          printf ("  position:    x: %9.3f, y: %9.3f, z: %9.3f\n",
                  light->pos.x, light->pos.y, light->pos.z);
          printf ("  color:       r: %9.3f, g: %9.3f, b: %9.3f\n\n",
                  light->color.r, light->color.g, light->color.b);
        } else {
          printf ("<<spot light>> name: %s, id: %d\n", light->name, light->id);
          printf ("  parents:     %d, %d\n", light->parent1, light->parent2);
          printf ("  position:    x: %9.3f, y: %9.3f, z: %9.3f\n",
                  light->pos.x, light->pos.y, light->pos.z);
          printf ("  target:      x: %9.3f, y: %9.3f, z: %9.3f\n",
                  light->target.x, light->target.y, light->target.z);
          printf ("  color:       r: %9.3f, g: %9.3f, b: %9.3f\n",
                  light->color.r, light->color.g, light->color.b);
          printf ("  roll:           %9.3f\n", light->roll);
          printf ("  hotspot:        %9.3f\n", light->hotspot);
          printf ("  falloff:        %9.3f\n\n", light->falloff);
        }
        break;
      case ast3d_obj_object:
        obj = (c_OBJECT *)node->object;
        printf ("<<object>> name: %s, id: %d\n", obj->name, obj->id);
        printf ("  parent:      %d\n", obj->parent);
        printf ("  vertices:    %d, faces: %d\n", obj->numverts, obj->numfaces);
        printf ("  pivot:       x: %9.3f, y: %9.3f, z: %9.3f\n",
                obj->pivot.x, obj->pivot.y, obj->pivot.z);
        printf ("  translate:   x: %9.3f, y: %9.3f, z: %9.3f\n",
                obj->translate.x, obj->translate.y, obj->translate.z);
        printf ("  scale:       x: %9.3f, y: %9.3f, z: %9.3f\n",
                obj->scale.x, obj->scale.y, obj->scale.z);
        printf ("  rotate:      x: %9.3f, y: %9.3f, z: %9.3f, w: %9.3f\n",
                obj->rotate.x, obj->rotate.y, obj->rotate.z, obj->rotate.w);
        printf ("  flags:       ");
        if (obj->flags) {
          if (obj->flags & ast3d_obj_hidden) printf ("hidden ");
          if (obj->flags & ast3d_obj_chidden) printf ("display-hidden ");
          if (obj->flags & ast3d_obj_dummy) printf ("dummy ");
          if (obj->flags & ast3d_obj_morph) printf ("morph ");
        } else printf ("none");
        printf ("\n  matrix:\n");
        printf ("    xx: %9.3f xy: %9.3f xz: %9.3f xw: %9.3f\n",
                obj->matrix[X][X], obj->matrix[X][Y],
                obj->matrix[X][Z], obj->matrix[X][W]);
        printf ("    yx: %9.3f yy: %9.3f yz: %9.3f yw: %9.3f\n",
                obj->matrix[Y][X], obj->matrix[Y][Y],
                obj->matrix[Y][Z], obj->matrix[Y][W]);
        printf ("    zx: %9.3f zy: %9.3f zz: %9.3f zw: %9.3f\n",
                obj->matrix[Z][X], obj->matrix[Z][Y],
                obj->matrix[Z][Z], obj->matrix[Z][W]);
        printf("\n");
#ifdef CLAX_DEBUG
        printf ("  vertex list:\n");
        for (i = 0; i < obj->numverts; i++) {
          printf ("    x: %9.3f, y: %9.3f, z: %9.3f, u: %4.3f, v: %4.3f\n",
                  obj->vertices[i].vert.x,
                  obj->vertices[i].vert.y,
                  obj->vertices[i].vert.z,
                  obj->vertices[i].u, obj->vertices[i].v);
        }
        printf ("  face list:\n");
        for (i = 0; i < obj->numfaces; i++) {
          printf ("    a: %3d b: %3d c: %3d, flags %3d, material: %3d\n",
                  obj->faces[i].a, obj->faces[i].b, obj->faces[i].c,
                  obj->faces[i].flags, obj->faces[i].mat);
        }
#endif
        printf ("\n");
        break;
      case ast3d_obj_material:
        mat = (c_MATERIAL *)node->object;
        printf ("<<material>> name: %s, id: %d\n", mat->name, mat->id);
        printf ("  shading:     ");
        switch (mat->shading) {
          case ast3d_mat_flat: printf ("flat"); break;
          case ast3d_mat_gouraud: printf ("gouraud"); break;
          case ast3d_mat_phong: printf ("phong"); break;
          case ast3d_mat_metal: printf ("metal");
        }
        printf ("\n  flags:       ");
        if (mat->flags) {
          if (mat->flags & ast3d_mat_twosided) printf ("twosided ");
          if (mat->flags & ast3d_mat_soften) printf ("soften ");
          if (mat->flags & ast3d_mat_wire) printf ("wireframe ");
          if (mat->flags & ast3d_mat_transadd) printf ("transparency add ");
        } else printf("none ");
        printf ("\n  ambient:            r: %9.3f, g: %9.3f, b: %9.3f\n",
                mat->ambient.r, mat->ambient.g, mat->ambient.b);
        printf ("  diffuse:            r: %9.3f, g: %9.3f, b: %9.3f\n",
                mat->diffuse.r, mat->diffuse.g, mat->diffuse.b);
        printf ("  specular:           r: %9.3f, g: %9.3f, b: %9.3f\n",
                mat->specular.r, mat->specular.g, mat->specular.b);
        printf ("  shininess:             %9.3f\n", mat->shininess);
        printf ("  shininess strength:    %9.3f\n", mat->shin_strength);
        printf ("  transparency:          %9.3f\n", mat->transparency);
        printf ("  trans. falloff:        %9.3f\n", mat->trans_falloff);
        printf ("  reflection blur:       %9.3f\n", mat->refblur);
        printf ("  self illumination:     %9.3f\n", mat->self_illum);
        print_map (&mat->texture, "texture map:\n");
        print_map (&mat->bump, "bump map:\n");
        print_map (&mat->reflection, "reflection map:\n");
        printf ("\n");
        break;
      case ast3d_obj_ambient:
        amb = (c_AMBIENT *)node->object;
        printf ("<<ambient>> name: %s, id: %d\n", amb->name, amb->id);
        printf ("  color:       r: %9.3f, g: %9.3f, b: %9.3f\n\n",
                amb->color.r, amb->color.g, amb->color.b);
    }
  }
  printf ("<<end of world tree>>\n");
}

void ast3d_print_keyframer ()
{
/*
  ast3d_print_keyframer: output keyframer information to stdout.
*/
  k_NODE      *node;
  t_CAMERA    *cam;
  t_CAMERATGT *tcam;
  t_LIGHT     *light;
  t_LIGHTTGT  *tlight;
  t_SPOTLIGHT *spotl;
  t_OBJECT    *obj;
  t_AMBIENT   *amb;

  if (!ast3d_scene) {
    printf ("error: scene not loaded.\n");
    return;
  }
  if (!ast3d_scene->keyframer) {
    printf ("error: null keyframer tree.\n");
    return;
  }
  printf ("<<start of keyframer tree>>\n\n");
  printf ("  start frame: %f\n", ast3d_scene->f_start);
  printf ("  end frame:   %f\n\n", ast3d_scene->f_end);
  for (node = ast3d_scene->keyframer; node; node = node->next)
    switch (node->type) {
      case ast3d_track_camera:
        cam = (t_CAMERA *)node->track;
        printf ("<<camera source>> id: %d, name: %s\n", node->id,
                ((c_CAMERA *)node->object)->name);
        print_track (cam->pos,  "  position keys:", 1);
        print_track (cam->roll, "  roll keys:    ", 0);
        print_track (cam->fov,  "  fov keys:     ", 0);
        printf ("\n");
        break;
      case ast3d_track_cameratgt:
        tcam = (t_CAMERATGT *)node->track;
        printf ("<<camera target>> id: %d, name: %s\n", node->id,
                ((c_CAMERA *)node->object)->name);
        print_track (tcam->pos, "  position keys:", 1);
        printf ("\n");
        break;
      case ast3d_track_light:
        light = (t_LIGHT *)node->track;
        printf ("<<light source>> id: %d, name: %s\n", node->id,
                ((c_LIGHT *)node->object)->name);
        print_track (light->pos,   "  position keys:", 1);
        print_track (light->color, "  color keys:   ", 2);
        printf ("\n");
        break;
      case ast3d_track_spotlight:
        spotl = (t_SPOTLIGHT *)node->track;
        printf ("<<spotlight source>> id: %d, name: %s\n", node->id,
                ((c_LIGHT *)node->object)->name);
        print_track (spotl->pos,   "  position keys:", 1);
        print_track (spotl->color, "  color keys:   ", 2);
        print_track (spotl->roll,  "  roll keys:    ", 0);
        printf ("\n");
        break;
      case ast3d_track_lighttgt:
        tlight = (t_LIGHTTGT *)node->track;
        printf ("<<light target>> id: %d, name: %s\n", node->id,
                ((c_LIGHT *)node->object)->name);
        print_track (tlight->pos, "  position keys:", 1);
        printf ("\n");
        break;
      case ast3d_track_object:
        obj = (t_OBJECT *)node->track;
        printf ("<<object track>> id: %d, name: %s\n", node->id,
                ((c_OBJECT *)node->object)->name);
        print_track (obj->translate, "  position keys:", 1);
        print_track (obj->rotate,    "  rotation keys:", 3);
        print_track (obj->scale,     "  scale keys:   ", 1);
        print_track (obj->morph,     "  morph keys:   ", 4);
        print_track (obj->hide,      "  hide keys:    ", 5);
        printf ("\n");
        break;
      case ast3d_track_ambient:
        amb = (t_AMBIENT *)node->track;
        printf ("<<ambient track>> id: %d, name: %s\n", node->id,
                ((c_AMBIENT *)node->object)->name);
        print_track (amb->color, "  color keys:   ", 2);
        printf ("\n");
    }
  printf ("<<end of keyframer tree>>\n");
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
  c_VECTOR *vect = (c_VECTOR *)out;

  return spline_getkey_vect (track, frame, vect);
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


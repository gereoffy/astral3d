#ifndef _AST3D_H_
#define _AST3D_H_

#define ast3d_VERSION 0.10
#ifndef M_PI
#define M_PI 3.14159265359
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define EPSILON 1.0e-6
#define X 0
#define Y 1
#define Z 2
#define W 3

/*****************************************************************************
  type definitions
*****************************************************************************/

typedef signed short int16;
typedef signed long  int32;
typedef char         cbool;

/*****************************************************************************
  error codes
*****************************************************************************/

enum ast3d_error_ { /* error codes */
  ast3d_err_ok        = 0,                /* no error                       */
  ast3d_err_nomem     = 1,                /* not enough memory              */
  ast3d_err_nofile    = 2,                /* file not found                 */
  ast3d_err_badfile   = 3,                /* corrupted file                 */
  ast3d_err_badver    = 4,                /* unsupported version            */
  ast3d_err_badformat = 5,                /* unsupported format             */
  ast3d_err_badframe  = 6,                /* illegal frame number           */
  ast3d_err_badname   = 7,                /* illegal object name            */
  ast3d_err_noframes  = 8,                /* no keyframer data              */
  ast3d_err_notloaded = 9,                /* no scene loaded                */
  ast3d_err_nullptr   = 10,               /* null pointer assignment        */
  ast3d_err_invparam  = 11,               /* invalid parameter              */
  ast3d_err_spline    = 12,               /* less than 2 keys in spline     */
  ast3d_err_singular  = 13,               /* cannot inverse singular matrix */
  ast3d_err_badid     = 14,               /* bad object id number           */
  ast3d_err_exist     = 15,               /* object already exist           */
  ast3d_err_undefined = 256               /* internal error                 */
};

/*****************************************************************************
  track/object types
*****************************************************************************/

enum ast3d_object_ { /* object types */
  ast3d_obj_camera    = 1,                /* camera (c_CAMERA)     */
  ast3d_obj_light     = 2,                /* light (c_LIGHT)       */
  ast3d_obj_object    = 4,                /* object (c_OBJECT)     */
  ast3d_obj_material  = 8,                /* material (c_MATERIAL) */
  ast3d_obj_ambient   = 16                /* ambient (c_AMBIENT)   */
};

enum ast3d_track_ { /* track types */
  ast3d_track_camera    = 1,              /* camera track        */
  ast3d_track_cameratgt = 2,              /* camera target track */
  ast3d_track_light     = 4,              /* light track         */
  ast3d_track_spotlight = 8,              /* spotlight track     */
  ast3d_track_lighttgt  = 16,             /* light target track  */
  ast3d_track_object    = 32,             /* object track        */
  ast3d_track_ambient   = 64              /* ambient track       */
};

enum ast3d_key_ { /* key types */
  ast3d_key_pos      = 1,                 /* position track */
  ast3d_key_rotate   = 2,                 /* rotation track */
  ast3d_key_scale    = 3,                 /* scale track    */
  ast3d_key_fov      = 4,                 /* fov track      */
  ast3d_key_roll     = 5,                 /* roll track     */
  ast3d_key_color    = 6,                 /* color track    */
  ast3d_key_morph    = 7,                 /* morph track    */
  ast3d_key_hide     = 8,                 /* hide track     */
};

/*****************************************************************************
  flags
*****************************************************************************/

enum ast3d_flags_ { /* astral 3d flags */
  ast3d_hierarchy  = 1,                   /* hierarchical transformations */
  ast3d_calcnorm   = 2,                   /* calculate normals            */
  ast3d_domorph    = 4,                   /* internal object morph        */
  ast3d_transform  = 8,                   /* internal transformations     */
  ast3d_slerp      = 16,                  /* quaternion spherical interp. */
  ast3d_normlerp   = 32,                  /* morph: lerp normals          */
  ast3d_normcalc   = 64                   /* morph: recalculate normals   */
};

enum ast3d_mat_flags_ { /* astral 3d material flags */
  ast3d_mat_twosided = 1,                 /* two sided           */
  ast3d_mat_soften   = 2,                 /* soften              */
  ast3d_mat_wire     = 4,                 /* wireframe rendering */
  ast3d_mat_transadd = 8                  /* transparency add    */
};

enum ast3d_map_flags_ { /* astral 3d map flags */
  ast3d_map_mirror   = 2,                 /* mirror   */
  ast3d_map_negative = 8                  /* negative */
};

enum ast3d_shade_flags_ { /* astral 3d material shading */
  ast3d_mat_flat    = 1,                  /* flat shading    */
  ast3d_mat_gouraud = 2,                  /* gouraud shading */
  ast3d_mat_phong   = 3,                  /* phong shading   */
  ast3d_mat_metal   = 4                   /* metal shading   */
};

enum ast3d_face_flags_ { /* astral 3d face flags */
  ast3d_face_wrapU   = 1,                 /* face has texture wrapping (u) */
  ast3d_face_wrapV   = 2,                 /* face has texture wrapping (v) */
  ast3d_face_visible = 4                  /* visible flag (backface cull)  */
};

enum ast3d_obj_flags_ { /* astral 3d object flags */
  ast3d_obj_hidden  = 1,                   /* object is hidden        */
  ast3d_obj_chidden = 2,                   /* object is always hidden */
  ast3d_obj_dummy   = 4,                   /* object is dummy         */
  ast3d_obj_morph   = 8                    /* object is morphing      */
};

enum ast3d_light_flags_ { /* astral 3d light flags */
  ast3d_light_omni = 1,                    /* light is omni */
  ast3d_light_spot = 2                     /* light is spotlight */
};

enum ast3d_track_flags_ { /* astral 3d track flags */
  ast3d_track_repeat = 1,                  /* track repeat */
  ast3d_track_loop   = 2                   /* track loop   */
};

/*****************************************************************************
  world structures
*****************************************************************************/

typedef float c_MATRIX[3][4];

typedef struct _c_RGB { /* color struct */
  float rgb[4];                         /* red, green, blue (0 -> 1.0) */
} c_RGB;

typedef struct _c_VECTOR { /* vector */
  float x, y, z;                         /* vector [x,y,z] */
} c_VECTOR;

typedef struct _c_QUAT { /* quaternion */
  float w, x, y, z;                      /* quaternion (w,[x,y,z]) */
} c_QUAT;

typedef struct _c_BOUNDBOX { /* bounding box */
  c_VECTOR min;                          /* bounding box */
  c_VECTOR max;                          /* bounding box */
} c_BOUNDBOX;

typedef struct _c_MORPH { /* morph struct */
  int32 from, to;                        /* morph: from/to object  */
  float alpha;                           /* morph stage (0 -> 1.0) */
} c_MORPH;

typedef struct _c_MAP { /* map struct */
  char  *file;                           /* map filename   */
  int32 flags;                           /* map flags      */
  float U_scale, V_scale;                /* 1/U, 1/V scale */
  float U_offset, V_offset;              /* U, V offset    */
  float rot_angle;                       /* rotation angle */
} c_MAP;

typedef struct _c_MATERIAL { /* material struct */
  char  *name;                           /* material name        */
  int32 id;                              /* material id          */
  int32 shading, flags;                  /* shading, flags       */
  c_RGB ambient;                         /* ambient color        */
  c_RGB diffuse;                         /* diffuse color        */
  c_RGB specular;                        /* specular color       */
  float shininess;                       /* shininess            */
  float shin_strength;                   /* snininess strength   */
  float transparency;                    /* transparency         */
  float trans_falloff;                   /* transparency falloff */
  float refblur;                         /* reflection blur      */
  float self_illum;                      /* self illuminance     */
  c_MAP texture;                         /* texture map          */
  c_MAP bump;                            /* bump map             */
  c_MAP reflection;                      /* reflection map       */
} c_MATERIAL;

typedef struct _c_VERTEX { /* vertex struct */
  c_VECTOR vert, pvert;                  /* vertex              */
  c_VECTOR norm, pnorm;                  /* vertex normal       */
  float    u, v;                         /* texture coordinates */
  int     visible;			 /* visibility		*/		
} c_VERTEX;

typedef struct _c_FACE { /* face struct */
  int32      a, b, c;                      /* vertices of triangle    */
  int32      flags;                        /* face flags: ast3d_face_* */
  int32      mat;                          /* face material           */
  c_VERTEX   *pa, *pb, *pc;                /* pointers to vertices    */
  c_MATERIAL *pmat;                        /* pointer to material     */
  c_VECTOR   norm, pnorm;                  /* face normal             */
  int	     visible;			   /* face visibility	      */	
} c_FACE;

typedef struct _c_LIGHT { /* light struct */
  char     *name;                        /* light name            */
  int32    id, parent1, parent2;         /* object id, parents    */
  int32    flags;                        /* light flags           */
  c_VECTOR pos, target;                  /* light position        */
  float    roll;                         /* roll (spotlight)      */
  float    hotspot, falloff;             /* hotspot, falloff      */
  c_RGB    color;                        /* light color           */
} c_LIGHT;

typedef struct _c_CAMERA { /* camera struct */
  char     *name;                        /* camera name                */
  int32    id, parent1, parent2;         /* object id, parent          */
  c_VECTOR pos, target;                  /* source, target vectors     */
  float    fov, roll;                    /* field of view, roll        */
  float    sizeX, sizeY;                 /* screen size (set by user)  */
  float    aspectratio;                  /* aspect ratio (set by user) */
  float    perspX, perspY;               /* perspective multipliers    */
  c_MATRIX matrix;                       /* camera matrix              */
} c_CAMERA;

typedef struct _c_OBJECT { /* object struct */
  char       *name;                      /* object name                */
  int32      id, parent;                 /* object id, object parent   */
  int32      numverts;                   /* number of vertices         */
  int32      numfaces;                   /* number of faces            */
  int32      flags;                      /* object flags: ast3d_obj_*   */
  c_VERTEX   *vertices;                  /* object vertices            */
  c_FACE     *faces;                     /* object faces               */
  c_VECTOR   pivot;                      /* object pivot point         */
  c_VECTOR   translate;                  /* object translation vector  */
  c_VECTOR   scale;                      /* object scale vector        */
  c_BOUNDBOX bbox, pbbox;                /* object bounding box        */
  c_QUAT     rotate;                     /* object rotation quaternion */
  c_MORPH    morph;                      /* object morph               */
  c_MATRIX   matrix;                     /* object keyframer matrix    */
} c_OBJECT;

typedef struct _c_AMBIENT { /* ambient struct */
  char  *name;                           /* ambient name    */
  int32 id;                              /* ambient id      */
  c_RGB color;                           /* ambient color   */
} c_AMBIENT;

typedef struct _w_NODE { /* world node */
  int32          type;                   /* object type           */
  void           *object;                /* object                */
  struct _w_NODE *next, *prev;           /* next node             */
} w_NODE;

/*****************************************************************************
  keyframer structures
*****************************************************************************/

typedef union _t_KDATA { /* key data union */
  int32    _int;                         /* boolean/object id              */
  float    _float;                       /* float                          */
  c_VECTOR _vect;                        /* vector                         */
  c_QUAT   _quat;                        /* quaternion                     */
} t_KDATA;

typedef struct _t_KEY { /* key struct */
  float          frame;                  /* current frame                  */
  float          tens, bias, cont;       /* tension, bias, continuity      */
  float          easeto, easefrom;       /* ease to, ease from             */
  t_KDATA        val;                    /* the interpolated values        */
  float          dsa, dsb, dsc, dsd, dda, ddb, ddc, ddd;
  c_QUAT         ds, dd;
  c_QUAT         qa;
  struct _t_KEY *next, *prev;
} t_KEY;

typedef struct _t_TRACK { /* track struct */
  int32  flags;                          /* track flags              */
  int32  numkeys;                        /* number of keys           */
  float  frames;                         /* number of frames         */
  t_KEY *keys;                           /* the track                */
  t_KEY *last;                           /* pointer to last used key */
} t_TRACK;

typedef struct _t_CAMERA { /* camera track */
  t_TRACK *pos;                          /* position            */
  t_TRACK *fov, *roll;                   /* field of view, roll */
} t_CAMERA;

typedef struct _t_CAMERATGT { /* camera target track */
  t_TRACK *pos;                          /* position */
} t_CAMERATGT;

typedef struct _t_LIGHT { /* light track */
  t_TRACK *pos;                          /* position */
  t_TRACK *color;                        /* color    */
} t_LIGHT;

typedef struct _t_LIGHTTGT { /* light target track */
  t_TRACK *pos;                          /* position */
} t_LIGHTTGT;

typedef struct _t_SPOTLIGHT { /* spotlight track */
  t_TRACK *pos;                          /* position */
  t_TRACK *color;                        /* color    */
  t_TRACK *roll;                         /* roll     */
  t_TRACK *hotspot;                      /* hotspot  */
  t_TRACK *falloff;                      /* falloff  */
} t_SPOTLIGHT;

typedef struct _t_OBJECT { /* object track */
  t_TRACK *translate;                    /* position */
  t_TRACK *scale;                        /* scale    */
  t_TRACK *rotate;                       /* rotation */
  t_TRACK *morph;                        /* morph    */
  t_TRACK *hide;                         /* hide     */
} t_OBJECT;

typedef struct _t_AMBIENT { /* ambient track */
  t_TRACK *color;                        /* color */
} t_AMBIENT;

typedef struct _k_NODE { /* keyframer node */
  int32          type;                   /* track type             */
  int32          id;                     /* track id               */
  void           *track, *object;        /* track / object pointer */
  struct _k_NODE *parent;                /* parent node            */
  struct _k_NODE *child, *brother;       /* hierarchy tree         */
  struct _k_NODE *next, *prev;           /* next/previous node     */
} k_NODE;

typedef struct _c_SCENE { /* scene (world, keyframer) */
  float    f_start, f_end, f_current;    /* start/end/current frame */
  w_NODE   *world, *wtail;               /* world                   */
  k_NODE   *keyframer, *ktail;           /* keyframer               */
} c_SCENE;

/*****************************************************************************
  externals
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

  extern char ast3d_version[];
  extern char ast3d_copyright[];

/*****************************************************************************
  library functions (astral 3d api)
*****************************************************************************/

  int32 ast3d_init (int32 flags);
/*
  int32 ast3d_init (int32 flags)
  initializes astral 3d, and does some internal stuff.
  returns error codes: ast3d_ok, ast3d_nomem, ast3d_undefined.

  note: this function must be called once, before accessing any other
        astral 3d routines.

  see also: ast3d_flag_*
*/

  int32 ast3d_done ();
/*
  int32 ast3d_done ()
  deinitializes astral 3d, and does some internal stuff.
  returns error codes: ast3d_ok, ast3d_undefined.

  note: this function must be called once you exit your program.
*/

  int32 ast3d_load_world (char *filename, c_SCENE *scene);
/*
  int32 ast3d_loadscene (char *filename, c_SCENE *scene)
  loads scene from supported fileformat (not keyframer).
  returns error codes: ast3d_ok, ast3d_nomem, ast3d_badfile, ast3d_undefined.
*/

  int32 ast3d_load_motion (char *filename, c_SCENE *scene);
/*
  int32 ast3d_loadmotion (char *filename, c_SCENE *scene)
  loads keyframer from supported fileformat (not scene).
  returns error codes: ast3d_ok, ast3d_nomem, ast3d_badfile, ast3d_undefined.

  note: ast3d_loadscene must be called before this.
*/

  int32 ast3d_setactive_scene (c_SCENE *scene);
/*
  int32 ast3d_setactive (c_SCENE *scene)
  sets active scene "scene".
  returns error codes: ast3d_ok, ast3d_nullptr, ast3d_undefined.

  note: this function must be called right after loading, and before
        using any other astral 3d functions.
*/

  void cam_lens_fov (float lens, float *fov);
  int32 ast3d_getactive_scene (c_SCENE **scene);
  int32 ast3d_getactive_camera (c_CAMERA **camera);
  int32 ast3d_byname (char *name, w_NODE **node);
  int32 ast3d_byid (int32 id, w_NODE **node);
  int32 ast3d_findfirst (int32 attr, w_NODE **node);
  int32 ast3d_findnext (int32 attr, w_NODE **node);
  int32 ast3d_add_world (int32 type, void *obj);
  int32 ast3d_add_track (int32 type, int32 id, int32 parent, void *track, void *obj);
  int32 ast3d_free_world (c_SCENE *scene);
  int32 ast3d_free_motion (c_SCENE *scene);
  int32 ast3d_getkey_float (t_TRACK *track, float frame, float *out);
  int32 ast3d_getkey_vect (t_TRACK *track, float frame, c_VECTOR *out);
  int32 ast3d_getkey_quat (t_TRACK *track, float frame, c_QUAT *out);
  int32 ast3d_alloc_scene (c_SCENE **scene);
  int32 spline_getkey_quat (t_TRACK *track, float frame, c_QUAT *out);
  int32 ast3d_set_track (int32 type, int32 id, t_TRACK *track);
  int32 ast3d_update ();
  int32 ast3d_free_scene (c_SCENE *scene);
  void qt_identity (c_QUAT *out);
  void qt_zero (c_QUAT *out);
  void mat_zero (c_MATRIX out);
  void vec_zero (c_VECTOR *out);
  int32 ast3d_load_scene (char *filename, c_SCENE *scene);
  void qt_scale (c_QUAT *a, float s, c_QUAT *out);
  void qt_print (c_QUAT *a);
  void vec_print (c_VECTOR *a);
  void mat_print (c_MATRIX a);
  int32 mat_invscale (c_MATRIX a, c_MATRIX out);
  void mat_mulnorm (c_MATRIX a, c_VECTOR *b, c_VECTOR *out);
  void vec_midpoint (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out);
  int32 ast3d_free_mesh (c_SCENE *scene);
  void ast3d_print_world ();
  void ast3d_print_keyframer ();
  void qt_rescale (c_QUAT *a, float s, c_QUAT *out);
  void vec_rescale (c_VECTOR *a, float s, c_VECTOR *out);
  int32 ast3d_getkey_hide (t_TRACK *track, float frame, int32 *out);
  int32 ast3d_getkey_morph (t_TRACK *track, float frame, c_MORPH *out);
  int32 ast3d_setactive_camera (c_CAMERA *cam);
  int32 ast3d_save_scene (char *filename, c_SCENE *scene);
  int32 mat_normalize (c_MATRIX a, c_MATRIX out);
  int32 qt_equal (c_QUAT *a, c_QUAT *b);
  void mat_toeuler (c_MATRIX mat, c_VECTOR *out);
  void mat_pretrans (c_VECTOR *v, c_MATRIX mat, c_MATRIX out);
  float spline_ease (float t, float a, float b);
  void mat_transpose (c_MATRIX a, c_MATRIX out);
  int32 ast3d_collide (w_NODE *a, w_NODE *b, int32 *result);
  float qt_length (c_QUAT *a);
  float qt_dot (c_QUAT *a, c_QUAT *b);
  float qt_dotunit (c_QUAT *a, c_QUAT *b);
  void qt_negate (c_QUAT *a, c_QUAT *out);
  void qt_inverse (c_QUAT *a, c_QUAT *out);
  void qt_exp (c_QUAT *a, c_QUAT *out);
  void qt_log (c_QUAT *a, c_QUAT *out);
  void qt_lndif (c_QUAT *a, c_QUAT *b, c_QUAT *out);
  void qt_slerpl (c_QUAT *a, c_QUAT *b, float spin, float alpha, c_QUAT *out);
  int32 spline_initrot (t_TRACK *track);

  char *ast3d_geterror (int32 code);
/*
  char *ast3d_geterror (int code)
  returns an error string for given error code.
*/

  int32 ast3d_getframes (float *start, float *end);
/*
  int32 ast3d_getframes (float *start, float *end)
  returns the start and end frame number in keyframer to start/end.
  returns error codes: ast3d_ok, ast3d_noframes, ast3d_notloaded, ast3d_undefined.
*/

  int32 ast3d_setframe (float frame);
/*
  int32 ast3d_setframe (float frame)
  sets current frame number in the keyframer.
  returns error codes: ast3d_ok, ast3d_badframe, ast3d_noframes, ast3d_notloaded,
                       ast3d_undefined.
*/

  int32 ast3d_getframe (float *frame);
/*
  int32 ast3d_getframe (float *frame)
  returns current frame number in the keyframer.
  returns error codes: ast3d_ok, ast3d_noframes, ast3d_notloaded, ast3d_undefined.
*/

  void cam_update (c_CAMERA *cam);
/*
  void cam_update (c_CAMERA *cam)
  creates a matrix from camera "cam", giving result in "cam->mat".
  also returns perspective multipliers in "cam->perspX", "cam->perspY".

  note: dont forget to fill c_CAMERA.sizeX, c_CAMERA.sizeY,
                            c_CAMERA.aspectratio.
*/

/*****************************************************************************
  library functions (matrix routines)
*****************************************************************************/

  void mat_identity (c_MATRIX out);
/*
  void mat_identity (c_MATRIX out)
  returns identity matrix in "out".
*/

  void mat_copy (c_MATRIX a, c_MATRIX out);
/*
  void mat_copy (c_MATRIX a, c_MATRIX out)
  copies matrix "a" to matrix "out".
*/

  void mat_add (c_MATRIX a, c_MATRIX b, c_MATRIX out);
/*
  void mat_add (c_MATRIX a, c_MATRIX b, c_MATRIX out)
  matrix addition, [out] = [a]+[b].
*/

  void mat_sub (c_MATRIX a, c_MATRIX b, c_MATRIX out);
/*
  void mat_sub (c_MATRIX a, c_MATRIX b, c_MATRIX out)
  matrix substraction, [out]=[a]-[b].
*/

  void mat_mul (c_MATRIX a, c_MATRIX b, c_MATRIX out);
/*
  void mat_mul (c_MATRIX a, c_MATRIX b, c_MATRIX out)
  matrix multiplication, [out]=[a]*[b].
*/

  int32 mat_inverse (c_MATRIX a, c_MATRIX out);
/*
  void mat_inverse (c_MATRIX a, c_MATRIX out)
  calculates inverse matrix of "a", giving result in "out".
  returns error codes: ast3d_ok, ast3d_singular, ast3d_undefined.
*/

  void mat_settrans (c_VECTOR *v, c_MATRIX out);
/*
  void mat_settrans (c_VECTOR *v, c_MATRIX out)
  creates translation matrix "out" from vector "v".
*/

  void mat_setscale (c_VECTOR *v, c_MATRIX out);
/*
  void mat_setscale (c_VECTOR *v, c_MATRIX out)
  creates a scale matrix "out" from vector "v".
*/

  void mat_rotateX (float ang, c_MATRIX out);
/*
  void mat_rotateX (float ang, c_MATRIX out)
  creates rotation matrix around X axis "ang" degrees.
*/

  void mat_rotateY (float ang, c_MATRIX out);
/*
  void mat_rotateY (float ang, c_MATRIX out)
  creates rotation matrix around Y axis "ang" degrees.
*/

  void mat_rotateZ (float ang, c_MATRIX out);
/*
  void mat_rotateZ (float ang, c_MATRIX out)
  creates rotation matrix around Z axis "ang" degrees.
*/

  void mat_mulvec (c_MATRIX a, c_VECTOR *b, c_VECTOR *out);
/*
  void mat_mulvec (c_MATRIX a, c_VECTOR *b, c_VECTOR *out)
  multiplies vector "b" by matrix "a", giving result in "out".
*/

/*****************************************************************************
  library functions (vector routines)
*****************************************************************************/

  void vec_make (float x, float y, float z, c_VECTOR *out);
/*
  void vec_make (float x, float y, float z, c_VECTOR *out)
  create vector, out = [x,y,z].
*/

  void vec_copy (c_VECTOR *a, c_VECTOR *out);
/*
  void vec_copy (c_VECTOR *a, c_VECTOR *out)
  vector copy, out = a.
*/

  void vec_add (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out);
/*
  void vec_add (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
  vector addition, out = a+b.
*/

  void vec_sub (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out);
/*
  void vec_sub (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
  vector substraction, out = a-b.
*/

  void vec_mul (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out);
/*
  void vec_mul (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
  vector multiplication, out=a*b;
*/

  void vec_scale (c_VECTOR *a, float s, c_VECTOR *out);
/*
  void vec_scale (c_VECTOR *a, float s, c_VECTOR *out)
  scales vector "a" to a new length, giving result in "out".
*/

  void vec_negate (c_VECTOR *a, c_VECTOR *out);
/*
  void vec_negate (c_VECTOR *a, c_VECTOR *out)
  negates vector "a", giving result in "out".
*/

  int32 vec_equal (c_VECTOR *a, c_VECTOR *b);
/*
  int32 vec_equal (c_VECTOR *a, c_VECTOR *b)
  compares two vector "a" and "b".
*/

  float vec_length (c_VECTOR *a);
/*
  float vec_length (c_VECTOR *a)
  computes vector length.
*/

  float vec_distance (c_VECTOR *a, c_VECTOR *b);
/*
  float vec_distance (c_VECTOR *a, c_VECTOR *b)
  computes distance between two vectors "a" and "b".
*/

  float vec_dot (c_VECTOR *a, c_VECTOR *b);
/*
  float vec_dot (c_VECTOR *a, c_VECTOR *b)
  computes dot product of two vectors "a" and "b".
*/

  void vec_cross (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out);
/*
  void vec_cross (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
  computes cross product of two vectors "a" and "b", giving
  result in "out".
*/

  void vec_lerp (c_VECTOR *a, c_VECTOR *b, float alpha, c_VECTOR *out);
/*
  void vec_lerp (c_VECTOR *a, c_VECTOR *b, float alpha, c_VECTOR *out)
  liner interpolation of vectors.
  interpolates vectors "a" and "b", with interpolation parameter
  "alpha" (range 0 - 1.0) giving result in "out".
*/

  void vec_combine (c_VECTOR *a, c_VECTOR *b, float as, float bs,
                    c_VECTOR *out);
/*
  void vec_combine (c_VECTOR *a, c_VECTOR *b, float as, float bs,
                    c_VECTOR *out)
  computes linear combination of two vectors "a" and "b", with
  scalar "as" and "bs", giving result in "out".
*/

  void vec_normalize (c_VECTOR *a, c_VECTOR *out);
/*
  void vec_normalize (c_VECTOR *a, c_VECTOR *out)
  computes normalized vector "a", giving result in "out".
*/

/*****************************************************************************
  library functions (quaternion routines)
*****************************************************************************/

  void qt_fromang (float ang, float x, float y, float z, c_QUAT *out);
/*
  void qt_fromang (float ang, float x, float y, float z, c_QUAT *out)
  computes quaternion from [angle,axis] representation, giving result
  in "out".
*/

  void qt_toang (c_QUAT *a, float *ang, float *x, float *y, float *z);
/*
  void qt_toang (c_QUAT *a, float *ang, float *x, float *y, float *z)
  converts quaternion to [angle,axis] representation, giving result
  in "out".
*/

  void qt_make (float w, float x, float y, float z, c_QUAT *out);

  void qt_copy (c_QUAT *a, c_QUAT *out);
/*
  void qt_copy (c_QUAT *a, c_QUAT *out)
  quaternion copy, out = a.
*/

  void qt_add (c_QUAT *a, c_QUAT *b, c_QUAT *out);
/*
  void qt_add (c_QUAT *a, c_QUAT *b, c_QUAT *out)
  quaternion addition, out = a+b.
*/

  void qt_sub (c_QUAT *a, c_QUAT *b, c_QUAT *out);
/*
  void qt_sub (c_QUAT *a, c_QUAT *b, c_QUAT *out)
  quaternion substraction, out = a-b.
*/

  void qt_mul (c_QUAT *a, c_QUAT *b, c_QUAT *out);
/*
  void qt_mul (c_QUAT a, c_QUAT b, c_QUAT *out)
  multiplies quaternion "a" by quaternion "b", giving result in "out".
*/

  void qt_div (c_QUAT *a, c_QUAT *b, c_QUAT *out);
/*
  void qt_div (c_QUAT *a, c_QUAT *b, c_QUAT *out)
  quaternion division, out = a/b.
*/

  void qt_square (c_QUAT *a, c_QUAT *out);
/*
  void qt_square (c_QUAT *a, c_QUAT *out)
  quaternion square, out = a^2.
*/

  void qt_sqrt (c_QUAT *a, c_QUAT *out);
/*
  void qt_sqrt (c_QUAT *a, c_QUAT *out)
  quaternion square root, out = sqrt (a).
*/

  void qt_normalize (c_QUAT *a, c_QUAT *out);
/*
  void qt_normalize (c_QUAT *a, c_QUAT *out)
  normalizes quaternion "a", giving result in "out".
*/

  void qt_negate (c_QUAT *a, c_QUAT *out);
/*
  void qt_negate (c_QUAT *a, c_QUAT *out)
  forms multiplicative inverse of quaternion "a", giving result in "out".
*/

void qt_slerp (c_QUAT *a, c_QUAT *b, float spin, float alpha, c_QUAT *out);
/*
  void qt_slerp (c_QUAT *a, c_QUAT *b, float alpha, c_QUAT *out)
  spherical liner interpolation of quaternions.
  interpolates quaterions "a" and "b", with interpolation parameter
  "alpha" (range 0 - 1.0) giving result in "out".
*/

  void qt_matrix (c_QUAT *a, c_MATRIX mat);
/*
  void qt_matrix (c_QUAT *a, c_MATRIX mat)
  creates a matrix from quaternion "a", giving result in "mat".
*/

  void qt_invmatrix (c_QUAT *a, c_MATRIX mat);
/*
  void qt_invmatrix (c_QUAT *a, c_MATRIX mat)
  creates an inverse matrix from quaternion "q", giving result in "mat".
*/

  void qt_frommat (c_MATRIX mat, c_QUAT *out);
/*
  void qt_frommat (c_MATRIX mat, c_QUAT *out)
  converts a rotation matrix "mat" to quaternion, giving result in "out".
*/

/*****************************************************************************
  library functions (spline routines)
*****************************************************************************/

  int32 spline_init (t_TRACK *track);
/*
  int32 spline_init (t_TRACK *track)
  initializes spline interpolation, with an array of keys in "keys", given
  number of keys in "numkeys".
  returns error codes: ast3d_ok, ast3d_spline, ast3d_undefined.

  see also: t_KEY structure
*/

  int32 spline_getkey_float (t_TRACK *track, float frame, float *out);
/*
  int32 spline_getkey_float (t_KEY *keys, float frame, float *out)
  returns one-dimensional interpolated value for "frame", giving
  result in "out".
  returns error codes: ast3d_ok, ast3d_badframe, ast3d_undefined.

  the value interpolated is t_KEY.a
*/

  int32 spline_getkey_vect (t_TRACK *track, float frame, c_VECTOR *out);
/*
  int32 spline_getkey_vect (t_KEY *keys, float frame, float *out)
  returns three-dimensional interpolated value for "frame", giving
  result in "out".
  returns error codes: ast3d_ok, ast3d_badframe, ast3d_undefined.

  the values interpolated are t_KEY.a, t_KEY.b, t_KEY.c
*/

#ifdef __cplusplus
}
#endif
#endif

/* eof */
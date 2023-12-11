#ifndef _AST3D_H_
#define _AST3D_H_

/* ---------------- Configuration ------------------ */

/* Enable frustum culling for faces */
#define FRUSTUM_CULL

/* Enable spline object (mesh) morph */
#define SPLINE_MORPH

/* Enable removing duplicated vertices */
#define OTIMIZE_VERTEX

/* Sort faces for triangle strip. Not works yet... :( */
// #define TRIANGLE_STRIP

/* Support for generating lightmap coordinates */
#define LIGHTMAP_UV

/* Minimum face number for pre-trans backface culling */
// #define PRETRANS_CULL_FACELIMIT 2

/* ------------------------------------------------- */

#define EPSILON 1.0e-6
#define X 0
#define Y 1
#define Z 2
#define W 3

/*****************************************************************************
  type definitions
*****************************************************************************/

typedef signed short int16;
typedef signed int   int32;
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
  ast3d_obj_ambient   = 16,               /* ambient (c_AMBIENT)   */
  ast3d_obj_fog       = 32                /* fog     (c_AMBIENT)   */
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
  ast3d_key_hide     = 8                  /* hide track     */
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

enum ast3d_fog_type_ {
  ast3d_fog_none    = 0,
  ast3d_fog_fog     = 1,
  ast3d_fog_distcue = 2,
  ast3d_fog_layered = 4
};

enum ast3d_background_type_ {
  ast3d_backgr_none = 0,
  ast3d_backgr_solidcolor = 1,
  ast3d_backgr_gradient = 2,
  ast3d_backgr_bitmap = 4
};

enum ast3d_mat_flags_ { /* astral 3d material flags */
  ast3d_mat_twosided = 1,                 /* two sided           */
  ast3d_mat_soften   = 2,                 /* soften              */
  ast3d_mat_wire     = 4,                 /* wireframe rendering */
  ast3d_mat_transadd = 8,                 /* additive texture    */
  ast3d_mat_texture  = 16,                /* obj has texture     */
  ast3d_mat_texturealpha = 32,            /* texture has alpha   */
  ast3d_mat_reflect  = 64,                /* obj has reflect.map */
  ast3d_mat_reflectalpha = 128,           /* reflect has alpha   */
  ast3d_mat_transparent  = 256,           /* transparent object  */
  ast3d_mat_specularmap  = 512,           /* specular phongmap   */
  ast3d_mat_lightmap  = 1024,             /* object has lightmap */
  ast3d_mat_bump      = 2048,             /* object has bump map */
  ast3d_mat_env_positional = 4096,        /* envmap is positional */
  ast3d_mat_env_sphere = 8192,            /* envmap uses sphere mapping */
  ast3d_mat_reflect_light = 16384,        /* lightning on reflection map */
  ast3d_mat_projected_map = 32768         /* projected "water" map */
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
  ast3d_face_visible = 4,                 /* visible flag (backface cull)  */
  ast3d_face_trianglestrip = 8            /* triangle-strip                */
};

enum ast3d_obj_flags_ { /* astral 3d object flags */
  ast3d_obj_hidden  = 1,                   /* object is hidden        */
  ast3d_obj_chidden = 2,                   /* object is always hidden */
  ast3d_obj_dummy   = 4,                   /* object is dummy         */
  ast3d_obj_morph   = 8,                   /* object is morphing      */
  ast3d_obj_displaylist = 16,              /* object has display list */
  ast3d_obj_inside  = 32,                  /* we are in the object    */
  ast3d_obj_visible = 64,                  /* object is in frustum    */
  ast3d_obj_allvisible = 128,              /* all faces&vertices visible */
  ast3d_obj_trianglestrip = 256,           /* triangle-stip capable   */
  ast3d_obj_frustumcull = 512,             /* frustum cull faces      */
  ast3d_obj_particle = 1024,               /* particle system */
  ast3d_obj_lmapmake = 2048,               /* force lightmap generation */
  ast3d_obj_smoothing = 4096               /* skip smoothing group */
};

enum ast3d_light_flags_ { /* astral 3d light flags */
  ast3d_light_omni = 1,                    /* light is omni */
  ast3d_light_spot = 2,                    /* light is spotlight */
  ast3d_light_attenuation = 4              /* attenuation */
//  ast3d_light_corona_only = 8              /* fake light */
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
/*  float r, g, b;  */                    /* red, green, blue (0 -> 1.0) */
  float rgb[4];
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
  c_VECTOR p[8];
} c_BOUNDBOX;

typedef struct _c_MAP { /* map struct */
  char  *file;                           /* map filename   */
  int32 flags;                           /* map flags      */
  float U_scale, V_scale;                /* 1/U, 1/V scale */
  float U_offset, V_offset;              /* U, V offset    */
  float rot_angle;                       /* rotation angle */
  float amountof;
} c_MAP;

typedef struct {
    int nummaps;
    void *maps;
    float animphase;
    float uoffs;
    float voffs;
    float scale;
    float amount;
} c_projmap;

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
  c_MAP texture2;                        /* texture map          */
  c_MAP bump;                            /* bump map             */
  c_MAP opacitymap;                      /* opacity map          */
  c_MAP specularmap;                     /* specular map         */
  c_MAP shinenessmap;                    /* shininess map        */
  c_MAP selfillmap;                      /* selfillmap map       */
  c_MAP reflection;                      /* reflection map       */

  c_MAP texture_mask;                    /* texture mask         */
  c_MAP texture2_mask;                   /* texture mask         */
  c_MAP bump_mask;                       /* bump mask            */
  c_MAP opacitymap_mask;                 /* opacity mask         */
  c_MAP specularmap_mask;                /* specular mask        */
  c_MAP shinenessmap_mask;               /* shininess mask       */
  c_MAP selfillmap_mask;                 /* selfill mask         */
  c_MAP reflection_mask;                 /* reflection mask      */

  unsigned int texture_id;
  unsigned int reflection_id;
  unsigned int specularmap_id;
  unsigned int lightmap_id;
  unsigned int bumpmap_id;

  c_projmap projmap;

} c_MATERIAL;

typedef struct _c_VERTEX { /* vertex struct */
//  int      visible;
  unsigned char rgb[4];
  unsigned char refl_rgb[4];
  c_VECTOR vert, pvert;                  /* vertex              */
  c_VECTOR norm, pnorm;                  /* vertex normal       */
  c_VECTOR specular;                     /* envmap/lightmap u,v */
  float    bump_du, bump_dv;             /* bump texture coordinate offsets */
  c_VECTOR u_grad,v_grad;                /* u,v gradient vectors */
  float    u, v;                         /* texture coordinates */
  float    env_u,env_v;                  /* environment mapping coords */
  float    weight;
#ifdef OTIMIZE_VERTEX
  int opt_tmp;                           /* temp var. for optimvertex */
#endif
} c_VERTEX;

typedef struct _c_FACE { /* face struct */
  int32      a, b, c;                      /* vertices of triangle    */
  int32      flags;                        /* face flags: ast3d_face_* */
//  int32      mat;                          /* face material           */
  c_VERTEX   *pa, *pb, *pc;                /* pointers to vertices    */
//  c_MATERIAL *pmat;                        /* pointer to material     */
  c_VECTOR   norm;                         /* face normal             */
  c_VECTOR   pnorm;                        /* face normal             */
  float      D;                            /* distance from 0;0;0     */
  float      A;                            /* terulet/felulet         */
//  int        visible;                      /* visibility flag         */
  float u1,v1,u2,v2,u3,v3;                 /* texture coords          */
  float lu1,lv1,lu2,lv2,lu3,lv3;           /* lightmap coords         */
#ifdef TRIANGLE_STRIP
  struct _c_FACE *prev;
  struct _c_FACE *next;
#else
#ifdef LIGHTMAP_UV
  struct _c_FACE *next;
#endif
#endif
} c_FACE;

typedef struct _c_LIGHT { /* light struct */
  char     *name;                        /* light name            */
  int32    id, parent1, parent2;         /* object id, parents    */
  int32    flags;                        /* light flags           */
  c_VECTOR pos, target;                  /* light position        */
  c_VECTOR ppos, pdir;                   /* transformed light position */
  float    roll;                         /* roll (spotlight)      */
  float    hotspot, falloff;             /* hotspot, falloff      */
  c_RGB    color;                        /* light color           */
// script parameters:
  float    corona_scale;
  float    attenuation[3];
  int use_zbuffer;
  int enabled;  // default=1. if 0, no lighting calculation, only corona sprite
// calculated:
  float MatAmb[3],MatDiff[3],MatSpec[3];
  float CosFalloff,CosHotspot,SpotExp;
  int ambient;
  float inner_range,outer_range;
  int attenuate;
  int lightmap_calc_normal;
  int laser;
//  float specular_limit,specular_coef,specular_mult;
} c_LIGHT;

typedef struct _c_FOG { /* fog struct */
  int type;
  float znear;
  float fognear;
  float zfar;
  float fogfar;
  float fog_znear,fog_zfar;  // calced
  c_RGB color;
} c_FOG;

typedef struct _c_BACKGR { /* background struct */
  int type;
  c_RGB color;
} c_BACKGR;

typedef struct _c_FRUSTUM {
  float znear,zfar,x,y;
} c_FRUSTUM;

typedef struct _c_CAMERA { /* camera struct */
  char     *name;                        /* camera name                */
  int32    id, parent1, parent2;         /* object id, parent          */
  c_VECTOR pos, target;                  /* source, target vectors     */
  float    fov, roll;                    /* field of view, roll        */
//  float    sizeX, sizeY;                 /* screen size (set by user)  */
  float    aspectratio;                  /* aspect ratio (set by user) */
//  float    perspX, perspY;               /* perspective multipliers    */
  c_MATRIX matrix;                       /* camera matrix              */
  c_FRUSTUM frustum;                     /* frustum in camera space    */
} c_CAMERA;

typedef struct _c_MAPPING {
  int32    type;                         /* 0=planar 1=cylin. 2=sphere */
  float    utile,vtile;                  /* U and V tile               */
  c_VECTOR pos;                          /* X,Y,Z position             */
  c_MATRIX matrix;                       /* matrix                     */
} c_MAPPING;


typedef struct _c_AMBIENT { /* ambient struct */
  char  *name;                           /* ambient name    */
  int32 id;                              /* ambient id      */
  c_RGB color;                           /* ambient color   */
} c_AMBIENT;

typedef struct _c_PART {
  float color[3];
  float energy;
  c_VECTOR p;
  float v[3];
} c_PART;

typedef struct _c_PARTICLE {
  c_PART *p; // =NULL
  int np,maxnp;  // =0
  int texture_id;
  float eject_r,eject_vy,eject_vl,ridtri;
  float energy; // 0.8 energy scale
  int sizelimit; // 0  maximum sprite size
  float dieratio; // 0.0018  energy--
  float agrav; // -9.8  gravitation
  float colordecrement; // 0.999
  int type; // hogyan mozog  0=regi(mesa)  1=uj(hjb)
  float max_y;
} c_PARTICLE;

typedef struct _c_OBJECT { /* object struct */
  char       *name;                      /* object name                */
  int32      id, parent;                 /* object id, object parent   */
  int32      numverts;                   /* number of vertices         */
  int32      numfaces;                   /* number of faces            */
  int32      flags;                      /* object flags: ast3d_obj_*  */
  c_VERTEX   *vertices;                  /* object vertices            */
  c_FACE     *faces;                     /* object faces               */
  unsigned int *smoothing;               /* smoothing groups */
  char       *vert_visible;              /* vertex visibility */
  char       *face_visible;              /* face visibility */
  c_MATERIAL *pmat;                      /* pointer to material        */
  c_VECTOR   pivot;                      /* object pivot point         */
  c_VECTOR   translate;                  /* object translation vector  */
  c_VECTOR   scale;                      /* object scale vector        */
  c_BOUNDBOX bbox, pbbox;                /* object bounding box        */
  c_QUAT     rotate;                     /* object rotation quaternion */
  void       *morph;                     /* object morph  (t_TRACK *)  */
  c_MATRIX   matrix;                     /* object keyframer matrix    */
  c_MAPPING  mapping;                    /* texture mapping info       */
  float      bumpdepth;
  c_PARTICLE particle;
  int        enable_zbuffer;         /* 1=use zbuffer 2=readonly */
  float      vertexlights;           /* 0=disable   other=vertex light scale */
  float      explode_speed;
  float      explode_frame;
  int lm_xs,lm_ys;                  /* lightmap size */
  float A;                          /* teljes felulet */
  int lightmap_id;
  int lightmap_xsize,lightmap_ysize;
  float hair_len;
  int receive_laser;
  float laser_transparency,laser_reflection;
} c_OBJECT;

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
  c_OBJECT* _obj;                        /* morph object                   */
} t_KDATA;

typedef struct _t_KEY { /* key struct */
  float          frame;                  /* current frame                  */
  float          tens, bias, cont;       /* tension, bias, continuity      */
  float          easeto, easefrom;       /* ease to, ease from             */
  t_KDATA        val;                    /* the interpolated values        */
  float          dsa, dsb, dsc, dsd, dda, ddb, ddc, ddd;
#ifdef SPLINE_MORPH
  float          ksm, ksp, kdm, kdp;     /* need for calc dsa,dda,dsb...   */
  int            spline_type;          /* 0=linear 1=first 2=last 3=normal */
#endif
  c_QUAT         ds, dd;
  c_QUAT         qa;
  struct _t_KEY *next, *prev;            /* elozo/kovetkezo key vagy NULL */
  struct _t_KEY *loop_next, *loop_prev;  /* loop eseten mindig mutat valahova */
} t_KEY;

typedef struct _t_TRACK { /* track struct */
  int32  flags;                          /* track flags              */
  int32  numkeys;                        /* number of keys           */
  float  frames;                         /* frame of last key        */
  float  total_frames;                   /* total number of frames   */
  float  alpha;
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
  c_FOG    fog;
  c_BACKGR backgr;
//  c_FRUSTUM frustum;
  c_CAMERA *cam;
  float lightcorona_scale;
  int directional_lighting;
//  int sphere_map;
  float znear,zfar;
  int frustum_cull;
} c_SCENE;

/*****************************************************************************
  externals
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern c_MATERIAL *Default_MATERIAL;

extern int default_mat_flags;
extern int default_obj_flags;
extern int default_track_flags;
extern float default_corona_scale;

/*****************************************************************************
  library functions (astral 3d api)
*****************************************************************************/

void ast3d_fixUV(char *objname,int uvflag);
/* ast3d_fixUV:  Build bugfixed texture U,V table for each face */

  c_MATERIAL* create_Default_MATERIAL ();

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

  float cam_lens_fov (float lens);
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
  int32 ast3d_set_track (int32 type, int32 id, t_TRACK *track);
  int32 ast3d_update ();
  int32 ast3d_free_scene (c_SCENE *scene);
  void qt_identity (c_QUAT *out);
  void qt_zero (c_QUAT *out);
  int32 ast3d_load_scene (char *filename, c_SCENE *scene);
  void qt_scale (c_QUAT *a, float s, c_QUAT *out);
  void qt_print (c_QUAT *a);
  int32 ast3d_free_mesh (c_SCENE *scene);
  void ast3d_print_world ();
  void ast3d_print_keyframer ();
  void qt_rescale (c_QUAT *a, float s, c_QUAT *out);
  int32 ast3d_getkey_hide (t_TRACK *track, float frame, int32 *out);
  int32 ast3d_getkey_morph (t_TRACK *track, float frame, void **out);
//  int32 ast3d_getkey_morph (t_TRACK *track, float frame, c_MORPH *out);
  int32 ast3d_setactive_camera (c_CAMERA *cam);
  int32 ast3d_save_scene (char *filename, c_SCENE *scene);
  int32 qt_equal (c_QUAT *a, c_QUAT *b);
  float spline_ease (float t, float a, float b);
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

  void qt_make_objmat(c_OBJECT *obj);

  void qt_frommat (c_MATRIX mat, c_QUAT *out);
/*
  void qt_frommat (c_MATRIX mat, c_QUAT *out)
  converts a rotation matrix "mat" to quaternion, giving result in "out".
*/

void make_lightmap_uv(c_OBJECT *obj,int xsize,int ysize);

extern float vtxoptim_threshold;
extern int vtxoptim_uvcheck;


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

  int32 spline_getkey_quat (t_KEY *keys,float t, c_QUAT *out);
  int32 spline_getkey_float (t_KEY *keys,float t, float *out);
  int32 spline_getkey_vect (t_KEY *keys,float t, c_VECTOR *out);


#ifdef __cplusplus
}
#endif
#endif

/* eof */

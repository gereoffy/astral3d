
/* Uses own lighting if defined. if commented out then OpenGL lighting is used */
#define NO_LIGHTING

/*  */
//#define VERTEX_ARRAY

/* Maximum number of faces in _transparent_ objects */
#define MAXFACES 16384

#define MAX_CAMNO 16
#define MAX_LIGHTNO 32

extern c_SCENE *scene;
extern float ast3d_blend;  /* GLOBAL Blend value */

extern c_CAMERA *cameras[MAX_CAMNO];
extern int camno;

extern c_LIGHT *lights[MAX_LIGHTNO];
extern c_AMBIENT *ambient;
extern int lightno;

extern int window_w,window_h;
extern unsigned int lightmap;

extern void resize(int w,int h);
extern void ast3d_Perspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

extern void draw3dsframe(void);
extern int FindCameras(c_SCENE *scene);
extern void LoadMaterials(c_SCENE *scene);

extern float bump_du;
extern float bump_dv;

// extern void ast3d_fixUV(char *objname,int uvflag);

extern void particle_init(c_OBJECT *obj,int texture,int np);
extern void particle_redraw(c_OBJECT *obj,c_MATRIX objmat, float dt);
extern void particle_preplay(c_OBJECT *obj,float dt,int times);


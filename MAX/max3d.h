
#include "types.h"

#include "mesh.h"
#include "track.h"

#define PRINT_CHUNKS
#define MAX_CLASS 512
#define MAX_NODES 2048

//======================== SYSTEM ===============================

typedef struct node_st_ {
  unsigned short state;  // 0=unknown  1=updated  2=changed
  unsigned short flags;
  unsigned short classid;
  unsigned short refdb;
  int* reflist;
  char* (*update)(struct node_st_ *node);
  void* data;
  struct node_st_ *next;
} node_st;

#define STATE_UNKNOWN 0
#define STATE_UPDATED 1
#define STATE_CHANGED 2

typedef struct chunkhelp_st_ {
  struct chunkhelp_st_ *next;
  char *name;
  int id;
} chunkhelp_st;

typedef struct {
  char* refs[32];
  char** params[32];
  chunkhelp_st *chunks;
} chelp_st;

typedef struct {
  char* name;
  char* data;
// Class reader registration:
  short type;    // class type
  short subtype; // class subtype
  void (*class_init)(node_st *node);
  int (*class_chunk_reader)(node_st *node,FILE *f,int level,int chunk_id,int chunk_size);
  void (*class_uninit)(node_st *node);
// Help
  chelp_st* chelp;
} class_st;

typedef struct {
  int pdb;
  int* type;
  void** data;
} Class_ParamBlock;

//======================== ParamBlock2 ===============================

typedef struct {
  int xsize,ysize;
  char* file;
  char* type;
} PB2_bitmap;

typedef struct {
  byte status;
  union {
    int i;
    float f;
    void* p;
  } data;
  char* name; // optional
} PB2_arrayelement;

typedef struct {
  int type;
  int flags;
  byte status;
  union {
    int i;
    float f;
    void* p;
    PB2_arrayelement* array;
    PB2_bitmap* bm;
  } data;
  union {
    char *name;
    int arraysize;
  } u;
} PB2_param;

typedef struct {
  // Chunk 0009:
  int id;
  short sub_id;
  int pdb;
  int parent_node;
  PB2_param *params;
} Class_ParamBlock2;


//======================== ORIENTATION ===============================

typedef struct {
  Matrix mat;
} Orientation;

typedef struct {
  Quat quat;
  float *x;
  float *y;
  float *z;
} Class_EulerXYZ;

typedef struct {
  Point3 vect;
  float *x;
  float *y;
  float *z;
} Class_VectorXYZ;

typedef struct {
  Matrix mat;
  Point3 *pos;      // P0
  Quat *rot;        // P1
  Point3 *scale;    // P2
  Quat *scaleaxis;
  Class_EulerXYZ *euler;
} Class_PRS;

typedef struct {
  Matrix mat;
  Matrix *to; // pointer to P0.Node's mat
  Point3 *from;     // P1
  float *roll;      // P2
  Point3 *scale;    // P3
  Quat *scaleaxis;
  int axis; // +0x100=flip  0=X 1=Y 2=Z
} Class_LookAt;

//======================== MESH ===============================

typedef struct {
  Mesh mesh;
} Class_EditableMesh;

typedef struct {
  Mesh mesh;
  float* radius;
  float* radius2;
  float* Rotation;
  float* Twist;
  int* segs;
  int* sides;
  int* smooth;
  float* P;
  float* Q;
  float* E;
  float* Lumps;
  float* LumpHeight;
  int* BaseCurve;
  int* genUV;
  float* uTile;
  float* vTile;
  float* uOff;
  float* vOff;
  float* WarpHeight;
  float* WarpFreq;
} Class_TorusKnot;

//============================= Modifiers ===================================

typedef struct {
  int doBias; // flag
  float bias;


} Class_TwistDeformer;

//================================== Node ===================================

typedef struct {
  char *name;
  int parent;          // erre is kene dependelni!
  unsigned int flags;
  unsigned char wirecolor[4];
  TMatrix tm; // pivot
  // Calculated:
  Matrix tm_mat;  // mat_from_tm(tm)
  Matrix *orient_mat;  // PRS / LookAt mat-janak cime
  Matrix *parent_mat;  // NULL vagy a parent mat-janak cime
  Matrix mat; // calculated transformation matrix (tm*PRS*hierarchy)
  Matrix objmat; // mat*tm
  Mesh *mesh; // points to object mesh (editable/object/derived)
//  Matrix invmat; // inverse mat
} Class_Node;

typedef struct {
  node_st *Tracks;
  node_st *ParamBlocks;
  node_st *Shapes;
  node_st *Objects;
  node_st *Modifiers;
  node_st *ModifiedObjs;
  node_st *Nodes;
  int fps;
  int start_frame,end_frame,current_frame;
} Scene;

#define CLASSTYPE_ERROR 0
#define CLASSTYPE_TRACK 1
#define CLASSTYPE_PARAMBLOCK 2
#define CLASSTYPE_ORIENTATION 3

#define CLASSTYPE_SHAPE 4
#define CLASSTYPE_OBJECT 5
#define CLASSTYPE_MODIFIER 6
#define CLASSTYPE_MODOBJ 7
#define CLASSTYPE_MESH 8

#define CLASSTYPE_NODE 9
#define CLASSTYPE_SCENE 10

#define CLASSTYPE_MAP 20
#define CLASSTYPE_MATERIAL 21

#define CLASSTYPE_EULER_XYZ 100
#define CLASSTYPE_VECTOR_XYZ 101



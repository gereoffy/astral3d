
#define PRINT_CHUNKS
#define MAX_CLASS 512
#define MAX_NODES 2048

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

typedef struct { float x,y,z; } Point3;

typedef struct { float x,y,z,w;} Quat;

typedef struct { 
  Point3 amount;
  Quat axis;
} Scale;

typedef float Matrix[3][4];
#define X 0
#define Y 1
#define Z 2
#define W 3

typedef struct {
  Point3 pos;
  Quat rot;
  Scale scale;
} TMatrix;


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


typedef struct {
  Point3 p;
  Point3 n;
} Vertex;

typedef struct {
  int verts[3];         // vertex IDs
  unsigned long sg;     // smoothing group
  unsigned short flags; // flags
  unsigned short mtl;   // submaterial ID
} Face;

typedef struct {
  float u,v,w;
} TVertex;

typedef struct {
  int verts[3];         // tvertex IDs
} TFace;

typedef struct {
  int numverts;
  int numfaces;
  int numtfaces;
  int numtverts;
  Vertex *vertices;
  Face *faces;
  TVertex *tvertices;
  TFace *tfaces;
} Class_EditableMesh;

typedef struct {
  Class_EditableMesh mesh;
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



// Keyframer:

typedef struct {
  float tens,cont,bias,easefrom,easeto;
} TCBparams;

typedef struct {
  int flags;   // track flags
  Point3 val_vect; // float (x) or vect/color (xyz) value
  Quat val_quat;   // quaternion (xyzw) value
  int val_int;     // integer value
  int numkeys; // number of keys
  int frame_from,frame_to; // frame of first&last keys
  int keytype; // chunk id
  int* frames;
  int* keyflags;
  void* keys;  // key data
// Calced:
  int current_key;
  float current_alpha;
} Track;

//====================== Float Key ====================

typedef struct {
  float value;
} Float_Key;

// 2511
typedef struct {
  float value;
} Linear_Float_Key;

// 2516
typedef struct {
  float value;
  float in_tan,out_tan;
} Bezier_Float_Key;

// 2520
typedef struct {
  float value;
  TCBparams tcb;
  float deriv1,deriv2;
} TCB_Float_Key;

//====================== Pos Key ====================

typedef struct {
  Point3 value;
} Pos_Key;

// 2513
typedef struct {
  Point3 value;
} Linear_Pos_Key;

// 2524
typedef struct {
  Point3 value;
  Point3 in_tan,out_tan;
  int tmp1,tmp2,tmp3;      // 0     ????????
} Bezier_Pos_Key;

// 2521
typedef struct {
  Point3 value;
  TCBparams tcb;
  Point3 deriv1,deriv2;
} TCB_Pos_Key;

//====================== Rot Key ====================

typedef struct {
  Quat value;
} Rot_Key;

// 2514
typedef struct {
  Quat value;
} Linear_Rot_Key;

// 2518
typedef struct {
  Quat value;
  Quat in_tan,out_tan;
} Smooth_Rot_Key;   // Bezier???

// 2522
typedef struct {
  Quat value; //int y1,y2,y3,y4; // ? quaternion
  TCBparams tcb;
  Point3 axis;
  float angle;
  Quat deriv1,deriv2;
} TCB_Rot_Key;

//====================== Scale Key ====================

typedef struct {
  Scale value;
} Scale_Key;

// 2515
typedef struct {
  Scale value;
} Linear_Scale_Key;

// 2519
typedef struct {
  Scale value;
  Scale in_tan,out_tan;
} Bezier_Scale_Key;

// 2523
typedef struct {
  Scale value;
  TCBparams tcb;
  Scale deriv1,deriv2;
} TCB_Scale_Key;

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
  Class_EditableMesh *mesh; // points to object mesh (editable/object/derived)
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



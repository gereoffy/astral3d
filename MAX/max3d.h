
#define PRINT_CHUNKS
#define MAX_CLASS 512
#define MAX_NODES 2048

typedef struct node_st_ {
  unsigned short state;  // 0=unknown  1=updated  2=changed
  unsigned short flags;
  unsigned short classid;
  unsigned short refdb;
  int* reflist;
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

typedef struct { float w,x,y,z;} Quat;

typedef float Matrix[3][4];
#define X 0
#define Y 1
#define Z 2
#define W 3

typedef struct {
  Point3 pos;
  Quat rot;
  Point3 scale;
  Quat scaleaxis;
} TMatrix;

typedef struct {
  char *name;
  int parent;
  unsigned int flags;
  Point3 pos;
  TMatrix tm;
  Matrix mat; // calculated transformation matrix (tm*PRS*hierarchy)
  unsigned char wirecolor[4];
} Class_Node;

typedef struct {
  TMatrix tm;
  Matrix mat;
} Class_PRS;

typedef struct {
  Point3 from;
  Point3 to;
  float roll;
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

// Keyframer:

typedef struct {
  float tens,cont,bias,easefrom,easeto;
} TCBparams;

typedef struct {
  int flags;   // track flags
  Point3 val_vect; // float (x) or vect/color (xyz) value
  Quat val_quat;   // quaternion (xyzw) value
  int numkeys; // number of keys
  int frame_from,frame_to; // frame of first&last keys
  int keytype; // chunk id
  void* keys;  // key data
} Track;

//====================== Float Key ====================

typedef struct {
  int frame,flags;
  float value;
} Float_Key;

typedef struct {
  int frame,flags;
  float value;
} Linear_Float_Key;

typedef struct {
  int frame,flags;
  float value;
  float in_tan,out_tan;
} Bezier_Float_Key;

typedef struct {
  int frame,flags;
  float value;
  TCBparams tcb;
  float deriv1,deriv2;
} TCB_Float_Key;

//====================== Pos Key ====================

typedef struct {
  int frame,flags;
  Point3 value;
} Pos_Key;

typedef struct {
  int frame,flags;
  Point3 value;
  TCBparams tcb;
  Point3 deriv1,deriv2;
} TCB_Pos_Key;

typedef struct {
  int frame,flags;
  Point3 value;
  Point3 in_tan,out_tan;
  int tmp1,tmp2,tmp3;      // 0     ????????
} Bezier_Pos_Key;

//====================== Rot Key ====================

typedef struct {
  int frame,flags;
  Quat quatvalue; //int y1,y2,y3,y4; // ? quaternion
  TCBparams tcb;
  Quat value;
  Quat deriv1,deriv2;
} TCB_Rot_Key;

//====================== Scale Key ====================

typedef struct {
  int frame,flags;
  Point3 value;
  Quat axisvalue;
} Scale_Key;

typedef struct {
  int frame,flags;
  Point3 value;
  Quat axisvalue;
  TCBparams tcb;
  Point3 d1_value;
  Quat d1_axis;
  Point3 d2_value;
  Quat d2_axis;
} TCB_Scale_Key;

typedef struct {
  int frame,flags;
  Point3 value;
  Quat axisvalue;
  Point3 it_value;
  Quat it_axis;
  Point3 ot_value;
  Quat ot_axis;
} Bezier_Scale_Key;


typedef struct {
  node_st *Tracks;
  node_st *ParamBlocks;
  node_st *Shapes;
  node_st *Objects;
  node_st *Modifiers;
  node_st *ModifiedObjs;
  node_st *Nodes;
} Scene;

#define CLASSTYPE_ERROR 0
#define CLASSTYPE_TRACK 1
#define CLASSTYPE_PARAMBLOCK 2
#define CLASSTYPE_ORIENTATION 3
#define CLASSTYPE_SHAPE 4
#define CLASSTYPE_OBJECT 5
#define CLASSTYPE_MODIFIER 6
#define CLASSTYPE_MODOBJ 7
#define CLASSTYPE_NODE 8
#define CLASSTYPE_MAP 20
#define CLASSTYPE_MATERIAL 21




typedef struct { float x,y,z; } Point3;

typedef struct { float w,x,y,z;} Quat;

typedef struct {
  float tens,cont,bias,easefrom,easeto;
} TCBparams;

typedef struct {
  int flags;
  int frame_from,frame_to;
  int numkeys;
  int keytype; // chunk id
  void* value;
  void* keys;
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
  int x1,x2;
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
  int x1,x2,x3,x4,x5,x6;   // derivaltak?
} TCB_Pos_Key;

typedef struct {
  int frame,flags;
  Point3 value;
  float x1,x2,x3,x4,x5,x6;   // derivaltak?
  int y1,y2,y3;              // 0
} Bezier_Pos_Key;

//====================== Rot Key ====================

typedef struct {
  int frame,flags;
  Quat quatvalue; //int y1,y2,y3,y4; // ? quaternion
  TCBparams tcb;
  Quat value;
  int x1,x2,x3,x4,x5,x6,x7,x8; // derivaltak???
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
  int x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14; // derivaltak??? (2*7)
} TCB_Scale_Key;





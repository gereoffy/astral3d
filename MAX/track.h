
typedef struct { float x,y,z; } Point3;

typedef struct { float w,x,y,z;} Quat;

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


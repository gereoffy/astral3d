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


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


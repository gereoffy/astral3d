
typedef struct {
  Point3 p;  // position
  Point3 n;  // normal
  Point3 tp; // transformed position
  Point3 tn; // transformed normal
} Vertex;

typedef struct {
  int verts[3];         // vertex IDs
  unsigned long sg;     // smoothing group
  unsigned short flags; // flags
  unsigned short mtl;   // submaterial ID
  Point3 n;             // facenormal
  float D;              // fave--origo distance  (for pre-trans backface cull)
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
} Mesh;

